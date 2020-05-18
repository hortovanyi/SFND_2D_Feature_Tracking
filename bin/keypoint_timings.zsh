#!/usr/bin/env zsh

#detectors = "HARRIS FAST BRISK ORB AKAZE SIFT"
#descriptors = "BRIEF ORB FREAK AKAZE SIFT"

for detector ( HARRIS FAST BRISK ORB AKAZE SIFT);
  do #echo "detector $detector";
     for descriptor (BRIEF ORB FREAK AKAZE SIFT);
       do #echo "descriptor $descriptor";
	  # echo "$detector-$descriptor";
	  if [[ "$detector" != "AKAZE" && "$descriptor" == "AKAZE" ]] 
	  then
	    echo "timings $detector-$descriptor | - | - | - | - | - | - | - | - | - | - |";
          else
	    ./2D_feature_tracking $detector $descriptor|grep timings ;
	  fi;
     done
done

