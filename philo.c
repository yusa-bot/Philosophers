/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 20:10:31 by ayusa             #+#    #+#             */
/*   Updated: 2026/02/19 14:49:14 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void log_neighbor_eat(t_philo *philo)
{
	philo[i - 1].left_philo_eat = 1;
	philo[i + 1].right_philo_eat = 1;
}

// philosopher_routine
static int	eat_routine(t_philo *philo)
{
	pthread_mutex_lock(&philo->meal_mutex);
	philo->last_eat_time = get_time_us();
	pthread_mutex_unlock(&philo->meal_mutex);
	log_print(philo, "is eating");
	if (ft_usleep(philo->data->time_to_eat_us))
		return (1);
	pthread_mutex_lock(&philo->meal_mutex);
	philo->eat_count++;

	log_neighbor_eat(philo);

	pthread_mutex_unlock(&philo->meal_mutex);
	pthread_mutex_unlock(philo->fork_left);
	pthread_mutex_unlock(philo->fork_right);
	return (0);
}

static void	take_forks(t_philo *philo)
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

void	if_one_philo(t_philo *philo)
{
	pthread_mutex_lock(philo->fork_left);
	log_print(philo, "has taken a fork");
	ft_usleep(philo->data->time_to_die_us);
	pthread_mutex_unlock(philo->fork_left);
}

static int	philo_cycle(t_philo *philo)
{
	int	stop;

	take_forks(philo);
	if (eat_routine(philo))
		return (1);
	log_print(philo, "is sleeping");
	if (ft_usleep(philo->data->time_to_sleep_us))
		return (1);
	log_print(philo, "is thinking");
	pthread_mutex_lock(&philo->data->stop_flag_mutex);
	stop = philo->data->stop_flag;
	pthread_mutex_unlock(&philo->data->stop_flag_mutex);
	return (stop);
}

check_neighbor_eat()
{
	if (もし隣の哲学者がお腹を空かせている)
	{
		if (自分の前回の食事以降にその隣人がまだ食事をしていない)
		{
			自分はその共有フォークを手に取らない（＝隣人に譲る）
		}
	}
}

void	*philosopher_routine(void *arg)
{
	t_philo	*philo;
	int		stop;

	philo = (t_philo *)arg;
	if (philo->data->n_philo == 1)
	{
		if_one_philo(philo);
		return (NULL);
	}
	// if (philo->x % 2 == 0)
	// 	ft_usleep(philo->data->time_to_eat_us);
	stop = 0;

	check_neighbor_eat();

	while (!stop)
		stop = philo_cycle(philo);
	return (NULL);
}
