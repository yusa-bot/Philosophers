
// size_t number_of_philosophers(1), time_t time_to_die(2), time_t time_to_eat(3), time_t time_to_sleep(4), size_t number_of_times_each_philosopher_must_eat(5) )av
int init_data(t_data *data, char **av, int ac)
{
	data->stop_flag_mutex_succ = 0;
	data->log_mutex_succ = 0;

	data->stop_flag = 0;
	data->number_of_philosophers = ft_atoi(av[1]);

	data->time_to_die_us = ft_atoi(av[2]) * 1000;
	data->time_to_eat_us = ft_atoi(av[3]) * 1000;
	data->time_to_sleep_us = ft_atoi(av[4]) * 1000;

	if (ac == 6)
		data->number_of_times_each_philosopher_must_eat = ft_atoi(av[5]);
	else
		data->number_of_times_each_philosopher_must_eat = -1;

	// forks_mutexはmainで作成済

	if (pthread_mutex_init(&data->stop_flag_mutex, NULL))
		return 1; //失敗
	data->stop_flag_mutex_succ = 1;
	if (pthread_mutex_init(&data->log_mutex, NULL))
		return 1; //失敗
	data->log_mutex_succ = 1;

	// 範囲チェック
	if (data->number_of_philosophers < 1 \
		|| data->time_to_die_us < 1 || data->time_to_eat_us < 1 || data->time_to_sleep_us < 1 \
		|| (data->number_of_times_each_philosopher_must_eat == 0 || data->number_of_times_each_philosopher_must_eat < -1))
			return 1;

	return 0;
}