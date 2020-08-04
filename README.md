# nanoros

## Description
Nano-size ROS (Robot Operating System) implimentation.

## Install

### Binary Install (Windows)
Currently, exe installer can be built with this project.
Wait for a moment...please

### How to build
```
$ git clone https://github.com/sugarsweetrobotics/nanoros.git
$ cd nanoros
$ git submodule update --init --recursive
$ mkdir build
$ cd build
$ cmake ../
$ make
```


### How to use

#### Environemntal Variables
If you want to communicate with another host in which ROS system is launched, you need to set Environmental variables:
```
$ export ROS_MASTER_URI=http://{your roscore host ip address}:{your roscore host port number}
$ export ROS_IP={your machine IP address that is installed nanoros}
```

#### Add Data Types
Currently nanoros searches Data type stub in CURRENT DIRECTORY and $NANOROS_STUB_DIR. In the standard build process, the pre-installed data-type stubs are placed in {nanoros_dir}/build/bin.

```
$ cd ${nanoros_dir}/build/bin
$ nanorostopic list # This will show rostopic list
$ nanorosservice list # This will show rosservice list
```

## Current Status


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
  * nanorostopic (rostopic command)
    * list
    * type
    * echo
    * pub
  * nanorosservice (rosservice command)
    * list
    * type
    * call
  * nanorosmsgparser (Msg file parser) (partly implemented)
  * nanorossrvparser (not implemented)
  

## Platform
Windows
MacOS
Linux


