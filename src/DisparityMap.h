#ifndef PROJECT_PI_DISPARITYMAP_H
#define PROJECT_PI_DISPARITYMAP_H

#include <opencv2/opencv.hpp>
#include <string>

class DisparityMap
{
public:
    static cv::Mat computeDisparityMap(const cv::Mat& left,
                                       const cv::Mat& right,
                                       int maxDisparity,
                                       int windowSize);

    static cv::Mat normalizeDisparityMap(const cv::Mat& disparity);

    static void generateDisparityMapFromFiles(const std::string& projectRoot);

    static cv::Mat computeSAD(const cv::Mat& leftImage,
                              const cv::Mat& rightImage,
                              int windowSize,
                              int maxDisparity);

    static cv::Mat normalizeToGrayscale(const cv::Mat& rawDisparity,
                                        int maxDisparity);
};

#endif
