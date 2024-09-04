#!/bin/sh

export FASTRTPS_DEFAULT_PROFILES_FILE=${PWD}/my_fastdds.xml
export RMW_FASTRTPS_USE_QOS_FROM_XML=1
export RMW_IMPLEMENTATION=rmw_fastrtps_cpp
