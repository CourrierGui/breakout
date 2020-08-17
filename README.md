# Breakout
Implementation of the Breakout game in C++.

![alt text](resources/screenshot.png)

This game comes from the [LearnOpenGL](https://learnopengl.com) website and was
built using my game engine available
[here](https://github.com/CourrierGui/pangolin) (commit
e7a6d375f946978d7895bf61c556de22cb0a1848).

# Installation steps

```
git clone https://github.com/CourrierGui/breakout
cd breakout && mkdir -p build
git submodule init && git submodule update
```

Until I find/code an open source alternative, I'm stuck with irrKlang as a sound engine.
Go to [irrKlang](https://www.ambiera.com/irrklang/downloads.html) to download the sound engine and unzip the files in *breakout/extern/irrklang*.
Then copy the file *breakout/extern/irrKlang/irrKlang-64bit-1.6.0/bin/linux-gcc-64/ikpMP3.so* to *build/apps*.

I used the [freetype](https://www.freetype.org/download.html) library to
display text in the game. You can follow the instructions to install it in the
directory *extern/freetype2*. The installation from the git repository requires:

```
cd extern/freetype2
./autogen.sh
./configure
make
```
The library should then be located at *extern/freetype2/objs/.libs/libfreetype.so*.
Make sure to install libpng too.

Finally,
```
cd build
cmake .. && make
cd apps
./breakout
```
