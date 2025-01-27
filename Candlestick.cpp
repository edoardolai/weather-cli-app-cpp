#include "Candlestick.h"
#include <string>
using namespace std;

Candlestick::Candlestick(double open, double high, double low, double close, string timestamp) :
    open{ open }, high{ high }, low{ low }, close{ close }, timestamp{ timestamp } {};

ostream& operator<<(ostream& os, Candlestick& candlestick) {
    os << candlestick.timestamp << "\t" << candlestick.open << "\t" << candlestick.high << "\t" << candlestick.low << "\t" << candlestick.close << endl;
    return os;
}