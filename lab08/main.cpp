#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp> // Include headers for OpenCV GUI functionality
#include <iostream>

int main(int argc, char** argv) {
    using namespace cv;

    // Create a VideoCapture object to read from the default camera
    VideoCapture vc(0);

    // Check if the camera was opened successfully
    if (!vc.isOpened()) {
        std::cout << "Error: Could not open camera" << std::endl;
        return 1;
    }

    // Create a window for display
    namedWindow("Window", WINDOW_AUTOSIZE);
    Mat frame;

    while (waitKey(15) != 27) {
        // Read a frame from the camera
        vc >> frame;

        // Check if the frame was empty (i.e., end of video)
//        if (frame.empty()) {
//            break;
//        }

        // Flip the frame horizontally (mirror effect)
        flip(frame, frame, 1);

        // Convert the frame to HSV color space --------------
        Mat hsv;
        cvtColor(frame, hsv, COLOR_BGR2HSV);

        // Threshold the HSV image to get only red colors
        Mat mask;
//        inRange(hsv, Scalar(150, 85, 150), Scalar(180, 255, 255), mask);
//        inRange(hsv, Scalar(160, 85, 150), Scalar(179, 255, 255), mask);
        inRange(hsv, Scalar(150,80,150), Scalar(185,255,255), mask);


        // Find contours in the mask
        std::vector<std::vector<Point>> contours;
        std::vector<Point> contour1, contour2;

        findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
// Check if there are at least two contours
        if (contours.size() >= 2) {
// Sort the contours by size (largest first)
            std::sort(contours.begin(), contours.end(),
                      [](const std::vector<Point> &a, const std::vector<Point> &b) {
                          return contourArea(a) > contourArea(b);
                      });

            // Select the two largest contours
            contour1 = contours[0];
            contour2 = contours[1];
        }

        // Draw the two largest contours on the frame
        drawContours(frame, contour1, -1, Scalar(0, 0, 255), 2);
        drawContours(frame, contour2, -1, Scalar(0, 0, 255), 2);

        // Compute the center point of each contour
        Point center1, center2;
        if (contour1.size() > 0) {
            Moments moments1 = moments(contour1);
            center1 = Point(moments1.m10 / moments1.m00, moments1.m01 / moments1.m00);
        }
        if (contour2.size() > 0) {
            Moments moments2 = moments(contour2);
            center2 = Point(moments2.m10 / moments2.m00, moments2.m01 / moments2.m00);
        }

        // Draw a red line between the center points of the two contours
        if (center1 != Point(0, 0) && center2 != Point(0, 0)) {
            line(frame, center1, center2, Scalar(0, 0, 255), 2);
        }
// Display the resulting frame
        imshow("Window", frame);
    }

    return 0;
}
