# PatterRecognitionFC

requires, openCV, cmake and working ROOT (tested with v6.04)

compile
usage : ./PixelShop [readout_chip_image.file] [substrate_image.file]
Once the maximum amout of pixels are found, press command (or ctrl) + P and click calculate.

where chip and substrate are matching pixel detectors image for a dual microscope. The software will try to calculate the offset between pixels in both image. 

Some parameters in the script might need to be tweeked according to your image , see the comments in the code.   
