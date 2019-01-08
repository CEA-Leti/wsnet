# WSNet #

Welcome to the **WSNet** Simulator!

WSNet is an event-driven simulator for wireless networks on a large scale. 

## Platforms ##

WSNet has been used on the following platforms:

  * Linux
  * Mac OS X (partially)

## Requirements ##

Currently, we support Linux and Mac OS X (partially). We will also make our best
effort to support other platforms (e.g. Windows).
However, since core members of the WSNet project does not usually use
these platforms, WSNet encounter some problems there.  If
you notice any problems on your platform, please create an issue. 
Patches for fixing them are even more welcome!

### Linux Requirements ###

These are the base requirements to build and use WSNet from a source
package (as described [here](doc/dev/building/compiling.md)):

  * [CMake](https://cmake.org/) v3.11.0 or newer
  * GNU-compatible Make
  * POSIX-standard shell
  * GCC v5 or newer
  * [Python](https://www.python.org/) v2.8 or newer (for running some of
    the tests and scripts)

Libraries:

  * libglib2.0-0
  * libxml2
  * libgmodule2.0 

Note that if you are planning to contribute code, you'll need the development 
files of the libraries (e.g. libxml2-dev).

## Contributing Code ##

We welcome patches.  Please read the [Contributing Guide](CONTRIBUTING.md)
for how you can contribute. You can also refer to the [documentation folder](doc/README.md)
for further explanations in different topics.

Happy simulations!