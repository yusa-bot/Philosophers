/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prc.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 00:00:00 by ayusa             #+#    #+#             */
/*   Updated: 2026/02/17 20:01:13 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

// monitor_loop
int	check_die_per_philo(t_philo *philos, int i)
{
	long long	now;
	long long	let_tmp;
	long long	timestamp;

	now = get_time_us();
	pthread_mutex_lock(&philos[i].meal_mutex);
	let_tmp = philos[i].last_eat_time;
	pthread_mutex_unlock(&philos[i].meal_mutex);
	if (now - let_tmp > philos[i].data->time_to_die_us)
	{
		timestamp = (now - philos[i].data->start_time) / 1000;
		pthread_mutex_lock(&philos[i].data->log_mutex);
		pthread_mutex_lock(&philos[i].data->stop_flag_mutex);
		philos[i].data->stop_flag = 1;
		printf("%lld %zu died\n", timestamp, philos[i].x);
		pthread_mutex_unlock(&philos[i].data->stop_flag_mutex);
		pthread_mutex_unlock(&philos[i].data->log_mutex);
		return (1);
	}
	return (0);
}

void	check_eat_per_philo(int i, t_philo *philos,
		t_data *data, int *full_eat_people)
{
	int	curr_eat_count;

	if (data->n_must_eat_per_philo > -1)
	{
		pthread_mutex_lock(&philos[i].meal_mutex);
		curr_eat_count = philos[i].eat_count;
		pthread_mutex_unlock(&philos[i].meal_mutex);
		if (curr_eat_count < data->n_must_eat_per_philo)
			*full_eat_people = 0;
	}
}

int	monitor_loop_per_philo(t_philo *philos, t_data *data,
		int *full_eat_people)
{
	int	i;

	i = 0;
	while (i < data->n_philo)
	{
		if (check_die_per_philo(philos, i))
			return (1);
		check_eat_per_philo(i, philos, data, full_eat_people);
		i++;
	}
	return (0);
}

void	monitor_loop(t_philo *philos, t_data *data)
{
	int	full_eat_people;

	while (1)
	{
		if (data->n_must_eat_per_philo == -1)
			full_eat_people = 0;
		else
			full_eat_people = 1;
		if (monitor_loop_per_philo(philos, data, &full_eat_people))
			return ;
		if (full_eat_people)
		{
			pthread_mutex_lock(&data->stop_flag_mutex);
			data->stop_flag = 1;
			pthread_mutex_unlock(&data->stop_flag_mutex);
			return ;
		}
		if (ft_usleep(1000))
			return ;
	}
}

// philosopher_routine
int eat_routine(t_philo	*philo)
{
	// いただきます
	pthread_mutex_lock(&philo->meal_mutex);
	philo->last_eat_time = get_time_us();
	pthread_mutex_unlock(&philo->meal_mutex);
	log_print(philo, "is eating");
	if (ft_usleep(philo->data->time_to_eat_us))
		return (1); //失敗
	pthread_mutex_lock(&philo->meal_mutex);
	philo->eat_count++;
	pthread_mutex_unlock(&philo->meal_mutex);

	// ごちそうさま
	pthread_mutex_unlock(philo->fork_left);
	pthread_mutex_unlock(philo->fork_right);

	return 0;
}


take_forks(t_philo	*philo)
{
	if (philo->x % 2) //奇数
	{
		pthread_mutex_lock(philo->fork_left);
		log_print(philo, "has taken a fork");
		pthread_mutex_lock(philo->fork_right);
		log_print(philo, "has taken a fork");
	}
	else //偶数
	{
		pthread_mutex_lock(philo->fork_right);
		log_print(philo, "has taken a fork");
		pthread_mutex_lock(philo->fork_left);
		log_print(philo, "has taken a fork");
	}
}


void	*philosopher_routine(void *arg)
{
	t_philo	*philo;
	int		now_flag;

	philo = (t_philo *)arg;
	now_flag = 0;

	while (!now_flag)
	{
		take_forks(philo);
		if (eat_routine(philo))
			return (NULL);

		// 睡眠
		log_print(philo, "is sleeping");
		if (ft_usleep(philo->data->time_to_sleep_us))
			return (NULL);
		// 思考
		log_print(philo, "is thinking");
		// stop_flag読み取り
		pthread_mutex_lock(&philo->data->stop_flag_mutex);
		now_flag = philo->data->stop_flag;
		pthread_mutex_unlock(&philo->data->stop_flag_mutex);
	}
	return (NULL);
}


// philo program
int	forks_generate(t_data *data)
{
	int	i;

	data->forks_mutex_succ = 0;
	// フォークメモリ確保
	data->forks_mutex = malloc(sizeof(pthread_mutex_t) * (data->n_philo));
	if (!data->forks_mutex)
		return (1);
	i = 0;
	while (i < data->n_philo) // フォーク生成
	{
		if (pthread_mutex_init(&data->forks_mutex[i], NULL))
		{ //失敗
			while (--i >= 0)
				pthread_mutex_destroy(&data->forks_mutex[i]);
			return (1);
		}
		i++;
	}
	data->forks_mutex_succ = 1;
	return (0);
}

int init_all(char **av, t_philo **philos, t_data *data, pthread_t **threads)
{
	data->n_philo = ft_atoi(av[1]);

	*philos = malloc(sizeof(t_philo) * (data->n_philo)); // 哲学者は構造体の配列だから、メモリ確保
	if (!*philos)
		return (1);

	if (forks_generate(data)) // フォーク生成
		return (free(*philos), 1);

	if (init_data(data, av, ac)) // 引数をdata構造体に
		return (handle_cleanup(philos, data), 1);

	*threads = malloc(sizeof(pthread_t) * data->n_philo);
	if (!*threads)
		return (handle_cleanup(philos, data), 1);

	if (init_philo(*philos, data)) // 哲学者全員分 初期化
	{                              //失敗
		free(*threads);
		handle_cleanup(philos, data);
		return (1);
	}
	return 0;
}

int run_philo( t_philo *philos, t_data *data, pthread_t	*threads)
{
	int	i;

	data->start_time = get_time_us();
	i = 0;
	while (i < data->n_philo)
	{
		philos[i].last_eat_time = data->start_time;
		i++;
	}

	if (run_threads(philos, threads, data)) // スレッド 開始
		return 1;

	monitor_loop(philos, data); // 死亡/終了を監視
	return 0;
}

end_philo(t_philo **philos, t_data data, pthread_t	*threads)
{
	int i;

	i = 0;
	while (i < data.n_philo)
	{
		pthread_join(threads[i], NULL); // 失敗は「無効なスレッドID」や「既にjoin済み」の場合
		i++;
	}

	handle_cleanup(philos, &data);
	free(threads);
}

// size_t n_philo, time_t time_to_die, time_t time_to_eat, time_t time_to_sleep, size_t n_must_eat_per_philo
int	main(int ac, char **av)
{
	t_data		data;
	t_philo		*philos;
	pthread_t	*threads;

	if (ac != 5 && ac != 6) // arg filter
		return (1);

	if (init_all(av, &philos, &data, &threads))
		return 1;


	if (run_philo(philos, &data, threads))
	{
		free(threads);
		handle_cleanup(&philos, &data);
		return (1);
	}

	end_philo(&philos, data, threads);

	return (0);
}