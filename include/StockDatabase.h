#pragma once

#include "StockData.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <set>
#include <queue>

// Comparator for sorting stocks by volume (used in query 13)
struct VolumeComparator
{
    bool operator()(const StockData &a, const StockData &b)
    {
        return a.volume > b.volume;
    }
};

// Comparator for sorting stocks by closing price (used in query 14)
struct ClosePriceComparator
{
    bool operator()(const StockData &a, const StockData &b)
    {
        return a.close < b.close;
    }
};

// Comparator for sorting stocks by dividends (used in query 15)
struct DividendsComparator
{
    bool operator()(const StockData &a, const StockData &b)
    {
        return a.dividends < b.dividends;
    }
};

class StockDatabase
{
private:
    std::vector<StockData> data;                                                               // Stores all stock data
    std::unordered_map<std::string, std::vector<StockData>> tickerMap;                         // Maps ticker to its records
    std::unordered_map<std::string, std::vector<StockData>> dateMap;                           // Maps date to its records
    std::unordered_map<std::string, std::unordered_map<std::string, StockData>> tickerDateMap; // Maps ticker and date to its record

public:
    void loadData(const std::string &filename);
    std::vector<StockData> getDataByDate(const std::string &date);
    double getAverageClosePrice(const std::string &ticker);
    double getHighestPriceInPeriod(const std::string &ticker, const std::string &startDate, const std::string &endDate);
    std::set<std::string> getAllUniqueTickers();
    bool doesTickerExist(const std::string &ticker);
    int countDatesAboveThreshold(double threshold);
    double getClosingPrice(const std::string &ticker, const std::string &date);
    std::vector<std::pair<std::string, double>> getDatesAndClosingPrices(const std::string &ticker);
    double getTotalVolume(const std::string &ticker);
    bool doesDataExist(const std::string &ticker, const std::string &date);
    std::pair<double, double> getOpeningAndClosingPrices(const std::string &ticker, const std::string &date);
    double getDividend(const std::string &ticker, const std::string &date);
    std::vector<StockData> getTop10StocksByVolume(const std::string &date);
    std::vector<StockData> getBottom5StocksByClosingPrice();
    std::vector<StockData> getTop5StocksByDividends();
};
