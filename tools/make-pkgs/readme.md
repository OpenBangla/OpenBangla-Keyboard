## make-pkgs

**make-pkgs** is a collection of bash scripts, intended to make the experience of compiling and generating iBus/fcitx OBK packages for various distros simpler and user-friendly.
This tool is primarily intended to be used by end users, with simple switches for operation.

**NOTE** : It is **HIGHLY** recommended you have [toolbx](https://containertoolbx.org/) (`toolbox` package) installed in your system, it is required for the `--toolbox` flag to function.

## The importance of toolbx containers

[toolbx](https://containertoolbx.org/) is an user-friendly CLI utility for creating and entering "containers" for various linux distros, similar to Distrobox.
toolbx is used in make-pkgs to compile and package OBK for distros other than your own.

For example, If you are on Debian and you want to generate `.rpm`s specifically for Fedora 37.

## How to use

- (Optional) Copy this folder to a convenient location (e.g. ~/Scripts)
- Make the make-pkgs.sh script executable by doing `chmod +x make-pkgs.sh`
- Use the tool by running `./make-pkgs.sh --<flags>`

For a list of commands, run `./make-pkgs.sh --help`
\
Generated packages are stored inside the make-pkgs folder in a directory named "builds"

## Examples

Here are some examples and their explanation on how to use make-pkgs

1. This command will compile and generate ibus OBK packages for Debian from the `develop` branch, development tools and libraries will be installed on your host system.
```
./make-pkgs.sh --ibus --develop --debian`
```

2. This command will compile and generate ibus and fcitx OBK packages for Debian 11 and Fedora 38 from the `develop` branch, development tools and libraries will be installed inside of toolbox containers, your host system will remain untouched.
```
./make-pkgs.sh --ibus --fcitx --develop --debian 11 --fedora 38 --toolbox
```

3. This command will remove all toolbox containers named "obk-toolbox-*" and clear the directory where the script stores it's files (by default in ~/.obk-build/)
```
./make-pkgs.sh --clean
```
