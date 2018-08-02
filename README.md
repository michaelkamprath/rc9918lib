# rc9918lib
A C library for using the [TMS9918A video board](https://github.com/jblang/rc9918) on a [RC2014 retrocomputer](https://rc2014.co.uk).

This library is still verymuch a work in progress. It has been tested against the rc9918 rev 1 board on a RC2014 running RomWBW v2.9.1-pre.5.

## Links
* [RC9918 Board](https://github.com/jblang/rc9918)
* [RC2014 Retrocomputer](https://rc2014.co.uk)
* [z88dk Z80 Cross Compiler](https://www.z88dk.org/)

## Building
To build this library, you first must have the [z88dk Z80 cross compiler](https://www.z88dk.org/) installed. 

Right now, only the examples can be built as stand alone CP/M apps. To build all examples, in the repository's root directory type:
```
make all
```
The build examples will be placed in the `examples/bin` directory. 

## Basic Design
The library created around a `context` handle, which is created when you initialize a board. To the user, the `context` handle is opaque, but internally it contains  state of the rc9918 board it was configured for. This means it is possible to have more than one active `context` in an application. Each `context` should be initialized to an unique set of ports. Right now the library is set up to permit only 2 active contexts, but more can be enabled by setting a higher value to the compiler macro `RC9918_CONTEXT_ARRAY_SIZE`. 

Each call to the library requires that a `context` handle be passed to it, and the function will operate on the board that `context` represents. 

## To Do

1. Complete library for all graphic modes of the TMS9918A, possibly rethinking the library's API.
2. Create and example or two for each graphics mode.
3. Update library for forthcoming revision 2 of the rc9918 board, notably to enable interrupts.
4. Update Makefile to allow compile time overriding of `RC9918_CONTEXT_ARRAY_SIZE` value. 
5. Optimize, optimize, optimize 
6. Make default fonts optionally compiled to save space is not needed
