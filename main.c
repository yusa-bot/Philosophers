#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
# include <limits.h>

// 構造体 形式定義
typedef struct s_data
{
	long long start_time;

	int stop_flag;
	size_t number_of_philosophers;

	int number_of_times_each_philosopher_must_eat;

	int time_to_eat_mcs;
	int time_to_sleep_mcs;
	int time_to_die_ms;

	pthread_mutex_t *forks_mutex;
	pthread_mutex_t stop_flag_mutex;
	pthread_mutex_t log_mutex;

	int forks_mutex_succ;
	int stop_flag_mutex_succ;
	int log_mutex_succ;
}	t_data;

typedef struct s_philo
{
	size_t x;

	size_t eat_count;
	long long last_eat_time;

	pthread_mutex_t *fork_left;
	pthread_mutex_t *fork_right;

	pthread_mutex_t meal_mutex;
	int meal_mutex_succ;

	t_data *data;
}	t_philo;

void *philosopher_routine(void *arg);
void mutexes_destroy(t_data *data, t_philo *philos);
void log_print(t_philo *philo, char *msg);

////////////////////////////////////

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
			return ((int)LONG_MAX);
		if (check_overflow(sign, result, *str) == -1)
			return ((int)LONG_MIN);
		result = result * 10 + (long)(*str - '0');
		str++;
	}
	return ((int)(result * sign));
}

long long get_time_ms(void)
{
	struct timeval tv;

	if (gettimeofday(&tv, NULL) < 0)
		return 1; //失敗
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
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
		while (i++ < data->number_of_philosophers)
			pthread_mutex_destroy(&data->forks_mutex[i]);
	}

	i = 0;
	while (i++ < data->number_of_philosophers)
		pthread_mutex_destroy(&philos[i].meal_mutex);
}

void philo_error_util(t_philo **philos, t_data *data)
{
	mutexes_destroy(data, philos);
	free(data->forks_mutex);

	free(*philos);
	return ;
}

// size_t number_of_philosophers(1), time_t time_to_die(2), time_t time_to_eat(3), time_t time_to_sleep(4), size_t number_of_times_each_philosopher_must_eat(5) )av
int init_data(t_data *data, char **av, int ac)
{
	data->stop_flag = 0;
	data->number_of_philosophers = ft_atoi(av[1]);

	if (ac == 6)
		data->number_of_times_each_philosopher_must_eat = ft_atoi(av[5]);
	else
		data->number_of_times_each_philosopher_must_eat = -1;

	data->time_to_eat_mcs = ft_atoi(av[3]) * 1000;
	data->time_to_sleep_mcs = ft_atoi(av[4]) * 1000;
	data->time_to_die_ms = ft_atoi(av[2]);

	// forks_mutexはmainで作成済

	if (pthread_mutex_init(&data->stop_flag_mutex, NULL))
		return 1; //失敗
	data->stop_flag_mutex_succ = 1;
	if (pthread_mutex_init(&data->log_mutex, NULL))
		return 1; //失敗
	data->log_mutex_succ = 1;

	// 範囲チェック
	if (data->number_of_philosophers < 1 \
		|| (data->number_of_times_each_philosopher_must_eat == 0 || data->number_of_times_each_philosopher_must_eat < -1) \
		|| data->time_to_die_ms < 1 || data->time_to_eat_mcs < 1 || data->time_to_sleep_mcs < 1 \
		|| (ft_atoi(av[3]) >= 1000 || ft_atoi(av[4]) >= 1000)) // usleep
			return 1;

	return 0;
}

int init_philo(t_philo *philos, t_data *data)
{
	int i = 0;

	// 構造体 初期化
	while (i < data->number_of_philosophers)
	{
		philos[i].x = i;

		if (pthread_mutex_init(&philos[i].meal_mutex, NULL))
			return 1; //失敗
		philos[i].meal_mutex_succ = 1;

		philos[i].eat_count = 0;

		long long now = get_time_ms();
		philos[i].last_eat_time = now;

		// 自分の左右forkの番号を教える
		philos[i].fork_left = &data->forks_mutex[i];
		philos[i].fork_right = &data->forks_mutex[(i + 1) % data->number_of_philosophers];
		philos[i].data = data;
		i++;
	}
	return 0;
}

int run_threads(t_philo *philos, pthread_t *threads, t_data *data)
{
	// スレッド 開始
	int i = 0;
	while (i < data->number_of_philosophers)
	{
		if (pthread_create(&threads[i], NULL, philosopher_routine, &philos[i]))
		{ //失敗
			pthread_mutex_lock(&data->stop_flag_mutex);
			data->stop_flag = 1;
			pthread_mutex_unlock(&data->stop_flag_mutex);
			while (--i >= 0)
				pthread_join(threads[i], NULL);
			return 1;
		}
		i++;
	}
	return 0;
}

void log_print(t_philo *philo, char *msg)
{
	// time stamp
	long long now = get_time_ms();
	long long timestamp = now - philo->data->start_time;

	pthread_mutex_lock(&philo->data->log_mutex);
	if (!philo->data->stop_flag)
          printf("%lld %zu %s\n", timestamp, philo->x, msg);
	pthread_mutex_unlock(&philo->data->log_mutex);
}

int die_check(t_philo *philos, int i)
{
	// 現在時刻取得
	long long now = get_time_ms();

	// 餓死していたら
	pthread_mutex_lock(&philos[i].meal_mutex);
	long long let_tmp = philos[i].last_eat_time;
	pthread_mutex_unlock(&philos[i].meal_mutex);

	if (now - let_tmp > philos[i].data->time_to_die_ms)
	{
		pthread_mutex_lock(&philos[i].data->stop_flag_mutex);
		philos[i].data->stop_flag = 1;
		log_print(&philos[i], "died");
		pthread_mutex_unlock(&philos[i].data->stop_flag_mutex);

		return 1; //正常に中断
	}
	return 0; //正常に継続
}

int monitor_loop(t_philo *philos, t_data *data)
{
	while (1) // 無限監視
	{
		int i = 0;
		int full_eat_people;

		full_eat_people = 0;

		while (i < data->number_of_philosophers) // 全員チェック
		{
			// 餓死 チェック
			if (die_check(philos, i))
				return 0;

			// みんな食べきったかチェック
			if (data->number_of_times_each_philosopher_must_eat > -1)
			{
				pthread_mutex_lock(&philos[i].meal_mutex);
				int curr_eat_count = philos[i].eat_count;
				pthread_mutex_unlock(&philos[i].meal_mutex);

				if (curr_eat_count < data->number_of_times_each_philosopher_must_eat)
					full_eat_people = 0;
			}

			i++;
		}

		if (full_eat_people)
		{
			pthread_mutex_lock(&data->stop_flag_mutex);
			data->stop_flag = 1;
			pthread_mutex_unlock(&data->stop_flag_mutex);
			return 0;
		}

		usleep(1000);
	}

	return 0;
}


void *philosopher_routine(void *arg)
{
	t_philo *philo = (t_philo *)arg;
	int now_flag = 0;

	// 哲学者 一人ごと
	while (!now_flag)
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

		// いただきます
		log_print(philo, "is eating");
		if (usleep(philo->data->time_to_eat_mcs) < 0)
			return (NULL); //失敗
		// 食事関連 情報登録
		pthread_mutex_lock(&philo->meal_mutex);
		philo->last_eat_time = get_time_ms();
		philo->eat_count++;
		pthread_mutex_unlock(&philo->meal_mutex);

		// ごちそうさま
		pthread_mutex_unlock(philo->fork_left);
		pthread_mutex_unlock(philo->fork_right);

		// 睡眠
		log_print(philo, "is sleeping");
		if (usleep(philo->data->time_to_sleep_mcs) < 0)
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

int forks_generate(t_data *data)
{
	data->forks_mutex = malloc(sizeof(pthread_mutex_t) * (data->number_of_philosophers)); // フォークメモリ確保
	if (!data->forks_mutex)
		return 1;
	int i = 0;
	while (i < data->number_of_philosophers) // フォーク生成
	{
		if (pthread_mutex_init(&data->forks_mutex[i], NULL))
		{ //失敗
			while (--i >= 0)
				pthread_mutex_destroy(&data->forks_mutex[i]);
			return 1;
		}
		i++;
	}
	data->forks_mutex_succ = 1;
	return 0;
}

// size_t number_of_philosophers, time_t time_to_die, time_t time_to_eat, time_t time_to_sleep, size_t number_of_times_each_philosopher_must_eat
int main(int ac, char **av)
{
	t_data   data;
	t_philo *philos;

	if (ac != 5 && ac != 6) // arg filter
		return 1;

	data.number_of_philosophers = ft_atoi(av[1]);

	philos = malloc(sizeof(t_philo) * (data.number_of_philosophers)); // 哲学者は構造体の配列だから、メモリ確保
	// ヌル終端不要。構造体配列は個数を変数で管理する
	if (!philos)
		return 1;

	if (forks_generate(&data)) // フォーク生成
	{ //失敗
		free(philos);
		return 1;
	}

	if (init_data(&data, av, ac)) // 引数をdata構造体に
	{ //失敗
		philo_error_util(&philos, &data);
		return 1;
	}

	pthread_t *threads = malloc(sizeof(pthread_t) * data.number_of_philosophers); // スレッドのメモリを保存する、pthread_t型の配列を人数分作成
	if (!threads)
	{ //失敗
		philo_error_util(&philos, &data);
		return 1;
	}

	if (init_philo(philos, &data)) // 哲学者全員分 初期化
	{ //失敗
		free(threads);
		philo_error_util(&philos, &data);
		return 1;
	}

	// log 基準時間
	data.start_time = get_time_ms();

	if (run_threads(philos, threads, &data)) // スレッド 開始
	{ //失敗
		free(threads);
		philo_error_util(&philos, &data);
		return 1;
	}

	monitor_loop(philos, &data); // 死亡/終了を監視

	int i = 0; // 全員が安全に終了したことを保証 (stop_flagだが、usleepなどで各哲学者return にタイムラグ)
	while (i < data.number_of_philosophers)
	{
		pthread_join(threads[i], NULL); // 失敗は「無効なスレッドID」や「既にjoin済み」の場合
		i++;
	}

	// 終了時処理
	mutexes_destroy(&data, philos); // destroy が失敗するのは「まだロックされている mutexを壊そうとした」等のバグ。正常な流れ（全スレッド終了後）では起きない
	free(data.forks_mutex);
	free(philos);
	free(threads);

	return 0;
}
