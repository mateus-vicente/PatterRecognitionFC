# PatterRecognitionFC

requires numpy, openCV and working pyROOT (tested with v6.04)

tested with miniconda with python 2.7

usage : python -i PR_Pixels.py -c chip.bmp -s substrate.bmp

where chip and substrate are matching pixel detectors image for a dual microscope. The software will try to calculate the offset between pixels in both image. 

Some parameters in the script might need to be tweeked according to your image , see the comments in the file   



