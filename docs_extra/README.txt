In this folder you can find additional documents that are not specific to Astrial,
but will allow a better optimization of data processing also on astrial.

mobilenetv2-yuv_detail.en.pdf: 
==============================
a document showing how a careful measure of 
the bandwidth across the PCie bus of imx8 can improve the full pipeline performance. 
Balancing CPU and NPU while choosing the correct image format can improve performance
thanks to modifications to the network input structure.
Using a different image pixel format (YCbCr 4:2:0 vs RGB 4:4:4) is not having an impact
on the final accuracy
