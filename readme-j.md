# 組込用ファームウエアの基本構造

組込用のファームウエアを作る時の基本構造を解説する。

とくに、次をスコープとする。

* 自動化テストを導入する。開発時からテストを導入することでバグを早期に発見しデバッグの手間を減らして開発効率を向上させる。
* MCUの規模としては、コントローラレベル。つまり、Cortex-M0〜M3ぐらい。とくに演算が必要な場合はM7でも良いが制御の複雑さとしてはM3で足りる程度を想定。
* RTOSを使う程度の複雑さを対象とする。LinuxのようなOSを前提とはしない。ここで書いたことは、無OSの割り込み駆動の環境でもほとんどそのまま適用できると思う。しかし、ハードウエア制御、ユーザインターフェイス、通信制御などを最低限行わなければならない現代のIoTファームウエアでは、RTOSを導入したほうが合理的だ。ここでは、Cortex-M、CMSIS-RTOS v2、FreeROTSを前提とする。
* 簡単のために、STM32 Nucleoをターゲットボードとするが、任意の同程度の組み込み環境に適用できるはずだ。
* 対象言語はC。開発環境はチップベンダー提供の統合開発環境を前提としよう。統合開発環境ではGUIでペリフェラルを設定し初期化コードやペリフェラルに対するAPIを生成してくれる。ただしここでは、自宅のLinux環境で作業しているので、STM32CubeMXでコード生成した後はVS Codeでコードを書き、ビルドはMakefileによる。


## ツリー構造

最近のMCUはベンダーからIDEや設定ツールが提供されていて、ペリフェラルの設定をGUIで行い、コード生成してくれる環境が多い。そういった環境では、自動生成されたコードと、開発者が手書きで書いたコードを、いかに分離するかがポイントになる。

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
├── flash.sh                                 OpenOCDを使ってフラッシュするスクリプト。
├── readme.md
```


## 複数人での開発


### コードフォーマット

`clang-format`を使い、`.clang-format`をレポジトリに登録する。すべててclang-formatでフォーマットしてからコミットする。適切なエディタを使っていれば、簡単なフォーマットで整形できるだろう。

どのようなフォーマットにするかは、時代や好みもあるので、ここでは議論しないが、手でフォーマットしないことが大切。議論が生じるようなら、その結果を`.clang-format`に反映させる。

それと同様に`.editorconfig`もリポジトリに登録しておこう。ほとんどの近代的なエディタは対応しているはずだ。

## RTOSは必要か？

簡単な組み込み開発ではRTOSを使うか使わないか、という選択が生じる。当然、使わずに済むならそれでよい。ではどのような時に使いたいと思うのか。

私の経験では、**複数の応答速度（とくに非同期）の処理が同時に必要な時はRTOSを使うのが合理的**だと感じている。たとえば、ADCで信号をサンプリングしつつ、信号処理して無線で送信する場合。ADCのサンプリング、信号処理のタイミング、無線モジュールとの通信タイミングが異なることが多い。とくにWiFiやBTのプロトコルを同じMCU内で処理する時は、応答のレスポンスを待ったりするので、通信の1つの処理がADCのサンプリングで完結しない。シリアル通信でデータを入力されて、それを加工し送信する場合も、入力が非同期の場合は似たような状況になる。
そういった場合はRTOSを使わなかったとしても、結局自前でタスクを分割して並行に流れるようにコントロールすることになるのだ。

今回はRTOSとしてFreeRTOSを使う想定だが、ARM Cortex-Mの場合はCMSIS-RTOSとして（v1とv2がある）RTOSのAPIが標準化されているので、他のRTOS（たとえばRTXなど）でも同じようになるだろう。

* [https://docs.aws.amazon.com/ja_jp/freertos-kernel/latest/dg/freertos-kernel-dg.pdf](https://docs.aws.amazon.com/ja_jp/freertos-kernel/latest/dg/freertos-kernel-dg.pdf)
* [http://www.keil.com/pack/doc/CMSIS/RTOS2/html/index.html](http://www.keil.com/pack/doc/CMSIS/RTOS2/html/index.html)

非常にタイミングクリティカルな場合はRTOSと割り込みを組み合わせて使うこともできる。その場合は、割り込みコンテキストか通常コンテキストかを意識して書き分けなければならない。

### システムタイマの設定

STM32の場合は、OSが使うタイマとして`SysTick`ではなく`TIMx`を使うように推奨されるので、CubeMXの設定で`TIMx`をシステムタイマとして使うように設定する。`System Core`→`SYS`→`Timebase Source`で、なるべく他で使わなさそうなタイマを選べば良い。これは、HALのタイムベース（`HAL_GetTick()`など）のタイムベースとして`SysTick`ではなく指定したタイマを使うということ。RTOSの方で`SysTick`を専有するためにHALとの衝突を避けるための処置である。

OSのタイマを適切に使うことで、`osDelay(ms)`のようなディレイ関数がタスク中で使える。`osDelay(ms)`は、引数で指定したミリセコンドだけ待つが、呼び出したスレッドは`BLOCKED`状態にして他に待っているタスクがあれば制御を渡す。また、タイマタスクなどの定期的な実行も可能になる。

UIなどの遅いタスクは、ベタ書き＆`osDelay()`の組み合わせで書いて良い。他のタスクとの並行動作はRTOSが面倒を見てくれる。

サンプリングタスクなどの応答が重要なタスクはタイマタスクに設定する。

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

組み込みシステムでは消費電力を低減するために、不要な場合はMCUをスリープモードに入れる。RTOSには、一般的にアイドルタスクが最低優先度のタスクとして定義されていて、他に実行待ちのタスクがない時にアイドルタスクが呼ばれる。つまり、アイドルタスクの中でスリープモードに入れば不要な時はスリープするようになる。

Cortex-Mの場合、スリープに入るインストラクションは`WFI`(wait for interrupt)として知られている。STM32の場合は、清く正しくAPI(`HAL_PWR_EnterSLEEPMode()`)を使おう。RTOSがOS_TICK（デフォルトでは1ms）ごとにSLEEPから起動して実行しなければならないタスクがあれば実行してくれる。

CubeMXであれば、`USE_IDLE_HOOK`を`Enable`にしておくと、アイドルタスクが呼ばれた時にコールバックされるフック関数`vApplicationIdleHook(void)`
を生成してくれる。このアイドルフックはFreeRTOSの機能なので、命名規則がFreeRTOS風になっている。自動生成側はweak symbolになっているので、そのまま上書き定義すればよい。

```c
void vApplicationIdleHook( void )
{
    HAL_PWR_EnterSLEEPMode(0, PWR_SLEEPENTRY_WFI);
}
```

### タイマタスクの設定

タイマタスクは周期的に実行するようなタスクのために定義される。実際には、暗黙に生成されるタイマデーモンから、定期的に指定したタスクが呼び出される。タイマタスクは、呼び出されるごとにタイマ制約時間内で上から下まで実行して終了するような関数として記述する。できる限り短時間で終了するように書かれなければならない。また、自身の実行をブロックし他のタスクに実行権を移す`osDelay()`のような関数は使えない。

今考えているアプリケーションでは、メインのタスクを邪魔しないように空き時間に実行するタスクではなく、タイマタスクはサンプリングなどのハードリアルタイム処理である。初期値はタイマタスクのプライオリティが低くなっているが、`osPriorityRealtime`と同じぐらいの48に修正しておこう。


### タスクの構成

コード生成系との兼ね合いがあるが、次のようなタスク構成は有用だろう。きちんと、このへんの設計をしようと思えば、使っている自動生成のツールのクセ、およびコンパイラの`#include`の処理、リンカの振る舞いについてきちんと知っておかなければならない。さもないと、分離をしようとおもったにもかかわらず複雑な依存関係を作りこんでしまうこともある。

* CubeMXの場合はタスクのCode Generation Optionに`As external`とすれば、外部ファイルにあるとしてリンクのみ行うことができる。雛形が欲しければ`As weak`とすれば良い。
* `Makefile`で対象ファイルをコンパイル・リンクするようにする。IDEを使っている場合はビルドツリーに組み込む。


|task name  |Priority            |CodeGeneration|EntryPoint        |
|-----------|--------------------|--------------|------------------|
|defaultTask|`osPriorityNormal`  |As Weak       |`StartDefaultTask`|

|timer name |Type                |CodeGeneration|Callback |
|-----------|--------------------|--------------|---------|
|RtTimer    |`osTimerPeriodic`   |As External   |`RtTimer`|

* `StartDefaultTask`
    + `main.c`のweak symbolを`App/blinky.c`の中で上書きするように定義する。
    + 通常のコンテキストでの実行。
    + アプリ、テストなどのタスクを条件に応じて起動する。
        - `#define TEST`ならば、テストルーチンを起動する。
        - `#undef TEST`ならば、アプリルーチンを起動する。
    + タイマタスク以外のタスクは、関数としては終了してはならない。無限ループとなるように書く。
* `StartApp()`
    + `StartDefaultTask`から呼ばれるアプリ本体。
    + タイマ（この想定ではハードウエア制御のための高頻度高精度タイマルーチン）を起動する。
    + 低速応答は`osDelay()`で処理する。
    + アプリケーションのメインのタスクは、初期化をしたあと無限ループに入る。この関数は終了してはならない。
    + 無限ループの中で適切に他のタスクに実行権を譲るように書く。
* `StartTest()`
    + `AppTask`のタスクコールバックから呼ばれる普通の関数。
    + テストを実行する。
    + テストがOKならLEDを点灯して無限ループで停止。
    + テストがNGならLEDを消灯して無限ループで停止。
* `RtTimer`
    + サンプリングタスクなどハードリアルタイムなタスクは、タイマータスクとして実装する。
    + タイマータスクは明示的なスタートが必要。
    + タイマが発火したら初期化時に指定したコールバック関数が呼ばれるだけなので実装は楽。
    + 他スレッドとの情報共有は、グローバル変数かキューが使われる。
        - グローバル変数の場合は、書き込み、読み出しにタイミングによるレースコンディションに注意。読み書きの同期を図るために、適切にセマフォを使うことが必要になるだろう。
        - キューの場合はRTOSのAPIで生成する。
* IdleTask
    + 明示的には生成されないが、暗黙に生成される。
    + アイドル時はスリープに入るようにしておくと消費電流を節約できる（上述）。

こうすれば、自動生成と相性がよく、再生成を問題なく実行でき、ユーザ作成アプリ（`App/`以下）を分離できる。

`App/blinky.c`
```c
void StartDefaultTask(void *argument)
{
#if TEST
    StartTest();
#elif HWTEST
    StartHwTest()
#else
    StartApp();
#endif // TEST
   assert(0);
}

void uart_putchar(char c) { HAL_UART_Transmit(&huart2, (uint8_t *)&c, 1, 1); }

static void StartApp(void)
{
    int i = 0;
    osStatus_t ret;
    xputchar = uart_putchar;

    if (!RtTimerHandle) {
        xprintf("timer creation failuer.\r\n");
        assert(0);
    }

    ret = osTimerStart(RtTimerHandle, 100);
    if (ret != osOK) {
        xprintf("timer start failuer.\r\n");
        assert(0);
    }

    for (;;) {
        osDelay(100);
        i++;
        xprintf("d:%d x:%x\r\n", i, i, i);
    }
}

/* RtCallback function */
void RtCallback(void *argument)
{
    static uint32_t i;
    static bool led;
    if (i % 20 == 0) {
        if (led) {
            HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
            led = false;
        } else {
            HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
            led = true;
        }
    }
    i++;  
}
void vApplicationIdleHook(void)
{
    HAL_PWR_EnterSLEEPMode(0, PWR_SLEEPENTRY_WFI);
}
```

#### Early Exit

上の例において`StartApp()`でタイマタスクを起動するところがEarly Exitになっていることにも気づいて欲しい。
次の例は、深い入れ子の（バッド）パターンだ。

```c
static status StartApp(void)
{
    int i = 0;
    osStatus_t ret;
    xputchar = uart_putchar;

    if (RtTimerHandle != 0) {
        ret = osTimerStart(RtTimerHandle, 100);
        if (ret == osOK) {
            for (;;) {
                osDelay(100);
                i++;
                xprintf("d:%d x:%x\r\n", i, i, i);
            }
        } else {
            xprintf("timer start failuer.\r\n");
            return -1;
        }
    } else {
        xprintf("timer creation failuer.\r\n");
        return -1;
    }
}
```


## FWテストのスコープ

組み込みのテストにおいて、ときどき議論となるのがペリフェラル（MCU内蔵、および外付け）の取り扱いだ。たとえば、センサからのデータをADCで取り込み、ノイズ除去をして、UART経由で無線モジュールに送信し、サーバにデータを蓄積するというシステムを考えよう。この場合、自動化されたFWのユニットテスト的なものはどうなるだろうか？

* ADCでのデータ取り込み：入力電圧に応じた変換結果がえられる。
* ノイズ除去：入力データからノイズを除去した出力データがえられる。これは、入力データを与えて出力データを期待値と比較すれば良さそう。
* UARTで無線モジュールに送信：ボーレートやフォーマットは無線モジュールが期待するものに対して適正だろうか。
* サーバにデータが蓄積される：無線モジュールが正常に動作して、サーバと通信を確立し、サーバアプリが正常にデータをストアする。

こうやってみてみればファームウエア単体ではノイズ除去の部分ぐらいしかテストしようがない。それ以外のシステムのテストはテストモードのファームウエアを作って、部分的なペリフェラルの動作テストをするのがいいだろう。そして、そのような動作テスト・ファームウエアは使い捨てではなくいつでも使えるようにしておくべきだし、使いやすいようにして、頻繁に動作テストを行うべきだろう。

ユニットテストを書くことの意義は言うまでもないが、そのプログラムの仕様がコード化されていることだ。いくら自然言語で仕様を書いても抜け漏れがあり、そのような境界でバグが発生する。ユニットテストはそのような振る舞いを例示によって定義したもの、と捉えると書きやすいだろう。


## テストモードの作成

通常、IDEでプロジェクトを生成すると`Debug`と`Release`のコンフィグレーションが設定されることが多い。そこに`Test`、`HwTest`というコンフィグレーションを付け加えて欲しい。設定は次のとおりだ。

|config   |compiler option|`#define`|
|---------|---------------|---------|
|`Release`|optimized      |`NDEBUG` |
|`Debug`  |normal         |         |
|`Test`   |normal         |`TEST`   |
|`HwTest` |normal         |`HWTEST` |

`-DTEST`のときは、アプリケーションのメインルーチンを実行するのではなくテストルーチンを実行する。テストルーチンは、テストを順次実行し、実行結果は`assert()`で判定され、すべてのテストがパスするとトータルでパスとなる。

* テストは失敗したら`assert`で、赤LED点灯の無限ループ。基本的に、テストがパスしたことを確認するので、どのテストに失敗したかはUARTに出力するかデバッガで追う。
    + つまり、デバッグポートでUARTまたは、SWD経由のITM出力を通じて、`printf`が動くようにしておかなければならない。
* テストが全部成功したら、緑LED点灯の無限ループで終了。

`-DHWTEST`のときは、ハードウエアテストモードで動作する。

ペリフェラルを特定のモードで動作させ、合否判定は`assert()`ではなくオシロスコープなどの測定器で行われる。


### テスト関数の位置

ユニットテストの関数を、被テスト関数と同じファイルに書くか、別のファイルに書くか、という流儀がある。得失は次のとおり。

* 被テスト関数と同じファイルにテストを書く
    + テストを追加すると、製品コードに変更がなくても、製品コードのリビジョンが上がる。
    + 同じファイルなので`static`スコープが見える。
* 被テスト関数とテスト関数を別ファイルに書く
    + 管理が楽。とくに、テストを付かしても実行ファイルのリビジョンが上がらない。
    + 別ファイルなので、シェル芸を通じてテスト関数の自動収集＆自動実行がやりやすい。IDE派にはシェル芸は関係ないが。
    + `static`スコープの関数や変数の扱いが面倒。

個人的な意見だが、被テスト関数と同じファイル内に`#ifdef TEST`としてテスト関数を書き、テストランナーからその関数を呼び出したほうが、主に`static`の扱いが簡単なので好みだ。

次の関数は`TEST`が`#define`されている場合、上述の`StartDefaultTask`から呼ばれる。1文字出力関数を置き換えて出力結果をバッファに取り、`xprintf`に関してフォーマットのテストを行う。テストが失敗すれば`assert(0)`で停止。すべてのテストが合格すれば、緑LEDを点灯して無限ループになる。


```c
#ifdef TEST

#include <string.h>

static char s_buf[256];
static char s_cnt = 0;

void buf_putchar(char c) { s_buf[s_cnt++] = c; }

static void StartTest(void)
{
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

    xputchar = buf_putchar;      // 1文字出力関数をバッファライタにする。

    // xprintf("printf") == "printf"
    s_cnt = 0;
    xprintf("printf");
    s_buf[s_cnt] = 0;
    assert(!strcmp(s_buf, "printf"));    // バッファに書いた文字列を期待値と比較する。test fail→assert fail

    // xprintf("d:%d x:%x", 10, 10) == "d:10 x:a"
    s_cnt = 0;
    xprintf("d:%d x:%x", 10, 10);
    s_buf[s_cnt] = 0;
    assert(!strcmp(s_buf, "d:10 x:a"));

    // After all test pass, GREEN LED(LD2) lights.
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
    for (;;) {
    }
}
#endif // TEST
```

## ハードウエアテストモード

`-DHWTEST`でファームウエアがビルドされたときは、ペリフェラルのテストプログラムが走るのは良いアイデアだ。システムに最低でも1つ、入力ボタンがあれば、入力ボタンを押すことによって、テスト対象のペリフェラルが切り替わると使いやすい。たとえば次のような。いつでも同じ条件でハードウエアのテストが実行されるように、HWテストモードのFWはリポジトリに登録されてきちんと管理されなければならない。

```c
void StartDefaultTask(void *argument)
{
#if TEST
    StartTest();
#elif HWTEST
    StartHwTest();
#else
    StartApp();
#endif // TEST
    assert(0);
}

#ifdef HWTEST

static bool button_pushed(void)
{
    // GPIO_PIN_SET => button is not pushed.
    if (HAL_GPIO_ReadPin(B1_GPIO_Port,B1_Pin)==GPIO_PIN_SET) {
        return false;
    }
    osDelay(150);
    if (HAL_GPIO_ReadPin(B1_GPIO_Port,B1_Pin)==GPIO_PIN_SET) {
        return false;
    }
    return true;    
}

static void StartHwTest(void) {

    xputchar = uart_putchar;
    xprintf("push button to start HW_TEST\r\n");

    while(!button_pushed()); 
    xprintf("LED turn on\r\n");
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);

    while(!button_pushed()); 
    xprintf("LED turn off\r\n");
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
    
    while(!button_pushed()); 

    // After all test pass, GREEN LED(LD2) lights.
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
    xprintf("HW_TEST completed!\r\n");
    for (;;) {
    }
}

#endif // HWTEST
```

## printf

デバッグやテストでは、シリアルポートに文字を出力できるようになっていると非常に便利だ。開発ボードでは、デバッグ用のシリアルポートを設けておくことを強く推奨する。もしポートが開いていなくて、IDEとMCUがサポートしていればSWDのITMを使っても良い。

以下にいろいろな実装について述べる。

しかし`printf`の使いみちは、なにもデバッグ時やテスト時にログを出すだけではない。UART通信のフォーマットを構築するのにも使える。また、実動作時にも`assert`を有効にしておいて、引っかかった時は内蔵フラッシュに記録を残しつつリブートして動作を継続しておき後で解析するなど、の用途にも使える。つまり、いろいろ使いみちがあるので、実環境でも使いやすい、小型でカスタマイズできる`printf`を実装して組み込んでおくと便利だ。

### UARTの使い方

Nucleoの場合、ペリフェラルとして`USART2`が有効化される。これはボード上でSTlinkにつながっており、PCからは仮想COMポートに見える。
Linuxの場合は`/dev/stlinkv2_console`(たいてい`/ttyACM0`へのSymLink)がデバイスファイルである。puttyなどを`apt-get`でインストールして、このデバイスファイルを選択すればシリアル通信ができる。

ファームウエア側ではCubeMXが初期化コードを生成してくれているので、`HAL_UART_*`に`&huart2`を渡せば`USART2`を使うことができる。

### メーカ提供のIDEの場合

多くの場合、セミホスティング環境でデバッガを繋げば、デバッガのコンソールへ`printf`できるようになるだろう。

Cortex-M3以上の場合はSWOを使ったある程度高速な標準出力が得られるがCortex-M0/M0+の場合はSWDを使った低速なものになるかもしれない。


### arm-none-eabi-gcc + nanolibの場合

本記事のように環境を構築すると、ツールチェーンとして`arm-none-eabi-gcc`が使われ、ライブラリとして`nanolib`がリンクされる。この環境ではコンパイラ組み込みの`printf`を利用することができる。そのためには、指定のUARTポートに文字を出力するドライバを定義してやらなければならない。

まず、アプリのコード中に`printf("aaa");`を書いてビルドしてみる。そうすると、無事にビルドが通る。OpenOCDでフラッシュするためにはHEXファイルが使われるが、コンパイラの基本的なアウトプットはELFファイルだ。`objcopy`でそれをHEXに変換している。`objdump`でELFを解析すれば、ドライバの書き方がわかるだろう（ググったけど、環境がニッチなのか適切な記事が出てこなかった）。

```
$ arm-none-eabi-objdump -D blinky/build/blinky.elf
```

とすれば逆アセンブル出力が得られる。アプリから`printf`を呼び出したところを探してみると、内蔵printfは`iprintf`という関数コールになっている。
そこから呼び出されている関数を辿って行くと、`__swbuf_r`という関数が最終的に文字列を出力していそうだ。


[https://github.com/32bitmicro/newlib-nano-1.0/blob/master/newlib/libc/stdio/vfprintf.c](https://github.com/32bitmicro/newlib-nano-1.0/blob/master/newlib/libc/stdio/vfprintf.c)


** この項は書きかけ **


### 自作する場合

練習がてら非常に簡単なものを自作してもよい。自分のツールボックスの中に1つあるといろいろな場面で使い回しができて便利だ。

基本的な構造としては次のようになることが多いだろう。

* （mallocを使いたくないので）フォーマット文字を1文字づつ読んでインタプリトする。
* 整形文字でなければ、そのまま出力する。整形文字なら整形して出力する。
* 外部に定義される1文字出力関数（UART、ITM、USB、BT-SPPなど）を呼ぶ。

もちろん、`sprintf()`をまず実装して、それを使った`printf()`の実装でも良い。

```c
extern void xputchar(char c);

static void xitoa(int i, int base)
{
    int d, x;
    if (i < 0) {
        xputchar('-');
        i = -i;
    }
    if (base == 10) {
        x = 1000000000;
    } else if (base == 16) {
        x = 0x10000000;
    }
    while (i < x) {         // 先頭の'0'はサプレス
        x = x / base;
    }
    while (1) {             // 上の桁から順に1文字づつ出力する。
        d = i / x;
        if (base == 10) {
            xputchar(d + '0');
        } else if (base == 16) {
            xputchar(  (d < 10) ? (d + '0') : (d - 10 + 'a'));
        }
        i = i - d * x;
        x = x / base;
        if (!x) {
            break;
        }
    }
}

void xprintf(const char *fmt, ...)
{
    va_list list;
    va_start(list, fmt);

    const char *cp = fmt;
    char c;
    while((c = *cp++)) {               // ポインタを文字変数に取る。ポインタのままやってもいい。趣味。
        if (c == '%') {                // '%'ならフォーマット指示子
            c = *cp++;
            if (c == 'd') {
                xitoa(va_arg(list, int), 10);
            } else if (c == 's') {
                xprintf(va_arg(list, char *));
            }
        } else {                       // それ以外はそのまま出力
            xputchar(c);
        }
    }
    va_end(list);
}
```

`xputchar`は別に、UARTを使った1文字出力関数として定義しておく。

```c
void xputchar(char c) { HAL_UART_Transmit(&huart2, (uint8_t *)&c, 1, 1); }
```

