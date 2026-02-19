/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 20:10:31 by ayusa             #+#    #+#             */
/*   Updated: 2026/02/19 23:29:47 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void	take_forks(t_philo *philo)
{
	if (philo->x % 2)
	{
		pthread_mutex_lock(philo->fork_left);
		log_print(philo, "has taken a fork");
		pthread_mutex_lock(philo->fork_right);
		pthread_mutex_lock(&philo->meal_mutex);
		philo->last_eat_time = get_time_us();
		pthread_mutex_unlock(&philo->meal_mutex);
		log_print(philo, "has taken a fork");
	}
	else
	{
		pthread_mutex_lock(philo->fork_right);
		log_print(philo, "has taken a fork");
		pthread_mutex_lock(philo->fork_left);
		pthread_mutex_lock(&philo->meal_mutex);
		philo->last_eat_time = get_time_us();
		pthread_mutex_unlock(&philo->meal_mutex);
		log_print(philo, "has taken a fork");
	}
}

static int	philosopher_routine(t_philo *philo)
{
	int			stop;

	pthread_mutex_lock(&philo->data->stop_flag_mutex);
	stop = philo->data->stop_flag;
	pthread_mutex_unlock(&philo->data->stop_flag_mutex);
	if (stop)
		return (1);
	take_forks(philo);
	if (eat_routine(philo))
		return (1);
	log_print(philo, "is sleeping");
	ft_usleep(philo->data->time_to_sleep_us);
	if (think_routine(philo))
		return (1);
	return (0);
}

static void	one_philo(t_philo	*philo)
{
	pthread_mutex_lock(philo->fork_left);
	log_print(philo, "has taken a fork");
	ft_usleep(philo->data->time_to_die_us);
	pthread_mutex_unlock(philo->fork_left);
}

void	*philosopher_routine_handle(void *arg)
{
	t_philo	*philo;
	int		stop;

	philo = (t_philo *)arg;
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
	if (philo->data->n_philo == 1)
		one_philo(philo);
	if (philo->x % 2 == 0)
		ft_usleep(philo->data->time_to_eat_us / 2);
	stop = 0;
	while (!stop)
		stop = philosopher_routine(philo);
	return (NULL);
}
