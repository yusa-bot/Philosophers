/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 20:13:19 by ayusa             #+#    #+#             */
/*   Updated: 2026/02/18 13:51:22 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

// av: n_philo(1) t_die(2) t_eat(3) t_sleep(4) n_must_eat_per_philo(5)
static int	init_data(t_data *data, char **av)
{
	data->stop_flag_mutex_succ = 0;
	data->log_mutex_succ = 0;
	data->stop_flag = 0;
	data->n_philo = ft_atoi(av[1]);
	data->time_to_die_us = ft_atoi(av[2]) * 1000;
	data->time_to_eat_us = ft_atoi(av[3]) * 1000;
	data->time_to_sleep_us = ft_atoi(av[4]) * 1000;
	if (data->ac == 6)
		data->n_must_eat_per_philo = ft_atoi(av[5]);
	else
		data->n_must_eat_per_philo = -1;
	if (pthread_mutex_init(&data->stop_flag_mutex, NULL))
		return (1);
	data->stop_flag_mutex_succ = 1;
	if (pthread_mutex_init(&data->log_mutex, NULL))
		return (1);
	data->log_mutex_succ = 1;
	if (data->n_philo < 1 || data->time_to_die_us < 1
		|| data->time_to_eat_us < 1 || data->time_to_sleep_us < 1
		|| data->n_must_eat_per_philo == 0 || data->n_must_eat_per_philo < -1)
		return (1);
	return (0);
}

static int	init_philo(t_philo *philos, t_data *data)
{
	int	i;

	i = 0;
	while (i < data->n_philo)
	{
		philos[i].x = i + 1;
		philos[i].meal_mutex_succ = 0;
		if (pthread_mutex_init(&philos[i].meal_mutex, NULL))
			return (1);
		philos[i].meal_mutex_succ = 1;
		philos[i].eat_count = 0;
		philos[i].last_eat_time = 0;
		philos[i].fork_left = &data->forks_mutex[i];
		philos[i].fork_right = &data->forks_mutex[(i + 1) % data->n_philo];
		philos[i].data = data;
		i++;
	}
	return (0);
}

static int	forks_generate(t_data *data)
{
	int	i;

	data->forks_mutex_succ = 0;
	data->forks_mutex
		= malloc(sizeof(pthread_mutex_t) * (data->n_philo));
	if (!data->forks_mutex)
		return (1);
	i = 0;
	while (i < data->n_philo)
	{
		if (pthread_mutex_init(&data->forks_mutex[i], NULL))
		{
			while (--i >= 0)
				pthread_mutex_destroy(&data->forks_mutex[i]);
			return (1);
		}
		i++;
	}
	data->forks_mutex_succ = 1;
	return (0);
}

int	init_all(char **av, t_philo **philos,
		t_data *data, pthread_t **threads)
{
	data->n_philo = ft_atoi(av[1]);
	*philos = malloc(sizeof(t_philo) * (data->n_philo));
	if (!*philos)
		return (1);
	if (forks_generate(data))
		return (free(*philos), 1);
	if (init_data(data, av))
		return (handle_cleanup(philos, data), 1);
	*threads = malloc(sizeof(pthread_t) * data->n_philo);
	if (!*threads)
		return (handle_cleanup(philos, data), 1);
	if (init_philo(*philos, data))
	{
		free(*threads);
		handle_cleanup(philos, data);
		return (1);
	}
	return (0);
}
