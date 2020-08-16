# Tutorial

## Using Topic In Out (for Windows users)

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

