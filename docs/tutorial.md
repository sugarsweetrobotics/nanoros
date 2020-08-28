# Tutorial

# Using Topic In Out (for Windows users)

Open your "command prompt".
First, you must start "nanoroscore" process. Which is the main server for your ROS system.
```
> nanoroscore
```

Next, open another "command prompt".
Start topic publisher with...
```
> nanorostopic pub /test_topic std_msgs/Int32 ^"{\^"data\^": 1234}^" -r 1.0
```
"-r" option is used with continuously publishing rate [Hz].

Open another "command prompt" to check the topic.
```
> nanorostopic list
```
This command will output the list of topic like
```
/test_topic
```

Then, you can echo the data of topic 
```
> nanorostopic echo /test_topic
```

# Beginner Tutorial

## Building talker_listener project
```
> git clone https://github.com/sugarsweetrobotics/nanoros_beginner_tutorial
> cd nanoros_beginner_tutorial/talker_listener
> cmake . -D "Visual Studio 2019 14"
> start talker_listener.sln
```

Then, build with your Visual Studio

## Using listener
Open command prompt,
```
> nanoroscore
```

Another command prompt,
```
> nanorostopic pub /hello std_msgs/String ^"{\^"data\^": \^"Hello nanoros!\^"}^"
```

Then, select your Visual Studio and set "listener" project as "a Startup Project" by right-clicking menu of the listener project.
Do debug the listener project. You will get the message "Hello nanoros!".

