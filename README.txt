===============================================================================
Astrial Primer
===============================================================================

This is a basic primer leveraging ASTRIAL (www.astrial.ai) hardware.

ASTRIAL is an SoM based on two main components: NXP i.mx8m-plus CPU 
and Hailo H8 for NPU. The Hailo NPU allows an efficient inference 
of neural networks (especially CNN) for object detection and similar applications.

The first set of examples is about how to trigger a GPIO pin:
- from sysfs
- using gpiolib in C and python
- using hardware PWM ports

The second set of examples is about computer vision.
- how to capture an image from camera using GStreamer and OpenCV
- how to benchmark Hailo compiled networks for speed, data transfer 
  and latency using HailoRT library
- how to execute a full pipeline for object detection using GStreamer, 
  python or C++ code.

For additional information on NXP imx8m-plus CPU please refer to NXP official documentation:
https://www.nxp.com/products/processors-and-microcontrollers/arm-processors/i-mx-applications-processors/i-mx-8-applications-processors/i-mx-8m-plus-arm-cortex-a53-machine-learning-vision-multimedia-and-industrial-iot:IMX8MPLUS

For additional information on Hailo H8 NPU please refer to the HAilo website and GitHub repository:
https://github.com/hailo-ai
