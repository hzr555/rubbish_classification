#include <yolo.hpp>

int YOLO::count(int number)
{
    this->serial_number++;

    if (number == 0){ 
        this->number1++;
        cout << serial_number << " 有害垃圾 " << number1 << " OK!" << endl;
    }
    else if (number == 11){
        this->number2++;
        cout << serial_number << " 可回收垃圾 " << number2 << " OK!" << endl;
    }
    else if (number == 12){
        this->number3++;
        cout << serial_number << " 厨余垃圾 " << number3 << " OK!" << endl;
    }
    else if (number == 13){
        this->number4++;
        cout << serial_number << " 其他垃圾 " << number4 << " OK!" << endl;
    }
}

