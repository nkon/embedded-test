#!/bin/sh
ELF_FILE=blinky/build/blinky.elf
openocd -f board/st_nucleo_f103rb.cfg -c "init" -c "reset init" -c "stm32f1x mass_erase 0" -c "flash write_image $ELF_FILE" -c "reset halt" -c "reset run" -c "exit"
