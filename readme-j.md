# 組込用ファームウエアの基本構造

組込用のファームウエアを作る時の基本構造を解説する。

とくに、次をスコープとする。

* 自動化テストを導入する。開発時からテストを導入することでバグを早期に発見しデバッグの手間を減らして開発効率を向上させる。
* RTOSを使う程度の複雑さを対象とする。LinuxのようなOSを前提とはしない。ここで書いたことは、無OSの割り込み駆動の環境でもほとんどそのまま適用できると思う。しかし、ハードウエア制御、ユーザインターフェイス、通信制御などを最低限行わなければならない現代のIoTファームウエアでは、RTOSを導入したほうが合理的だ。ここでは、Cortex-M、CMSIS-RTOS v2、FreeROTSを前提とする。
* 簡単のために、STM32 Nucleoをターゲットボードとするが、任意の同程度の組み込み環境に適用できるはずだ。
* 対象言語はC。開発環境はチップベンダー提供の統合開発環境を前提としよう。統合開発環境ではGUIでペリフェラルを設定し初期化コードやペリフェラルに対するAPIを生成してくれる。ただしここでは、自宅のLinux環境で作業しているので、STM32CubeMXでコード生成した後はVS Codeでコードを書き、ビルドはMakefileによる。


## ツリー構造

* プロジェクトルートの「1つ下」にコード生成する。プロジェクト直下にコード生成しない。
* 自動生成されたツリーとは別に`App`ディレクトリを作り、自分で書いたコードはその下に入れる。ただし、必要最小限で生成コードを編集しても良い。なるべく早く、`App`下のコードに飛んでくる。


```
./                                             プロジェクトルート
├── blinky/                                 コードジェネレータが生成するプロジェクト
│   ├── App/                               自作コーディングの結果はこの下に
│   │   ├── blinky.c
│   │   └── blinky.h
│   ├── Drivers/                           CubeMXが自動生成
│   ├── Inc/                               CubeMXが自動生成
│   ├── Makefile
│   ├── Middlewares/                       CubeMXが自動生成
│   │   └── Third_Party/
│   │       └── FreeRTOS/
│   ├── STM32F103RBTx_FLASH.ld
│   ├── Src/                               CubeMXが自動生成
│   │   ├── freertos.c
│   │   ├── main.c
│   │   ├── stm32f1xx_hal_msp.c
│   │   ├── stm32f1xx_hal_timebase_tim.c
│   │   ├── stm32f1xx_it.c
│   │   └── system_stm32f1xx.c
│   ├── blinky.ioc
│   ├── build/                              ビルドディレクトリ
│   └── startup_stm32f103xb.s
├── datasheet/                               関連データシートをまとめておくと、あとで便利
├── flash.sh
├── readme.md
```


## 複数人での開発



## RTOSの基礎

### システムタイマの設定

STM32の場合は、OSが使うタイマとしてSysTickではなくTIMxを使うように推奨されるので、CubeMXの設定でTIMxをOSが使うように設定する。
基本的には自動で設定してくれる。OSのタイマを適切に使うことで、`osDelay(ms)`のようなディレイ関数がタスク中で使える。`osDelay(ms)`は、引数で指定したミリセコンドだけ待つが、呼び出したスレッドは`BLOCKED`状態にして他に待っているタスクがあれば制御を渡す。

UIなどの遅いタスクは、ベタ書き＆`osDelay`の組み合わせで書いて良い。他のタスクとの並行動作はRTOSが面倒を見てくれる。

```c
void StartUiTask(void *argument)
{
  /* USER CODE BEGIN StartUiTask */
  /* Infinite loop */
  for(;;)
  {
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin,GPIO_PIN_RESET);
    osDelay(1000);
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin,GPIO_PIN_SET);
    osDelay(1000);
  }
  /* USER CODE END StartUiTask */
}

```

### アイドルタスクの設定

組み込みシステムでは消費電力を低減するために、不要な場合はMCUをスリープモードに入れる。RTOSには、一般的にアイドルタスクが、最低優先度のタスクとして定義されていて、他に実行待ちのタスクがない時にアイドルタスクが呼ばれる。つまり、アイドルタスクの中でスリープモードに入れば、不要な時にスリープするようになる。Cortex-Mの場合、スリープに入るインストラクションは`WFI`(wait for interrupt)として知られているがSTM32の場合は、清く正しくAPI(`HAL_PWR_EnterSLEEPMode()`)を使おう。RTOSがOS_TICK（デフォルトでは1ms）ごとにSLEEPから起動して実行しなければならないタスクがあれば実行してくれる。

```c
void StartDefaultTask(void *argument)
{
  for(;;)
  {
    osDelay(1);
    HAL_PWR_EnterSLEEPMode(0, PWR_SLEEPENTRY_WFI);
  }
}
```

## アプリ関数を外部ファイルに移す。

なるべく早い段階で、自動生成するファイルとユーザコードが入るファイルを分けるべきだ。

* CubeMXの場合はタスクのCode Generation Optionに`As external`とすれば、外部ファイルにあるとしてリンクのみ行うことができる。雛形が欲しければ`As weak`とすれば良い。
* `Makefile`で対象ファイルをコンパイル・リンクするようにする。

### タスクの構成

コード生成系との兼ね合いがあるが、次のようなタスク構成は有用だろう。


|task name  |Priority            |CodeGeneration|
|-----------|--------------------|--------------|
|defaultTask|`osPriorityLow`     |Default       |
|AppTask    |`osPriorityNormal`  |As External   |

|timer name |Type                |CodeGeneration|
|-----------|--------------------|--------------|
|RtTimer    |`osTimerPeriodic`   |As External   |


* `defaultTask`
    + Sleep Modeへ入るために`osPriorityLow`とする。
    + すきあらばSleep Modeに入る。
* `AppTask`
    + コード生成が`As External`なので、`main.c`には生成されない。`App/blinly.c`にタスクコールバックを作成する。
    + defaultTaskより高い優先度にする。
    + `#define TEST`ならば、テストルーチンを起動する。
    + `#undef TEST`ならば、アプリルーチンを起動する。
* `StartApp()`
    + `AppTask`のタスクコールバックから呼ばれる普通の関数。
    + タイマ（この想定ではハードウエア制御のための高頻度高精度タイマルーチン）を起動する。
    + 低速応答は`osDelay()`で処理する。
* `StartTest()`
    + `AppTask`のタスクコールバックから呼ばれる普通の関数。
    + テストを実行する。
    + テストがOKならLEDを点灯して無限ループで停止。
    + テストがNGならLEDを消灯して無限ループで停止。

こうすれば、自動生成される`main.c`は`StartDefaultTask()`の中を1行書き換える（`HAL_PWR_EnterSLEEPMode()`を呼ぶ）だけで、他の手書きの修正は`App/`以下に閉じ込めておくことができる。さらに、`main.c`の変更は、`/* USER CODE BEGIN */`で囲まれているので、再生成しても保存される。


`main.c`
```c
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    HAL_PWR_EnterSLEEPMode(0, PWR_SLEEPENTRY_WFI);
  }
  /* USER CODE END 5 */ 
}
```

`App/blinky.c`
```c
void StartAppTask(void *argument)
{
#if TEST
    StartTest();
#else
    StartApp();
#endif // TEST
}

void StartApp(void)
{
  osTimerStart(RtTimerHandle, 10);
  for(;;)
  {
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin,GPIO_PIN_RESET);
    osDelay(1000);
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin,GPIO_PIN_SET);
    osDelay(1000);
  }
}

void StartTest(void *argument)
{
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin,GPIO_PIN_RESET);
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin,GPIO_PIN_SET);
  for(;;)
  {
  }
}

/* RtCallback function */
void RtCallback(void *argument)
{
  /* USER CODE BEGIN RtCallback */
  
  /* USER CODE END RtCallback */
}
```


## テストモードの作成

通常、IDEでプロジェクトを生成すると`Debug`と`Release`のコンフィグレーションが設定されることが多い。そこに`Test`というコンフィグレーションを付け加えて欲しい。設定は次のとおりだ。

|config   |compiler option|`#define`|
|---------|---------------|---------|
|`Release`|optimized      |`NDEBUG` |
|`Debug`  |normal         |         |
|`Test`   |normal         |`TEST`   |

`-DTEST`のときは、アプリケーションのメインルーチンを実行するのではなく、テストルーチンを実行する。

テストルーチンは、テストを順次実行する。

* テストは失敗したら`assert`で、赤LED点灯の無限ループ。基本的に、テストがパスしたことを確認するので、どのテストに失敗したかは、UARTに出力するか、デバッガで追う。
    + つまり、デバッグポートでUARTまたは、SWD経由のITM出力を通じて、`printf`が動くようにしておかなければならない。
* テストが全部成功したら、緑LED点灯の無限ループで終了。


### テスト関数の位置

ユニットテストの関数を、被テスト関数と同じファイルに書くか、別のファイルに書くか、という流儀がある。得失は次のとおり。

* 被テスト関数と同じファイルにテストを書く
    + テストを追加すると、製品コードに変更がなくても、製品コードのファイルのリビジョンが上がる。
    + 同じファイルなので`static`スコープが見える。
* 被テスト関数とテスト関数を別ファイルに書く
    + 管理が楽。とくに、テストを付かしても実行ファイルのリビジョンが上がらない。
    + 別ファイルなので、シェル芸を通じてテスト関数の自動収集＆自動実行がやりやすい。IDE派にはシェル芸は関係ないが。
    + `static`スコープの関数や変数の扱いが面倒。

個人的な意見だが、被テスト関数と同じファイル内に`#ifdef TEST`としてテスト関数を書き、テストランナーからその関数を呼び出したほうが、主に`static`の扱いが簡単なので好みだ。あとは命名規則を工夫して自動化に協力しょう。


