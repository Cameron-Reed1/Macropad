#!/usr/bin/env bash

if [ -d "$1" ]; then
    printf "Flashing $1/Macropad.uf2\n"
    picotool load -fuvx "$1/Macropad.uf2" || picotool load -fuvx "$1/Macropad.uf2"
elif [ -d "build" ] && [ -f "build/Macropad.uf2" ]; then
    printf "Flashing build/Macropad.uf2\n"
    picotool load -fuvx "build/Macropad.uf2" || picotool load -fuvx "build/Macropad.uf2"
else
    printf "Cannot flash. Build directory was not specified and build/Macropad.uf2 does not exist\n"
fi

