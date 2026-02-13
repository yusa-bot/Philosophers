
int main(size_t number_of_philosophers, time_t ime_to_die, time_t time_to_die, time_t time_to_sleep, size_t number_of_times_each_philosopher_must_eat)
{
	// 哲学者
	pthread_t *X_arr =  malloc(sizeof(pthread_t) * (number_of_philosophers + 1));
	if (!X_arr)
		return 1;
	X_arr[number_of_philosophers] = '\0';

	// フォーク
	pthread_mutex_t *fork_X_arr = malloc(sizeof(pthread_mutex_t) * (number_of_philosophers + 1));
	if (!X_arr)
		return 1;
	X_arr[number_of_philosophers] = '\0';

	int i = 0;
	// 哲学者, フォーク 初期化
	while (i < number_of_philosophers)
	{
		// 哲学者 生成
		if (pthread_create(X_arr[i], NULL, void *(*start_routine)(void *), void *arg))
			return 1; //失敗
		if (pthread_join(X_arr[i], NULL))
			return 1; //失敗

		// フォーク 生成
		if (pthread_mutex_init(fork_X_arr[i], NULL))
			return 1; //失敗

		// 食事回数カウンター
		//format
		size_t x'i'_eat_count = 0;
		i++;
	}


	size_t number_of_times_each_philosopher_eat = 0;

	// 食事
	if (!pthread_mutex_lock(fork_X_arr[0])) // フォーク0 lock成功
	{
		if (!pthread_mutex_lock(fork_X_arr[1])) // フォーク2 lock成功
			return 1; //失敗

		// いただきます
		if (usleep(time_to_die) < 0)
			return 1; //失敗

		// ごちそうさま
		if (pthread_mutex_unlock(fork_X_arr[0])) // フォーク0 unlock成功
			return 1; //失敗
		if (pthread_mutex_unlock(fork_X_arr[1])) // フォーク1 unlock成功
			return 1; //失敗

		//  死亡時間カウント　スタート
		if (gettimeofday(tv, NULL) < 0)
			return 1; //失敗
		time_t to_die = tv->tv_sec * 1000 + tv->tv_usec / 1000;
	}
	else // lock失敗
		既に他スレッドがロックしている場合、解放されるまでブロックする。

	// 睡眠
	if (usleep(time_to_die) < 0)
		return 1; //失敗

	// 思考
	if (usleep(time_to_die) < 0)
		return 1; //失敗


	if (number_of_times_each_philosopher_must_eat == number_of_times_each_philosopher_eat)
	{
		// 終了時処理
		if (pthread_mutex_destroy(pthread_mutex_t *mutex)) // ロックされていないフォークの破棄
			return 1; //失敗
		free(X_arr);
		return 0;
	}

}