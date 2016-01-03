![rosc logo](http://web1.synapticon.com/pub/rosc-Logo_150.png)
====
rosc is a dependency-free ROS client implementation in ANSI C (catkin required to build, but not on the target system to run the node) which aims to support small embedded systems as well as any operating system. 
Its long-term goal is to become a hardened, efficient and highly portable implementation of the ROS middleware, 
making it a good choice for use in industrial applications or product development.

rosc alpha supports subscribers and publishers, with some known issues with nested messages currently remaining, maybe also with dynamic arrays.

See Doxygen Documentation at:
http://synapticon.github.io/rosc/


How to try it. 
===
Currently it is still an alpha. To try it out you have to do the following:

  mkdir ~/rosc_ws
  
  cd ~/rosc_ws 
  
  git clone git@github.com:synapticon/rosc.git
  
  cd rosc
  
  rm CMakeLists.txt
  
  catkin_init_workspace
  
  cd /opt/ros/indigo/etc/ros/genmsg
  
  sudo touch genc
  
  cd /opt/ros/indigo/lib
  
  sudo ln -s ~/rosc_ws/rosc/genc
  
  cd ~/rosc_ws
  
  catkin_make --source rosc
