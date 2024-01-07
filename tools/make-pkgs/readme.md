## make-pkgs
**make-pkgs** is a collection of bash scripts, intended to make the experience of compiling OBK and generating packages for various distros simpler and user-friendly.
This is made as an user-facing tool, with simple switches for operation.
\
**NOTE** : It is **HIGHLY** recommended you have the `toolbox` package installed.

## The importance of Toolbx containers
Toolbx is a user-friendly CLI utility for creating and entering "containers" for various linux distros, similar to Distrobox.
\
Toolbx is used in make-pkgs to compile and package OBK for distros other than your own.
\
For example, If you are on Debian and you want to generate `.rpm`s specifically for Fedora 37, you can do so.

## How to use
Copy this folder, make the make-pkgs.sh script executable by doing `chmod +x make-pkgs.sh`
\
Then use the script using `./make-pkgs.sh --<commands>`
\
For a list of commands, use `./make-pkgs.sh --help`
\
Generated packages are stored inside the make-pkgs folder inside a directory named "builds"

## Examples
Here are some examples and their explanation on how to use make-pkgs

`./make-pkgs.sh --ibus --develop --debian`
\
This will compile and generate ibus OBK packages for Debian from the `develop` branch, development tools and libraries will be installed on your host system.

`./make-pkgs.sh --ibus --fcitx --develop --debian 11 --fedora 38 --toolbox`
\
This will compile and generate ibus and fcitx OBK packages for Debian 11 and Fedora 38 from the `develop` branch, development tools and libraries will be installed inside of toolbox containers, your host system will remain untouched.

`./make-pkgs.sh --clean`
\
This will remove all toolbox containers named "obk-toolbox-*" and clear the directory where the script stores it's files (by default in ~/.obk-build/)
