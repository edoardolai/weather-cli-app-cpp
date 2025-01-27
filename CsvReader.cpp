#include "CsvReader.h"
#include "Candlestick.h"
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

CSVReader::CSVReader() {}

//set up filter strucutre defaulting both temp and date filters to be fully inclusive of original dataset values
struct DataFilters {
    float minTemp = numeric_limits<float>::lowest();
    float maxTemp = numeric_limits<float>::max();
    string startDate = "";
    string endDate = "";

    bool isInTempRange(float temp) const {
        return temp >= minTemp && temp <= maxTemp;
    }

    bool isInDateRange(const string& date) const {
        if (startDate.empty() && endDate.empty()) return true;
        if (startDate.empty()) return date <= endDate;
        if (endDate.empty()) return date >= startDate;
        return date >= startDate && date <= endDate;
    }
};

string CSVReader::getDateSubstr(const string& date, Timeframe timeframe, string year) {

    if (timeframe == Timeframe::Monthly && year != "0") {
        string dateYear = date.substr(0, 4);
        if (dateYear != year) {
            return "";
        }
        string month = date.substr(5, 2);
        return year + "-" + month;
    }
    else {
        return date.substr(0, 4);
    }

}

vector<string> CSVReader::tokenise(string csvLine, char separator) {
    vector<string> tokens;
    string token;
    stringstream ss(csvLine);
    while (getline(ss, token, separator)) {
        tokens.push_back(token);
    }
    return tokens;
}

vector<Candlestick> CSVReader::computeCandlesticks(const string& filePath,
const string& country,
const Timeframe& timeframe,
const string year,
float minTemp,
float maxTemp,
string startDate,
string endDate
) {

    ifstream file(filePath);
    if (!file.is_open()) {
        cerr << "Error: Could not open the file." << endl;
        return {};
    }

    //setting up filters based on user input values (if specified)
    DataFilters filters{ minTemp, maxTemp, startDate, endDate };
    vector<Candlestick> candlesticks;
    string line;

    if (!getline(file, line)) {
        cerr << "Error: File is empty." << endl;
        return {};
    }

    // find specified country column index
    vector<string> headers = tokenise(line, ',');
    int countryIndex = -1;
    for (int i = 0; i < headers.size(); ++i) {
        if (headers[i] == country) {
            countryIndex = i;
            break;
        }
    }

    if (countryIndex == -1) {
        cerr << "Error: Country '" << country << "' not found in the header." << endl;
        throw invalid_argument("Country not found");
    }

    string currentDateGroup;
    float sumTemperatures = 0.0f;
    int tempCount = 0; // count of temp for current timeframe
    float high = numeric_limits<float>::lowest();
    float low = numeric_limits<float>::max();
    float previousClose = 0.0f;
    bool firstGroup = true;

    while (getline(file, line)) {
        if (line.empty()) continue;
        vector <string> tokens = tokenise(line, ',');

        if (tokens.size() < headers.size()) {
            cerr << "Warning: Line has insufficient columns. Skipping line." << endl;
            continue;
        }

        string date = getDateSubstr(tokens[0], timeframe, year);
        if (!filters.isInDateRange(date)) continue; //skip if date is not in range


        if (date.empty()) continue; //to avoid computing further for monthly candlesticks
        float temperature;
        try { //handle possible cases where column doesn't have valid values for temperature
            temperature = stof(tokens[countryIndex]);
        }
        catch (const invalid_argument& e) {
            cerr << "Warning: Invalid temperature value '" << tokens[countryIndex] << "'. Skipping line." << endl;
            continue;
        }

        if (currentDateGroup.empty()) {
            currentDateGroup = date;
        }
        if (date != currentDateGroup) {
            // Calculate candlestick data for the completed group
            float close = sumTemperatures / tempCount;
            // if no previous time frame, default first group open to its close
            float open = firstGroup ? close : previousClose;
            if (high <= filters.maxTemp && low >= filters.minTemp) {
                candlesticks.emplace_back(open, high, low, close, currentDateGroup);
            }
            // resetting for next candlestick
            currentDateGroup = date;
            sumTemperatures = 0.0f;
            tempCount = 0;
            high = numeric_limits<float>::lowest();
            low = numeric_limits<float>::max();
            previousClose = close; // setting the close of this group as the open for the next
            firstGroup = false;
        }

        // keep summing temperature data as long as entries have the same dategroup
        sumTemperatures += temperature;
        tempCount++;
        high = max(high, temperature);
        low = min(low, temperature);
    }
    //last group
    if (tempCount > 0) {
        if (high <= filters.maxTemp && low >= filters.minTemp) {

            float close = sumTemperatures / tempCount;
            float open = firstGroup ? close : previousClose;
            candlesticks.emplace_back(open, high, low, close, currentDateGroup);
        }
    }

    file.close();
    return candlesticks;
}
