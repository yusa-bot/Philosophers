/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 20:10:31 by ayusa             #+#    #+#             */
/*   Updated: 2026/02/21 17:04:35 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void	one_philo(t_philo	*philo)
{
	pthread_mutex_lock(philo->fork_left);
	log_print(philo, "has taken a fork");
	ft_usleep(philo->data->time_to_die_us);
	pthread_mutex_unlock(philo->fork_left);
}

// all pthread_create と philos[i].last_eat_time = get_time_us() が all 終わっているか？
static void	wait_all_pthread(t_philo *philo)
{
	while (1)
	{
		pthread_mutex_lock(&philo->data->start_mutex);
		if (philo->data->start_ready)
		{
			pthread_mutex_unlock(&philo->data->start_mutex);
			break ;
		}
		pthread_mutex_unlock(&philo->data->start_mutex);
		usleep(50);
	}
}

void	*philosopher_routine_handle(void *arg)
{
	t_philo	*philo;
	int		stop;

	philo = (t_philo *)arg;
	wait_all_pthread(philo);
	if (philo->data->n_philo == 1)
	{
		one_philo(philo);
		return (NULL);
	}
	if (philo->x % 2 == 0)
		ft_usleep(philo->data->time_to_eat_us / 2);
	stop = 0;
	while (!stop)
		stop = philosopher_routine(philo);
	return (NULL);
}
