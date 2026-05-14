#include "ImageLoader.h"

#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>

using namespace cv;
using namespace std;

bool ImageLoader::fileExists(const string& path)
{
    return filesystem::exists(path);
}

bool ImageLoader::loadGrayscaleImage(const string& path, Mat& image)
{
    if (!fileExists(path))
    {
        cout << "Fisierul nu exista: " << path << endl;
        return false;
    }

    image = imread(path, IMREAD_UNCHANGED);

    if (image.empty())
    {
        cout << "Imaginea nu poate fi citita: " << path << endl;
        return false;
    }

    if (image.type() != CV_8UC1)
    {
        cout << "Imaginea trebuie sa fie grayscale pe 8 biti/pixel: " << path << endl;
        cout << "Tip OpenCV primit: " << image.type() << endl;
        return false;
    }

    return true;
}

bool ImageLoader::loadStereoPair(const string& leftPath,
                                 const string& rightPath,
                                 Mat& leftImage,
                                 Mat& rightImage)
{
    if (!loadGrayscaleImage(leftPath, leftImage))
    {
        return false;
    }

    if (!loadGrayscaleImage(rightPath, rightImage))
    {
        return false;
    }

    if (leftImage.size() != rightImage.size())
    {
        cout << "Imaginile stereo nu au aceeasi dimensiune." << endl;
        cout << "Left: " << leftImage.cols << "x" << leftImage.rows << endl;
        cout << "Right: " << rightImage.cols << "x" << rightImage.rows << endl;
        return false;
    }

    return true;
}

string ImageLoader::framePath(const string& folder, const string& prefix, int index)
{
    stringstream ss;
    ss << folder << "/" << prefix << "_" << setw(3) << setfill('0') << index << ".bmp";
    return ss.str();
}

vector<int> ImageLoader::findAvailableFrameIndices(const string& leftFolder,
                                                   const string& rightFolder)
{
    vector<int> indices;

    for (int index = 0; index <= 999; index++)
    {
        string leftPath = framePath(leftFolder, "left", index);
        string rightPath = framePath(rightFolder, "right", index);

        if (fileExists(leftPath) && fileExists(rightPath))
        {
            indices.push_back(index);
        }
    }

    return indices;
}
