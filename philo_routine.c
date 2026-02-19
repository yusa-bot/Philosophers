/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_routine.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 23:27:09 by ayusa             #+#    #+#             */
/*   Updated: 2026/02/19 23:30:01 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	eat_routine(t_philo *philo)
{
	log_print(philo, "is eating");
	ft_usleep(philo->data->time_to_eat_us);
	pthread_mutex_lock(&philo->meal_mutex);
	philo->eat_count++;
	pthread_mutex_unlock(&philo->meal_mutex);
	pthread_mutex_unlock(philo->fork_left);
	pthread_mutex_unlock(philo->fork_right);
	return (0);
}

int	think_routine(t_philo *philo)
{
	long long	think_time;
	long long	eat_us;
	long long	sleep_us;

	eat_us = philo->data->time_to_eat_us;
	sleep_us = philo->data->time_to_sleep_us;
	think_time = 0;
	if (philo->data->n_philo % 2 == 0)
		think_time = eat_us - sleep_us;
	else
		think_time = (eat_us * 2) - sleep_us;
	log_print(philo, "is thinking");
	if (think_time > 0)
	{
		if (think_time > 5000)
			ft_usleep(think_time - 5000);
		else
			ft_usleep(think_time);
	}
	return (0);
}
