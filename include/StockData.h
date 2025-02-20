#pragma once

#include <string>

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
