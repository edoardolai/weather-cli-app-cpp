#include "CandlesticksCollection.h"
#include "CsvReader.h"
#include <vector>
#include <iostream>
#include <iomanip>
#include <string>
using namespace std;

CandlesticksCollection::CandlesticksCollection(
    string filename,
    string country,
    Timeframe timeframe,
    string year,
    float minTemp,
    float maxTemp,
    string startDate,
    string endDate
) :
    filename{ filename },
    country{ country },
    timeframe{ timeframe },
    minTemp{ minTemp },
    maxTemp{ maxTemp },
    startDate{ startDate },
    endDate{ endDate }
{
    candlesticks = CSVReader::computeCandlesticks(filename, country, timeframe, year, minTemp, maxTemp, startDate, endDate);
};

string CandlesticksCollection::timeframeToString(Timeframe tf) {
    switch (tf) {
    case Timeframe::Monthly: return "Monthly";
    case Timeframe::Yearly: return "Yearly";
    }
}

// show candlesticks data in table-like format
void CandlesticksCollection::displayCandlesticks() {
    cout << CandlesticksCollection::timeframeToString(this->timeframe) << " candlesticks representation for " << this->country << " temperature:" << endl << endl;
    cout << "Date\tOpen\tHigh\tLow\tClose" << endl << endl; //headers
    cout << fixed;
    cout.precision(3); //for conistency in output
    for (Candlestick cs : candlesticks) {
        cout << cs;
    }
}

float CandlesticksCollection::scaleTemp(double temp, double minTemp, double degreesPerRow, int numRows) {
    // simple scaling: align min temperature with 0, then divide by number of degrees per row to get the offset in terms of rows for given temperature
    double offset = (temp - minTemp) / degreesPerRow;
    int row = static_cast<int>(round(offset));
    return (numRows - 1) - row; // invert so high temps are near the top of the plot
};

//sets up plot layout 
PlotData CandlesticksCollection::initializePlotData(float degPerRow, float padding)
{
    PlotData pd;

    //min max for vertical limit
    pd.minTemp = candlesticks[0].low;
    pd.maxTemp = candlesticks[0].high;
    for (auto& cs : candlesticks) {
        pd.minTemp = std::min(pd.minTemp, (double)cs.low);
        pd.maxTemp = std::max(pd.maxTemp, (double)cs.high);
    }
    pd.minTemp -= padding;
    pd.maxTemp += padding;

    //set specified scale factor
    pd.degreesPerRow = degPerRow;
    pd.numRows = static_cast<int>(ceil((pd.maxTemp - pd.minTemp) / pd.degreesPerRow)) + 1;

    //adjust based on number of x values so that x values are displayed on a single row
    int colsPerCandle = (timeframe == Timeframe::Yearly) ? 5 : 8;
    pd.numCols = static_cast<int>(candlesticks.size()) * colsPerCandle + 2;

    pd.labelWidth = (timeframe == Timeframe::Yearly) ? 4 : 7;

    pd.grid = std::vector<std::vector<std::string>>(
        pd.numRows,
        std::vector<std::string>(pd.numCols, " ")
    );

    pd.bottomRow = pd.numRows - 1;

    // y-axis
    for (int r = 0; r < pd.numRows; r++) {
        pd.grid[r][0] = (r != pd.bottomRow) ? "|" : "+";
    }
    // x-axis
    for (int c = 1; c < pd.numCols; c++) {
        pd.grid[pd.bottomRow][c] = "-";
    }

    return pd;
}

//print upopulated plot, with axis and relative labels
void CandlesticksCollection::printPlot(
    PlotData& pd,
    std::vector<std::string>& timestamps,
    int xLabelOffset
)
{
    // printing rows
    for (int r = 0; r < pd.numRows; r++) {
        double rowTemp = pd.minTemp + (pd.numRows - 1 - r) * pd.degreesPerRow;

        if (r % 5 == 0) { //printing y labels every 5 to not overcrowd plot
            std::cout << std::setw(3) << std::fixed << std::setprecision(0) << rowTemp << " ";
        }
        else {
            std::cout << "    ";
        }

        for (int c = 0; c < pd.numCols; c++) {
            std::cout << pd.grid[r][c];
        }
        std::cout << std::endl;
    }

    //x labels
    std::cout << string(xLabelOffset, ' '); //initial offset to align start of labels to start of x values
    for (string& ts : timestamps) {
        std::cout << std::setw(pd.labelWidth) << std::left << ts << " ";
    }
    std::cout << std::endl;
}

void CandlesticksCollection::plotCandlesticksOnGrid(PlotData& pd)
{
    // column set up (dependent of timeframe)
    int colsPerCandle, candleCenterOffset, candleBodyStart, candleBodyWidth;
    if (timeframe == Timeframe::Yearly) {
        colsPerCandle = 5;
        candleCenterOffset = 2;
        candleBodyStart = 1;
        candleBodyWidth = 3;
    }
    else {
        colsPerCandle = 8;
        candleCenterOffset = 3;
        candleBodyStart = 2;
        candleBodyWidth = 3;
    }

    // plotting of candlesticks
    for (int i = 0; i < (int)candlesticks.size(); i++) {
        const Candlestick& cs = candlesticks[i];

        int openRow = scaleTemp(cs.open, pd.minTemp, pd.degreesPerRow, pd.numRows);
        int closeRow = scaleTemp(cs.close, pd.minTemp, pd.degreesPerRow, pd.numRows);
        int highRow = scaleTemp(cs.high, pd.minTemp, pd.degreesPerRow, pd.numRows);
        int lowRow = scaleTemp(cs.low, pd.minTemp, pd.degreesPerRow, pd.numRows);

        int candleLeft = 1 + i * colsPerCandle;
        int candleCenter = candleLeft + candleCenterOffset;

        int topBox = std::min(openRow, closeRow);
        int botBox = std::max(openRow, closeRow);

        //wicks
        for (int r = highRow; r < topBox; r++) {
            if (r >= 0 && r < pd.numRows) {
                pd.grid[r][candleCenter] = "|";
            }
        }
        for (int r = botBox + 1; r <= lowRow; r++) {
            if (r >= 0 && r < pd.numRows) {
                pd.grid[r][candleCenter] = "|";
            }
        }

        // fill color (bullish=green, bearish=red)
        std::string fillColor = (cs.close >= cs.open)
            ? "\033[42m \033[0m"
            : "\033[41m \033[0m";

        //boxes 
        for (int r = topBox; r <= botBox; r++) {
            if (r >= 0 && r < pd.numRows) {
                for (int w = 0; w < candleBodyWidth; w++) {
                    int col = candleLeft + candleBodyStart + w;
                    if (col < pd.numCols) {
                        pd.grid[r][col] = fillColor;
                    }
                }
            }
        }
    }
}

void CandlesticksCollection::plotStackedBarsOnGrid(PlotData& pd)
{
    //column set up (timeframe dependent)
    int colsPerBar = (timeframe == Timeframe::Yearly) ? 5 : 8;
    int barWidth = 3;

    // plotting of bars
    for (int i = 0; i < (int)candlesticks.size(); i++) {
        const Candlestick& cs = candlesticks[i];

        //define portions of bars
        int highStart = scaleTemp(cs.high, pd.minTemp, pd.degreesPerRow, pd.numRows);
        int highEnd = scaleTemp(cs.low, pd.minTemp, pd.degreesPerRow, pd.numRows);
        int highTop = std::min(highStart, highEnd);
        int highBottom = std::max(highStart, highEnd);

        int lowStart = scaleTemp(cs.low, pd.minTemp, pd.degreesPerRow, pd.numRows);
        int lowEnd = scaleTemp(0, pd.minTemp, pd.degreesPerRow, pd.numRows);
        int lowTop = std::min(lowStart, lowEnd);
        int lowBottom = std::max(lowStart, lowEnd);

        int barLeft = 1 + i * colsPerBar + 1; // offset from y-axis

        // highs
        for (int r = highBottom; r >= highTop; r--) {
            if (r >= 0 && r < pd.numRows) {
                for (int w = 0; w < barWidth; w++) {
                    int col = barLeft + w;
                    if (col < pd.numCols) {
                        pd.grid[r][col] = "\033[41m \033[0m";
                    }
                }
            }
        }
        // lows
        for (int r = lowBottom; r >= lowTop; r--) {
            if (r >= 0 && r < pd.numRows) {
                for (int w = 0; w < barWidth; w++) {
                    int col = barLeft + w;
                    if (col < pd.numCols) {
                        pd.grid[r][col] = "\033[44m \033[0m";
                    }
                }
            }
        }
    }
}

void CandlesticksCollection::plotCandlesticks() {
    if (candlesticks.empty()) {
        std::cout << "No candlesticks to plot." << std::endl;
        return;
    }
    // collect timestamps
    std::vector<std::string> timestamps;
    for (auto& cs : candlesticks) {
        timestamps.push_back(cs.timestamp);
    }

    PlotData pd = initializePlotData();
    plotCandlesticksOnGrid(pd);
    printPlot(pd, timestamps, 4);
}

void CandlesticksCollection::plotStackedBars() {
    if (candlesticks.empty()) {
        std::cout << "No data to plot." << std::endl;
        return;
    }
    // Gather timestamps
    std::vector<std::string> timestamps;
    for (auto& cs : candlesticks) {
        timestamps.push_back(cs.timestamp);
    }

    PlotData pd = initializePlotData(3.0);
    plotStackedBarsOnGrid(pd);
    printPlot(pd, timestamps, 6);

    //legend
    std::cout << "\nLegend:" << std::endl;
    std::cout << "\033[41m  \033[0m High Temperature" << std::endl;
    std::cout << "\033[44m  \033[0m Low Temperature" << std::endl;
}

vector<Candlestick> CandlesticksCollection::predictNextPeriods(int periodsToPredict) {
    vector<Candlestick> predictions;
    if (candlesticks.empty()) return predictions;

    if (timeframe == Timeframe::Yearly) {
        // using last 3 periods (or less if not enough data)
        const int lookback = min(12, static_cast<int>(candlesticks.size()));

        // getting trends from recent candlesticks
        float highTrend = 0, lowTrend = 0, closeTrend = 0;
        for (int i = candlesticks.size() - lookback; i < candlesticks.size() - 1; i++) {
            highTrend += (candlesticks[i + 1].high - candlesticks[i].high) / lookback;
            lowTrend += (candlesticks[i + 1].low - candlesticks[i].low) / lookback;
            closeTrend += (candlesticks[i + 1].close - candlesticks[i].close) / lookback;
        }

        float lastHigh = candlesticks.back().high;
        float lastLow = candlesticks.back().low;
        float lastClose = candlesticks.back().close;
        string lastTimestamp = candlesticks.back().timestamp;

        for (int i = 0; i < periodsToPredict; i++) {
            float predictedHigh = lastHigh + highTrend + (static_cast<float>(rand()) / RAND_MAX - 0.5f);
            float predictedLow = lastLow + lowTrend + (static_cast<float>(rand()) / RAND_MAX - 0.5f);
            float predictedClose = lastClose + closeTrend + (static_cast<float>(rand()) / RAND_MAX - 0.5f);

            int year = stoi(lastTimestamp) + 1;
            string nextTimestamp = to_string(year);

            predictions.emplace_back(lastClose, predictedHigh, predictedLow, predictedClose, nextTimestamp);

            lastHigh = predictedHigh;
            lastLow = predictedLow;
            lastClose = predictedClose;
            lastTimestamp = nextTimestamp;
        }
    }
    else {  // monthly
        // create arrays to store average temperatures for each month
        vector<float> monthlyAvgHigh(12, 0);
        vector<float> monthlyAvgLow(12, 0);
        vector<float> monthlyAvgClose(12, 0);
        vector<float> monthlyAvgOpen(12, 0);
        vector<int> monthCounts(12, 0);

        // calculation of avg temp for each month
        for (Candlestick& cs : candlesticks) {
            int month = stoi(cs.timestamp.substr(5, 2)) - 1; // 0-based month index
            monthlyAvgHigh[month] += cs.high;
            monthlyAvgLow[month] += cs.low;
            monthlyAvgClose[month] += cs.close;
            monthlyAvgOpen[month] += cs.open;  // Track open values
            monthCounts[month]++;
        }

        // averagin all counts
        for (int i = 0; i < 12; i++) {
            if (monthCounts[i] > 0) {
                monthlyAvgHigh[i] /= monthCounts[i];
                monthlyAvgLow[i] /= monthCounts[i];
                monthlyAvgClose[i] /= monthCounts[i];
                monthlyAvgOpen[i] /= monthCounts[i];
            }
        }

        // get last known values for future timestamps references
        string lastTimestamp = candlesticks.back().timestamp;
        int currentYear = stoi(lastTimestamp.substr(0, 4));
        int currentMonth = stoi(lastTimestamp.substr(5, 2));

        // actual predictions
        for (int i = 0; i < periodsToPredict; i++) {
            currentMonth++;
            if (currentMonth > 12) {
                currentMonth = 1;
                currentYear++;
            }

            int monthIndex = currentMonth - 1;

            // adding randomness fluctuation while considering seasonal patterns (generally warmest and generally coldest months)
            float randomFactor = 0.1f;
            float random = ((static_cast<float>(rand()) / RAND_MAX) - 0.5f) * 2 * randomFactor;

            float predictedOpen = monthlyAvgOpen[monthIndex] * (1 + random);
            float predictedHigh = monthlyAvgHigh[monthIndex] * (1 + random);
            float predictedLow = monthlyAvgLow[monthIndex] * (1 + random);
            float predictedClose = monthlyAvgClose[monthIndex] * (1 + random);

            // checking predictions maintain proper high/low relationship
            predictedHigh = max(predictedHigh, max(predictedLow, predictedClose));
            predictedLow = min(predictedLow, min(predictedHigh, predictedClose));

            string nextTimestamp = to_string(currentYear) + "-" +
                (currentMonth < 10 ? "0" : "") + to_string(currentMonth);

            predictions.emplace_back(predictedOpen, predictedHigh, predictedLow,
                                   predictedClose, nextTimestamp);
        }
    }


    return predictions;
}
