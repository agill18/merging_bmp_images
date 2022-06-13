# merging_bmp_images
Program to blend two bmp images together into one image. 


## Input/Output

Input:  [imagefile1.bmp] [imagefile2.bmp] [ratio] [outputfile.bmp] 

* imagefile1.bmp - the name of the first bmp image file 
* imagefile2.bmp - the name of the second bmp image file 
* ratio - a decimal that determines how much of imagefile1 and imagefile2 will be in the result; a ratio of 0.5 means the resultimage will be a 50:50 mixture of both; 0.3 means 30% imagefile1 and 70% imagefile2
* outputfile.bmp - the bmp file where the blended image will be visible

Output: [outputfile] 


## How it works? 

* If both of the images have the same resolution/size, the program simply blends the pixels on the same x/y coordinates according to the given ratio 
* If both images don't have the same resolution/size, the program uses billinear interpolation to fit the smaller image to the size of the bigger image according to the given ratio 


