#include "DisparityMap.h"

#include "ImageLoader.h"

#include <climits>
#include <filesystem>
#include <iostream>

using namespace cv;
using namespace std;

Mat DisparityMap::computeDisparityMap(const Mat& left,
                                      const Mat& right,
                                      int maxDisparity,
                                      int windowSize)
{
    if (left.empty() || right.empty())
    {
        cout << "Eroare: imaginile pentru disparity map sunt goale." << endl;
        return Mat();
    }

    if (left.size() != right.size())
    {
        cout << "Eroare: imaginile pentru disparity map nu au aceeasi dimensiune." << endl;
        return Mat();
    }

    if (left.type() != CV_8UC1 || right.type() != CV_8UC1)
    {
        cout << "Eroare: imaginile trebuie sa fie grayscale pe 8 biti/pixel." << endl;
        return Mat();
    }

    if (windowSize < 3)
    {
        windowSize = 3;
    }

    if (windowSize % 2 == 0)
    {
        windowSize++;
    }

    if (maxDisparity < 1)
    {
        maxDisparity = 1;
    }

    if (maxDisparity > 255)
    {
        maxDisparity = 255;
    }

    int radius = windowSize / 2;
    Mat disparity(left.rows, left.cols, CV_8UC1, Scalar(0));

    for (int y = radius; y < left.rows - radius; y++)
    {
        for (int x = radius; x < left.cols - radius; x++)
        {
            int bestDisparity = 0;
            int bestCost = INT_MAX;

            for (int d = 0; d <= maxDisparity; d++)
            {
                if (x - d - radius < 0)
                {
                    continue;
                }

                int cost = 0;

                for (int wy = -radius; wy <= radius; wy++)
                {
                    for (int wx = -radius; wx <= radius; wx++)
                    {
                        int leftValue = left.at<uchar>(y + wy, x + wx);
                        int rightValue = right.at<uchar>(y + wy, x + wx - d);
                        cost += abs(leftValue - rightValue);
                    }
                }

                if (cost < bestCost)
                {
                    bestCost = cost;
                    bestDisparity = d;
                }
            }

            disparity.at<uchar>(y, x) = static_cast<uchar>(bestDisparity);
        }
    }

    return disparity;
}

Mat DisparityMap::normalizeDisparityMap(const Mat& disparity)
{
    if (disparity.empty())
    {
        cout << "Eroare: disparity map-ul este gol si nu poate fi normalizat." << endl;
        return Mat();
    }

    double minValue;
    double maxValue;
    minMaxLoc(disparity, &minValue, &maxValue);

    Mat normalized(disparity.rows, disparity.cols, CV_8UC1, Scalar(0));

    if (maxValue <= minValue)
    {
        return normalized;
    }

    for (int y = 0; y < disparity.rows; y++)
    {
        for (int x = 0; x < disparity.cols; x++)
        {
            int value = disparity.at<uchar>(y, x);
            int normalizedValue = static_cast<int>((value - minValue) * 255.0 / (maxValue - minValue));
            normalized.at<uchar>(y, x) = static_cast<uchar>(normalizedValue);
        }
    }

    return normalized;
}

void DisparityMap::generateDisparityMapFromFiles(const string& projectRoot)
{
    string leftPath = projectRoot + "/data/left/left_000.bmp";
    string rightPath = projectRoot + "/data/right/right_000.bmp";
    string outputFolder = projectRoot + "/data/output/depth_maps";
    string outputPath = outputFolder + "/depth_000.bmp";

    Mat left;
    Mat right;

    if (!ImageLoader::loadStereoPair(leftPath, rightPath, left, right))
    {
        return;
    }

    int maxDisparity = 64;
    int windowSize = 7;

    cout << "Generez disparity map cu SAD..." << endl;
    cout << "maxDisparity = " << maxDisparity << ", windowSize = " << windowSize << endl;

    Mat disparity = computeDisparityMap(left, right, maxDisparity, windowSize);
    Mat normalized = normalizeDisparityMap(disparity);

    if (normalized.empty())
    {
        return;
    }

    filesystem::create_directories(outputFolder);

    if (!imwrite(outputPath, normalized))
    {
        cout << "Eroare: nu pot salva depth map-ul in: " << outputPath << endl;
        return;
    }

    cout << "Depth map salvat in: " << outputPath << endl;

    imshow("Computed depth map", normalized);
    waitKey(0);
    destroyAllWindows();
}

Mat DisparityMap::computeSAD(const Mat& leftImage,
                             const Mat& rightImage,
                             int windowSize,
                             int maxDisparity)
{
    if (windowSize % 2 == 0)
    {
        windowSize++;
    }

    int radius = windowSize / 2;
    Mat rawDisparity(leftImage.rows, leftImage.cols, CV_8UC1, Scalar(0));

    for (int y = radius; y < leftImage.rows - radius; y++)
    {
        for (int x = radius + maxDisparity; x < leftImage.cols - radius; x++)
        {
            int bestDisparity = 0;
            int bestCost = INT_MAX;

            for (int d = 0; d <= maxDisparity; d++)
            {
                int cost = 0;

                for (int wy = -radius; wy <= radius; wy++)
                {
                    for (int wx = -radius; wx <= radius; wx++)
                    {
                        int leftValue = leftImage.at<uchar>(y + wy, x + wx);
                        int rightValue = rightImage.at<uchar>(y + wy, x + wx - d);
                        cost += abs(leftValue - rightValue);
                    }
                }

                if (cost < bestCost)
                {
                    bestCost = cost;
                    bestDisparity = d;
                }
            }

            rawDisparity.at<uchar>(y, x) = static_cast<uchar>(bestDisparity);
        }
    }

    return rawDisparity;
}

Mat DisparityMap::normalizeToGrayscale(const Mat& rawDisparity,
                                       int maxDisparity)
{
    Mat normalized(rawDisparity.rows, rawDisparity.cols, CV_8UC1, Scalar(0));

    if (maxDisparity <= 0)
    {
        return normalized;
    }

    for (int y = 0; y < rawDisparity.rows; y++)
    {
        for (int x = 0; x < rawDisparity.cols; x++)
        {
            int disparity = rawDisparity.at<uchar>(y, x);
            int value = disparity * 255 / maxDisparity;
            normalized.at<uchar>(y, x) = static_cast<uchar>(value);
        }
    }

    return normalized;
}
