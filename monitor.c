/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 20:11:03 by ayusa             #+#    #+#             */
/*   Updated: 2026/02/18 14:33:42 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static int	check_die_per_philo(t_philo *philos, int i)
{
	long long	now;
	long long	let_tmp;
	long long	timestamp;

	now = get_time_us();
	pthread_mutex_lock(&philos[i].meal_mutex);
	let_tmp = philos[i].last_eat_time;
	pthread_mutex_unlock(&philos[i].meal_mutex);
	if (now - let_tmp > philos[i].data->time_to_die_us)
	{
		timestamp = (now - philos[i].data->start_time) / 1000;
		pthread_mutex_lock(&philos[i].data->log_mutex);
		pthread_mutex_lock(&philos[i].data->stop_flag_mutex);
		philos[i].data->stop_flag = 1;
		printf("%lld %d died\n", timestamp, philos[i].x);
		pthread_mutex_unlock(&philos[i].data->stop_flag_mutex);
		pthread_mutex_unlock(&philos[i].data->log_mutex);
		return (1);
	}
	return (0);
}

static void	check_eat_per_philo(int i, t_philo *philos,
		t_data *data, int *full_eat_people)
{
	int	curr_eat_count;

	if (data->n_must_eat_per_philo > -1)
	{
		pthread_mutex_lock(&philos[i].meal_mutex);
		curr_eat_count = philos[i].eat_count;
		pthread_mutex_unlock(&philos[i].meal_mutex);
		if (curr_eat_count < data->n_must_eat_per_philo)
			*full_eat_people = 0;
	}
}

static int	monitor_loop_per_philo(t_philo *philos, t_data *data,
		int *full_eat_people)
{
	int	i;

	i = 0;
	while (i < data->n_philo)
	{
		if (check_die_per_philo(philos, i))
			return (1);
		check_eat_per_philo(i, philos, data, full_eat_people);
		i++;
	}
	return (0);
}

void	monitor_loop(t_philo *philos, t_data *data)
{
	int	full_eat_people;

	while (1)
	{
		if (data->n_must_eat_per_philo == -1)
			full_eat_people = 0;
		else
			full_eat_people = 1;
		if (monitor_loop_per_philo(philos, data, &full_eat_people))
			return ;
		if (full_eat_people)
		{
			pthread_mutex_lock(&data->stop_flag_mutex);
			data->stop_flag = 1;
			pthread_mutex_unlock(&data->stop_flag_mutex);
			return ;
		}
		if (ft_usleep(1000))
			return ;
	}
}
