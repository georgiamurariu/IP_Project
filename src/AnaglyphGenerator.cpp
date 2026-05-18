#include "AnaglyphGenerator.h"

using namespace cv;

Mat AnaglyphGenerator::createSimpleAnaglyph(const Mat& leftImage,
                                            const Mat& rightImage)
{
    Mat anaglyph(leftImage.rows, leftImage.cols, CV_8UC3);

    for (int y = 0; y < leftImage.rows; y++)
    {
        for (int x = 0; x < leftImage.cols; x++)
        {
            uchar leftPixel = leftImage.at<uchar>(y, x);
            uchar rightPixel = rightImage.at<uchar>(y, x);

            Vec3b color;
            color[0] = rightPixel; // B
            color[1] = rightPixel; // G
            color[2] = leftPixel;  // R

            anaglyph.at<Vec3b>(y, x) = color;
        }
    }

    return anaglyph;
}

Mat AnaglyphGenerator::createAnaglyphWithDisparity(const Mat& leftImage,
                                                   const Mat& rightImage,
                                                   const Mat& disparityMap)
{
    Mat anaglyph(leftImage.rows, leftImage.cols, CV_8UC3);
    int maxShift = 16;

    for (int y = 0; y < leftImage.rows; y++)
    {
        for (int x = 0; x < leftImage.cols; x++)
        {
            uchar leftPixel = leftImage.at<uchar>(y, x);
            uchar depthValue = disparityMap.at<uchar>(y, x);

            // Depth map-ul este normalizat 0..255. Il transformam intr-o deplasare mica.
            int shift = depthValue * maxShift / 255;
            int shiftedX = x - shift;

            uchar rightPixel = 0;
            if (shiftedX >= 0 && shiftedX < leftImage.cols)
            {
                // Sintetizam imaginea dreapta folosind imaginea stanga si depth map-ul
                rightPixel = leftImage.at<uchar>(y, shiftedX);
            }

            Vec3b color;
            color[0] = rightPixel; // B
            color[1] = rightPixel; // G
            color[2] = leftPixel;  // R

            anaglyph.at<Vec3b>(y, x) = color;
        }
    }

    return anaglyph;
}
