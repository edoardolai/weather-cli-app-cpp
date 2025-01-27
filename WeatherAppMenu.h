#pragma once

#include "CandlesticksCollection.h"
#include <string>
using namespace std;

class WeatherAppMenu {
public:
    WeatherAppMenu(const string& filename);
    const string& filename;
    string country;
    string year;
    void init();

private:
    CandlesticksCollection collection;

    Timeframe currentTimeframe;
    string currentYear; // for monthly timeframe

    // menu actions
    void displayCandlesticks();
    void plotCandlesticks();
    void plotStackedBars();
    void setTimeframe();
    void setCountry();
    void predictTemperatures();

    // helper functions
    void processUserOption(int option);
    int getUserOption();
    void printMenu();
    void setTimeframe(Timeframe tf);
    void setYear();

    //for filtering
    float minTemp = numeric_limits<float>::lowest();
    float maxTemp = numeric_limits<float>::max();
    string startDate = "";
    string endDate = "";
    void setFilters();
    void resetFilters();
};