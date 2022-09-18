#include <yolo.hpp>

void YOLO::targetTo(int flag)
{
    uart_process_2::uart_send send_msg;
    // send_msg.flag = 
    if (flag == 0)
    {
        send_msg.curPitch = 1;
    }
    // else if (flag == "Recycle")
    // {
    //     send_msg.flag = 2;
    // }
    // else if (flag == "Kitchen")
    // {
    //     send_msg.flag = 3;
    // }
    // else if (flag == "Other")
    // {
    //     send_msg.flag = 4;
    // }
    // if (flag == "person")      send_msg.flag = 1;
    
    this->uartPublisher.publish(send_msg);
    // this->uartSent = send_msg;
}