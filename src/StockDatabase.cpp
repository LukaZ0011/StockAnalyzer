#include "StockDatabase.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <unordered_set>
#include <queue>

void StockDatabase::loadData(const std::string &filename)
{
    std::ifstream file(filename);
    std::string line;
    std::getline(file, line); // Skip header

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        StockData record;
        std::string temp;

        // Parse Date and Ticker (these are required fields)
        std::getline(ss, record.date, ',');
        std::getline(ss, record.ticker, ',');

        // Check if the line is missing required fields
        if (record.date.empty() || record.ticker.empty())
        {
            std::cerr << "Skipping line with missing Date or Ticker: " << line << std::endl;
            continue;
        }

        // Parse Open, High, Low, Close, Volume, Dividends
        bool isValid = true;
        std::vector<std::string> fields = {"Open", "High", "Low", "Close", "Volume", "Dividends"};
        std::vector<double *> values = {&record.open, &record.high, &record.low, &record.close, &record.volume, &record.dividends};

        for (size_t i = 0; i < fields.size(); ++i)
        {
            std::getline(ss, temp, ',');
            if (temp.empty())
            {
                std::cerr << "Skipping line with empty " << fields[i] << " field: " << line << std::endl;
                isValid = false;
                break;
            }
            try
            {
                *values[i] = std::stod(temp);
            }
            catch (const std::invalid_argument &e)
            {
                std::cerr << "Skipping line with invalid " << fields[i] << " value: " << line << " - " << e.what() << std::endl;
                isValid = false;
                break;
            }
        }

        // Skip the line if any field is invalid
        if (!isValid)
        {
            continue;
        }

        // Add the record to the database
        data.push_back(record);
        tickerMap[record.ticker].push_back(record);
        dateMap[record.date].push_back(record);
        tickerDateMap[record.ticker][record.date] = record;
    }
}
// Query 1: Get data by date
std::vector<StockData> StockDatabase::getDataByDate(const std::string &date)
{
    return dateMap[date];
}
// Query 2: Get average closing price for a specific ticker
double StockDatabase::getAverageClosePrice(const std::string &ticker)
{
    double sum = 0;
    int count = 0;
    for (const auto &record : tickerMap[ticker])
    {
        sum += record.close;
        count++;
    }
    return (count == 0) ? 0 : sum / count;
}
// Query 3: Get highest price for a specific ticker in a period
double StockDatabase::getHighestPriceInPeriod(const std::string &ticker, const std::string &startDate, const std::string &endDate)
{
    double highest = 0;
    for (const auto &record : tickerMap[ticker])
    {
        if (record.date >= startDate && record.date <= endDate)
        {
            if (record.high > highest)
            {
                highest = record.high;
            }
        }
    }
    return highest;
}
// Query 4: Get all unique tickers
std::set<std::string> StockDatabase::getAllUniqueTickers()
{
    std::set<std::string> uniqueTickers;
    for (const auto &record : data)
    {
        uniqueTickers.insert(record.ticker);
    }
    return uniqueTickers;
}
// Query 7: Get closing price for a specific ticker on a specific date
bool StockDatabase::doesTickerExist(const std::string &ticker)
{
    return tickerMap.find(ticker) != tickerMap.end();
}
// Query 8: Count dates where closing price is above a threshold
int StockDatabase::countDatesAboveThreshold(double threshold)
{
    int count = 0;
    for (const auto &entry : dateMap)
    {
        for (const auto &record : entry.second)
        {
            if (record.close > threshold)
            {
                count++;
                break;
            }
        }
    }
    return count;
}
// Query 7: Get closing price for a specific ticker on a specific date
double StockDatabase::getClosingPrice(const std::string &ticker, const std::string &date)
{
    if (tickerDateMap.find(ticker) != tickerDateMap.end() && tickerDateMap[ticker].find(date) != tickerDateMap[ticker].end())
    {
        return tickerDateMap[ticker][date].close;
    }
    return -1; // Return -1 if not found
}
// Query 8: Get dates and closing prices for a specific ticker
std::vector<std::pair<std::string, double>> StockDatabase::getDatesAndClosingPrices(const std::string &ticker)
{
    std::vector<std::pair<std::string, double>> result;
    for (const auto &record : tickerMap[ticker])
    {
        result.push_back({record.date, record.close});
    }
    return result;
}
// Query 9: Get total trading volume for a specific ticker
double StockDatabase::getTotalVolume(const std::string &ticker)
{
    double totalVolume = 0;
    for (const auto &record : tickerMap[ticker])
    {
        totalVolume += record.volume;
    }
    return totalVolume;
}
// Query 10: Check if data exists for a specific date and ticker
bool StockDatabase::doesDataExist(const std::string &ticker, const std::string &date)
{
    return tickerDateMap.find(ticker) != tickerDateMap.end() && tickerDateMap[ticker].find(date) != tickerDateMap[ticker].end();
}
// Query 11: Get opening and closing prices for a specific ticker and date
std::pair<double, double> StockDatabase::getOpeningAndClosingPrices(const std::string &ticker, const std::string &date)
{
    if (tickerDateMap.find(ticker) != tickerDateMap.end() && tickerDateMap[ticker].find(date) != tickerDateMap[ticker].end())
    {
        return {tickerDateMap[ticker][date].open, tickerDateMap[ticker][date].close};
    }
    return {-1, -1}; // Return {-1, -1} if not found
}
// Query 12: Get dividend for a specific ticker on a specific date
double StockDatabase::getDividend(const std::string &ticker, const std::string &date)
{
    if (tickerDateMap.find(ticker) != tickerDateMap.end() && tickerDateMap[ticker].find(date) != tickerDateMap[ticker].end())
    {
        return tickerDateMap[ticker][date].dividends;
    }
    return -1; // Return -1 if not found
}
// Query 13: Get top 10 stocks with the highest trading volume on a specific date
std::vector<StockData> StockDatabase::getTop10StocksByVolume(const std::string &date)
{
    std::priority_queue<StockData, std::vector<StockData>, VolumeComparator> pq;
    for (const auto &record : dateMap[date])
    {
        pq.push(record);
        if (pq.size() > 10)
        {
            pq.pop();
        }
    }
    std::vector<StockData> result;
    while (!pq.empty())
    {
        result.push_back(pq.top());
        pq.pop();
    }
    std::reverse(result.begin(), result.end());
    return result;
}
// Query 14: Get 5 stocks with the lowest closing prices in the entire dataset
std::vector<StockData> StockDatabase::getBottom5StocksByClosingPrice()
{
    std::priority_queue<StockData, std::vector<StockData>, ClosePriceComparator> pq;
    std::unordered_set<std::string> uniqueTickers;

    for (const auto &record : data)
    {
        if (uniqueTickers.find(record.ticker) == uniqueTickers.end())
        {
            pq.push(record);
            uniqueTickers.insert(record.ticker);
            if (pq.size() > 5)
            {
                uniqueTickers.erase(pq.top().ticker);
                pq.pop();
            }
        }
    }

    std::vector<StockData> result;
    while (!pq.empty())
    {
        result.push_back(pq.top());
        pq.pop();
    }
    std::reverse(result.begin(), result.end());
    return result;
}
// Query 15: Maintain a list of the top 5 stocks with the highest dividends paid
std::vector<StockData> StockDatabase::getTop5StocksByDividends()
{
    std::priority_queue<StockData, std::vector<StockData>, DividendsComparator> pq;
    for (const auto &record : data)
    {
        pq.push(record);
        if (pq.size() > 5)
        {
            pq.pop();
        }
    }
    std::vector<StockData> result;
    while (!pq.empty())
    {
        result.push_back(pq.top());
        pq.pop();
    }
    std::reverse(result.begin(), result.end());
    return result;
}