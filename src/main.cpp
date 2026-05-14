#include <iostream>
#include <filesystem>
#include <opencv2/opencv.hpp>

#include "AnaglyphGenerator.h"
#include "DisparityMap.h"
#include "ImageLoader.h"
#include "VideoGenerator.h"

using namespace std;
using namespace cv;

string projectPath()
{
    return string(PROJECT_ROOT);
}

void testOpenCV()
{
    Mat img(300, 500, CV_8UC3, Scalar(255, 255, 255));

    putText(
        img,
        "OpenCV works!",
        Point(80, 150),
        FONT_HERSHEY_SIMPLEX,
        1.2,
        Scalar(0, 0, 255),
        2
    );

    imshow("Test OpenCV", img);
    waitKey(0);
}

void convertDatasetToBmp(const string& projectRoot, const string& datasetName, int index)
{
    string rawPath = projectRoot + "/data/raw/" + datasetName + "/";
    string leftPath = projectRoot + "/data/left/";
    string rightPath = projectRoot + "/data/right/";
    string depthPath = projectRoot + "/data/output/depth_maps/";

    filesystem::create_directories(leftPath);
    filesystem::create_directories(rightPath);
    filesystem::create_directories(depthPath);

    Mat left = imread(rawPath + "im2.png", IMREAD_GRAYSCALE);
    Mat right = imread(rawPath + "im6.png", IMREAD_GRAYSCALE);
    Mat disp = imread(rawPath + "disp2.png", IMREAD_GRAYSCALE);

    if (left.empty() || right.empty())
    {
        cout << "Eroare la incarcarea datasetului: " << datasetName << endl;
        return;
    }

    char leftName[100];
    char rightName[100];
    char depthName[100];

    sprintf(leftName, "left_%03d.bmp", index);
    sprintf(rightName, "right_%03d.bmp", index);
    sprintf(depthName, "ground_truth_%03d.bmp", index);

    imwrite(leftPath + leftName, left);
    imwrite(rightPath + rightName, right);

    if (!disp.empty())
    {
        imwrite(depthPath + depthName, disp);
    }

    cout << "Dataset convertit: " << datasetName << endl;
    cout << "Creat: " << leftPath + leftName << endl;
    cout << "Creat: " << rightPath + rightName << endl;

    if (!disp.empty())
    {
        cout << "Creat: " << depthPath + depthName << endl;
    }
}

void generateSimpleAnaglyphFromFiles()
{
    string root = projectPath();
    string leftPath = root + "/data/left/left_000.bmp";
    string rightPath = root + "/data/right/right_000.bmp";
    string outputFolder = root + "/data/output/frames";
    string outputPath = outputFolder + "/anaglyph_000.bmp";

    Mat left;
    Mat right;

    if (!ImageLoader::loadStereoPair(leftPath, rightPath, left, right))
    {
        return;
    }

    filesystem::create_directories(outputFolder);

    Mat anaglyph = AnaglyphGenerator::createSimpleAnaglyph(left, right);

    if (!imwrite(outputPath, anaglyph))
    {
        cout << "Eroare: nu pot salva anagliful in: " << outputPath << endl;
        return;
    }

    cout << "Anaglif simplu salvat in: " << outputPath << endl;

    imshow("Simple anaglyph", anaglyph);
    waitKey(0);
    destroyAllWindows();
}

void generateAnaglyphUsingDepthMap()
{
    string root = projectPath();
    string leftPath = root + "/data/left/left_000.bmp";
    string rightPath = root + "/data/right/right_000.bmp";
    string depthPath = root + "/data/output/depth_maps/depth_000.bmp";
    string outputFolder = root + "/data/output/frames";
    string outputPath = outputFolder + "/anaglyph_depth_000.bmp";

    Mat left;
    Mat right;
    Mat depth;

    if (!ImageLoader::loadStereoPair(leftPath, rightPath, left, right))
    {
        return;
    }

    if (!ImageLoader::loadGrayscaleImage(depthPath, depth))
    {
        cout << "Ruleaza mai intai optiunea 4 pentru a genera depth_000.bmp." << endl;
        return;
    }

    if (left.size() != depth.size())
    {
        cout << "Eroare: depth map-ul nu are aceeasi dimensiune cu imaginile stereo." << endl;
        return;
    }

    filesystem::create_directories(outputFolder);

    Mat anaglyph = AnaglyphGenerator::createAnaglyphWithDisparity(left, right, depth);

    if (!imwrite(outputPath, anaglyph))
    {
        cout << "Eroare: nu pot salva anagliful cu depth map in: " << outputPath << endl;
        return;
    }

    cout << "Anaglif cu depth map salvat in: " << outputPath << endl;

    imshow("Anaglyph using depth map", anaglyph);
    waitKey(0);
    destroyAllWindows();
}

void processFullStereoSequence()
{
    string root = projectPath();
    string leftFolder = root + "/data/left";
    string rightFolder = root + "/data/right";
    string framesFolder = root + "/data/output/frames";
    string depthFolder = root + "/data/output/depth_maps";

    filesystem::create_directories(framesFolder);
    filesystem::create_directories(depthFolder);

    int maxDisparity = 64;
    int windowSize = 7;
    int processedFrames = 0;

    for (int index = 0; index <= 999; index++)
    {
        string leftPath = ImageLoader::framePath(leftFolder, "left", index);
        string rightPath = ImageLoader::framePath(rightFolder, "right", index);

        if (!ImageLoader::fileExists(leftPath) || !ImageLoader::fileExists(rightPath))
        {
            if (index == 0)
            {
                cout << "Nu exista prima pereche stereo left_000.bmp / right_000.bmp." << endl;
            }
            break;
        }

        Mat left;
        Mat right;

        cout << "Procesez perechea " << index << endl;

        if (!ImageLoader::loadStereoPair(leftPath, rightPath, left, right))
        {
            break;
        }

        Mat disparity = DisparityMap::computeDisparityMap(left, right, maxDisparity, windowSize);
        Mat depth = DisparityMap::normalizeDisparityMap(disparity);

        if (depth.empty())
        {
            break;
        }

        string depthPath = ImageLoader::framePath(depthFolder, "depth", index);
        if (!imwrite(depthPath, depth))
        {
            cout << "Eroare: nu pot salva depth map-ul in: " << depthPath << endl;
            break;
        }

        Mat anaglyph = AnaglyphGenerator::createAnaglyphWithDisparity(left, right, depth);
        string framePath = ImageLoader::framePath(framesFolder, "anaglyph_depth", index);

        if (!imwrite(framePath, anaglyph))
        {
            cout << "Eroare: nu pot salva frame-ul anaglif in: " << framePath << endl;
            break;
        }

        cout << "Salvat: " << depthPath << endl;
        cout << "Salvat: " << framePath << endl;
        processedFrames++;
    }

    cout << "Procesare secventa terminata. Cadre procesate: " << processedFrames << endl;
}

void generateFinalVideo()
{
    string root = projectPath();
    string framesFolder = root + "/data/output/frames";
    string outputPath = root + "/data/output/anaglyph_video.avi";
    int fps = 2;

    filesystem::create_directories(root + "/data/output");
    VideoGenerator::createVideoFromFrames(framesFolder, outputPath, fps);
}

int main()
{
    int option;

    do
    {
        cout << "\n===== Project_PI - 3D Anaglyph Video =====\n";
        cout << "1 - Test OpenCV\n";
        cout << "2 - Convert raw datasets to BMP grayscale\n";
        cout << "3 - Generate simple anaglyph\n";
        cout << "4 - Generate disparity/depth map\n";
        cout << "5 - Generate anaglyph using depth map\n";
        cout << "6 - Process full stereo sequence\n";
        cout << "7 - Generate final video\n";
        cout << "0 - Exit\n";
        cout << "Option: ";
        cin >> option;

        switch (option)
        {
            case 1:
                testOpenCV();
                break;

            case 2:
                convertDatasetToBmp(projectPath(), "teddy", 0);
                convertDatasetToBmp(projectPath(), "cones", 1);
                break;

            case 3:
                generateSimpleAnaglyphFromFiles();
                break;

            case 4:
                DisparityMap::generateDisparityMapFromFiles(projectPath());
                break;

            case 5:
                generateAnaglyphUsingDepthMap();
                break;

            case 6:
                processFullStereoSequence();
                break;

            case 7:
                generateFinalVideo();
                break;

            case 0:
                cout << "Exit.\n";
                break;

            default:
                cout << "Invalid option.\n";
                break;
        }

    } while (option != 0);

    return 0;
}
