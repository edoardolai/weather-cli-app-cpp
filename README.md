# Weather App C++

# Weather Data Visualization Application

A C++ university project developed for the Object-Oriented Programming module at University of London. This application visualizes historical weather temperature data using candlestick and stacked bar charts.

## Features

- Display temperature data in candlestick format
- Plot candlestick charts for temperature visualization
- Plot stacked bar charts for temperature ranges
- Temperature data filtering capabilities
- Multiple timeframe views (yearly and monthly)
- Temperature prediction based on historical patterns

## Installation

### Prerequisites

- C++ compiler (C++11 or later)
- Make (optional but recommended)

### Building from Source

1. Clone the repository:

```bash
git clone https://github.com/yourusername/weather-app-cpp.git
cd weather-app-cpp
```

2. Compile the source files:

```bash
# Using g++
g++ -std=c++11 *.cpp -o weather_app
```

## Usage

1. Run the compiled executable:

```bash
./weather_app
```

2. Follow the interactive menu to:
   - Select a country
   - Choose timeframe (yearly/monthly)
   - Set temperature filters
   - View different visualizations
   - Make temperature predictions

## Data Format

The application expects a CSV file with the following format:

- First row: headers with country names
- First column: dates in YYYY-MM-DD format
- Temperature values in Celsius

## Project Structure

- `main.cpp` - Entry point
- `WeatherAppMenu.cpp/h` - User interface handling
- `Candlestick.cpp/h` - Candlestick data structure
- `CandlesticksCollection.cpp/h` - Collection management
- `CsvReader.cpp/h` - Data file parsing

## License

This project is part of university coursework and is available for educational purposes.
