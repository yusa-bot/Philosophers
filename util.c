/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   util.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 18:10:41 by ayusa             #+#    #+#             */
/*   Updated: 2026/02/19 21:27:33 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void    log_print(t_philo *philo, char *msg)
{
    long long   now;
    long long   timestamp;
    int         stop;

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

int	ft_usleep(long long us)
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
		else
			usleep(remaining / 2);
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
