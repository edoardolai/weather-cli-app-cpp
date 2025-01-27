#pragma once 
#include "Candlestick.h"
#include <vector>
#include <string>
enum class Timeframe { Yearly, Monthly };

class CSVReader {

public:
    CSVReader();
    static vector<Candlestick> computeCandlesticks(const string& filePath,
    const std::string& country,
    const Timeframe& timeframe,
    const string year,
    float minTemp = numeric_limits<float>::lowest(),
    float maxTemp = numeric_limits<float>::max(),
    string startDate = "",
    string endDate = "");
    static vector<string> tokenise(string csvLine, char separator);
    static string getDateSubstr(const std::string& date, Timeframe timeframe, string year);
};