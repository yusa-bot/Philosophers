/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 20:15:10 by ayusa             #+#    #+#             */
/*   Updated: 2026/02/17 20:15:35 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	handle_cleanup(t_philo **philos, t_data *data)
{
	mutexes_destroy(data, *philos);
	free(data->forks_mutex);
	free(*philos);
}

void	mutexes_destroy(t_data *data, t_philo *philos)
{
	int	i;

	if (data->stop_flag_mutex_succ)
		pthread_mutex_destroy(&data->stop_flag_mutex);
	if (data->log_mutex_succ)
		pthread_mutex_destroy(&data->log_mutex);
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