#pragma once

#include <string>

// Structure to store stock data
struct StockData
{
    std::string date;
    std::string ticker;
    double open;
    double high;
    double low;
    double close;
    double volume;
    double dividends;
};
