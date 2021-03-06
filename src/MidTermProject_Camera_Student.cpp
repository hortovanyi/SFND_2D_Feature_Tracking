/* INCLUDES FOR THIS PROJECT */
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <deque>
#include <cmath>
#include <limits>
#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <numeric>

#include "dataStructures.h"
#include "matching2D.hpp"

using namespace std;

/* MAIN PROGRAM */
int main(int argc, const char *argv[])
{

    /* INIT VARIABLES AND DATA STRUCTURES */

    // data location
    string dataPath = "../";

    // camera
    string imgBasePath = dataPath + "images/";
    string imgPrefix = "KITTI/2011_09_26/image_00/data/000000"; // left camera, color
    string imgFileType = ".png";
    int imgStartIndex = 0; // first file index to load (assumes Lidar and camera names have identical naming convention)
    int imgEndIndex = 9;   // last file index to load
    int imgFillWidth = 4;  // no. of digits which make up the file index (e.g. img-0001.png)

    // misc
    int dataBufferSize = 2;       // no. of images which are held in memory (ring buffer) at the same time
    std::deque<DataFrame> dataBuffer; // list of data frames which are held in memory at the same time
    bool bVis = false;            // visualize results

    // per image values
    vector<double> keypointSizes;
    vector<double> keypointMeans;
    vector<double> keypointVariances;
    vector<int> keypointMatches;
    vector<double> keypointTimings;

    /* MAIN LOOP OVER ALL IMAGES */

    for (size_t imgIndex = 0; imgIndex <= imgEndIndex - imgStartIndex; imgIndex++)
    {
        /* LOAD IMAGE INTO BUFFER */

        // assemble filenames for current index
        ostringstream imgNumber;
        imgNumber << setfill('0') << setw(imgFillWidth) << imgStartIndex + imgIndex;
        string imgFullFilename = imgBasePath + imgPrefix + imgNumber.str() + imgFileType;

        // load image from file and convert to grayscale
        cv::Mat img, imgGray;
        img = cv::imread(imgFullFilename);
        cv::cvtColor(img, imgGray, cv::COLOR_BGR2GRAY);

        //// STUDENT ASSIGNMENT
        //// TASK MP.1 -> replace the following code with ring buffer of size dataBufferSize

        // push image into data frame buffer
        DataFrame frame;
        frame.id = imgNumber.str();
        frame.cameraImg = imgGray;
        dataBuffer.push_back(frame);
        while (dataBuffer.size() > dataBufferSize)
            dataBuffer.pop_front();

        cout << "imgIndex: " << imgIndex << " dataBuffer.size(): " << dataBuffer.size() << " ids: ";
        for (auto f: dataBuffer)
            cout << f.id << " ";
        cout << endl;
        //// EOF STUDENT ASSIGNMENT
        cout << "#1 : LOAD IMAGE INTO BUFFER done" << endl;


        /* DETECT IMAGE KEYPOINTS */

        // double t = (double)cv::getTickCount();
        cv::TickMeter tm;
        tm.start();
    
        // extract 2D keypoints from current image
        vector<cv::KeyPoint> keypoints; // create empty feature list for current image
        string detectorType = "SIFT";
        if (argc ==3)
            detectorType = argv[1];
        // string detectorType = "AKAZE";
        // string detectorType = "ORB";
        // string detectorType = "BRISK";
        // string detectorType = "FAST";
        // string detectorType = "HARRIS";
        // string detectorType = "SHITOMASI";


        if (detectorType.compare("SHITOMASI") == 0)
        {
            detKeypointsShiTomasi(keypoints, imgGray, false);
        }
        else if(detectorType.compare("HARRIS") == 0)
        {
            detKeypointsHarris(keypoints, imgGray, false);
        }
        else if(detectorType.compare("FAST") == 0)
        {
            detKeypointsFAST(keypoints, imgGray, false);
        }
        else if(detectorType.compare("BRISK") == 0)
        {
            detKeypointsBRISK(keypoints, imgGray, false);
        }
        else if(detectorType.compare("ORB") == 0)
        {
            detKeypointsORB(keypoints, imgGray, false);
        }
        else if(detectorType.compare("AKAZE") == 0)
        {
            detKeypointsAKAZE(keypoints, imgGray, false);
        }
        else if(detectorType.compare("SIFT") == 0)
        {
            detKeypointsSIFT(keypoints, imgGray, false);
        }

        // only keep keypoints on the preceding vehicle
        bool bFocusOnVehicle = true;
        cv::Rect vehicleRect(535, 180, 180, 150);
        if (bFocusOnVehicle)
        {
            vector<cv::KeyPoint> vehicleKeyPoints;
            for (auto kp: keypoints)
                if (vehicleRect.contains(kp.pt))
                    vehicleKeyPoints.push_back(kp);

            keypoints = vehicleKeyPoints;
            
            keypointSizes.push_back(keypoints.size());
            
            // calculate mean of the keypoint sizes
            double sizeTotal = 0.0;
            for (auto kp : keypoints) {
                sizeTotal += kp.size;
            }
            
            double mean = sizeTotal/keypoints.size();
            keypointMeans.push_back(mean);

            // calculate variance of the keypoint sizes
            auto add_square = [mean] (double sum, double i) 
            {
                auto d = i - mean;
                return sum + d*d;
            };
            double varianceTotal = 0.0;
            for (auto kp: keypoints)
                varianceTotal = add_square(varianceTotal, kp.size);
            double variance = varianceTotal /keypoints.size();
            keypointVariances.push_back(variance);

            cout << detectorType << " imgIndex: " << imgIndex << " vehicle keypoints: " << keypoints.size(); 
            cout << " mean: " << mean << " variance: " << variance << endl;


        }


        // optional : limit number of keypoints (helpful for debugging and learning)
        bool bLimitKpts = false;
        if (bLimitKpts)
        {
            int maxKeypoints = 50;

            if (detectorType.compare("SHITOMASI") == 0)
            { // there is no response info, so keep the first 50 as they are sorted in descending quality order
                keypoints.erase(keypoints.begin() + maxKeypoints, keypoints.end());
            }
            cv::KeyPointsFilter::retainBest(keypoints, maxKeypoints);
            cout << " NOTE: Keypoints have been limited!" << endl;
        }

        // push keypoints and descriptor for current frame to end of data buffer
        (dataBuffer.end() - 1)->keypoints = keypoints;
        cout << "#2 : DETECT KEYPOINTS done" << endl;

        /* EXTRACT KEYPOINT DESCRIPTORS */

        //// STUDENT ASSIGNMENT
        //// TASK MP.4 -> add the following descriptors in file matching2D.cpp and enable string-based selection based on descriptorType
        //// -> BRIEF, ORB, FREAK, AKAZE, SIFT

        cv::Mat descriptors;
        // string descriptorType = "BRISK"; // BRIEF, ORB, FREAK, AKAZE, SIFT
        // string descriptorType = "ORB"; 
        // string descriptorType = "FREAK"; 
        // string descriptorType = "AKAZE"; 
        string descriptorType = "SIFT"; 
        if (argc == 3)
            descriptorType = argv[2];
        descKeypoints((dataBuffer.end() - 1)->keypoints, (dataBuffer.end() - 1)->cameraImg, descriptors, descriptorType);
        //// EOF STUDENT ASSIGNMENT

        // push descriptors for current frame to end of data buffer
        (dataBuffer.end() - 1)->descriptors = descriptors;

        // timing
        // t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
        tm.stop();
        double t = tm.getTimeMilli();
        keypointTimings.push_back(t);
        std::cout << "imgIndex: " << imgIndex << " keypoint detection and descriptor extraction in " << t << " ms." << endl;
            
        cout << "#3 : EXTRACT DESCRIPTORS done" << endl;

        if (dataBuffer.size() > 1) // wait until at least two images have been processed
        {

            /* MATCH KEYPOINT DESCRIPTORS */

            vector<cv::DMatch> matches;
            string matcherType = "MAT_BF";        // MAT_BF, MAT_FLANN
            // string matcherType = "MAT_FLANN";        // MAT_BF, MAT_FLANN
            // string selectorType = "SEL_NN";       // SEL_NN, SEL_KNN
            string selectorType = "SEL_KNN";       // SEL_NN, SEL_KNN

            //// STUDENT ASSIGNMENT
            //// TASK MP.5 -> add FLANN matching in file matching2D.cpp
            //// TASK MP.6 -> add KNN match selection and perform descriptor distance ratio filtering with t=0.8 in file matching2D.cpp


            matchDescriptors((dataBuffer.end() - 2)->keypoints, (dataBuffer.end() - 1)->keypoints,
                             (dataBuffer.end() - 2)->descriptors, (dataBuffer.end() - 1)->descriptors,
                             matches, descriptorType, matcherType, selectorType);

            //// EOF STUDENT ASSIGNMENT

            // store the number of matches
            keypointMatches.push_back(matches.size());

            // store matches in current data frame
            (dataBuffer.end() - 1)->kptMatches = matches;

            cout << "#4 : MATCH KEYPOINT DESCRIPTORS done" << endl;

            // visualize matches between current and previous image
            bVis = false;
            if (bVis)
            {
                cv::Mat matchImg = ((dataBuffer.end() - 1)->cameraImg).clone();
                cv::drawMatches((dataBuffer.end() - 2)->cameraImg, (dataBuffer.end() - 2)->keypoints,
                                (dataBuffer.end() - 1)->cameraImg, (dataBuffer.end() - 1)->keypoints,
                                matches, matchImg,
                                cv::Scalar::all(-1), cv::Scalar::all(-1),
                                vector<char>(), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

                string windowName = "Matching keypoints between two camera images";
                // cv::namedWindow(windowName, 7);
                cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE) ;
                cv::imshow(windowName, matchImg);
                cout << "Press key to continue to next image" << endl;
                cv::waitKey(0); // wait for key to be pressed
            }
            bVis = false;
        }


    } // eof loop over all images


    // render stats
    cout << "size | " ;
    for (auto s: keypointSizes)
        cout << s <<" | ";
    cout << endl;

    cout << "mean | " ;
    for (auto n: keypointMeans)
        cout << n <<" | ";
    cout << endl;

    cout << "variance | " ;
    for (auto n: keypointVariances)
        cout << n <<" | ";
    cout << endl;

    cout << "matches " << argv[1] <<"-"<< argv[2]<<"| 0 | ";
    for (auto n: keypointMatches)
        cout << n << " | ";
    cout << endl;

    cout << "timings " << argv[1] <<"-"<< argv[2]<<" | ";
    double sum = 0.0;
    for (auto n: keypointTimings) {
        sum += n;
        cout << n << " | ";
    }
    cout << sum/keypointTimings.size() << " | ";
    cout << endl;

    return 0;
}
