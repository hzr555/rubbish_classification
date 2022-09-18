#include <yolo.hpp>
#include <cv_bridge/cv_bridge.h>
#include <stdlib.h>
#include <sys/signal.h>
#include <opencv2/core/utility.hpp>
#include "opencv2/freetype.hpp"

YOLO yolo(IdentityRubbish::Other);
uart_process_2::uart_receive currentReceive;
int m_oner = 1, m_full = 2;
vector<int> serial;

void onCameraRawImageReceive(const sensor_msgs::ImageConstPtr &msg)
{
    // auto start = std::chrono::system_clock::now();
    // sorter.uartReceive = currentReceive;
    // sorter << cv_bridge::toCvCopy(msg, "bgr8")->image;
    // // clearScreen();
    // auto end = std::chrono::system_clock::now();
    // auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    
    // std::cout << " - Timing: Total time elapsed: " << std::to_string(duration) << "ms." << std::endl;
}

void failSafe(int)
{
    std::cout << "[FailSafe] I'm dying." << std::endl;
}

void onUartMessageReceived(const uart_process_2::uart_receive &msg)
{
	currentReceive = msg;
    m_oner = msg.red_blue;

    m_full = msg.shoot_speed_mod;
    // cout << "m_full:" << m_full << endl;

	// yolo.update(msg);
}

int main(int argc, char** argv)
{   
    Net_config darknet_yolo_nets[4] = {
        {0.5, 0.4, 416, 416, "coco.names", "/home/huangzengrong/Desktop/asd/yolov2-tiny(1).cfg", "/home/huangzengrong/Desktop/asd/yolov2-tiny_last.weights", "yolov2"},
        {0.5, 0.4, 416, 416, "/home/huangzengrong/Desktop/asd/coco.names", "/home/huangzengrong/Desktop/asd/models/yolov3-tiny.cfg", "/home/huangzengrong/Desktop/asd/models/yolov3-tiny.weights", "yolov3"},
        {0.5, 0.4, 416, 416, "coco.names", "models/yolov4t.cfg", "models/yolov4tw.weights", "yolov4"},
        {0.5, 0.4, 416, 416, "/home/huangzengrong/Desktop/asd/coco.names", "/home/huangzengrong/Desktop/asd/models/laji_yolov4.cfg", "/home/huangzengrong/Desktop/asd/models/laji_yolov4_new.weights", "laji-classification"}
    };

    signal(SIGINT, failSafe);
	signal(SIGABRT, failSafe);
    
    ros::init(argc, argv, "rubbish_classify");

    ros::NodeHandle nh;
    // image_transport::ImageTransport it(nh);
    // image_transport::Subscriber cameraRawImageSubscriber = it.subscribe("DahuaCamera/LowDims", 1, onCameraRawImageReceive);

	ros::Subscriber uartMessageSubsriber = nh.subscribe("uart_receive", 1, onUartMessageReceived); //接收串口模式
    ros::Publisher uartPublisher = nh.advertise<uart_process_2::uart_send>("uart_send",1);
    // ros::Publisher debugImagePublisher = nh.advertise<sagitari_debug::sagitari_img_debug>("Sagitari/debugImage", 1);
    
    VideoCapture video(0);
    VideoCapture camera("/home/huangzengrong/Videos/video_static_spd.avi");

    Mat srcimg;
    Mat dstimg;
    int label = 2330;
    int frame = 0;      //帧数
    int label_last = 3000;
    int serial_number = 0;  //序号

    int number1 = 0;    //harm
    int number2 = 0;    //recycle
    int number3 = 0;    //kitchen
    int number4 = 0;    //other

    cv::Ptr<cv::freetype::FreeType2> ft = cv::freetype::createFreeType2(); 
    ft->loadFontData("/usr/share/fonts/simsun.ttc", 0);
    string text = "满载";
    
    cout << "视频帧率=" << camera.get(CAP_PROP_FPS) << endl;
    
    ros::Rate rate(200);
    while (ros::ok())
    {   
        video >> srcimg;   
        camera >> dstimg;

        uart_process_2::uart_send msg;

        msg.attackFlag = 0;
        msg.curDistance = 0;
        msg.curYaw = 0;
        msg.predictLatency = 0;
        msg.predictPitch = 0;
        msg.predictYaw = 0;
        msg.time = 0;

        if (dstimg.empty())
        {
            cout << "Open video failed!!!" << endl;
        }

        string imgPath = "/home/huangzengrong/Desktop/asd/src/1.jpg";
        Mat src = imread(imgPath);
        cv::resize(src, src, Size(640, 640));
        // imshow("src", src);

        YOLO yolo_model(darknet_yolo_nets[1]);
        // imshow("frame", srcimg);

        try{ 
            // 识别分类环节
            if (m_oner)
            // if (1)
            {   
                label = yolo_model.detect(srcimg);
                cout << "label:" << label << endl;
        
                if (label < 80) 
                {
                    // label = 2330;
                    // msg.curPitch = rand()%10; 
                    msg.curPitch = label;
                    uartPublisher.publish(msg);

                    // judge && count
                    if (label_last != label || (frame > 40 && label_last == label))
                    {
                        frame = 0;

                        serial_number = serial_number + 1; 
                        if (serial_number >= 2)      
                        {
                            label_last = label;
                        }         

                        if (label == 10)
                        { 
                            number1++;
                            string text1 = to_string(serial_number) + " 有害垃圾 " + to_string(number1) + " OK! ";   
                            
                            cout << serial_number << " 有害垃圾 " << number1 << " OK!" << endl;
                            // char pchText[128];
                            // snprintf(pchText, 128, "%s", "有害垃圾");
                            ft->putText(src, text1, cv::Point(240, src.rows/2), 20, cv::Scalar(0, 0, 0), 1, 8, true);

                            // putText(src, text1, Point(200, src.rows/2), FONT_HERSHEY_PLAIN, 1.5, Scalar(0, 0, 0), 2);  
                            // putTextZH(src, text1, Point(200, src.rows/2), FONT_HERSHEY_PLAIN, 1.5, Scalar(0, 0, 0), 2);     
                        }
                        else if (label == 39)
                        {
                            number2++;                        
                            string text2 = to_string(serial_number) + " 可回收垃圾 " + to_string(number2) + " OK! ";   

                            cout << serial_number << " 可回收垃圾 " << number2 << " OK!" << endl;
                            ft->putText(src, text2, cv::Point(240, src.rows/2), 20, cv::Scalar(0, 0, 0), 1, 8, true);
                        }
                        else if (label == 12)
                        {
                            number3++;                        
                            string text3 = to_string(serial_number) + " 厨余垃圾 " + to_string(number3) + " OK! ";   

                            // cout << serial_number << " 厨余垃圾 " << number3 << " OK!" << endl;
                            ft->putText(src, text3, cv::Point(240, src.rows/2), 20, cv::Scalar(0, 0, 0), 1, 8, true);
                        }
                        else if (label == 13)
                        {
                            number4++;
                            string text4 = to_string(serial_number) + " 其他垃圾 " + to_string(number4) + " OK! ";   

                            // cout << serial_number << " 其他垃圾 " << number4 << " OK!" << endl;
                            ft->putText(src, text4, cv::Point(240, src.rows/2), 20, cv::Scalar(0, 0, 0), 1, 8, true);
                        }
                    }
                    /*else if ((frame%10) == 0)
                    {
                        frame = 0;

                        if (label == 0)
                        { 
                            number1++;
                            string text1 = to_string(serial_number) + " 有害垃圾 " + to_string(number1) + " OK! ";   
                            
                            cout << serial_number << " 有害垃圾 " << number1 << " OK!" << endl;
                            // char pchText[128];
                            // snprintf(pchText, 128, "%s", "有害垃圾");
                            ft->putText(src, text1, cv::Point(240, src.rows/2), 20, cv::Scalar(0, 0, 0), 1, 8, true);

                            // putText(src, text1, Point(200, src.rows/2), FONT_HERSHEY_PLAIN, 1.5, Scalar(0, 0, 0), 2);  
                            // putTextZH(src, text1, Point(200, src.rows/2), FONT_HERSHEY_PLAIN, 1.5, Scalar(0, 0, 0), 2);     
                        }
                        else if (label == 39)
                        {
                            number2++;                        
                            string text2 = to_string(serial_number) + " 可回收垃圾 " + to_string(number2) + " OK! ";   

                            cout << serial_number << " 可回收垃圾 " << number2 << " OK!" << endl;
                            ft->putText(src, text2, cv::Point(240, src.rows/2), 20, cv::Scalar(0, 0, 0), 1, 8, true);
                        }
                        else if (label == 12)
                        {
                            number3++;                        
                            string text3 = to_string(serial_number) + " 厨余垃圾 " + to_string(number3) + " OK! ";   

                            // cout << serial_number << " 厨余垃圾 " << number3 << " OK!" << endl;
                            ft->putText(src, text3, cv::Point(240, src.rows/2), 20, cv::Scalar(0, 0, 0), 1, 8, true);
                        }
                        else if (label == 13)
                        {
                            number4++;
                            string text4 = to_string(serial_number) + " 其他垃圾 " + to_string(number4) + " OK! ";   

                            // cout << serial_number << " 其他垃圾 " << number4 << " OK!" << endl;
                            ft->putText(src, text4, cv::Point(240, src.rows/2), 20, cv::Scalar(0, 0, 0), 1, 8, true);
                        }
                    }*/
                    cout << serial_number << "    " << label_last << "    " << label << endl;
                }
                else
                {
                    frame++;
                    msg.curPitch = -100;
                    uartPublisher.publish(msg);
                    // cout << "No target. Keep searching... " << endl;
                }                    
                cout << "frame:" << frame << endl;

            }

            // 满载检测环节
            if (m_full == 1)
            {
                // ROS_INFO("满载");
                ft->putText(src, text, cv::Point(140, 160), 20, cv::Scalar(0, 0, 0), 1, 8, true);
            }          
            else if (m_full == 2)
            {
                // ROS_INFO("满载");
                ft->putText(src, text, cv::Point(140, 480), 20, cv::Scalar(0, 0, 0), 1, 8, true);
            }     
            else if (m_full == 3)     
            {
                // ROS_INFO("满载");
                ft->putText(src, text, cv::Point(460, 160), 20, cv::Scalar(0, 0, 0), 1, 8, true);
            }
            else if (m_full == 4)
            {
                // ROS_INFO("满载");
                ft->putText(src, text, cv::Point(460, 480), 20, cv::Scalar(0, 0, 0), 1, 8, true);
            }         
            
            imshow("result", src);
            static const string kWinName = "Deep learning object detection in OpenCV";
            namedWindow(kWinName, WINDOW_NORMAL);        
            imshow(kWinName, srcimg);        
            // imshow("垃圾分类宣传视频", dstimg);
            waitKey(1);
            // destroyAllWindows();
        }
        catch (cv::Exception e)
        {
            // this->sendDebugImage("Exception frame", input);
            ROS_ERROR("The process has died...");
        }
        // return *this;        

        ros::spinOnce();
        rate.sleep();
    }
    return 0;
}