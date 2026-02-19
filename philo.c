/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 20:10:31 by ayusa             #+#    #+#             */
/*   Updated: 2026/02/19 22:13:21 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static int	eat_routine(t_philo *philo)
{
	log_print(philo, "is eating");
	if (ft_usleep(philo->data->time_to_eat_us))
	{
		pthread_mutex_unlock(philo->fork_left);
		pthread_mutex_unlock(philo->fork_right);
		return (1);
	}
	pthread_mutex_lock(&philo->meal_mutex);
	philo->eat_count++;
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
	long long	eat_us;
	long long	sleep_us;

	pthread_mutex_lock(&philo->data->stop_flag_mutex);
	stop = philo->data->stop_flag;
	pthread_mutex_unlock(&philo->data->stop_flag_mutex);
	if (stop)
		return (1);

	take_forks(philo);
	if (eat_routine(philo))
		return (1);
	log_print(philo, "is sleeping");
	if (ft_usleep(philo->data->time_to_sleep_us))
		return (1);
	log_print(philo, "is thinking");


	long think_time = 0;
    if (philo->data->n_philo % 2 == 0) // 偶数
        think_time = philo->data->time_to_eat_us - philo->data->time_to_sleep_us;
    else // 奇数
        think_time = (philo->data->time_to_eat_us * 2) - philo->data->time_to_sleep_us;
    if (think_time > 0)
    {
        if (think_time > 5000)
            ft_usleep(think_time - 5000);
        else
            ft_usleep(think_time);
    }


	eat_us = philo->data->time_to_eat_us;
	sleep_us = philo->data->time_to_sleep_us;
	if (philo->data->n_philo % 2 != 0 && eat_us >= sleep_us)
		ft_usleep((eat_us - sleep_us) / 2);
	pthread_mutex_lock(&philo->data->stop_flag_mutex);
	stop = philo->data->stop_flag;
	pthread_mutex_unlock(&philo->data->stop_flag_mutex);
	return (stop);
}

void	*philosopher_routine_handle(void *arg)
{
	t_philo	*philo;
	int		stop;

	philo = (t_philo *)arg;


	// 【追加】全員が揃うまで待機する（よーいドン！）
    while (1)
    {
        pthread_mutex_lock(&philo->data->start_mutex);
        if (philo->data->start_ready)
        {
            pthread_mutex_unlock(&philo->data->start_mutex);
            break;
        }
        pthread_mutex_unlock(&philo->data->start_mutex);
        usleep(50); // 待機中のCPU負荷を下げる
    }


	if (philo->data->n_philo == 1)
	{
		pthread_mutex_lock(philo->fork_left);
		log_print(philo, "has taken a fork");
		ft_usleep(philo->data->time_to_die_us);
		pthread_mutex_unlock(philo->fork_left);
		return (NULL);
	}

	if (philo->x % 2 == 0)
		ft_usleep(philo->data->time_to_eat_us / 2);

	stop = 0;
	while (!stop)
		stop = philosopher_routine(philo);
	return (NULL);
}
