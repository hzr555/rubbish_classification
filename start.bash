#!/usr/bin/env zsh
set -e
# killall roslaunch
source devel/setup.zsh
catkin_make
roslaunch launch/test.launch
