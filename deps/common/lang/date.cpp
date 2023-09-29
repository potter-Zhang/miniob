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

inline bool is_leap_year(int year)
{
    return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
}

bool Date::check(){
    int year = value_ / 10000;
    int remain = value_ - year * 10000;
    int month = remain / 100;
    int day = remain % 100;
    if ((year < 1970 || year > 2038) || (month <=0 || month > 12) || (day <= 0 || day > 31))
        return false;
    int max_day_in_month[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    const int max_day = max_day_in_month[month - 1];
    if (day > max_day)
        return false;
    if (month == 2  && (!is_leap_year(year) && day > 28 || is_leap_year(year) && day == 28))
        return false;
    if (value_ > 20380228)
        return false;
    return true;
}
