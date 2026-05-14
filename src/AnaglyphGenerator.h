#ifndef PROJECT_PI_ANAGLYPHGENERATOR_H
#define PROJECT_PI_ANAGLYPHGENERATOR_H

#include <opencv2/opencv.hpp>

class AnaglyphGenerator
{
public:
    static cv::Mat createSimpleAnaglyph(const cv::Mat& leftImage,
                                        const cv::Mat& rightImage);

    static cv::Mat createAnaglyphWithDisparity(const cv::Mat& leftImage,
                                               const cv::Mat& rightImage,
                                               const cv::Mat& disparityMap);
};

#endif
