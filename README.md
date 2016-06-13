# OpenBangla Keyboard
![openbangla-keyboard](https://cloud.githubusercontent.com/assets/9459891/15998078/ead3cae2-315c-11e6-8595-df1efbe478d2.png)

An OpenSource, Unicode compliant Bengali Input Method for GNU/Linux systems.

### Features
* Phonetic Keyboard Layout (Avro Phonetic)
* Fixed Keyboard Layout
* Can use Avro Keyboard Layout file (version 5)
* Layout Viewer
* Preview window when using Phonetic Keyboard Layout
* Features Avro Keyboard like Top Bar

### Installing
#### Linux
Linux packages can be downloaded from [here](https://github.com/OpenBangla/OpenBangla-Keyboard/releases)

If you need help, there is a wiki [page](https://github.com/OpenBangla/OpenBangla-Keyboard/wiki/Installing%20OpenBangla%20Keyboard)

Tested in Ubuntu 16.04

**Currently Debian Package(.deb) are only supported**

### Compiling on GNU/Linux
#### Build & Install procedures
OpenBangla Keyboard currently needs following libraries and binaries
* GNU GCC, G++ compiler
* GNU Make
* CMake
* Qt 5 libraries
* iBus development library

On a Ubuntu/Debian system you can easily install them like this
```
sudo apt-get install build-essential cmake libibus-1.0-dev qt5-default qtdeclarative5-dev
```

After you have installed required libraries and binaries. Clone this repository and change the directory to the cloned folder and issue the commands:
```
mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX='/usr' -DLIBEXEC_DIR='/usr/libexec'
make
sudo make install
```
Use iBus UI or run ibus-setup to add **OpenBangla Keyboard**. You can get help on adding **OpenBangla Keyboard** in **Text entry settings** in this [page](https://github.com/OpenBangla/OpenBangla-Keyboard/wiki/Installing%20OpenBangla%20Keyboard)
