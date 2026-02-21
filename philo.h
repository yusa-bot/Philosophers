/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 19:32:12 by ayusa             #+#    #+#             */
/*   Updated: 2026/02/21 14:33:08 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <limits.h>
# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/time.h>
# include <unistd.h>
# include <string.h>

typedef struct s_data
{
	long long		start_time;

	int				stop_flag;
	int				n_philo;

	int				ac;
	int				n_must_eat_per_philo;

	long long		time_to_eat_us;
	long long		time_to_sleep_us;
	long long		time_to_die_us;

	pthread_mutex_t	*forks_mutex;
	pthread_mutex_t	stop_flag_mutex;
	pthread_mutex_t	log_mutex;

	int				start_ready;
	pthread_mutex_t	start_mutex;

	int				forks_mutex_succ;
	int				stop_flag_mutex_succ;
	int				log_mutex_succ;
	int				start_mutex_succ;
}					t_data;

typedef struct s_philo
{
	int				x;

	int				eat_count;
	long long		last_eat_time;

	pthread_mutex_t	*fork_left;
	pthread_mutex_t	*fork_right;

	pthread_mutex_t	meal_mutex;
	int				meal_mutex_succ;

	t_data			*data;
}					t_philo;

void				log_print(t_philo *philo, char *msg);
void				ft_usleep(long long us);
void				ft_usleep_to_target_time(long long target_time_us);
long long			get_time_us(void);
int					ft_atoi(const char *str);
void				handle_cleanup(t_philo **philos, t_data *data);
void				monitor_loop(t_philo *philos, t_data *data);
void				*philosopher_routine_handle(void *arg);
int					init_all(char **av, t_philo **philos,
						t_data *data, pthread_t **pthreads);
int					philosopher_routine(t_philo *philo);

#endif
