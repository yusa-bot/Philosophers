#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

// 構造体 形式定義
typedef struct s_data
{
	bool stop_flag;
	size_t number_of_philosophers;

	size_t number_of_times_each_philosopher_must_eat;
	size_t full_eat_people_count;

	time_t time_to_eat_m;
	time_t time_to_sleep_m;
	time_t time_to_die_m;

	// pthread_mutex_t forks_mutex;
	pthread_mutex_t stop_flag_mutex;
}	t_data;

typedef struct s_philo
{
	size_t x;

	size_t eat_count;
	time_t last_eat_time;

	pthread_mutex_t fork_0;
	pthread_mutex_t fork_1;
}	t_philo;

////////////////////////////////////

void init_data(t_data data, size_t number_of_philosophers, time_t time_to_die, time_t time_to_eat, time_t time_to_sleep, size_t number_of_times_each_philosopher_must_eat)
{
	data->stop_flag = 0;
	data->number_of_philosophers = number_of_philosophers;

	data->number_of_times_each_philosopher_must_eat = number_of_times_each_philosopher_must_eat;
	data->full_eat_people_count = 0;

	time_t time_to_eat_m = time_to_eat * 1000;
	data->time_to_eat_m = time_to_eat_m;

	time_t time_to_sleep_m = time_to_sleep * 1000;
	data->time_to_sleep_m = time_to_sleep_m;

	time_t time_to_die_m = time_to_die * 1000;
	data->time_to_die_m = time_to_die_m;

	data->stop_flag_mutex = pthread_mutex_init();
	return ;
}

void init_philo(t_philo philo, size_t i)
{
	philo->x = i;
	philo->eat_count = 0;
	philo->last_eat_time = 0;

	philo->fork_0 = pthread_mutex_init();
	philo->fork_0 = pthread_mutex_init();
}

void monitor_loop(t_philo struct_philo, t_data data)
{
	while (1)
	{
		int i = 0;
		while (i < number_of_philosophers)
		{
			// 現在時刻取得
			if (gettimeofday(tv, NULL) < 0)
				return 1; //失敗
			time_t to_die_m = tv->tv_sec * 1000 + tv->tv_usec / 1000;
			// 死んでいたら
			if (to_die_m - philos[i].last_eat_time > data->time_to_die)
			{
				pthread_mutex_lock(data->stop_flag);
				data->stop_flag = 1;
				pthread_mutex_unlock(data->stop_flag);
				return ;
			}

			if (data->full_eat_people_count == data->number_of_times_each_philosopher_must_eat)
			{
				pthread_mutex_lock(data->stop_flag);
				data->stop_flag = 1;
				pthread_mutex_unlock(data->stop_flag);
				return ;
			}

			// usleep(1000);
		}
	}
}


void philosopher_routine(t_philo philo)
{
	// 哲学者 一人ごと
	i = 0;
	while (!data->stop_flag)
	{
		size_t tmp = (i+1) % number_of_philosophers;
		if (i % 2) //奇数
		{
			if (!pthread_mutex_lock(fork_X_arr[tmp])); // フォークtmp lock成功
				return 1; //失敗
			if (!pthread_mutex_lock(fork_X_arr[i])); // フォークx lock成功
				return 1; //失敗
		}
		else //偶数
		{
			if (!pthread_mutex_lock(fork_X_arr[i])); // フォークx lock成功
				return 1; //失敗
			if (!pthread_mutex_lock(fork_X_arr[tmp])); // フォークtmp lock成功
				return 1; //失敗
		}

		// いただきます
		time_t time_to_eat_m = time_to_eat * 1000;
		if (usleep(time_to_eat_m) < 0)
			return 1; //失敗

		// ごちそうさま
		if (pthread_mutex_unlock(fork_X_arr[tmp])) // フォーク0 unlock成功
			return 1; //失敗
		if (pthread_mutex_unlock(fork_X_arr[i])) // フォーク1 unlock成功
			return 1; //失敗

		//  死亡時間カウント　スタート
		if (gettimeofday(tv, NULL) < 0)
			return 1; //失敗
		time_t to_die_m = tv->tv_sec * 1000 + tv->tv_usec / 1000;

		// 睡眠
		time_t time_to_sleep_m = time_to_sleep * 1000;
		if (usleep(time_to_sleep_m) < 0)
			return 1; //失敗

		// 思考

		i++;
	}
}


int main(size_t number_of_philosophers, time_t time_to_die, time_t time_to_eat, time_t time_to_sleep, size_t number_of_times_each_philosopher_must_eat)
{
	// arg filter
	if (time_to_eat >= 1000 || time_to_sleep >= 1000)
		return 1; // usleep

	// 哲学者メモリ確保
	t_philo *philos =  malloc(sizeof(t_philo) * (number_of_philosophers + 1));
	if (!philos)
		return 1;
	philos[number_of_philosophers] = '\0';
	// 構造体定義
	int i = 0;
	while (i <= number_of_philosophers)
	{
		init_philo(philos[i], i, );
		i++;
	}

	// フォークメモリ確保
	pthread_mutex_t *fork_X_arr = malloc(sizeof(pthread_mutex_t) * (number_of_philosophers + 1));
	if (!X_arr)
		return 1;
	X_arr[number_of_philosophers] = '\0';

	// 構造体定義
	t_data   *data;
	init_data(data, number_of_philosophers, time_to_die, time_to_eat, time_to_sleep, number_of_times_each_philosopher_must_eat);

	int i = 0;
	// 哲学者, フォーク 初期化
	while (i < number_of_philosophers)
	{
		// 哲学者 生成
		if (pthread_create(X_arr[i], NULL, philosopher_routine, &philos[i]))
			return 1; //失敗
		if (pthread_join(X_arr[i], NULL))
			return 1; //失敗

		// フォーク 生成
		if (pthread_mutex_init(fork_X_arr[i], NULL))
			return 1; //失敗
	}

	// 死亡/終了をグローバルで監視
	monitor_loop(struct_philo, data);

	// 哲学者 一人ごとの生活
	philosopher_routine();

	// スレッドが途中で強制終了しないようにメインを待たせる。
	pthread_join();
	// 人数分join

	// 終了時処理
	if (pthread_mutex_destroy(pthread_mutex_t *mutex)) // ロックされていないフォークの破棄
		return 1; //失敗

	i = 0;
	while ()
	{
		free(X_arr[i]);
		i++;
	}
	free(X_arr);

	return 0;
}