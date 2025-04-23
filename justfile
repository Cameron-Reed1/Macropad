build:
    cmake --build build

install: build
    cmake --install build

init:
    cmake -B build -S .

clean:
    rm -r build

alias run := install
