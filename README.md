# nanoros

## Description
Nano-size ROS (Robot Operating System) implimentation.

## Install

### Binary Install (Windows)
[github release paage](https://github.com/sugarsweetrobotics/nanoros/releases/)

### How to build (MacOS, Linux)
```
$ git clone https://github.com/sugarsweetrobotics/nanoros.git
$ cd nanoros
$ git submodule update --init --recursive
$ mkdir build
$ cd build
$ cmake ../
$ make 
```
This process does not generate the datatype packers. Please generate datatypes with following.

(You are in 'build' directory)
```
$ cd ../automatically_generated_packers
$ sh generate_packer_from_data_types.sh
```

Then build again

```
$ cd ../build
$ cmake ../
$ make
$ make install # Usually you need to add sudo
```

After install, please set environmental variables specific for nanoros.
(You'd better modify your '~/.bash_profile')
```
$ export NANOROS_PACKER_DIR=/usr/local/share/nanoros/packers
```


### How to use

#### Environemntal Variables
If you want to communicate with another host in which ROS system is launched, you need to set Environmental variables:
```
$ export ROS_MASTER_URI=http://{your roscore host ip address}:{your roscore host port number}
$ export ROS_IP={your machine IP address that is installed nanoros}
```

#### Add Data Types
Currently nanoros searches Data type packer in CURRENT DIRECTORY and $NANOROS_PACKER_DIR. 
The "packer" is serializer/deserializer for ROS datatype.
In the standard build process, the pre-installed data-type packers are placed in {nanoros_dir}/build/bin.


##### Use topics
```
$ nanorostopic list # This will show rostopic list
```

```
$ nanorostopic echo /hello # This will echo topic data
```

```
$ nanorostopic pub /hello std_msgs/Int32 '{"data": 4}' # This will publish topic
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
  

## Platform
Windows
MacOS
Linux


