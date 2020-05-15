#ifndef dataStructures_h
#define dataStructures_h

#include <vector>
#include <string>
#include <opencv2/core.hpp>


struct DataFrame { // represents the available sensor information at the same time instance
    std::string id;
    cv::Mat cameraImg; // camera image
    
    std::vector<cv::KeyPoint> keypoints; // 2D keypoints within camera image
    cv::Mat descriptors; // keypoint descriptors
    std::vector<cv::DMatch> kptMatches; // keypoint matches between previous and current frame
};


#endif /* dataStructures_h */
