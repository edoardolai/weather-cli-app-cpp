#include "WeatherAppMenu.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <algorithm>
#include "CsvReader.h"

using namespace std;

WeatherAppMenu::WeatherAppMenu(const string& filename) :
    filename{ filename } {
    //setting default timeframe
    currentTimeframe = Timeframe::Yearly;
};

// init and start the menu loop
void WeatherAppMenu::init() {
    setCountry();
    int input;
    while (true) {
        printMenu();
        input = getUserOption();
        if (input == 9) { //exit menu
            cout << "Exiting application. Goodbye!" << endl;
            break;
        }
        processUserOption(input);
    }
}

// menu items
void WeatherAppMenu::printMenu() {
    cout << "\n===== Weather Application Menu =====" << endl;
    cout << "\nSelected Timeframe: " << CandlesticksCollection::timeframeToString(currentTimeframe) << endl;
    cout << "Selected Country: " << country << endl;
    cout << endl;
    cout << "Please choose an option:" << endl;
    cout << "1. Display Candlesticks" << endl;
    cout << "2. Plot Candlesticks" << endl;
    cout << "3. Plot Stacked Bars" << endl;
    cout << "4. Change Timeframe" << endl;
    cout << "5. Change Country" << endl;
    cout << "6. Set Filters" << endl;
    cout << "7. Reset Filters" << endl;
    cout << "8. Predict temperatures" << endl;
    cout << "9. Exit" << endl;
    cout << "=========================================" << endl;
}

int WeatherAppMenu::getUserOption() {
    int userOption = 0;
    string line;
    getline(cin, line);
    try {
        userOption = stoi(line);
    }
    catch (const exception& e) {
        userOption = 0;
    }
    return userOption;
}

void WeatherAppMenu::processUserOption(int option) {
    switch (option) {
    case 1:
        displayCandlesticks();
        break;
    case 2:
        plotCandlesticks();
        break;
    case 3:
        plotStackedBars();
        break;
    case 4:
        setTimeframe();
        break;
    case 5:
        setCountry();
        break;
    case 6:
        setFilters();
        break;
    case 7:
        resetFilters();
        break;
    case 8:
        predictTemperatures();
        break;
    case 9:
        break;
    default:
        cout << "Invalid choice. Please select a valid option (1-9)." << endl;
    }
}

void WeatherAppMenu::setFilters() {
    cout << "\n===== Set Data Filters =====" << endl;

    cout << "Enter minimum temperature or press Enter to skip: ";
    string input;
    getline(cin, input);
    if (!input.empty()) {
        try {
            minTemp = stof(input);
        }
        catch (const exception& e) {
            cout << "Invalid input. Minimum temperature filter cleared." << endl;
            minTemp = numeric_limits<float>::lowest();
        }
    }
    else {
        minTemp = numeric_limits<float>::lowest();
    }

    cout << "Enter maximum temperature or press Enter to skip: ";
    getline(cin, input);
    if (!input.empty()) {
        try {
            maxTemp = stof(input);
        }
        catch (const exception& e) {
            cout << "Invalid input. Maximum temperature filter cleared." << endl;
            maxTemp = numeric_limits<float>::max();
        }
    }
    else {
        maxTemp = numeric_limits<float>::max();
    }

    cout << "Enter start date or press Enter to skip: ";
    getline(cin, startDate);

    cout << "Enter end date or press Enter to skip: ";
    getline(cin, endDate);

    // updated candlesticks collection with new filters
    collection = currentTimeframe == Timeframe::Monthly ?
        CandlesticksCollection(filename, country, currentTimeframe, currentYear, minTemp, maxTemp, startDate, endDate) :
        CandlesticksCollection(filename, country, currentTimeframe, "0", minTemp, maxTemp, startDate, endDate);

    cout << "\nFilters updated successfully!" << endl;
    if (minTemp != numeric_limits<float>::lowest() || maxTemp != numeric_limits<float>::max() ||
        !startDate.empty() || !endDate.empty()) {
        cout << "Active filters:" << endl;
        if (minTemp != numeric_limits<float>::lowest())
            cout << "- Minimum temperature: " << minTemp << "°C" << endl;
        if (maxTemp != numeric_limits<float>::max())
            cout << "- Maximum temperature: " << maxTemp << "°C" << endl;
        if (!startDate.empty())
            cout << "- Start date: " << startDate << endl;
        if (!endDate.empty())
            cout << "- End date: " << endDate << endl;
    }
}

// print candlesticks in textual form
void WeatherAppMenu::displayCandlesticks() {
    cout << "\nDisplaying Candlesticks:" << endl;
    collection.displayCandlesticks();
}

void WeatherAppMenu::plotCandlesticks() {
    cout << "\nPlotting Candlesticks:" << endl;
    collection.plotCandlesticks();
}
void WeatherAppMenu::plotStackedBars() {
    cout << "\nPlotting Stacked Bar Chart:" << endl;
    collection.plotStackedBars();
}

// set/update timeframe, specifying year is required for monthly timeframe
void WeatherAppMenu::setTimeframe() {
    cout << "\nSelect Timeframe:" << endl;
    cout << "1. Yearly" << endl;
    cout << "2. Monthly" << endl;
    int tfOption = getUserOption();

    Timeframe selectedTF = Timeframe::Yearly;
    switch (tfOption) {
    case 1:
        selectedTF = Timeframe::Yearly;
        break;
    case 2:
        selectedTF = Timeframe::Monthly;
        setYear();
        break;
    default:
        cout << "Invalid choice. Timeframe remains unchanged." << endl;
        return;
    }

    setTimeframe(selectedTF);

    // update collection with new timeframe
    collection = currentTimeframe == Timeframe::Monthly ?
        CandlesticksCollection(filename, country, currentTimeframe, currentYear, minTemp, maxTemp, startDate, endDate) :
        CandlesticksCollection(filename, country, currentTimeframe, "0", minTemp, maxTemp, startDate, endDate);

    cout << "Timeframe updated to " << collection.timeframeToString(selectedTF);
    if (selectedTF == Timeframe::Monthly) {
        cout << " " << currentYear;
    }
    cout << "." << endl;
}

void WeatherAppMenu::setCountry() {
    bool validCountry = false;

    while (!validCountry) {
        cout << "Please enter a country: ";
        getline(cin, country);

        //validate that country actually exists in list
        try {
            currentTimeframe = Timeframe::Yearly;
            collection = CandlesticksCollection(filename, country, currentTimeframe);
            validCountry = true;
        }
        catch (const invalid_argument& e) {
            cout << "Error: Invalid country name. Please try again." << endl;
            continue;
        }
    }
}
void WeatherAppMenu::setTimeframe(Timeframe tf) {
    currentTimeframe = tf;
}

// prompt to enter year in case monthly timeframe is chosen
void WeatherAppMenu::setYear() {
    cout << "Enter the year in format YYYY: ";
    string year;
    getline(cin, year);
    currentYear = year;
}

void WeatherAppMenu::resetFilters() {
    minTemp = numeric_limits<float>::lowest();
    maxTemp = numeric_limits<float>::max();
    startDate = "";
    endDate = "";

    collection = currentTimeframe == Timeframe::Monthly ?
        CandlesticksCollection(filename, country, currentTimeframe, currentYear, minTemp, maxTemp, startDate, endDate) :
        CandlesticksCollection(filename, country, currentTimeframe, "0", minTemp, maxTemp, startDate, endDate);
}

void WeatherAppMenu::predictTemperatures() {
    cout << "\nHow many periods ahead would you like to predict? ";
    string input;
    getline(cin, input);
    int periods;
    try {
        periods = stoi(input);
        if (periods <= 0) throw invalid_argument("Invalid number");
    }
    catch (const exception& e) {
        cout << "Invalid input. Please enter a positive number." << endl;
        return;
    }

    vector<Candlestick> predictions = collection.predictNextPeriods(periods);

    cout << "\nPredicted temperature patterns for next " << periods << " periods:" << endl;
    cout << "(Based on patterns from last 3 periods)" << endl << endl;

    cout << "Time\t\tOpen\tHigh\tLow\tClose" << endl;
    cout << fixed << setprecision(2);
    for (const Candlestick& candle : predictions) {
        cout << candle.timestamp << (candle.timestamp.length() < 8 ? "\t\t" : "\t")
            << candle.open << "\t"
            << candle.high << "\t"
            << candle.low << "\t"
            << candle.close << endl;
    }
}