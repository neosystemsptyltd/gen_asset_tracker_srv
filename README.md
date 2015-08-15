# gen_asset_tracker_srv
Generic asset tracking tracker : server listener software

# Introduction

This is the socket listener for the tracking software.

## Software

I know it is a bit uncoventional, but the POCO C++ libraries as well as the boost V1.58 libraries have been configured with the project. This is to ensure that the correct operation and access to the right libraries can always be guaranteed.

The project is somewhat large with the included libraries.

## Bulding

* refer to the POCO C++ libraries and Boost libraries for build instructions
* The neotracsrvtcp function can be built by running make in the neotracsrvtcp folder. The C++ project was done with Netbeans 8.0, so it might be easier for some to open it in the Netbeans IDE.

At this stage the build has only been tested on Ubuntu 14.04 32 bit edition, but it should work for most other systems.

# License

GNU AFFERO GENERAL PUBLIC LICENSE V3.0. See LICENSE file elsewhere in this project.
