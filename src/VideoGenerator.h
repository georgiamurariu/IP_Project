#ifndef PROJECT_PI_VIDEOGENERATOR_H
#define PROJECT_PI_VIDEOGENERATOR_H

#include <string>

class VideoGenerator
{
public:
    static bool createVideoFromFrames(const std::string& framesFolder,
                                      const std::string& outputPath,
                                      int fps);
};

#endif
