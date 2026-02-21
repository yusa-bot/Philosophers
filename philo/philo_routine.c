/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_routine.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 23:27:09 by ayusa             #+#    #+#             */
/*   Updated: 2026/02/21 22:25:01 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void	take_forks(t_philo *philo)
{
	if (philo->x % 2 != 0)
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

static void	eat_routine(t_philo *philo)
{
	long long	eat_target_time;

	log_print(philo, "is eating");
	pthread_mutex_lock(&philo->meal_mutex);
	eat_target_time = philo->last_eat_time + philo->data->time_to_eat_us;
	pthread_mutex_unlock(&philo->meal_mutex);
	ft_usleep_to_target_time(eat_target_time);
	pthread_mutex_unlock(philo->fork_left);
	pthread_mutex_unlock(philo->fork_right);
	pthread_mutex_lock(&philo->meal_mutex);
	philo->eat_count++;
	pthread_mutex_unlock(&philo->meal_mutex);
}

static void	sleep_routine(t_philo *philo)
{
	long long	start_sleep;

	log_print(philo, "is sleeping");
	start_sleep = get_time_us();
	ft_usleep_to_target_time(start_sleep + philo->data->time_to_sleep_us);
}

static void	think_routine(t_philo *philo)
{
	long long	thinking_time;
	long long	think_target_time;

	log_print(philo, "is thinking");
	thinking_time = philo->data->time_to_eat_us - philo->data->time_to_sleep_us;
	if (thinking_time > 0)
	{
		pthread_mutex_lock(&philo->meal_mutex);
		think_target_time = philo->last_eat_time + philo->data->time_to_eat_us
			+ philo->data->time_to_sleep_us + thinking_time - 1000;
		pthread_mutex_unlock(&philo->meal_mutex);
		ft_usleep_to_target_time(think_target_time);
	}
	else if (philo->data->n_philo % 2 != 0)
		usleep(1000);
}

int	philosopher_routine(t_philo *philo)
{
	int			stop;

	pthread_mutex_lock(&philo->data->stop_flag_mutex);
	stop = philo->data->stop_flag;
	pthread_mutex_unlock(&philo->data->stop_flag_mutex);
	if (stop)
		return (1);
	take_forks(philo);
	eat_routine(philo);
	sleep_routine(philo);
	think_routine(philo);
	return (0);
}
