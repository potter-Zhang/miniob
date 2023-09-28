#pragma once

class Date{
public:
    Date() = default;
    Date(char* c);
    Date(int i);
    int value();
private:
    int value_;
};