######################################################################
# RPi camera only allows RAW format, so we need to encode with jpegenc
######################################################################

# multi-frames: capture few frames for AWB on Astrial
gst-launch-1.0 v4l2src device=/dev/video3 num-buffers=3 ! 'video/x-raw,width=1920,height=1080,format=YUY2,framerate=30/1' ! multifilesink index=0 location=frame%04d.jpg sync=true async=false

# only one frame
# gst-launch-1.0 v4l2src device=/dev/video2 num-buffers=1 ! 'video/x-raw,width=1920,height=1080,format=YUY2' ! filesink location=image.jpg

