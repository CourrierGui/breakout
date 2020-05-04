# breakout
Implementation of the Breakout game in C++.

This game comes from the [LearnOpenGL](https://learnopengl.com) website and was built using my game engine available [here](https://github.com/CourrierGui/pangolin).

# Installation steps

```
git clone https://github.com/CourrierGui/breakout
cd breakout && mkdir -p build
git submodule init && git submodule update
```

Until I find/code an open source alternative, I'm stuck with irrKlang as a sound engine.
Go to [irrKlang](https://www.ambiera.com/irrklang/downloads.html) to download the sound engine and unzip the files in *breakout/extern/irrklang*.
Then copy the file *breakout/extern/irrKlang/irrKlang-64bit-1.6.0/bin/linux-gcc-64/ikpMP3.so* to *build/apps*.

I used the [freetype](https://www.freetype.org/download.html) library to build
the game. You can follow the instructions to install it in the directory
*extern/freetype2/objs/.libs/libfreetype.so*. Make sure to install libpng too.

Finally,
```
cd build
cmake .. && make
cd apps
./breakout
```
