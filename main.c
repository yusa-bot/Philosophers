#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

// 構造体 形式定義
typedef struct s_data
{
	int stop_flag;
	size_t number_of_philosophers;

	size_t number_of_times_each_philosopher_must_eat;

	int time_to_eat_ms;
	int time_to_sleep_ms;
	int time_to_die_ms;

	pthread_mutex_t *forks_mutex;
	pthread_mutex_t stop_flag_mutex;
	pthread_mutex_t log_mutex;
}	t_data;

typedef struct s_philo
{
	size_t x;

	size_t eat_count;
	long long last_eat_time;

	pthread_mutex_t *fork_left;
	pthread_mutex_t *fork_right;

	t_data *data;

	pthread_t *threads;
}	t_philo;

////////////////////////////////////

void philo_error_util(t_philo **philo, t_data data)
{
	int i;
	free(data->forks_mutex);
	free(*philo->threads);

	mutexes_destroy(data);

	free(*philo);
	return ;
}

long long get_time_ms(void)
{
	struct timeval tv;

	if (gettimeofday(tv, NULL) < 0)
		return 1; //失敗
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
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

	data->time_to_die_m = ft_atoi(av[2]) * 1000;
	data->time_to_eat_ms = ft_atoi(av[3]) * 1000;;
	data->time_to_sleep_ms = ft_atoi(av[4]) * 1000;;

	// forks_mutexはmainで作成済

	if (pthread_mutex_init(&data->stop_flag_mutex, NULL))
		return 1; //失敗
	if (pthread_mutex_init(&data->log_mutex, NULL))
		return 1; //失敗

	// 範囲チェック
	if (data->number_of_philosophers < 1 \
		|| (data->number_of_times_each_philosopher_must_eat == 0 || data->number_of_times_each_philosopher_must_eat < -1) \
		|| data->time_to_die_m < 1 || data->time_to_eat_ms < 1 || data->time_to_sleep_ms < 1 \
		|| (ft_atoi(av[3]) >= 1000 || ft_atoi(av[4]) >= 1000)) // usleep
			return 1;

	return 0;
}

int init_philo(t_philo *philo, t_data data)
{
	int i = 0;

	// 構造体 初期化
	while (i <= philo[i].data->number_of_philosophers)
	{
		philo[i].x = i;
		philo[i].eat_count = 0;

		long long now = get_time_ms();
		if (now < 0)
			return 1;
		philo[i].last_eat_time = now;

		// 自分の左右forkの番号を教える
		philo[i].fork_left = &data->forks_mutex[i];
		philo[i].fork_right = &data->forks_mutex[(i+1) % data->number_of_philosophers];
		philo[i].data = &data;
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
		{ //失敗
			while (i >= 0)
			{
				philo->data->stop_flag = 1;
				if (pthread_join(philo.threads[i]), NULL)
					return 1 //失敗
				i--;
			}
			return 1;
		}
		i++;
	}
	return 0;
}

int die_check(t_philo *philo)
{
	// 現在時刻取得
	long long now = get_time_ms();
	if (now < 0)
		return 1;
	// 餓死していたら
	if (now - philo[i].last_eat_time > philo[i].data->time_to_die)
	{
		pthread_mutex_lock(&philo[i].data->stop_flag);
		philo[i].data->stop_flag = 1;
		pthread_mutex_unlock(&philo[i].data->stop_flag);
		return ;
	}
}

int monitor_loop(t_philo *philo)
{
	while (1) // 無限監視
	{
		int i = 0;
		int full_eat_people = 1;
		while (i < philo[i].data->number_of_philosophers) // 全員チェック
		{
			// 餓死 チェック
			if (die_check(philo))
				return 1;

			// みんな食べきったかチェック
			if (philo[i].data->number_of_times_each_philosopher_must_eat > -1)
			{
				if (philo[i].eat_count < philo[i].data->number_of_times_each_philosopher_must_eat)
					full_eat_people = 0;
			}

			i++;
		}

		if (full_eat_people)
		{
			pthread_mutex_lock(&philo[i].data->stop_flag);
			philo[i].data->stop_flag = 1;
			pthread_mutex_unlock(&philo[i].data->stop_flag); // 自分がlockしたなら失敗しない。
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
		size_t tmp = (philo->x) % number_of_philosophers;
		if (philo->x % 2) //奇数
		{
			pthread_mutex_lock(philo->fork_left);
			pthread_mutex_lock(philo->fork_right);
		}
		else //偶数
		{
			pthread_mutex_lock(philo->fork_right);
			pthread_mutex_lock(philo->fork_left);
		}

		// いただきます
		if (usleep(philo->data->time_to_eat_ms) < 0)
			return ; //失敗
		// 食事関連 情報登録
		long long tmp = get_time_ms();
		if (tmp < 0)
			return ;
		philo->last_eat_time = tmp;
		philo->eat_count++;

		// ごちそうさま
		pthread_mutex_unlock(philo->fork_left);
		pthread_mutex_unlock(philo->fork_right);

		// 睡眠
		if (usleep(philo->data->time_to_sleep_ms) < 0)
			return ; //失敗

		// 思考

		// stop_flag読み取り
		thread_mutex_lock(&philo[i].data->stop_flag);
		now_flag = philo[i].data->stop_flag;
		pthread_mutex_unlock(&philo[i].data->stop_flag);
	}

	return ;
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
			while (i >= 0)
			{
				pthread_mutex_destroy(&data->forks_mutex[j]);
				i--;
			}
			return 1;
		}
		i++;
	}
}

void mutexes_destroy(t_data *data)
{
	// mutex_destroy処理
	pthread_mutex_destroy(&data->stop_mutex);
	pthread_mutex_destroy(&data->log_mutex);

	i = 0;
	while (i < data->number_of_philosophers)
	{
		pthread_mutex_destroy(&data->forks_mutex[i]);
		i++;
	}
}

// size_t number_of_philosophers, time_t time_to_die, time_t time_to_eat, time_t time_to_sleep, size_t number_of_times_each_philosopher_must_eat
int main(int ac, char **av)
{
	t_data   data;
	t_philo *philo;

	if (ac != 5 && ac != 6) // arg filter
		return 1;

	data.number_of_philosophers = ft_atoi(av[1]);

	philo = malloc(sizeof(t_philo) * (data.number_of_philosophers)); // 哲学者は構造体の配列だから、メモリ確保
	if (!philo)
		return 1;

	if (forks_generate(&data)) // フォーク生成
	{ //失敗
		free(philo);
		return 1;
	}

	if (init_data(&data, av, ac)) // 引数をdata構造体に
	{ //失敗
		philo_error_util(&philo, data);
		return 1
	}

	philo.threads = malloc(sizeof(pthread_t) * (data.number_of_philosophers)); // スレッドのメモリを保存する、pthread_t型の配列を人数分作成
	if (!philo.threads)
	{ //失敗
		philo_error_util(&philo, data);
		return 1
	}

	if (init_philo(philo, &data)) // 哲学者全員分 初期化
	{ //失敗
		philo_error_util(&philo, data);
		return 1
	}

	if (run_threads(philo, &philo.threads)) // スレッド 開始
	{ //失敗
		philo_error_util(&philo, data);
		return 1
	}

	i = 0; // 全員が安全に終了したことを保証 (stop_flagだが、usleepなどで各哲学者return にタイムラグ)
	while (i <= data.number_of_philosophers)
	{
		pthread_join(philo.threads[i], NULL); // 失敗は「無効なスレッドID」や「既にjoin済み」の場合
		i++;
	}

	if (monitor_loop(philo)) // 死亡/終了をグローバルで監視
		return 1;

	// 終了時処理
	mutexes_destroy(&data); // destroy が失敗するのは「まだロックされている mutexを壊そうとした」等のバグ。正常な流れ（全スレッド終了後）では起きない
	free(philos);
	free(philo.threads);

	return 0;
}
