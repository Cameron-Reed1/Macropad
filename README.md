This is the firmware I wrote for my (Adafruit MacroPad RP2040)[https://www.adafruit.com/product/5128]


### Building from source

```
git clone https://github.com/Cameron-Reed1/Macropad.git
cd Macropad
mkdir build
cmake -S . -B build
cmake --build build
```

Alternatively, if you have (just)[https://github.com/casey/just] installed:

```
git clone https://github.com/Cameron-Reed1/Macropad.git
cd Macropad
just init
just build
```


### Installing

Note: The first time, you will need to put the macropad into BOOTSEL mode, after that picotool will do it for you

```
cmake --install build
```


### Build and install in one command

```
cmake --build build --target install
```

Or, with just:

```
just install
```
