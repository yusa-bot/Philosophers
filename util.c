
long long get_time_ms(void)
{
	struct timeval tv;

	if (gettimeofday(tv, NULL) < 0)
		return 1; //失敗
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

// size_t number_of_philosophers(1), time_t time_to_die(2), time_t time_to_eat(3), time_t time_to_sleep(4), size_t number_of_times_each_philosopher_must_eat(5) )av
void init_data(t_data *data, char **av, int ac)
{
	data->stop_flag = 0;
	data->number_of_philosophers = ft_atoi(av[1]); // 範囲チェック

	if (ac == 6)
		data->number_of_times_each_philosopher_must_eat = ft_atoi(av[5]); // 範囲チェック
	else
		data->number_of_times_each_philosopher_must_eat = -1;

	if (ft_atoi(av[3]) >= 1000 || ft_atoi(av[4]) >= 1000)
		return 1; // usleep
	data->time_to_die_m = ft_atoi(av[2]) * 1000;
	data->time_to_eat_ms = ft_atoi(av[3]) * 1000;;
	data->time_to_sleep_ms = ft_atoi(av[4]) * 1000;;

	// forks_mutexはmainで作成済

	pthread_mutex_init(&data->stop_flag_mutex, NULL);
	pthread_mutex_init(&data->log_mutex, NULL);
	return 0;
}

int init_philo(t_philo *philo, t_data *data)
{
	int i = 0;

	// 構造体 初期化
	while (i <= philo[i].data->number_of_philosophers)
	{
		philo[i].x = i;
		philo[i].eat_count = 0;

		long long now = get_time_ms();
		if (now < 0)
			return ;
		philo[i].last_eat_time = now;

		// 自分の左右forkの番号を教える
		philo[i].fork_left = &data->forks_mutex[i];
		philo[i].fork_right = &data->forks_mutex[(i+1) % data->number_of_philosophers];
		philo[i].data = *data;
		i++;
	}
	return 0;
}
int run_threads(t_philo *philo, pthread_t **threads)
{
	// スレッド 開始
	i = 0;
	while (i <= philo[i].data->number_of_philosophers)
	{
		if (pthread_create(&threads[i], NULL, *philosopher_routine, &philo[i]))
			return 1; //失敗
		i++;
	}
	return 0;
}