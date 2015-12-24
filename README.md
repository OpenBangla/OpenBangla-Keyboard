# OpenBangla Keyboard
An OpenSource, Cross-platform, Unicode compliant Bengali Input Method.

This is the first effort to write Bengali(Bangla) Cross-platform. With the power of Unicode, your data compatibility is sured. Get the same feeling across all platforms. Enjoy the same typing style and method in all platforms.

### Project Status
OpenBangla Keyboard is in early stage of development. Currently it works on iBus supported GNU/Linux distributions.

### Compiling on GNU/Linux
#### Build & Install procedures
Clone this repository and change the directory to the cloned folder and issue the commands:
```
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX='/usr' -DLIBEXEC_DIR='/usr/libexec'
make
sudo make install
```
Use iBus UI or run ibus-setup to add **OpenBangla Keyboard**.

**TIPS:** You can use the guides to add OpenBangla Keyboard to iBus from [ibus-avro](http://linux.omicronlab.com/) project. **Do not follow the installing instructions. Type and select OpenBangla Keyboard from the lists, not Avro Phonetic.**
