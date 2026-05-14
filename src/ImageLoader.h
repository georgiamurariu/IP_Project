#ifndef PROJECT_PI_IMAGELOADER_H
#define PROJECT_PI_IMAGELOADER_H

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

class ImageLoader
{
public:
    static bool fileExists(const std::string& path);
    static bool loadGrayscaleImage(const std::string& path, cv::Mat& image);
    static bool loadStereoPair(const std::string& leftPath,
                               const std::string& rightPath,
                               cv::Mat& leftImage,
                               cv::Mat& rightImage);

    static std::string framePath(const std::string& folder,
                                 const std::string& prefix,
                                 int index);

    static std::vector<int> findAvailableFrameIndices(const std::string& leftFolder,
                                                      const std::string& rightFolder);
};

#endif
