c-shquote
=========

POSIX Shell Compatible Argument Parser

The c-shquote project is a standalone implementation of POSIX Shell compatible
argument parsing written in Standard ISO-C11. To use c-shquote, include
c-shquote.h and link to the libcshquote.so library. A pkg-config entry is
provided as well. For API documentation, see the c-shquote.h header file, as
well as the docbook comments for each function.

### Project

 * **Website**: <https://c-util.github.io/c-shquote>
 * **Bug Tracker**: <https://github.com/c-util/c-shquote/issues>

### Requirements

The requirements for this project are:

 * `libc` (e.g., `glibc >= 2.16`)

At build-time, the following software is required:

 * `meson >= 0.41`
 * `pkg-config >= 0.29`

### Build

The meson build-system is used for this project. Contact upstream
documentation for detailed help. In most situations the following
commands are sufficient to build and install from source:

```sh
mkdir build
cd build
meson setup ..
ninja
meson test
ninja install
```

No custom configuration options are available.

### Repository:

 - **web**:   <https://github.com/c-util/c-shquote>
 - **https**: `https://github.com/c-util/c-shquote.git`
 - **ssh**:   `git@github.com:c-util/c-shquote.git`

### License:

 - **Apache-2.0** OR **LGPL-2.1-or-later**
 - See AUTHORS file for details.
