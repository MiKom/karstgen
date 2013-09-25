karstgen
========

Karst cave generator. This project is accompanying my master's thesis in thesis repo.

Installation
------------

### Dependencies

On Ubuntu 13.04 karstgen requires following packages to
compile successfully:

* libboost-program-options-dev
* cmake
* libglm-dev
* OpenCL runtime (for nvidia it's shipped with driver in package nvidia-current)

For test suite to work *libgtest-dev* must also be installed and compiled.

*libavr.so* that is built from https://github.com/MiKom/vorticity must be
located in */usr/local/lib*

### Compilation

To do out-of-tree build create build directory and invoke:

	cmake /path/to/sources
	make

To build documentation invoke:

	make doc

To runt test suite invoke

	make test

Running
-------

Karstgen is made of two executables *blobber* and *mcblob*. Both of them provide
help messages when invoked with --help option.

To run blobber with synthetic input presented in the thesis invoke in build
directory:

	cat ./examples/blobber/synthetic.in | ./blobber -p 15 -s 10 | ./mcblob -o ./out.obj
