/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 19:32:31 by ayusa             #+#    #+#             */
/*   Updated: 2026/02/18 17:09:43 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static int	run_threads(t_philo *philos, pthread_t *threads, t_data *data)
{
	int	i;

	i = 0;
	while (i < (int)data->n_philo)
	{
		if (pthread_create(&threads[i], NULL, philosopher_routine, &philos[i]))
		{
			pthread_mutex_lock(&data->stop_flag_mutex);
			data->stop_flag = 1;
			pthread_mutex_unlock(&data->stop_flag_mutex);
			while (--i >= 0)
				pthread_join(threads[i], NULL);
			return (1);
		}
		i++;
	}
	return (0);
}

static int	run_philo(t_philo *philos, t_data *data, pthread_t *threads)
{
	size_t	i;

	data->start_time = get_time_us();
	i = 0;
	while (i < data->n_philo)
	{
		philos[i].last_eat_time = data->start_time;
		i++;
	}
	if (run_threads(philos, threads, data))
		return (1);
	monitor_loop(philos, data);
	return (0);
}

static void	end_philo(t_philo **philos, t_data data, pthread_t *threads)
{
	size_t	i;

	i = 0;
	while (i < data.n_philo)
	{
		pthread_join(threads[i], NULL);
		i++;
	}
	handle_cleanup(philos, &data);
	free(threads);
}

// n_philo, time_to_die, time_to_eat, time_to_sleep, n_must_eat
int	main(int ac, char **av)
{
	t_data		data;
	t_philo		*philos;
	pthread_t	*threads;

	if (ac != 5 && ac != 6)
		return (1);
	data.ac = ac;
	if (init_all(av, &philos, &data, &threads))
		return (1);
	if (run_philo(philos, &data, threads))
	{
		free(threads);
		handle_cleanup(&philos, &data);
		return (1);
	}
	end_philo(&philos, data, threads);
	return (0);
}
