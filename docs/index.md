
# Description
Nano-size ROS (Robot Operating System) implimentation available in multiple platforms, Windows, MacOS, and Linux.

# Install

### Binary Install (Windows)
[github release paage](https://github.com/sugarsweetrobotics/nanoros/releases/)

### Source Install (MacOS)

```
$ git clone https://github.com/sugarsweetrobotics/nanoros.git
$ cd nanoros
$ git submodule update --init --recursive
$ mkdir build
$ cd build
$ cmake ../
$ make
$ make install # maybe you need 'sudo'
```

After install, please set environmental variables specific for nanoros
```
$ export NANOROS_PACKER_DIR=/usr/local/share/nanoros/packers
```


# Tutorial
Please visit [tutorial page](tutorial)


# Current Status


* Client Side
  * Master API
  * Slave API
  * Parameter API (not implemented)

* Node Functionarities
  * Topic
    * Publisher
    * Subscriber
  * Service
    * Provider
    * Client
  * Parameter (not implemented)

* tools
  * nanoroscore
    * master_server
    * parameter_server (not implemented)
    * logging_server (not implemented)
  * nanorosnode (rosnode command) (not implemented)
  * nanorosmsg (not implemented)
  * nanorossrv (not implemented)
  * nanorostopic 
    * list
    * type
    * echo
    * pub
  * nanorosservice
    * list
    * type
    * call
  * nanorosmsgparser (partly implemented)
  * nanorossrvparser (not implemented)
  

# Platform
Windows
MacOS
Linux


