#pragma once 
#include "Candlestick.h"
#include "CsvReader.h"
#include <vector>
#include <string>
using namespace std;

// shared data for both candlesticks and bars plotting
struct PlotData {
    double minTemp, maxTemp;
    double degreesPerRow;
    int numRows, numCols;
    int bottomRow;
    int labelWidth;
    std::vector<std::vector<std::string>> grid;
};

class CandlesticksCollection {
public:
    CandlesticksCollection() :
        filename(""),
        country(""),
        timeframe(Timeframe::Yearly),
        minTemp(numeric_limits<float>::lowest()),
        maxTemp(numeric_limits<float>::max()),
        startDate(""),
        endDate("")
    {
    };
    CandlesticksCollection(
        string filename,
        string country,
        Timeframe timeframe,
        string year = "0",
        float minTemp = numeric_limits<float>::lowest(),
        float maxTemp = numeric_limits<float>::max(),
        string startDate = "",
        string endDate = ""
    );
    // assignment operator overloading needed for updating candlestick collection based on user input in weather app menu
    CandlesticksCollection& operator=(const CandlesticksCollection& other) {
        if (this != &other) {
            filename = other.filename;
            country = other.country;
            timeframe = other.timeframe;
            candlesticks = other.candlesticks;
            startDate = other.startDate;
            endDate = other.endDate;
            minTemp = other.minTemp;
            maxTemp = other.maxTemp;
        }
        return *this;
    }
    static string timeframeToString(Timeframe timeframe);
    void displayCandlesticks();
    void plotCandlesticks();
    void plotStackedBars();
    void printPlot(
         PlotData& pd,
         vector<string>& timestamps,
         int xLabelOffset
    );
    PlotData initializePlotData(float degPerRow = 1.0, float padding = 2.0);
    void plotStackedBarsOnGrid(PlotData& pd);
    void plotCandlesticksOnGrid(PlotData& pd);
    vector<Candlestick> predictNextPeriods(int periodsToPredict);


private:
    vector <Candlestick> candlesticks;
    string filename;
    string country;
    Timeframe timeframe;
    string year;
    float minTemp;
    float maxTemp;
    string startDate;
    string endDate;
    // helper function to map temperature to y axis
    float scaleTemp(double temp, double minTemp, double degreesPerRow, int numRows);
};