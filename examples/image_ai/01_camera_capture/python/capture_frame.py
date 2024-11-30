from time import time
import cv2

# Create a new VideoCapture object, using "/dev/video3"
cam = cv2.VideoCapture(3)

# Initialise variables to store current time difference as well as previous time call value
previous = time()
delta = 0

frame_num=0

# Keep looping, save every N seconds (N=3)
while True:
    # Get the current time, increase delta and update the previous variable
    current = time()
    delta += current - previous
    previous = current

    # Check if 3 (or some other value) seconds passed
    if delta > 3:
        # Operations on image
        # Reset the time counter
        delta = 0

        # save and keep streaming
        _, img = cam.read()

        frame_num=frame_num+1
        filename="frame"+str(frame_num)+".jpg"
        print("save frame: {}".format(frame_num))
        cv2.imwrite(filename, img)

