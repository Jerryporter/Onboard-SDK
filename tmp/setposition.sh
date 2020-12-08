#! /usr/bin/zsh
if [ -z "`ls /tmp | grep "position.txt"`" ]; then
    echo "position is not exist"
else
    rm /tmp/position.txt
fi
cd /home/chy/Desktop/img_position/cmake-build-debug
./img_position