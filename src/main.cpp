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

void convertDatasetToBmp(const string& projectRoot, const string& datasetName)
{
    string rawPath = projectRoot + "/data/raw/" + datasetName + "/";
    string leftPath = projectRoot + "/data/left/";
    string rightPath = projectRoot + "/data/right/";
    string depthPath = projectRoot + "/data/output/depth_maps/";

    filesystem::create_directories(leftPath);
    filesystem::create_directories(rightPath);
    filesystem::create_directories(depthPath);

    int baseline = 2; 
    int generatedFrames = 0;

    for (int i = 0; i <= 999; i++)
    {
        char leftFile[100];
        char rightFile[100];
        Mat left, right;
        bool found = false;

        const char* extensions[] = {".ppm", ".jpg", ".jpeg", ".png", ".bmp"};
        for (const char* ext : extensions)
        {
            // Format 1: imX (multi-view, constant baseline)
            snprintf(leftFile, sizeof(leftFile), "im%d%s", i, ext);
            snprintf(rightFile, sizeof(rightFile), "im%d%s", i + baseline, ext);
            left = imread(rawPath + leftFile, IMREAD_GRAYSCALE);
            right = imread(rawPath + rightFile, IMREAD_GRAYSCALE);
            if (!left.empty() && !right.empty()) { found = true; break; }

            // Format 2: image0_X / image1_X (sequence format, might start at 0 or 1)
            // check i
            snprintf(leftFile, sizeof(leftFile), "image0_%d%s", i, ext);
            snprintf(rightFile, sizeof(rightFile), "image1_%d%s", i, ext);
            left = imread(rawPath + leftFile, IMREAD_GRAYSCALE);
            right = imread(rawPath + rightFile, IMREAD_GRAYSCALE);
            if (!left.empty() && !right.empty()) { found = true; break; }
            
            // check i+1 (if index starts at 1, like boats)
            if (i == 0) {
                // If i=0 fails, we don't break, the loop will eventually reach i=1 which checks image0_1
                // Wait, if it checks i=1 on the next iteration, we just need to NOT break on i=0 if it's missing, 
                // but if we don't break, the loop stops at `if (!found) break;`.
                // Let's just check i+1 so that if i=0 is missing but i=1 exists, we count it.
                snprintf(leftFile, sizeof(leftFile), "image0_%d%s", i+1, ext);
                snprintf(rightFile, sizeof(rightFile), "image1_%d%s", i+1, ext);
                left = imread(rawPath + leftFile, IMREAD_GRAYSCALE);
                right = imread(rawPath + rightFile, IMREAD_GRAYSCALE);
                if (!left.empty() && !right.empty()) { found = true; break; }
                
                snprintf(leftFile, sizeof(leftFile), "im%d%s", i+1, ext);
                snprintf(rightFile, sizeof(rightFile), "im%d%s", i+1+baseline, ext);
                left = imread(rawPath + leftFile, IMREAD_GRAYSCALE);
                right = imread(rawPath + rightFile, IMREAD_GRAYSCALE);
                if (!left.empty() && !right.empty()) { found = true; break; }
            }
        }

        if (!found)
        {
            if (i == 0) continue; // If 0 doesn't exist, maybe it starts at 1. If 1 doesn't exist, it will break.
            break;
        }

        char leftName[100];
        char rightName[100];
        snprintf(leftName, sizeof(leftName), "left_%03d.bmp", generatedFrames);
        snprintf(rightName, sizeof(rightName), "right_%03d.bmp", generatedFrames);

        imwrite(leftPath + leftName, left);
        imwrite(rightPath + rightName, right);

        cout << "Creat perechea " << generatedFrames << ": " << leftFile << " + " << rightFile << endl;
        generatedFrames++;
    }

    if (generatedFrames == 0)
    {
        cout << "Eroare la incarcarea datasetului: " << datasetName << " (nu am gasit .ppm)" << endl;
    }
    else
    {
        cout << "Dataset convertit: " << datasetName << " (" << generatedFrames << " cadre generat(e))" << endl;
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

        Mat anaglyph = AnaglyphGenerator::createSimpleAnaglyph(left, right);
        string framePath = ImageLoader::framePath(framesFolder, "anaglyph", index);

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
                convertDatasetToBmp(projectPath(), "boats");
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
