/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 20:15:10 by ayusa             #+#    #+#             */
/*   Updated: 2026/02/21 17:22:09 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void	mutexes_destroy(t_data *data, t_philo *philos)
{
	int	i;

	if (data->stop_flag_mutex_succ)
		pthread_mutex_destroy(&data->stop_flag_mutex);
	if (data->log_mutex_succ)
		pthread_mutex_destroy(&data->log_mutex);
	if (data->start_mutex_succ)
		pthread_mutex_destroy(&data->start_mutex);
	i = 0;
	if (data->forks_mutex_succ)
	{
		while (i < data->n_philo)
		{
			pthread_mutex_destroy(&data->forks_mutex[i]);
			i++;
		}
	}
	i = 0;
	while (i < data->n_philo)
	{
		if (philos[i].meal_mutex_succ)
			pthread_mutex_destroy(&philos[i].meal_mutex);
		i++;
	}
}

void	handle_cleanup(t_philo **philos, t_data *data)
{
	mutexes_destroy(data, *philos);
	free(data->forks_mutex);
	free(*philos);
}

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
