* Generate Project
    + STM32CubeMX
    + Board: NUCLEO-F103RB
    + Pin & Config
        - GPIO: スイッチ入力 B1 と LED出力 LD2 が設定されている。
        - USART2: デフォルトでアクティブ。USB-COMと繋がっている。
        - FREERTOS: CMSIS_V2で設定
* Project Manager
    + Toolchain: Makefile

blinkyを作る(とりあえずOS無しバージョン)

flash.sh で書き込む。

