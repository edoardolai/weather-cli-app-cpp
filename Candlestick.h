#pragma once
#include <string>
#include <ostream>
using namespace std;

class Candlestick {
public:
    Candlestick(double open, double high, double low, double close, string timestamp);
    double open;
    double high;
    double low;
    double close;
    string timestamp;
    friend std::ostream& operator<<(std::ostream& os, Candlestick& candlestick);

};
