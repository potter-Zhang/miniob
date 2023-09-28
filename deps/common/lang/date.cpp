#include <sstream>
#include <string>
#include "date.h"

Date::Date(char* c){
    std::stringstream ss;
    std::string s;
    int i_array[3] = {10000,100,1};
    int count = 0;
    int num = 0;
    ss << c;
    while (getline(ss,s,'-')){
        num += atoi(s.data())*i_array[count];
        count ++;
    }
    this->value_ = num;
}

Date::Date(int i){
    this->value_ = i;
}

int Date::value(){
    return value_;
}