/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 20:10:31 by ayusa             #+#    #+#             */
/*   Updated: 2026/02/17 20:10:42 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

// philosopher_routine
int	eat_routine(t_philo *philo)
{
	pthread_mutex_lock(&philo->meal_mutex);
	philo->last_eat_time = get_time_us();
	pthread_mutex_unlock(&philo->meal_mutex);
	log_print(philo, "is eating");
	if (ft_usleep(philo->data->time_to_eat_us))
		return (1);
	pthread_mutex_lock(&philo->meal_mutex);
	philo->eat_count++;
	pthread_mutex_unlock(&philo->meal_mutex);
	pthread_mutex_unlock(philo->fork_left);
	pthread_mutex_unlock(philo->fork_right);
	return (0);
}

void	take_forks(t_philo *philo)
{
	if (philo->x % 2)
	{
		pthread_mutex_lock(philo->fork_left);
		log_print(philo, "has taken a fork");
		pthread_mutex_lock(philo->fork_right);
		log_print(philo, "has taken a fork");
	}
	else
	{
		pthread_mutex_lock(philo->fork_right);
		log_print(philo, "has taken a fork");
		pthread_mutex_lock(philo->fork_left);
		log_print(philo, "has taken a fork");
	}
}

void	*philosopher_routine(void *arg)
{
	t_philo	*philo;
	int		now_flag;

	philo = (t_philo *)arg;
	now_flag = 0;
	while (!now_flag)
	{
		take_forks(philo);
		if (eat_routine(philo))
			return (NULL);
		log_print(philo, "is sleeping");
		if (ft_usleep(philo->data->time_to_sleep_us))
			return (NULL);
		log_print(philo, "is thinking");
		pthread_mutex_lock(&philo->data->stop_flag_mutex);
		now_flag = philo->data->stop_flag;
		pthread_mutex_unlock(&philo->data->stop_flag_mutex);
	}
	return (NULL);
}
