#include "OrderBookAggregator.h"
#include "ExchangeAPI.h"
#include "utility.h"
#include <iostream>
#include <fstream>  // Include for file operations

void OrderBookAggregator::fetchAndProcessOrderBooks() {
    // Define URLs for APIs
    const std::string coinbaseUrl = "https://api.pro.coinbase.com/products/BTC-USD/book?level=2";
    const std::string geminiUrl = "https://api.gemini.com/v1/book/BTCUSD";
    const std::string krakenUrl = "https://api.kraken.com/0/public/Depth?pair=XBTUSD";

    // Open a file stream to read the configuration
    std::ifstream configFile("config.txt");
    bool includeKraken = false;  // Default value

    if (configFile.is_open()) {
        std::string config;
        std::getline(configFile, config);
        includeKraken = (config == "include_kraken");
        configFile.close();
    }
    else {
        std::cerr << "Unable to open config file. Kraken exchange will be excluded by default." << std::endl;
    }

    std::ifstream inFile("quantity.txt");
    double quantity = 10.0;  // Default quantity

    if (inFile.is_open()) {
        inFile >> quantity;
        inFile.close();
    }
    else {
        std::cerr << "Unable to open quantity file. Using default quantity of 10 Bitcoin." << std::endl;
    }

    // Fetch order books from APIs
    std::string coinbaseData = httpGet(coinbaseUrl);
    std::string geminiData = httpGet(geminiUrl);
    std::string krakenData;

    if (includeKraken) {
        krakenData = httpGet(krakenUrl);
    }

    // Process the data
    OrderBook coinbaseOrderBook = ExchangeAPI::parseOrderBookCoinbase(coinbaseData);
    OrderBook geminiOrderBook = ExchangeAPI::parseOrderBookGemini(geminiData);
    OrderBook krakenOrderBook;

    if (includeKraken) {
        krakenOrderBook = ExchangeAPI::parseOrderBookKraken(krakenData);
    }

    // Open a file stream to write the output
    std::ofstream outFile("order_books.txt");

    if (outFile.is_open()) {
        outFile << "Coinbase Order Book:" << std::endl;
        coinbaseOrderBook.print(outFile);  // Print to file instead of console

        outFile << "Gemini Order Book:" << std::endl;
        geminiOrderBook.print(outFile);  // Print to file instead of console

        if (includeKraken) {
            outFile << "Kraken Order Book:" << std::endl;
            krakenOrderBook.print(outFile);
        }

        // Calculate and print price to buy the specified quantity
        //double quantity = 10.0;  // Default quantity

        double coinbaseBuyPrice = coinbaseOrderBook.calculatePriceToBuy(quantity);
        double geminiBuyPrice = geminiOrderBook.calculatePriceToBuy(quantity);
        double krakenBuyPrice = includeKraken ? krakenOrderBook.calculatePriceToBuy(quantity) : -1.0;

        if (coinbaseBuyPrice >= 0) {
            outFile << "Price to buy " << quantity << " Bitcoin on Coinbase: " << coinbaseBuyPrice << std::endl;
        }
        else {
            outFile << "Not enough offers to buy " << quantity << " Bitcoin on Coinbase." << std::endl;
        }
        if (geminiBuyPrice >= 0) {
            outFile << "Price to buy " << quantity << " Bitcoin on Gemini: " << geminiBuyPrice << std::endl;
        }
        else {
            outFile << "Not enough offers to buy " << quantity << " Bitcoin on Gemini." << std::endl;
        }
        if (includeKraken && krakenBuyPrice >= 0) {
            outFile << "Price to buy " << quantity << " Bitcoin on Kraken: " << krakenBuyPrice << std::endl;
        }
        else if (includeKraken) {
            outFile << "Not enough offers to buy " << quantity << " Bitcoin on Kraken." << std::endl;
        }

        // Calculate and print price to sell the specified quantity
        double coinbaseSellPrice = coinbaseOrderBook.calculatePriceToSell(quantity);
        double geminiSellPrice = geminiOrderBook.calculatePriceToSell(quantity);
        double krakenSellPrice = includeKraken ? krakenOrderBook.calculatePriceToSell(quantity) : -1.0;

        if (coinbaseSellPrice >= 0) {
            outFile << "Price to sell " << quantity << " Bitcoin on Coinbase: " << coinbaseSellPrice << std::endl;
        }
        else {
            outFile << "Not enough bids to sell " << quantity << " Bitcoin on Coinbase." << std::endl;
        }
        if (geminiSellPrice >= 0) {
            outFile << "Price to sell " << quantity << " Bitcoin on Gemini: " << geminiSellPrice << std::endl;
        }
        else {
            outFile << "Not enough bids to sell " << quantity << " Bitcoin on Gemini." << std::endl;
        }
        if (includeKraken && krakenSellPrice >= 0) {
            outFile << "Price to sell " << quantity << " Bitcoin on Kraken: " << krakenSellPrice << std::endl;
        }
        else if (includeKraken) {
            outFile << "Not enough bids to sell " << quantity << " Bitcoin on Kraken." << std::endl;
        }
        outFile.close();
    }
    else {
        std::cerr << "Unable to open file for writing." << std::endl;
    }
}
