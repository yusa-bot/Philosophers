/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   util.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 18:10:41 by ayusa             #+#    #+#             */
/*   Updated: 2026/02/19 15:36:22 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	log_print(t_philo *philo, char *msg)
{
	long long	now;
	long long	timestamp;

	now = get_time_us();
	timestamp = (now - philo->data->start_time) / 1000;
	pthread_mutex_lock(&philo->data->log_mutex);
	pthread_mutex_lock(&philo->data->stop_flag_mutex);
	if (!philo->data->stop_flag)
		printf("%lld %d %s\n", timestamp, philo->x, msg);
	pthread_mutex_unlock(&philo->data->stop_flag_mutex);
	pthread_mutex_unlock(&philo->data->log_mutex);
}

long long	get_time_us(void)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) < 0)
		return (1);
	return (tv.tv_sec * 1000000LL + tv.tv_usec);
}

int	ft_usleep(int us)
{
	while (us >= 500)
	{
		if (usleep(499) < 0)
			return (1);
		us -= 499;
	}
	if (us > 0)
	{
		if (usleep(us) < 0)
			return (1);
	}
	return (0);
}

// ft_atoi
static int	check_overflow(int sign, long current, char next)
{
	int	digit;

	digit = next - '0';
	if (sign == 1)
	{
		if (current > LONG_MAX / 10 || (current == LONG_MAX / 10
				&& digit > LONG_MAX % 10))
			return (1);
	}
	if (sign == -1)
	{
		if (current > -(LONG_MIN / 10) || (current == -(LONG_MIN / 10)
				&& digit > -(LONG_MIN % 10)))
			return (-1);
	}
	return (0);
}

int	ft_atoi(const char *str)
{
	int					sign;
	unsigned long int	result;

	sign = 1;
	result = 0;
	while (*str == 32 || (*str >= 9 && *str <= 13))
		str++;
	if (*str == '-')
	{
		sign = -1;
		str++;
	}
	else if (*str == '+')
		str++;
	while (*str >= '0' && *str <= '9')
	{
		if (check_overflow(sign, result, *str))
			return (-2);
		result = result * 10 + (long)(*str - '0');
		str++;
	}
	if (*str != '\0')
		return (-2);
	return ((int)(result * sign));
}
