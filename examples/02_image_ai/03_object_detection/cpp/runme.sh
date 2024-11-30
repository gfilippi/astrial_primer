#!/bin/bash

clear

if [ ! -d "./build/aarch64" ]; then
  echo
  echo "ERROR: you need to build first. Launch ./buildme.sh"
  echo
  exit
fi

CMD="./build/aarch64/vstream_detection_example_cpp -hef=./resources/yolov8s.hef -input=./resources/bus.jpg -num=1"

echo
echo "RUNNING: "
echo $CMD
echo
echo

./build/aarch64/vstream_detection_example_cpp -hef=./resources/yolov8s.hef -input=./resources/bus.jpg -num=1


# MULTIPLE RUNs to test FPS and LATENCY:
# ./build/aarch64/vstream_detection_example_cpp -hef=./resources/yolov8s.hef -input=./resources/bus.jpg -num=100
