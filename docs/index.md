
# Description
Nano-size ROS (Robot Operating System) implimentation available in multiple platforms, Windows, MacOS, and Linux.

# Install

### Binary Install (Windows)
Currently, exe installer can be built with this project.
Wait for a moment...please

### Source Install

```
$ git clone https://github.com/sugarsweetrobotics/nanoros.git
$ cd nanoros
$ git submodule update --init --recursive
$ mkdir build
$ cd build
$ cmake ../
$ make
```

# Tutorial
[tutorial for 0.0.1](tutorial)

### How to use

#### Environemntal Variables
If you want to communicate with another host in which ROS system is launched, you need to set Environmental variables:
```
$ export ROS_MASTER_URI=http://{your roscore host ip address}:{your roscore host port number}
$ export ROS_IP={your machine IP address that is installed nanoros}
```

#### Add Data Types
Currently nanoros searches Data type PACKER in CURRENT DIRECTORY and $NANOROS_PACKER_DIR. In the standard build process, the pre-installed data-type packers are placed in {nanoros_dir}/build/bin.

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



----

## Welcome to GitHub Pages

You can use the [editor on GitHub](https://github.com/sugarsweetrobotics/nanoros/edit/master/docs/index.md) to maintain and preview the content for your website in Markdown files.

Whenever you commit to this repository, GitHub Pages will run [Jekyll](https://jekyllrb.com/) to rebuild the pages in your site, from the content in your Markdown files.

### Markdown

Markdown is a lightweight and easy-to-use syntax for styling your writing. It includes conventions for

```markdown
Syntax highlighted code block

# Header 1
## Header 2
### Header 3

- Bulleted
- List

1. Numbered
2. List

**Bold** and _Italic_ and `Code` text

[Link](url) and ![Image](src)
```

For more details see [GitHub Flavored Markdown](https://guides.github.com/features/mastering-markdown/).

### Jekyll Themes

Your Pages site will use the layout and styles from the Jekyll theme you have selected in your [repository settings](https://github.com/sugarsweetrobotics/nanoros/settings). The name of this theme is saved in the Jekyll `_config.yml` configuration file.

### Support or Contact

Having trouble with Pages? Check out our [documentation](https://docs.github.com/categories/github-pages-basics/) or [contact support](https://github.com/contact) and we’ll help you sort it out.
>>>>>>> e60f18f325457ff54802f41b2e146e55f01f5909
