# Building

```
mkdir build
cmake -S . -B build
cmake --build build
```


# Installing

Note: The first time, you will need to put it into BOOTSEL mode, after that picotool will do it for you

```
cmake --install build
```


# Build and install at the same time

```
cmake --build build --target install
```

