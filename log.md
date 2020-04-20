# ebe02c59262b5e384c9d8b577efd52421a79d407

最初のコミット。

プロジェクト生成

* Generate Project
    + STM32CubeMX
    + Board: NUCLEO-F103RB
    + Pin & Config
        - GPIO:スイッチ入力:B1とLED出力:LD2が設定されている。
        - USART2:デフォルトでアクティブ。USB-COMと繋がっている。
        - FREERTOS:CMSIS_V2で設定
* Project Manager
    + Toolchain: Makefile

blinkyを作る（とりあえずOS無しバージョン）

`flash.sh`で書き込む。

動作確認。

# 5866f6b26b3b58d40e7de100bb5f83e0252593b6

* RTOSのタスクでBlinky.

# 9e7c1bf26c0ec29eb0582e3e15201fe235a60f20

* アイドルタスクでSleepするようにした。

# d3e037b9316f9cf1b6512acb35649a2bdf8365bb

* ユーザコードを`App/`下に移す。
* アプリケーションタスク、テストタスクの雛形を作る。

# 4d057775a1669be6172a69a239af71568d625ef6

* `#define TEST`でテストルーチンが走るようにする。

# 9e4299b27263ad833a9be8d09a6ebd8af46da29a

* `printf`のシンプルな実装

# a0b30638286b6e7e992ec81eb0ed976e51778738

* タイマタスクでLEDが点滅した。
* タスクの構造をリファクタリング。

# b8c6e47e8112c26a5d137eca7281658c015a6eb3

* `#define HWTEST`でハードウエアをテストするようにした。


