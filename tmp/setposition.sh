#! /usr/bin/zsh
if [ -z "`ls /tmp | grep "position.txt"`" ]; then
    echo "position is not exist"
else
    rm /tmp/position.txt
fi
cd /home/dji/Downloads/darknet-master
./darknet detect cfg/yolov3-tiny.cfg yolov3-tiny.weights ~/Downloads/123.jpg