# how to check camera capabilities:
# select the /dev/video(n) connected to your camera
 v4l2-ctl --list-formats-ext -d /dev/video2

