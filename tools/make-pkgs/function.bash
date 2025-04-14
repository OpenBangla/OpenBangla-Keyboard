#variables
FILE_DIR_OBK="$HOME/.obk-build/"
DISTRO_COUNT=0

cleanup () {
  echo "Removing obk-toolbox containers..."
  for tbc in $(toolbox list | sed -n 's/.*\(obk-toolbox[^\ ]*\).*/\1/p'); do
    toolbox rm -f "$tbc"
  done
  echo "Cleaning build directory..."
  rm -rf "$FILE_DIR_OBK" > /dev/null 2>&1
}
help () {
    echo "Usage: $0 [OPTIONS]"
    echo "Options:"
    echo "  --ibus                Compile IBus version"
    echo "  --fcitx               Compile Fcitx version (Doesn't work for OBK2 and earlier)"
    echo "  --develop             Compile from the develop branch"
    echo "  --fedora <version>    Compile for Fedora"
    echo "  --debian <version>    Compile for Debian"
    echo "  --toolbox             Compile inside of a toolbox"
    echo "  --clean               Clean \"$FILE_DIR_OBK\" directory and remove obk-toolbox-* containers"
    echo "  --help                Display this help message"
    echo " "
    echo "  -> NOTE: User needs to specify the --toolbox flag to compile for multiple distros at the same time."
    echo "           Otherwise, only one flag can be used at a time (e.g. --fedora and --debian can't be used together without --toolbox)"
    echo "  -> NOTE: When compiling inside of a toolbox, an additional <version> must be specified for the distro flags."
    echo "           (e.g. --fedora 37) This will generate packages for a specific version of a distro."
    echo "Example: "
    echo "  $0 --ibus --fcitx --develop --fedora 37 --debian 11 --toolbox"
    echo "  $0 --ibus --develop --debian"
}
add_config () {
  echo "$1" >> "$FILE_DIR_OBK"config.bash
}
#init FILE_DIR_OBK
filedir_init () {
    mkdir "$FILE_DIR_OBK" > /dev/null 2>&1
    rm -r "$FILE_DIR_OBK"config.bash > /dev/null 2>&1
    touch "$FILE_DIR_OBK"config.bash > /dev/null 2>&1
    add_config "START_DIR_OBK=$(pwd)"
    add_config 'add_config () {
  echo "$1" >> "$FILE_DIR_OBK"config.bash
}'
    add_config 'log_debug () {
  if [[ "$DEBUG_OBK" = "YES" ]]; then
  set -x
  exec &> >(tee -a "$1.log")
  fi
}'

    #rm -r "$FILE_DIR_OBK"distro.list > /dev/null 2>&1
    #touch "$FILE_DIR_OBK"distro.list > /dev/null 2>&1
}
parse_args () {
  for arg in "$@"; do
    case $arg in
      ("--ibus") add_config 'IM_IBUS_OBK=YES' ;;
      ("--fcitx") add_config 'IM_FCITX_OBK=YES' ;;
      ("--develop") add_config 'BRANCH_OBK=develop' ;;
      ("--toolbox") add_config 'TOOLBOX_ENABLE_OBK=YES' ;;
      ("--fedora")
        add_config 'FEDORA_OBK=YES'
        add_config "FEDORA_VERSION_OBK=$(echo "$@" | sed -n 's/.*--fedora \([0-9]*\).*/\1/p')"        
        ((DISTRO_COUNT++)) ;;
      ("--debian")
        add_config 'DEBIAN_OBK=YES'
        add_config "DEBIAN_VERSION_OBK=$(echo "$@" | sed -n 's/.*--debian \([0-9]*\).*/\1/p')"
        ((DISTRO_COUNT++)) ;;
      ("--clean")
        if [[ $(echo "$@" | wc -w) -gt 1 ]]; then
          echo 'Error: --clean cannot be used with other flags. Exiting...'
          exit 1
        else
          cleanup
          exit 1
        fi ;;
      ("--help") help && exit 1 ;;
      (--*)
        echo "Unknown argument: $arg"
        help
        echo "Exiting..."
        exit 1 ;;
      esac
  done
}
toolbox_error () {
  echo "Error: toolbx had an issue. Exiting..."
  exit 1
}
missing_version () {
  echo "Error: version for $1 missing. Exiting..."
  exit 1
}
log_debug () {
  if [[ "$DEBUG_OBK" = 'YES' ]]; then
  set -x
  exec &> >(tee -a "$1.log")
  fi
}

# SOURCE FILES
#function.bash : Contains initial variables and functions for use in make-pkgs.sh
#config.bash : As toolbox containers donot inherit any environment variables, PERSISTENT variables are stored in here and loaded into other .sh files using source command

# VARIABLES
# YOU NEED TO LOAD config.bash TO HAVE PERSIST VARS AVAILABLE IN YOUR SHELL SCRIPTS
#FILE_DIR_OBK (PERSIST) : General path where the scripts will store config data, git repos,build artifacts etc etc
#START_DIR_OBK (PERSIST) : directory from where the script was called
#DEBUG_OBK (PERSIST) : check if the debug flag is set
#IM_*_OBK, TOOLBOX_ENABLE_OBK, <DISTRO>*_OBK, BRANCH_OBK (PERSIST) : self-explanatory
#DISTRO_COUNT (TEMP) (make-pkgs.sh) : Counter to check if user tries to compile for multiple distros without --toolbox flag
#PACKAGE_FORMAT, PACKAGE_FILE (TEMP) (make-build.sh) : stores which package format the compilation should use, and the path to the generated package file.

# FUNCTIONS
#add_config (1 args) (PERSIST) : takes one argument as input and appends it to config.bash
#log_debug (1 args) (PERSIST) : takes one argument as the lof filename, checks if DEBUG_OBK is set, enables debugging.
#filedir_init (noargs) (make-pkgs.sh) : cleans and creates config.bash, then adds the FILE_DIR_OBK, START_DIR_OBK, add_config, log_debug to that.
#parse_args (1 args) (make-pkgs.sh) : parses arguments
#cleanup (noargs) (make-pkgs.sh) : cleans FILE_DIR_OBK and removes obk-toolbox containers created by the script
#help (noargs) (make_pkgs.sh) : shows helpinfo
#toolbox_error (noargs) (make_pkgs.sh) : shows error on toolbox container error
#missing_version (noargs) (make_pkgs.sh) : shows error on missing version with distro flag

# STRUCTURE OF THE PROJECT
#                    make-pkgs.sh
#                        |
#   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#   |                    |                   |
# toolbox            no toolbox           (others?)
#   |                    |                   |
#   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#   |                    |                   |    
# (fedora)            (debian)            (others)
#   |                    |                   |    
#   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#                        |
#                  make-build.sh