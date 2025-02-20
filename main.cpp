#include "StockDatabase.h"
#include <iostream>
#include <chrono>

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

int main()
{
    StockDatabase db;
    db.loadData("data/new.csv");

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
            start = std::chrono::high_resolution_clock::now();
            result = db.getDataByDate(date);
            break;

        case 2:
            std::cout << "Enter ticker: ";
            std::cin >> ticker;
            start = std::chrono::high_resolution_clock::now();
            value = db.getAverageClosePrice(ticker);
            break;

        case 3:
            std::cout << "Enter ticker: ";
            std::cin >> ticker;
            std::cout << "Enter start date (YYYY-MM-DD): ";
            std::cin >> startDate;
            std::cout << "Enter end date (YYYY-MM-DD): ";
            std::cin >> endDate;
            start = std::chrono::high_resolution_clock::now();
            value = db.getHighestPriceInPeriod(ticker, startDate, endDate);
            break;

        case 4:
            start = std::chrono::high_resolution_clock::now();
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
            start = std::chrono::high_resolution_clock::now();
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
            start = std::chrono::high_resolution_clock::now();
            value = db.countDatesAboveThreshold(threshold);
            std::cout << "Number of dates with at least one stock closing above " << threshold << ": " << value << "\n";
            break;

        case 7:
            std::cout << "Enter ticker: ";
            std::cin >> ticker;
            std::cout << "Enter date (YYYY-MM-DD): ";
            std::cin >> date;
            start = std::chrono::high_resolution_clock::now();
            value = db.getClosingPrice(ticker, date);
            break;

        case 8:
            std::cout << "Enter ticker: ";
            std::cin >> ticker;
            start = std::chrono::high_resolution_clock::now();
            datePricePairs = db.getDatesAndClosingPrices(ticker);
            break;

        case 9:
            std::cout << "Enter ticker: ";
            std::cin >> ticker;
            start = std::chrono::high_resolution_clock::now();
            value = db.getTotalVolume(ticker);
            break;

        case 10:
            std::cout << "Enter ticker: ";
            std::cin >> ticker;
            std::cout << "Enter date (YYYY-MM-DD): ";
            std::cin >> date;
            start = std::chrono::high_resolution_clock::now();
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
            start = std::chrono::high_resolution_clock::now();
            openClosePrices = db.getOpeningAndClosingPrices(ticker, date);
            break;

        case 12:
            std::cout << "Enter ticker: ";
            std::cin >> ticker;
            std::cout << "Enter date (YYYY-MM-DD): ";
            std::cin >> date;
            start = std::chrono::high_resolution_clock::now();
            value = db.getDividend(ticker, date);
            break;

        case 13:
            std::cout << "Enter date (YYYY-MM-DD): ";
            std::cin >> date;
            start = std::chrono::high_resolution_clock::now();
            result = db.getTop10StocksByVolume(date);
            break;

        case 14:
            start = std::chrono::high_resolution_clock::now();
            result = db.getBottom5StocksByClosingPrice();
            break;

        case 15:
            start = std::chrono::high_resolution_clock::now();
            result = db.getTop5StocksByDividends();
            break;

        case 0:
            std::cout << "Exiting...\n";
            return 0;

        default:
            std::cout << "Invalid choice. Please try again.\n";
            continue;
        }

        // Kraj mjerenja vremena
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "Vrijeme izvršavanja upita: " << duration.count() << " ms\n";

        // Ispis rezultata
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