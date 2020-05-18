Camera Based 2D Feature Tracking
================================

## Mid-Term report
Following is an interim report write up for the Udacity Sensor Fusion Nanodegree section related Camera Based 2D Feature Tracking. It covers data buffer optimisation, Keypoints, Keypoint Descriptors & Matching with a summary of the performance for HARRIS, BRISK, ORB, AKAZE and SIFT detectors & associated descriptors. 

## Data Buffer
### MP.1 Data Buffer Optimisation
A standard deque structure was used as the container for the databuffer. This allows for the data to be pushed and pulled of from both ends.
```C++
std::deque<DataFrame> dataBuffer;
```

the code to ensure that size did not exceed limits (ie `dataBufferSize = 2` ) is
```C++
dataBuffer.push_back(frame);
while (dataBuffer.size() > dataBufferSize)
    dataBuffer.pop_front();
```

`pop_front()` removes the earliest element from the queue.

## Keypoints
### MP.2 Keypoint Detection
HARRIS, FAST, BRISK, ORB, AKAZE and SIFT detectors were added by creating a unique method setKeypointsXXX (XXX is the formally mentioned detector types) signature in `matching2d.hpp` with the actual code in `matching2D_Student.cpp`. As OpenCV 4.3 was used, the SIFT detector, due to patent expiry, has been moved to `<opencv/features2d.hpp>` from `<opencv3/xfeatures2d/nonfree.hpp>. Default values were used for all detectors. Specific variables were created for each detector's defaults based on OpenCV documentation.

The original if test structure was extened in `MidTermProject_Camera_Student.cpp` to call the appropriate function that matched by the `detectorType` string.

### MP.3 Keypoint Removal

A pre-defined rectangle was used as a region of interest for the vehicle in front. If a keypoint is not contained inside of it, it is affectively removed when `bFocusOnVehicle` is `true`.
```c++
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
}
```

## Descriptors

### MP.4 Keypoint Descriptors

The if selection structure in the`descKeypoints` function in `matchingD_Student.cpp` was amended to include BRISK, ORB, FREAK, AKAZE and SIFT.  Default values were used for all descriptors. Specific variables were created for each descriptor's defaults based on OpenCV documentation.

### MP.5 Descriptor Matching

FLANN based matching was implemented in `matching2D_Student.cpp` when `matcherType` equals `"MAT_FLANN"` via
```c++
else if (matcherType.compare("MAT_FLANN") == 0)
{
    matcher = cv::FlannBasedMatcher::create();
}
```
similiarily k-nearest neighbour selection was implemented via
```c++
else if (selectorType.compare("SEL_KNN") == 0)
{ // k nearest neighbors (k=2)

    int k = 2;
    std::vector<std::vector<cv::DMatch>> knn_matches;
    matcher->knnMatch(descSource, descRef, knn_matches, k);
    
    .....
}
```

### MP.6 Descriptor Distance Ratio
Use the K-Nearest-Neighbor matching to implement the descriptor distance ratio test, which looks at the ratio of best vs. second-best match to decide whether to keep an associated pair of keypoints.

The following code was found from the OpenCV [tutorial feature flann matcher](https://docs.opencv.org/4.3.0/d5/d6f/tutorial_feature_flann_matcher.html). 

```c++
//-- Filter matches using the Lowe's ratio test
const float ratio_thresh = 0.8f;
for (size_t i = 0; i < knn_matches.size(); i++)
{
    if (knn_matches[i][0].distance < ratio_thresh * knn_matches[i][1].distance)
    {
        matches.push_back(knn_matches[i][0]);
    }
}
```

## Performance
### MP.7 Performance Evaluation 1
Count the number of keypoints on the preceding vehicle for all 10 images and take note of the distribution of their neighborhood size. Do this for all the detectors you have implemented.
### HARRIS
Attribue/Image|0|1|2|3|4|5|6|7|8|9
---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
size | 17 | 14 | 18 | 21 | 26 | 43 | 18 | 31 | 26 | 34 | 
mean | 6 | 6 | 6 | 6 | 6 | 6 | 6 | 6 | 6 | 6 | 
variance | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 

### FAST
Attribue/Image|0|1|2|3|4|5|6|7|8|9
---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
size | 419 | 427 | 404 | 423 | 386 | 414 | 418 | 406 | 396 | 401 | 
mean | 7 | 7 | 7 | 7 | 7 | 7 | 7 | 7 | 7 | 7 | 
variance | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 

### BRISK
Attribue/Image|0|1|2|3|4|5|6|7|8|9
---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
size | 264 | 282 | 282 | 277 | 297 | 279 | 289 | 272 | 266 | 254 | 
mean | 21.5492 | 21.7853 | 21.6509 | 20.3582 | 22.5911 | 22.9442 | 21.8014 | 22.1472 | 22.5558 | 22.0389 | 
variance | 212.496 | 212.129 | 191.029 | 159.23 | 220.702 | 249.964 | 215.379 | 226.018 | 230.701 | 215.124 | 

### ORB
Attribue/Image|0|1|2|3|4|5|6|7|8|9
---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
size | 92 | 102 | 106 | 113 | 109 | 125 | 130 | 129 | 127 | 128 | 
mean | 57.0723 | 57.2273 | 56.4948 | 55.1436 | 56.7442 | 56.6367 | 56.7683 | 55.4296 | 54.6723 | 54.3886 | 
variance | 661.073 | 680.589 | 672.109 | 629.784 | 625.649 | 596.796 | 646.396 | 611.86 | 638.084 | 560.291 | 

### AKAZE
Attribue/Image|0|1|2|3|4|5|6|7|8|9
---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
size | 166 | 157 | 161 | 155 | 163 | 164 | 173 | 175 | 177 | 179 | 
mean | 7.72917 | 7.4902 | 7.45211 | 7.57522 | 7.73319 | 7.68803 | 7.73878 | 7.82612 | 7.81555 | 7.88575 | 
variance | 15.3807 | 12.4226 | 12.6221 | 11.9644 | 11.8184 | 11.4138 | 11.7812 | 12.3183 | 12.2047 | 12.9699 | 

### SIFT
Attribue/Image|0|1|2|3|4|5|6|7|8|9
---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
size | 138 | 132 | 124 | 137 | 134 | 140 | 137 | 148 | 159 | 137 | 
mean | 4.98472 | 5.08981 | 4.93926 | 4.73123 | 4.7196 | 4.68398 | 5.40798 | 4.62187 | 5.51997 | 5.6251 | 
variance | 35.1615 | 38.1059 | 36.2424 | 27.4604 | 30.3653 | 31.0978 | 42.4276 | 26.4553 | 44.4893 | 44.638 | 

### MP.8 Performance Evaluation 2
Count the number of matched keypoints for all 10 images using all possible combinations of detectors and descriptors. In the matching step, the BF approach is used with the descriptor distance ratio set to 0.8.

det-desc/Image|0|1|2|3|4|5|6|7|8|9
---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
HARRIS-BRIEF| 0 | 14 | 11 | 15 | 20 | 24 | 26 | 16 | 24 | 23 | 
HARRIS-ORB| 0 | 12 | 13 | 16 | 18 | 24 | 18 | 15 | 24 | 20 | 
HARRIS-FREAK| 0 | 13 | 13 | 15 | 15 | 17 | 20 | 14 | 21 | 18 | 
HARRIS-AKAZE | - | - | - | - | - | - | - | - | - | - |
HARRIS-SIFT| 0 | 14 | 11 | 16 | 19 | 22 | 22 | 13 | 24 | 22 | 
FAST-BRIEF| 0 | 320 | 332 | 299 | 331 | 276 | 327 | 324 | 315 | 307 | 
FAST-ORB| 0 | 306 | 314 | 295 | 318 | 284 | 312 | 323 | 306 | 304 | 
FAST-FREAK| 0 | 251 | 250 | 228 | 252 | 234 | 269 | 252 | 243 | 246 | 
FAST-AKAZE | - | - | - | - | - | - | - | - | - | - |
FAST-SIFT| 0 | 316 | 325 | 297 | 311 | 291 | 326 | 315 | 300 | 301 | 
BRISK-BRIEF| 0 | 178 | 205 | 185 | 179 | 183 | 195 | 207 | 189 | 183 | 
BRISK-ORB| 0 | 160 | 171 | 157 | 170 | 154 | 180 | 171 | 175 | 172 | 
BRISK-FREAK| 0 | 160 | 178 | 156 | 173 | 160 | 183 | 169 | 179 | 168 | 
BRISK-AKAZE | - | - | - | - | - | - | - | - | - | - |
BRISK-SIFT| 0 | 182 | 193 | 169 | 183 | 171 | 195 | 194 | 176 | 183 | 
ORB-BRIEF| 0 | 49 | 43 | 45 | 59 | 53 | 78 | 68 | 84 | 66 | 
ORB-ORB| 0 | 65 | 69 | 71 | 85 | 91 | 101 | 95 | 93 | 91 | 
ORB-FREAK| 0 | 42 | 36 | 45 | 47 | 44 | 51 | 52 | 49 | 55 | 
ORB-AKAZE | - | - | - | - | - | - | - | - | - | - |
ORB-SIFT| 0 | 67 | 79 | 78 | 79 | 82 | 95 | 95 | 94 | 94 | 
AKAZE-BRIEF| 0 | 141 | 134 | 131 | 130 | 134 | 146 | 150 | 148 | 152 | 
AKAZE-ORB| 0 | 130 | 129 | 128 | 115 | 132 | 132 | 137 | 137 | 146 | 
AKAZE-FREAK| 0 | 126 | 128 | 128 | 121 | 123 | 132 | 145 | 148 | 137 | 
AKAZE-AKAZE| 0 | 138 | 138 | 133 | 127 | 129 | 146 | 147 | 151 | 150 | 
AKAZE-SIFT| 0 | 134 | 134 | 130 | 136 | 137 | 147 | 147 | 154 | 151 | 
SIFT-BRIEF| 0 | 86 | 78 | 76 | 85 | 69 | 74 | 76 | 70 | 88 | 
SIFT-ORB | - | - | - | - | - | - | - | - | - | - |
SIFT-FREAK| 0 | 64 | 72 | 65 | 66 | 63 | 58 | 64 | 65 | 79 | 
SIFT-AKAZE | - | - | - | - | - | - | - | - | - | - |
SIFT-SIFT| 0 | 82 | 81 | 85 | 93 | 90 | 81 | 82 | 102 | 104 | 

### MP.9 Performance Evaluation 3
Log the time it takes for keypoint detection and descriptor extraction. The results must be entered into a spreadsheet and based on this data, the TOP3 detector / descriptor combinations must be recommended as the best choice for our purpose of detecting keypoints on vehicles.


det-desc/Image|0|1|2|3|4|5|6|7|8|9|Avg
---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
HARRIS-BRIEF | 55.2531 | 22.1518 | 21.1117 | 19.8782 | 20.1853 | 38.9927 | 39.6117 | 42.2491 | 40.9774 | 47.7728 | 34.8184 | 
HARRIS-ORB | 57.264 | 44.2698 | 44.7528 | 43.9499 | 46.6108 | 60.5276 | 44.7123 | 54.9728 | 77.4228 | 52.18 | 52.6663 | 
HARRIS-FREAK | 56.7494 | 68.5422 | 31.9418 | 27.7439 | 28.5986 | 40.7889 | 35.7231 | 29.5984 | 36.6898 | 39.9169 | 39.6293 | 
HARRIS-AKAZE | - | - | - | - | - | - | - | - | - | - |
HARRIS-SIFT | 33.6216 | 36.3575 | 71.0743 | 32.906 | 28.0086 | 33.5674 | 23.3654 | 24.7923 | 24.2872 | 35.1216 | 34.3102 | 
FAST-BRIEF | 5.92334 | 4.72681 | 3.57879 | 4.28609 | 4.23255 | 4.5019 | 5.6786 | 6.88678 | 6.13625 | 3.27392 | 4.9225 | 
FAST-ORB | 6.24972 | 5.76662 | 5.8181 | 5.72113 | 5.5681 | 5.52739 | 5.67015 | 6.39658 | 7.14748 | 4.9302 | 5.87955 | 
FAST-FREAK | 33.4896 | 34.013 | 31.0203 | 34.0659 | 24.0822 | 22.6285 | 22.5991 | 22.3248 | 22.3235 | 22.3598 | 26.8907 | 
FAST-AKAZE | - | - | - | - | - | - | - | - | - | - |
FAST-SIFT | 25.2504 | 27.7235 | 31.6392 | 26.2796 | 19.3353 | 18.503 | 18.338 | 18.2378 | 18.5822 | 18.9891 | 22.2878 | 
BRISK-BRIEF | 259.711 | 182.593 | 195.84 | 189.694 | 182.912 | 193.374 | 189.62 | 191.523 | 188.158 | 188.639 | 196.206 | 
BRISK-ORB | 259.582 | 195.825 | 188.929 | 188.572 | 219.894 | 211.438 | 188.396 | 194.163 | 188.241 | 213.939 | 204.898 | 
BRISK-FREAK | 268.832 | 215.172 | 204.728 | 206.915 | 201.948 | 214.983 | 203.634 | 201.005 | 277.937 | 312.984 | 230.814 | 
BRISK-AKAZE | - | - | - | - | - | - | - | - | - | - |
BRISK-SIFT | 541.951 | 435.112 | 309.849 | 275.338 | 199.106 | 198.106 | 223.89 | 486.209 | 496.735 | 234.749 | 340.105 | 
ORB-BRIEF | 558.158 | 20.4006 | 17.0301 | 16.9363 | 16.9075 | 14.8166 | 15.7247 | 16.3519 | 16.0368 | 15.5425 | 70.7905 | 
ORB-ORB | 279.698 | 15.4751 | 13.4115 | 14.8058 | 15.92 | 15.9425 | 13.013 | 13.0467 | 12.8236 | 12.7482 | 40.6885 | 
ORB-FREAK | 152.851 | 29.2765 | 25.1912 | 26.683 | 28.9205 | 27.1076 | 24.6546 | 24.3505 | 24.648 | 24.015 | 38.7698 | 
ORB-AKAZE | - | - | - | - | - | - | - | - | - | - |
ORB-SIFT | 121.258 | 52.0469 | 26.6394 | 52.1279 | 25.7193 | 27.2747 | 28.0816 | 43.9631 | 39.1067 | 28.4695 | 44.4687 | 
AKAZE-BRIEF | 114.877 | 88.8784 | 45.6995 | 46.9762 | 46.796 | 43.5626 | 42.9786 | 44.1952 | 43.7148 | 51.6529 | 56.9331 | 
AKAZE-ORB | 143.112 | 91.161 | 51.5027 | 51.8503 | 50.991 | 53.1658 | 51.5069 | 52.2968 | 51.2404 | 69.7412 | 66.6568 | 
AKAZE-FREAK | 149.94 | 69.1664 | 94.6706 | 91.8043 | 63.1696 | 85.1574 | 63.3037 | 62.8166 | 62.2743 | 62.983 | 80.5286 | 
AKAZE-AKAZE | 158.322 | 88.4716 | 128.534 | 86.8999 | 83.6455 | 88.0139 | 119.38 | 107.268 | 93.3323 | 79.2755 | 103.314 | 
AKAZE-SIFT | 135.93 | 66.9868 | 59.2817 | 82.3835 | 73.4395 | 57.1846 | 56.4315 | 54.5691 | 56.8497 | 57.4792 | 70.0535 | 
SIFT-BRIEF | 138.202 | 99.0184 | 94.7119 | 82.2088 | 83.362 | 74.2994 | 74.2008 | 77.9133 | 86.0102 | 75.1574 | 88.5084 | 
SIFT-ORB | - | - | - | - | - | - | - | - | - | - |
SIFT-FREAK | 123.157 | 99.1414 | 106.457 | 92.8832 | 94.5238 | 94.8748 | 94.9007 | 96.675 | 112.054 | 94.2091 | 100.888 | 
SIFT-AKAZE | - | - | - | - | - | - | - | - | - | - |
SIFT-SIFT | 182.543 | 118.077 | 147.054 | 143.907 | 141.8 | 120.997 | 149.607 | 123.807 | 123.344 | 122.618 | 137.375 | 

#### TOP3 detector/descriptor combinations
1. FAST-BRIEF avg 4.9225 ms
2. FAST-ORB avg 5.87955 ms
3. FAST-SIFT avg 22.2878 ms