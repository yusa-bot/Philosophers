// ft_atoi
static int	check_overflow(int sign, long current, char next)
{
	int	digit;

	digit = next - '0';
	if (sign == 1)
	{
		if (current > LONG_MAX / 10
			|| (current == LONG_MAX / 10 && digit > LONG_MAX % 10))
			return (1);
	}
	if (sign == -1)
	{
		if (current > -(LONG_MIN / 10)
			|| (current == -(LONG_MIN / 10) && digit > -(LONG_MIN % 10)))
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
		if (check_overflow(sign, result, *str) == 1)
			return (-2);
		if (check_overflow(sign, result, *str) == -1)
			return (-2);
		result = result * 10 + (long)(*str - '0');
		str++;
	}
	return ((int)(result * sign));
}

int ft_usleep(int us)
{
	while (us >= 1000000)
	{
		if (usleep(999999) < 0)
			return 1;
		us -= 999999;
	}

	if (us > 0)
	{
		if (usleep(us) < 0)
			return 1;
	}
	return 0;
}

long long get_time_us(void)
{
	struct timeval tv;

	if (gettimeofday(&tv, NULL) < 0)
		return 1; //失敗
	return (tv.tv_sec * 1000000LL + tv.tv_usec);
}


void mutexes_destroy(t_data *data, t_philo *philos)
{
	// mutex_destroy処理
	if (data->stop_flag_mutex_succ)
		pthread_mutex_destroy(&data->stop_flag_mutex);
	if (data->log_mutex_succ)
		pthread_mutex_destroy(&data->log_mutex);

	int i = 0;
	if (data->forks_mutex_succ)
	{
		while (i < data->number_of_philosophers)
		{
			pthread_mutex_destroy(&data->forks_mutex[i]);
			i++;
		}
	}

	i = 0;
	while (i < data->number_of_philosophers)
	{
		if (philos[i].meal_mutex_succ)
			pthread_mutex_destroy(&philos[i].meal_mutex);
		i++;
	}
}

void philo_error_util(t_philo **philos, t_data *data)
{
	mutexes_destroy(data, *philos);
	free(data->forks_mutex);

	free(*philos);
	return ;
}