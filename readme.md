This is a tutorial project to setup new firmware project.

As an example, the following enviroment is assumed.
This project and tutorial document can also be applied to generic firmware project.

* MCU: STM32F103
* Board: Nucleo-F103RB
* Dev enviroment: Linux(Ubuntu)
* Dev language: C
* Tool chain: arm-none-eabi-gcc + OpenOCD
* Build too: Makefile
* Code generator: STM32CubeMX
* RTOS: FreeRTOS + CMSIS-RTOS v2 API

# getting started

* `make`: build application project(blink LED using RTOS timer task).
* `flash.sh`: write firmware to the board using OpenOCD and ST-Link.
* `make test`: make test application of firmware. Run tests on the target board.
* `make hwtest`: hardware test routines.

# Tutorial and explanation

Tutorial and explanation(Japanese)-> [readme-j.md](readme-j.md)

This tutorial contains the following.

* Source Tree Structure: separation of generated code and user code
* Basics of RTOS and CMSIS-RTOS API.
* Application + FW TEST + HW TEST
* Incremental development with testing.
* Simple imprementation of `printf`.
