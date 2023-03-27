#!/usr/bin/sh

gcc $(find . -name "*.c") -Iinclude -Og -g -o leaf -fsanitize=undefined,address
