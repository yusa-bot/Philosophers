*このプロジェクトは、42のカリキュラムの一環として、ayusaによって作成されました。*

# 説明

食事する哲学者問題（Dining Philosophers Problem）を、POSIXスレッドとミューテックスを用いて解決するプログラム。

哲学者たちが円卓に座り、食事・思考・睡眠を繰り返す。各哲学者の左右にフォークが1本ずつ置かれており、食事には両側のフォークが必要。哲学者は互いにコミュニケーションを取らず、誰かが餓死するとシミュレーションが終了する。

**ルール：**
- 各哲学者は独立したスレッドとして動作する
- フォークはミューテックスで保護され、同時アクセスを防ぐ
- 哲学者は食事・思考・睡眠のいずれか1つの状態のみを取る
- `time_to_die` ミリ秒以内に食事が開始されなければ哲学者は死亡する
- 全員が `number_of_times_each_philosopher_must_eat` 回食事を終えるとシミュレーション終了（指定時）
- データ競合（Data Race）が発生してはならない

**ログ出力形式：**
```
timestamp_in_ms X has taken a fork
timestamp_in_ms X is eating
timestamp_in_ms X is sleeping
timestamp_in_ms X is thinking
timestamp_in_ms X died
```

# 手順

**コンパイル：**
```
cd philo
make
```

**実行：**
```
./philo number_of_philosophers time_to_die time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat]
```

**引数：**
| 引数 | 説明 |
|------|------|
| `number_of_philosophers` | 哲学者の人数（= フォークの本数） |
| `time_to_die` | 最後の食事開始から次の食事開始までの制限時間（ミリ秒）。超過すると死亡 |
| `time_to_eat` | 1回の食事にかかる時間（ミリ秒） |
| `time_to_sleep` | 1回の睡眠にかかる時間（ミリ秒） |
| `number_of_times_each_philosopher_must_eat` | 全員がこの回数食べるとシミュレーション終了（省略可） |

**実行例：**
```
./philo 5 800 200 200
./philo 4 410 200 200
./philo 5 800 200 200 7
./philo 1 800 200 200
```

**後片付け：**
```
make fclean
```

# リソース

- [「(outdated) philosopherとは何だったのか」 - Qiita](https://qiita.com/42yliu/items/86d16cdbc584c250ca6e)
- [食事する哲学者の問題 - Wikipedia](https://ja.wikipedia.org/wiki/%E9%A3%9F%E4%BA%8B%E3%81%99%E3%82%8B%E5%93%B2%E5%AD%A6%E8%80%85%E3%81%AE%E5%95%8F%E9%A1%8C)
- [pthread - Linux man pages](https://man7.org/linux/man-pages/man7/pthreads.7.html)
- [gettimeofday(2) - Linux man pages](https://man7.org/linux/man-pages/man2/gettimeofday.2.html)

## AI使用法

本プロジェクトでは、以下のタスクにAI（Claude code, gemini）を使用した。

- 各関数（`pthread_*`、`gettimeofday`、`printf`）のmanページ解説と動作特性の説明
- ログ出力と関数の組み合わせによる死亡検出問題の原因分析
- `time_to_die`以内に死亡メッセージを出力するための精度改善手法の提案
- `ft_usleep`の仕様設計とビジーウェイト手法の解説
- `printf`・`gettimeofday`の特性と精度改善手法の解説
- 関数の命名規則とファイル分割設計の提案
- ピアレビューを想定した事前レビュー（norminette検査・動作テスト）
- コマンドライン引数のバリデーション設計の提案
- エラー分岐処理の必要性と実装方針の解説
- 本ファイルの検閲