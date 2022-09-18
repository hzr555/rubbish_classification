#ifndef _YOLO_HPP
#define _YOLO_HPP

#include <ros/ros.h>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <opencv2/dnn/dnn.hpp>
#include <opencv2/opencv.hpp>
#include <message_filters/subscriber.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <uart_process_2/uart_send.h>
#include <uart_process_2/uart_receive.h>
#include <sagitari_debug/sagitari_img_debug.h>

using namespace cv;
using namespace cv::dnn;
using namespace std;

enum class IdentityRubbish{
    Harm, Recycle, Kitchen, Other
};

struct Net_config
{
	float confThreshold; // Confidence threshold
	float nmsThreshold;  // Non-maximum suppression threshold
	int inpWidth;  // Width of network's input image
	int inpHeight; // Height of network's input image
	string classesFile;
	string modelConfiguration;
	string modelWeights;
	string netname;
};

struct Net_pth
{
    float confThreshold; // Confidence threshold
	float nmsThreshold;  // Non-maximum suppression threshold
	int inpWidth;  // Width of network's input image
	int inpHeight; // Height of network's input image
	string classesFile;
	string modelPytorch;
	string netname;
};

class YOLO
{
    public:
        enum class State {
		    SEARCHING, TRACKING
        };

        YOLO(Net_config config);
        YOLO(Net_pth pth);
		int detect(Mat& frame);

        YOLO& operator <<(cv::Mat&);        
        YOLO(IdentityRubbish);

        IdentityRubbish targetrubbish;
		State state;

        void update(const uart_process_2::uart_receive &receive);
        void targetTo(int flag);

        // ros::Publisher uartPublisher;
        // ros::Publisher debugImagePublisher;

        uart_process_2::uart_send uartSent;			// 串口发送数据
        uart_process_2::uart_receive uartReceive;	// 串口接受数据

        int process(Mat &frame, const vector<Mat> &outs);

        int count(int number);

    private:
    //yolo-detect parameters:
        float confThreshold;
		float nmsThreshold;
		int inpWidth;
		int inpHeight;
		char netname[20];
		vector<string> classes;
		Net net;
		int postprocess(Mat& frame, const vector<Mat>& outs);
		int drawPred(int classId, float conf, int left, int top, int right, int bottom, Mat& frame);

        bool m_receive = true;


};



#endif