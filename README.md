Introduction
============

Topology Generator Library

Get started
===========

How to compile
--------------

The first step is to compile the library that consists in invoking the cmake script. We assume that the Boost Library 1.65 had been compiled and installed inside the folder `$DIR/boost-165`.

```bash

DIR="$HOME/opt"

cmake -DCMAKE_INSTALL_PREFIX="$DIR/topgen" -DBOOST_ROOT="$DIR/boost-165" -DCMAKE_BUILD_TYPE=Debug .
make -j 5
make install
```
For more information, read the file `doc/user-guide.pdf`

Credits
=======

The authors
