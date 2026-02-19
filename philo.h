/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 19:32:12 by ayusa             #+#    #+#             */
/*   Updated: 2026/02/19 10:24:06 by ayusa            ###   ########.fr       */
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

typedef struct s_data
{
	long long		start_time;

	int				stop_flag;
	int				n_philo;

	int				ac;
	int				n_must_eat_per_philo;

	int				time_to_eat_us;
	int				time_to_sleep_us;
	int				time_to_die_us;

	pthread_mutex_t	*forks_mutex;
	pthread_mutex_t	stop_flag_mutex;
	pthread_mutex_t	log_mutex;

	int				forks_mutex_succ;
	int				stop_flag_mutex_succ;
	int				log_mutex_succ;
}					t_data;

typedef struct s_philo
{
	int			x;

	int			eat_count;
	long long		last_eat_time;

	pthread_mutex_t	*fork_left;
	pthread_mutex_t	*fork_right;

	pthread_mutex_t	meal_mutex;
	int				meal_mutex_succ;

	t_data			*data;
}					t_philo;

// util.c
void				log_print(t_philo *philo, char *msg);
int					ft_usleep(int us);
long long			get_time_us(void);
int					ft_atoi(const char *str);

// cleanup.c
void				handle_cleanup(t_philo **philos, t_data *data);

// monitor.c
void				monitor_loop(t_philo *philos, t_data *data);

// philo.c
void				*philosopher_routine(void *arg);

// init.c
int					init_all(char **av, t_philo **philos,
						t_data *data, pthread_t **threads);

#endif
