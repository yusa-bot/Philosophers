/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 19:32:31 by ayusa             #+#    #+#             */
/*   Updated: 2026/02/19 23:25:12 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static int	run_pthreads(t_philo *philos, pthread_t *pthreads, t_data *data)
{
	int	i;

	i = 0;
	while (i < data->n_philo)
	{
		if (pthread_create(&pthreads[i], NULL,
				philosopher_routine_handle, &philos[i]))
		{
			pthread_mutex_lock(&data->stop_flag_mutex);
			data->stop_flag = 1;
			pthread_mutex_unlock(&data->stop_flag_mutex);
			while (--i >= 0)
				pthread_join(pthreads[i], NULL);
			return (1);
		}
		i++;
	}
	return (0);
}

static int	ready_start_pthreads(t_philo *philos,
	pthread_t *pthreads, t_data *data)
{
	int	i;

	if (run_pthreads(philos, pthreads, data))
		return (1);
	data->start_time = get_time_us();
	i = 0;
	while (i < data->n_philo)
	{
		philos[i].last_eat_time = data->start_time;
		i++;
	}
	pthread_mutex_lock(&data->start_mutex);
	data->start_ready = 1;
	pthread_mutex_unlock(&data->start_mutex);
	return (0);
}

static int	run_philo(t_philo *philos, t_data *data, pthread_t *pthreads)
{
	if (ready_start_pthreads(philos, pthreads, data))
		return (1);
	monitor_loop(philos, data);
	return (0);
}

static void	end_philo(t_philo **philos, t_data *data, pthread_t *pthreads)
{
	int	i;

	i = 0;
	while (i < data->n_philo)
	{
		pthread_join(pthreads[i], NULL);
		i++;
	}
	handle_cleanup(philos, data);
	free(pthreads);
}

// n_philo, time_to_die, time_to_eat, time_to_sleep, n_must_eat
int	main(int ac, char **av)
{
	t_data		data;
	t_philo		*philos;
	pthread_t	*pthreads;

	if (ac != 5 && ac != 6)
		return (1);
	data.ac = ac;
	if (init_all(av, &philos, &data, &pthreads))
		return (1);
	if (run_philo(philos, &data, pthreads))
	{
		free(pthreads);
		handle_cleanup(&philos, &data);
		return (1);
	}
	end_philo(&philos, &data, pthreads);
	return (0);
}
