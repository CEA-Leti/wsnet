# Building WSNet

## Environment Variables

In order to configure and properly compile WSNet we need to define a number of environment variables. Below, you can find the lines added to the ```.cshrc``` file, you should consider doing the same.

```bash
# Path configurations of WSNet
setenv WSNET_SRC_PATH /path/to/wsnet/folder/

# Path of the installed resources
setenv WSNET_INSTALLED_PATH $WSNET_SRC_PATH/installed

# Path of the external models
setenv WSNET_EXTERNAL_MOD_PATH $WSNET_SRC_PATH/external_models

# Paths to the external libs after installation
setenv WSNET_MODDIR $WSNET_EXTERNAL_MOD_PATH/lib

# General PATH Variable
set path = ($WSNET_INSTALLED_PATH/bin/ $path)
```
Note that you need to change the ```/path/to/wsnet/folder/``` to the folder on which you have downloaded WSNet. Moreover, you can also change the destination folder for the installation. Simply change the folder of ```WSNET_INSTALLED_PATH```. The same is valid for external models (```WSNET_EXTERNAL_MOD_PATH```).

## Compiling 

In order to compile WSNet, you'll need a ```build``` folder. This folder is used to keep source files separeted from the CMake and Make generated files. In this way, we do not pollute our source files with buidling files.

If you still don't have it, create the ```build``` folder before proceeding:

```bash
$ cd $WSNET_SRC_PATH
$ mkdir -p build
```

Now that you have already created your ```build``` folder, you can access it:

```bash
$ cd build
```

In the ```build``` folder, you will be able to compile WSNet. 

```bash
$ cmake ..
```

CMake will then configure, check all dependencies and generate the Makefiles that will be used to compile. The output will be something similar to:

```bash
-- The C compiler identification is GNU 4.1.2
-- The CXX compiler identification is GNU 4.1.2
-- Check for working C compiler: /usr/bin/cc
-- Check for working C compiler: /usr/bin/cc -- works
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Detecting C compile features
-- Detecting C compile features - done
-- Check for working CXX compiler: /usr/bin/c++
-- Check for working CXX compiler: /usr/bin/c++ -- works
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Checking for ccache
-- Configuring and generating files for Linux
-- Found PkgConfig: /usr/bin/pkg-config (found version "0.21") 
-- Checking for one of the modules 'gmodule-2.0'
-- Found LibXml2: /usr/lib64/libxml2.so (found version "2.6.26") 
-- Found M: /usr/lib64/libm.so  
-- Configuring done
-- Generating done
-- Build files have been written to: /path/to/wsnet/folder/build
```
If no errors were produced, you can compile the code :

```bash
$ make all
```

If you want to install the files in the dedicated folder (```WSNET_INSTALLED_PATH```), you can proceed to :

```bash
$ make install
```

After this, you will be able to run wsnet from the command line.

### CMake Build Types

CMake Build Types statically specifies what build type (configuration) will be built in the build tree. Possible values are empty, Debug, Release, RelWithDebInfo and MinSizeRel. Each one of them will configure the build process in a specific way, as defined in the [WSNETCompilerSettings.cmake file](cmake/WSNETCompilerSettings.cmake).

In the example above we left this value empty, thus CMake will produce standard build type, which is the same as Debug. 

If we wanted to specify the Debug build type we would run the following command:

```bash
$ cmake -DCMAKE_BUILD_TYPE=Debug ..
```

This is particularly useful when you need to compile your code for debugging purpose or to distribute (release) your code with optimizations. 

