#include "VideoGenerator.h"

#include "ImageLoader.h"

#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

using namespace cv;
using namespace std;

bool VideoGenerator::createVideoFromFrames(const string& framesFolder,
                                           const string& outputPath,
                                           int fps)
{
    vector<Mat> frames;

    for (int index = 0; index <= 999; index++)
    {
        string framePath = ImageLoader::framePath(framesFolder, "anaglyph_depth", index);

        if (!ImageLoader::fileExists(framePath))
        {
            if (index == 0)
            {
                cout << "Nu exista primul frame: " << framePath << endl;
            }
            break;
        }

        Mat frame = imread(framePath, IMREAD_COLOR);
        if (frame.empty())
        {
            cout << "Nu pot citi frame-ul: " << framePath << endl;
            continue;
        }

        frames.push_back(frame);
    }

    if (frames.empty())
    {
        cout << "Nu exista cadre anaglife cu depth map in: " << framesFolder << endl;
        return false;
    }

    Size frameSize(frames[0].cols, frames[0].rows);
    VideoWriter writer(outputPath,
                       VideoWriter::fourcc('M', 'J', 'P', 'G'),
                       fps,
                       frameSize,
                       true);

    if (!writer.isOpened())
    {
        cout << "Nu pot crea video-ul: " << outputPath << endl;
        return false;
    }

    for (const Mat& frame : frames)
    {
        if (frame.size() != frameSize)
        {
            cout << "Frame ignorat deoarece are alta dimensiune." << endl;
            continue;
        }

        writer.write(frame);
    }

    writer.release();

    cout << "Video generat: " << outputPath << endl;
    cout << "Numar cadre: " << frames.size() << ", FPS: " << fps << endl;
    return true;
}
