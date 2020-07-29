# nanoros

## Description
Nano-size ROS (Robot Operating System) implimentation.

### How to build
```
$ git clone https://github.com/sugarsweetrobotics/nanoros.git
$ cd nanoros
$ mkdir build
$ cmake ../
```


### How to use

#### Environemntal Variables
```
$ export ROS_MASTER_URI=http://{your roscore host ip address}:{your roscore host port number}
$ export ROS_IP={your machine IP address that is installed nanoros}
```

Currently nanoros searches Data type stub in CURRENT DIRECTORY. In the standard build process, the pre-installed data-type stubs are placed in {nanoros_dir}/build/bin.

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
  * nanoroscore (not implemented)
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
  

## Platform
Windows
MacOS
Linux


