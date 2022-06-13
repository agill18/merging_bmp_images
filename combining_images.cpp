#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
using namespace std;

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned int LONG;

struct BITMAPFILEHEADER
{
    WORD bfType;      //specifies the file type
    DWORD bfSize;     //specifies the size in bytes of the bitmap file
    WORD bfReserved1; //reserved; must be 0
    WORD bfReserved2; //reserved; must be 0
    DWORD bfOffBits;  //species the offset in bytes from the bitmapfileheader to the bitmap bits
};

struct BITMAPINFOHEADER
{
    DWORD biSize;         //specifies the number of bytes required by the struct LONG biWidth; //specifies width in pixels
    LONG biWidth;         //specifies width in pixels
    LONG biHeight;        //species height in pixels
    WORD biPlanes;        //specifies the number of color planes, must be 1
    WORD biBitCount;      //specifies the number of bit per pixel DWORD biCompression;//spcifies the type of compression
    DWORD biCompression;  //spcifies the type of compression
    DWORD biSizeImage;    //size of image in bytes
    LONG biXPelsPerMeter; //number of pixels per meter in x axis LONG biYPelsPerMeter; //number of pixels per meter in y axis DWORD biClrUsed; //number of colors used by th ebitmap
    LONG biYPelsPerMeter; //number of pixels per meter in y axis
    DWORD biClrUsed;      //number of colors used by th ebitmap
    DWORD biClrImportant; //number of colors that are important
};

BYTE get_color(BYTE *imgData, int x_pixel, int y_pixel, int colorNum, int realWidthBytes)
{
    BYTE color = imgData[x_pixel * 3 + y_pixel * realWidthBytes + colorNum];
    return color;
}

BYTE get_color_bilinear(BYTE *imgData, float x, float y, int imgRealWidth, int colorNum, int height, int width)
{
    int x1 = x;
    int x2 = x1 + 1;
    int y1 = y;
    int y2 = y1 + 1;
    float dx = x - x1;
    float dy = y - y1;

    if (x2 >= width) {
        x2 = x1; 
    }
    if (y2 >= height) { 
        y2 = y1; 
    }


    BYTE color_left_upper = get_color(imgData, x1, y2, colorNum, imgRealWidth);
    BYTE color_right_upper = get_color(imgData, x2, y2, colorNum, imgRealWidth);
    BYTE color_left_lower = get_color(imgData, x1, y1, colorNum, imgRealWidth);
    BYTE color_right_lower = get_color(imgData, x2, y1, colorNum, imgRealWidth);

    BYTE color_left = color_left_upper * (1 - dy) + color_left_lower * dy;
    BYTE color_right = color_right_upper * (1 - dy) + color_right_lower * dy;    
    BYTE color_result = color_left * (dx) + color_right * (1 - dx);
    
    return color_result;
}

int main(int argc, char *argv[])
{
    // error checking for correct number of arguments
    if (argc != 5)
    {
        printf("Incorrect number of arguments \n");
        printf("Input should look like: [imagefile1] [imagefile2] [ratio] [outputfile] \n");
        return 0;
    }

    // error checking for correct ratio
    float ratio = atof(argv[3]);
    if (ratio > (float)1.0 || (float)ratio < 0)
    {
        printf("Please enter a valid ratio between 0 and 1 \n");
        return 0;
    }

    // error checking for correct name of file 1
    FILE *file1 = fopen(argv[1], "rb");
    if (!file1)
    {
        printf("First file given doesn't exist \n");
        return 0;
    }

    BITMAPFILEHEADER img1FH;
    BITMAPINFOHEADER img1IH;

    // reading in image 1 file header
    fread(&img1FH.bfType, sizeof(short), 1, file1);
    fread(&img1FH.bfSize, sizeof(int), 1, file1);
    fread(&img1FH.bfReserved1, sizeof(short), 1, file1);
    fread(&img1FH.bfReserved2, sizeof(short), 1, file1);
    fread(&img1FH.bfOffBits, sizeof(int), 1, file1);

    // reading in image 1 info header
    fread(&img1IH, sizeof(img1IH), 1, file1);

    // constructing the array with pixel information for image 1
    unsigned int img1Size = img1IH.biSizeImage;
    BYTE *img1Data = (BYTE *)malloc(img1Size);
    if (img1Data == NULL)
    {
        printf("Not enough space in system to store pixels \n");
        return 0;
    }
    fread(img1Data, img1Size, 1, file1);
    fclose(file1);

    // error checking for correct name of file 2
    FILE *file2 = fopen(argv[2], "rb");
    if (!file2)
    {
        printf("Second file given doesn't exist \n");
        return 0;
    }

    BITMAPFILEHEADER img2FH;
    BITMAPINFOHEADER img2IH;

    // reading in image 2 file header
    fread(&img2FH.bfType, sizeof(short), 1, file2);
    fread(&img2FH.bfSize, sizeof(int), 1, file2);
    fread(&img2FH.bfReserved1, sizeof(short), 1, file2);
    fread(&img2FH.bfReserved2, sizeof(short), 1, file2);
    fread(&img2FH.bfOffBits, sizeof(int), 1, file2);

    // reading in image 2 info header
    fread(&img2IH, sizeof(img2IH), 1, file2);

    // constructing the array with pixel information for image 2
    int img2Size = img2IH.biSizeImage;
    BYTE *img2Data = (BYTE *)malloc(img2Size);
    if (img2Data == NULL)
    {
        printf("Not enough space in system to store pixels \n");
        return 0;
    }
    fread(img2Data, img2Size, 1, file2);
    fclose(file2);

    // finding the bigger image and finding the realWidth of the images
    int sizeBiggerImage = img1Size;
    int bigger = 1; // 1 means first pic was bigger, 2 means second pic was bigger
    BYTE *biggerImageData = img1Data;
    BYTE *smallerImageData = img2Data;
    int biggerImageBiWidth = img1IH.biWidth;
    int biggerImageBiHeight = img1IH.biHeight;
    int smallerImageBiWidth = img2IH.biWidth;
    int smallerImageBiHeight = img2IH.biHeight;
    BITMAPINFOHEADER biggerIH = img1IH;
    BITMAPFILEHEADER biggerFH = img1FH;

    if (img1IH.biWidth < img2IH.biWidth)
    {
        bigger = 2;
        sizeBiggerImage = img2Size;
        biggerImageBiWidth = img2IH.biWidth;
        biggerImageBiHeight = img2IH.biHeight;
        smallerImageBiWidth = img1IH.biWidth;
        smallerImageBiHeight = img1IH.biHeight;
        biggerImageData = img2Data;
        smallerImageData = img1Data;
        biggerIH = img2IH;
        biggerFH = img2FH;
    }

    int biggerRealWidthBytes = img1IH.biWidth * 3;
    int smallerRealWidthBytes = img2IH.biWidth * 3;
    if (biggerRealWidthBytes % 4 != 0)
    {
        biggerRealWidthBytes += (4 - biggerRealWidthBytes % 4);
    }
    if (smallerRealWidthBytes % 4 != 0)
    {
        smallerRealWidthBytes += (4 - smallerRealWidthBytes % 4);
    }
    if (bigger == 2)
    {
        int temp = biggerRealWidthBytes;
        biggerRealWidthBytes = smallerRealWidthBytes;
        smallerRealWidthBytes = temp;
    }

    // constructing the result array
    BYTE *outputData = (BYTE *)malloc(sizeBiggerImage);

    for (int x = 0; x < biggerImageBiWidth; x++)
    {
        for (int y = 0; y < biggerImageBiHeight; y++)
        {
            BYTE b_imgB = get_color(biggerImageData, x, y, 0, biggerRealWidthBytes);
            BYTE g_imgB = get_color(biggerImageData, x, y, 1, biggerRealWidthBytes);
            BYTE r_imgB = get_color(biggerImageData, x, y, 2, biggerRealWidthBytes);

            float xs = x * ((float)smallerImageBiWidth / (float)biggerImageBiWidth);
            float ys = y * ((float)smallerImageBiHeight / (float)biggerImageBiHeight);

            BYTE b_imgS = get_color_bilinear(smallerImageData, xs, ys, smallerRealWidthBytes, 0, smallerImageBiHeight, smallerImageBiWidth);
            BYTE g_imgS = get_color_bilinear(smallerImageData, xs, ys, smallerRealWidthBytes, 1, smallerImageBiHeight, smallerImageBiWidth);
            BYTE r_imgS = get_color_bilinear(smallerImageData, xs, ys, smallerRealWidthBytes, 2, smallerImageBiHeight, smallerImageBiWidth);

            // apply ratio to both first and second colors and then mix colors
            BYTE b_result = b_imgB * ratio + b_imgS * (1 - ratio);
            BYTE g_result = g_imgB * ratio + g_imgS * (1 - ratio);
            BYTE r_result = r_imgB * ratio + r_imgS * (1 - ratio);

            if (bigger == 2)
            {
                b_result = b_imgS * ratio + b_imgB * (1 - ratio);
                g_result = g_imgS * ratio + g_imgB * (1 - ratio);
                r_result = r_imgS * ratio + r_imgB * (1 - ratio);
            }

            outputData[x * 3 + y * biggerRealWidthBytes + 0] = b_result;
            outputData[x * 3 + y * biggerRealWidthBytes + 1] = g_result;
            outputData[x * 3 + y * biggerRealWidthBytes + 2] = r_result;
        }
    }

    // writing headers and pixel information to new file
    FILE *outputFile = fopen(argv[4], "wb");
    fwrite(&biggerFH.bfType, sizeof(short), 1, outputFile);
    fwrite(&biggerFH.bfSize, sizeof(int), 1, outputFile);
    fwrite(&biggerFH.bfReserved1, sizeof(short), 1, outputFile);
    fwrite(&biggerFH.bfReserved2, sizeof(short), 1, outputFile);
    fwrite(&biggerFH.bfOffBits, sizeof(int), 1, outputFile);

    fwrite(&biggerIH, sizeof(biggerIH), 1, outputFile);

    fwrite(outputData, sizeBiggerImage, 1, outputFile);

    fclose(outputFile);

    // freeing the space taken up by the arrays used to store pixels
    free(img1Data);
    free(img2Data);
    free(outputData);

    return 0;
}