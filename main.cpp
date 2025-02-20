#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <chrono>
#include <set>
#include <unordered_set>
#include <queue>

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
    // Load data from CSV file
    void loadData(const std::string &filename)
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

    // Query 1: Get all stock data for a specific date
    std::vector<StockData> getDataByDate(const std::string &date)
    {
        return dateMap[date];
    }

    // Query 2: Calculate the average closing price for a specific ticker
    double getAverageClosePrice(const std::string &ticker)
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

    // Query 3: Find the highest price for a specific ticker in a given time period
    double getHighestPriceInPeriod(const std::string &ticker, const std::string &startDate, const std::string &endDate)
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

    // Query 4: Identify all unique tickers in the dataset
    std::set<std::string> getAllUniqueTickers()
    {
        std::set<std::string> uniqueTickers;
        for (const auto &record : data)
        {
            uniqueTickers.insert(record.ticker);
        }
        return uniqueTickers;
    }

    // Query 5: Check if a specific ticker exists in the dataset
    bool doesTickerExist(const std::string &ticker)
    {
        return tickerMap.find(ticker) != tickerMap.end();
    }

    // Query 6: Count the number of dates where at least one stock had a closing price above a threshold
    int countDatesAboveThreshold(double threshold)
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

    // Query 7: Get the closing price of a specific ticker on a specific date
    double getClosingPrice(const std::string &ticker, const std::string &date)
    {
        if (tickerDateMap.find(ticker) != tickerDateMap.end() && tickerDateMap[ticker].find(date) != tickerDateMap[ticker].end())
        {
            return tickerDateMap[ticker][date].close;
        }
        return -1; // Return -1 if not found
    }

    // Query 8: Get all dates and corresponding closing prices for a specific ticker
    std::vector<std::pair<std::string, double>> getDatesAndClosingPrices(const std::string &ticker)
    {
        std::vector<std::pair<std::string, double>> result;
        for (const auto &record : tickerMap[ticker])
        {
            result.push_back({record.date, record.close});
        }
        return result;
    }

    // Query 9: Calculate the total trading volume for a specific ticker
    double getTotalVolume(const std::string &ticker)
    {
        double totalVolume = 0;
        for (const auto &record : tickerMap[ticker])
        {
            totalVolume += record.volume;
        }
        return totalVolume;
    }

    // Query 10: Check if data exists for a specific date and ticker
    bool doesDataExist(const std::string &ticker, const std::string &date)
    {
        return tickerDateMap.find(ticker) != tickerDateMap.end() && tickerDateMap[ticker].find(date) != tickerDateMap[ticker].end();
    }

    // Query 11: Get opening and closing prices for a specific ticker and date in constant time
    std::pair<double, double> getOpeningAndClosingPrices(const std::string &ticker, const std::string &date)
    {
        if (tickerDateMap.find(ticker) != tickerDateMap.end() && tickerDateMap[ticker].find(date) != tickerDateMap[ticker].end())
        {
            return {tickerDateMap[ticker][date].open, tickerDateMap[ticker][date].close};
        }
        return {-1, -1}; // Return {-1, -1} if not found
    }

    // Query 12: Find the dividend amount for a specific ticker on a specific date
    double getDividend(const std::string &ticker, const std::string &date)
    {
        if (tickerDateMap.find(ticker) != tickerDateMap.end() && tickerDateMap[ticker].find(date) != tickerDateMap[ticker].end())
        {
            return tickerDateMap[ticker][date].dividends;
        }
        return -1; // Return -1 if not found
    }

    // Query 13: Find the top 10 stocks with the highest trading volume on a specific date
    std::vector<StockData> getTop10StocksByVolume(const std::string &date)
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

    // Query 14: Get the 5 stocks with the lowest closing prices in the entire dataset
    std::vector<StockData> getBottom5StocksByClosingPrice()
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
    std::vector<StockData> getTop5StocksByDividends()
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
};

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <chrono>
#include <set>
#include <queue>

// ... (Include the StockDatabase class and its methods here)

void displayMenu()
{
    std::cout << "\n=== Stock Database Queries ===\n";
    std::cout << "1. Get all stock data for a specific date\n";
    std::cout << "2. Calculate the average closing price for a specific ticker\n";
    std::cout << "3. Find the highest price for a specific ticker in a given time period\n";
    std::cout << "4. Identify all unique tickers in the dataset\n";
    std::cout << "5. Check if a specific ticker exists in the dataset\n";
    std::cout << "6. Count the number of dates where at least one stock had a closing price above a threshold\n";
    std::cout << "7. Get the closing price of a specific ticker on a specific date\n";
    std::cout << "8. Get all dates and corresponding closing prices for a specific ticker\n";
    std::cout << "9. Calculate the total trading volume for a specific ticker\n";
    std::cout << "10. Check if data exists for a specific date and ticker\n";
    std::cout << "11. Get opening and closing prices for a specific ticker and date\n";
    std::cout << "12. Find the dividend amount for a specific ticker on a specific date\n";
    std::cout << "13. Find the top 10 stocks with the highest trading volume on a specific date\n";
    std::cout << "14. Get the 5 stocks with the lowest closing prices in the entire dataset\n";
    std::cout << "15. Maintain a list of the top 5 stocks with the highest dividends paid\n";
    std::cout << "0. Exit\n";
    std::cout << "Enter your choice: ";
}

#include <chrono>

int main()
{
    StockDatabase db;
    db.loadData("all_stock_data.csv");

    int choice;
    std::string date, ticker, startDate, endDate;
    double threshold;
    std::vector<StockData> result;
    std::vector<std::pair<std::string, double>> datePricePairs;
    std::pair<double, double> openClosePrices;
    double value;

    while (true)
    {
        displayMenu();
        std::cin >> choice;

        // Početak mjerenja vremena
        auto start = std::chrono::high_resolution_clock::now();

        switch (choice)
        {
        case 1:
            std::cout << "Enter date (YYYY-MM-DD): ";
            std::cin >> date;
            start = std::chrono::high_resolution_clock::now(); // Reset timer before query
            result = db.getDataByDate(date);
            break;

        case 2:
            std::cout << "Enter ticker: ";
            std::cin >> ticker;
            start = std::chrono::high_resolution_clock::now(); // Reset timer before query
            value = db.getAverageClosePrice(ticker);
            break;

        case 3:
            std::cout << "Enter ticker: ";
            std::cin >> ticker;
            std::cout << "Enter start date (YYYY-MM-DD): ";
            std::cin >> startDate;
            std::cout << "Enter end date (YYYY-MM-DD): ";
            std::cin >> endDate;
            start = std::chrono::high_resolution_clock::now(); // Reset timer before query
            value = db.getHighestPriceInPeriod(ticker, startDate, endDate);
            break;

        case 4:
            start = std::chrono::high_resolution_clock::now(); // Reset timer before query
            {
                auto uniqueTickers = db.getAllUniqueTickers();
                std::cout << "Unique tickers:\n";
                for (const auto &t : uniqueTickers)
                {
                    std::cout << t << "\n";
                }
            }
            break;

        case 5:
            std::cout << "Enter ticker: ";
            std::cin >> ticker;
            start = std::chrono::high_resolution_clock::now(); // Reset timer before query
            if (db.doesTickerExist(ticker))
            {
                std::cout << "Ticker " << ticker << " exists.\n";
            }
            else
            {
                std::cout << "Ticker " << ticker << " does not exist.\n";
            }
            break;

        case 6:
            std::cout << "Enter threshold: ";
            std::cin >> threshold;
            start = std::chrono::high_resolution_clock::now(); // Reset timer before query
            value = db.countDatesAboveThreshold(threshold);
            std::cout << "Number of dates with at least one stock closing above " << threshold << ": " << value << "\n";
            break;

        case 7:
            std::cout << "Enter ticker: ";
            std::cin >> ticker;
            std::cout << "Enter date (YYYY-MM-DD): ";
            std::cin >> date;
            start = std::chrono::high_resolution_clock::now(); // Reset timer before query
            value = db.getClosingPrice(ticker, date);
            break;

        case 8:
            std::cout << "Enter ticker: ";
            std::cin >> ticker;
            start = std::chrono::high_resolution_clock::now(); // Reset timer before query
            datePricePairs = db.getDatesAndClosingPrices(ticker);
            break;

        case 9:
            std::cout << "Enter ticker: ";
            std::cin >> ticker;
            start = std::chrono::high_resolution_clock::now(); // Reset timer before query
            value = db.getTotalVolume(ticker);
            break;

        case 10:
            std::cout << "Enter ticker: ";
            std::cin >> ticker;
            std::cout << "Enter date (YYYY-MM-DD): ";
            std::cin >> date;
            start = std::chrono::high_resolution_clock::now(); // Reset timer before query
            if (db.doesDataExist(ticker, date))
            {
                std::cout << "Data exists for " << ticker << " on " << date << ".\n";
            }
            else
            {
                std::cout << "Data does not exist.\n";
            }
            break;

        case 11:
            std::cout << "Enter ticker: ";
            std::cin >> ticker;
            std::cout << "Enter date (YYYY-MM-DD): ";
            std::cin >> date;
            start = std::chrono::high_resolution_clock::now(); // Reset timer before query
            openClosePrices = db.getOpeningAndClosingPrices(ticker, date);
            break;

        case 12:
            std::cout << "Enter ticker: ";
            std::cin >> ticker;
            std::cout << "Enter date (YYYY-MM-DD): ";
            std::cin >> date;
            start = std::chrono::high_resolution_clock::now(); // Reset timer before query
            value = db.getDividend(ticker, date);
            break;

        case 13:
            std::cout << "Enter date (YYYY-MM-DD): ";
            std::cin >> date;
            start = std::chrono::high_resolution_clock::now(); // Reset timer before query
            result = db.getTop10StocksByVolume(date);
            break;

        case 14:
            start = std::chrono::high_resolution_clock::now(); // Reset timer before query
            result = db.getBottom5StocksByClosingPrice();
            break;

        case 15:
            start = std::chrono::high_resolution_clock::now(); // Reset timer before query
            result = db.getTop5StocksByDividends();
            break;

        case 0:
            std::cout << "Exiting...\n";
            return 0;

        default:
            std::cout << "Invalid choice. Please try again.\n";
            continue; // Skip timing for invalid choices
        }

        // Kraj mjerenja vremena
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "Vrijeme izvršavanja upita: " << duration.count() << " ms\n";

        // Ispis rezultata (ako je potrebno)
        switch (choice)
        {
        case 1:
            for (const auto &record : result)
            {
                std::cout << "Ticker: " << record.ticker << ", Open: " << record.open << ", High: " << record.high
                          << ", Low: " << record.low << ", Close: " << record.close << ", Volume: " << record.volume
                          << ", Dividends: " << record.dividends << "\n";
            }
            break;

        case 2:
            std::cout << "Average closing price for " << ticker << ": " << value << "\n";
            break;

        case 3:
            std::cout << "Highest price for " << ticker << " between " << startDate << " and " << endDate << ": " << value << "\n";
            break;

        case 7:
            if (value != -1)
            {
                std::cout << "Closing price for " << ticker << " on " << date << ": " << value << "\n";
            }
            else
            {
                std::cout << "Data not found.\n";
            }
            break;

        case 8:
            for (const auto &pair : datePricePairs)
            {
                std::cout << "Date: " << pair.first << ", Close: " << pair.second << "\n";
            }
            break;

        case 9:
            std::cout << "Total trading volume for " << ticker << ": " << value << "\n";
            break;

        case 11:
            if (openClosePrices.first != -1)
            {
                std::cout << "Open: " << openClosePrices.first << ", Close: " << openClosePrices.second << "\n";
            }
            else
            {
                std::cout << "Data not found.\n";
            }
            break;

        case 12:
            if (value != -1)
            {
                std::cout << "Dividend for " << ticker << " on " << date << ": " << value << "\n";
            }
            else
            {
                std::cout << "Data not found.\n";
            }
            break;

        case 13:
            for (const auto &record : result)
            {
                std::cout << "Ticker: " << record.ticker << ", Volume: " << record.volume << "\n";
            }
            break;

        case 14:
            for (const auto &record : result)
            {
                std::cout << "Ticker: " << record.ticker << ", Close: " << record.close << "\n";
            }
            break;

        case 15:
            for (const auto &record : result)
            {
                std::cout << "Ticker: " << record.ticker << ", Dividends: " << record.dividends << "\n";
            }
            break;
        }
    }

    return 0;
}
