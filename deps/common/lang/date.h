#pragma once

class Date{
public:
    Date() = default;
    Date(char* c);
    Date(int i);
    int value();
    bool check();
private:
    int value_;
};