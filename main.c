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
}	t_philo;

////////////////////////////////////

die_check(t_philo *philo)
{
	// 現在時刻取得
	long long now = get_time_ms();
	if (now < 0)
		return ;
	// 餓死していたら
	if (now - philo[i].last_eat_time > philo[i].data->time_to_die)
	{
		if (pthread_mutex_lock(&philo[i].data->stop_flag))
			return ;
		philo[i].data->stop_flag = 1;
		pthread_mutex_unlock(&philo[i].data->stop_flag);
		return ;
	}
}

void monitor_loop(t_philo *philo)
{
	while (1) // 無限監視
	{
		int i = 0;
		int full_eat_people = 1;
		while (i < philo[i].data->number_of_philosophers) // 全員チェック
		{
			// 餓死 チェック
			die_check();

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
			if (pthread_mutex_lock(&philo[i].data->stop_flag))
				return ;
			philo[i].data->stop_flag = 1;
			pthread_mutex_unlock(&philo[i].data->stop_flag); // 自分がlockしたなら失敗しない。
			return ;
		}

		usleep(1000);
	}

	return ;
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
			if (!pthread_mutex_lock(&philo->fork_left))
				return ; //失敗
			if (!pthread_mutex_lock(&philo->fork_right))
			{
				pthread_mutex_unlock(&philo->fork_left)
				return ; //失敗
			}
		}
		else //偶数
		{
			if (!pthread_mutex_lock(&philo->fork_right))
				return ; //失敗
			if (!pthread_mutex_lock(&philo->fork_left))
			{
				pthread_mutex_unlock(&philo->fork_right)
				return ; //失敗
			}
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
		pthread_mutex_unlock(&philo->fork_left);
		pthread_mutex_unlock(&philo->fork_right);

		// 睡眠
		if (usleep(philo->data->time_to_sleep_ms) < 0)
			return ; //失敗

		// 思考

		// stop_flag読み取り
		if (pthread_mutex_lock(&philo[i].data->stop_flag))
			return ; //失敗
		now_flag = philo[i].data->stop_flag;
		pthread_mutex_unlock(&philo[i].data->stop_flag);
	}

	return ;
}

// size_t number_of_philosophers, time_t time_to_die, time_t time_to_eat, time_t time_to_sleep, size_t number_of_times_each_philosopher_must_eat
int main(int ac, char **av)
{
	// arg filter
	if (ac != 5 && ac != 6)
		return 1;

	t_data   data;
	data.number_of_philosophers = ft_atoi(av[1]);

	if (data.number_of_philosophers < 2)
		return 1;

	// 哲学者は構造体の配列だから、メモリ確保
	t_philo *philo;
	t_philo *philo = malloc(sizeof(t_philo) * (data.number_of_philosophers));
	if (!philo)
		return 1;

	// フォークメモリ確保
	data.forks_mutex = malloc(sizeof(pthread_mutex_t) * (data.number_of_philosophers));
	if (!forks_mutex)
		return 1;
	// フォーク生成
	while (i < data.number_of_philosophers)
	{
		// フォーク 生成
		if (pthread_mutex_init(data.forks_mutex[i], NULL))
			return 1; //失敗
		i++;
	}

	// 引数をdata構造体に
	if (init_data(&data, av, ac))
		return 1 //失敗

	// スレッドのメモリを保存する、pthread_t型の配列を人数分作成
	pthread_t *threads = malloc(sizeof(pthread_t) * (data.number_of_philosophers));
	if (!threads)
		return 1;

	// 哲学者全員分 初期化 -> スレッド 開始
	init_philo(philo, &data);
	run_threads(philo, &threads);

	// 死亡/終了をグローバルで監視
	monitor_loop(philo);

	// 全員が安全に終了したことを保証 (stop_flagだが、usleepなどで各哲学者return にタイムラグ)
	i = 0;
	while (i <= data.number_of_philosophers)
	{
		if (pthread_join(threads[i]), NULL)
			return 1 //失敗
		i++;
	}


	// 終了時処理

	// mutex_destroy処理
	if (pthread_mutex_destroy(&data->stop_mutex)) // ロックされていないフォークの破棄
		return 1; //失敗
	if (pthread_mutex_destroy(&data->log_mutex))
		return 1; //失敗
	i = 0;
	while (i < number_of_philosophers)
	{
		if (pthread_mutex_destroy(&data->forks_mutex[i]))
			return 1; //失敗
		i++;
	}

	// free処理
	free(philos);
	free(threads);

	return 0;
}