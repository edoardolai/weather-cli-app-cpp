#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <limits>
#include <iomanip>
#include "Candlestick.h"
#include "WeatherAppMenu.h"
#include "CsvReader.h"
#include "CandlesticksCollection.h"

using namespace std;


int main() {
    string filename = "weather_data_EU_1980-2019_temp_only.csv";
    WeatherAppMenu app{ filename };
    app.init();
    return 0;
}