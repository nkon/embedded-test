# ebe02c59262b5e384c9d8b577efd52421a79d407

最初のコミット。

プロジェクト生成

* Generate Project
    + STM32CubeMX
    + Board: NUCLEO-F103RB
    + Pin & Config
        - GPIO: スイッチ入力 B1 と LED出力 LD2 が設定されている。
        - USART2: デフォルトでアクティブ。USB-COMと繋がっている。
        - FREERTOS: CMSIS_V2で設定
* Project Manager
    + Toolchain: Makefile

blinkyを作る（とりあえずOS無しバージョン）

flash.sh で書き込む。

動作確認。

# 5866f6b26b3b58d40e7de100bb5f83e0252593b6

* RTOSのタスクでBlinky.

# 9e7c1bf26c0ec29eb0582e3e15201fe235a60f20

* アイドルタスクでSleepするようにした。

# d3e037b9316f9cf1b6512acb35649a2bdf8365bb

* ユーザコードを`App/`下に移す。
* アプリケーションタスク、テストタスクの雛形を作る。

#

* `#define TEST`でテストルーチンが走るようにする。

