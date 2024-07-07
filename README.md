# MIR PACKAGE MANAGER

### STEPS TO COMPILE:

   1. Run the set_env.sh script to set up the Environment.
   2. Trigger the Makefile for generating `mir` executable using `make` command.

### QUICK INSTALLATION:

   1. From quick installation just push the mir binary to the `/usr/bin/` directory of your embedded device.


### FULL INSTALLTION:

   1. Add the Mir Makefile to your build configuration as new recipe.


### USAGE:

* `mir start`
    * One time action to generate resource map of packages and libraries that will not be affected by mir.
* `mir install <pkg_name>`
    * Installs the package name specified.
* `mir remove  <pkg_name>`
    * Uninstalls the mir managed package.

