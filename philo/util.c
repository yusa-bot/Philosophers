/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   util.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 18:10:41 by ayusa             #+#    #+#             */
/*   Updated: 2026/02/21 17:21:55 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	log_print(t_philo *philo, char *msg)
{
	long long	now;
	long long	timestamp;
	int			stop;

	pthread_mutex_lock(&philo->data->log_mutex);
	pthread_mutex_lock(&philo->data->stop_flag_mutex);
	stop = philo->data->stop_flag;
	pthread_mutex_unlock(&philo->data->stop_flag_mutex);
	if (!stop)
	{
		now = get_time_us();
		timestamp = (now - philo->data->start_time) / 1000;
		printf("%lld %d %s\n", timestamp, philo->x, msg);
	}
	pthread_mutex_unlock(&philo->data->log_mutex);
}

long long	get_time_us(void)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) < 0)
		return (1);
	return (tv.tv_sec * 1000000LL + tv.tv_usec);
}

void	ft_usleep(long long us)
{
	long long	start;
	long long	remaining;

	start = get_time_us();
	while (1)
	{
		remaining = us - (get_time_us() - start);
		if (remaining <= 0)
			break ;
		if (remaining > 500)
			usleep(499);
	}
}

void	ft_usleep_to_target_time(long long target_time_us)
{
	long long	remaining;

	while (1)
	{
		remaining = target_time_us - get_time_us();
		if (remaining <= 0)
			break ;
		if (remaining > 500)
			usleep(499);
	}
}
