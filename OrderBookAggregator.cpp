#include "OrderBookAggregator.h"
#include "ExchangeAPI.h"
#include "utility.h"
#include <iostream>
#include <fstream>
#include <algorithm>  // Include for sorting

// Helper function to merge order books
void mergeOrderBooks(OrderBook& merged, const OrderBook& other) {
    merged.offers.insert(merged.offers.end(), other.offers.begin(), other.offers.end());
    merged.bids.insert(merged.bids.end(), other.bids.begin(), other.bids.end());

    // Sort asks (ascending)
    std::sort(merged.offers.begin(), merged.offers.end(),
        [](const Order& a, const Order& b) {
            return a.price < b.price;
        });

    // Sort bids (descending)
    std::sort(merged.bids.begin(), merged.bids.end(),
        [](const Order& a, const Order& b) {
            return a.price > b.price;
        });
}
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
            outFile << "Total Offers on Coinbase: " << coinbaseOrderBook.offers.size() << std::endl;
            outFile << "Price to buy " << quantity << " Bitcoin(s) on Coinbase: " << coinbaseBuyPrice << std::endl;
            outFile << "Average execution price to buy " << quantity << " Bitcoin(s) on Coinbase: " << (coinbaseBuyPrice / quantity) << std::endl;
        }
        else {
            outFile << "Not enough offers to buy " << quantity << " Bitcoin(s) on Coinbase." << std::endl;
        }
        if (geminiBuyPrice >= 0) {
            outFile << "Total Offers on Gemini: " << geminiOrderBook.offers.size() << std::endl;
            outFile << "Price to buy " << quantity << " Bitcoin(s) on Gemini: " << geminiBuyPrice << std::endl;
            outFile << "Average execution price to buy " << quantity << " Bitcoin(s) on Gemini: " << (geminiBuyPrice / quantity) << std::endl;
        }
        else {
            outFile << "Not enough offers to buy " << quantity << " Bitcoin(s) on Gemini." << std::endl;
        }
        if (includeKraken && krakenBuyPrice >= 0) {
            outFile << "Total Offers on Kraken: " << krakenOrderBook.offers.size() << std::endl;
            outFile << "Price to buy " << quantity << " Bitcoin(s) on Kraken: " << krakenBuyPrice << std::endl;
            outFile << "Average execution price to buy " << quantity << " Bitcoin(s) on Kraken: " << (krakenBuyPrice / quantity) << std::endl;
        }
        else if (includeKraken) {
            outFile << "Not enough offers to buy " << quantity << " Bitcoin(s) on Kraken." << std::endl;
        }

        // Calculate and print price to sell the specified quantity
        double coinbaseSellPrice = coinbaseOrderBook.calculatePriceToSell(quantity);
        double geminiSellPrice = geminiOrderBook.calculatePriceToSell(quantity);
        double krakenSellPrice = includeKraken ? krakenOrderBook.calculatePriceToSell(quantity) : -1.0;

        if (coinbaseSellPrice >= 0) {
            outFile << "Total Bids on Coinbase: " << coinbaseOrderBook.bids.size() << std::endl;
            outFile << "Price to sell " << quantity << " Bitcoin(s) on Coinbase: " << coinbaseSellPrice << std::endl;
            outFile << "Average execution price to sell " << quantity << " Bitcoin(s) on Coinbase: " << (coinbaseSellPrice / quantity) << std::endl;
        }
        else {
            outFile << "Not enough bids to sell " << quantity << " Bitcoin(s) on Coinbase." << std::endl;
        }
        if (geminiSellPrice >= 0) {
            outFile << "Total Bids on Gemini: " << geminiOrderBook.bids.size() << std::endl;
            outFile << "Price to sell " << quantity << " Bitcoin(s) on Gemini: " << geminiSellPrice << std::endl;
            outFile << "Average execution price to sell " << quantity << " Bitcoin(s) on Gemini: " << (geminiSellPrice / quantity) << std::endl;
        }
        else {
            outFile << "Not enough bids to sell " << quantity << " Bitcoin(s) on Gemini." << std::endl;
        }
        if (includeKraken && krakenSellPrice >= 0) {
            outFile << "Total Bids on Kraken: " << krakenOrderBook.bids.size() << std::endl;
            outFile << "Price to sell " << quantity << " Bitcoin(s) on Kraken: " << krakenSellPrice << std::endl;
            outFile << "Average execution price to sell " << quantity << " Bitcoin(s) on Kraken: " << (krakenSellPrice / quantity) << std::endl;
        }
        else if (includeKraken) {
            outFile << "Not enough bids to sell " << quantity << " Bitcoin(s) on Kraken." << std::endl;
        }
        outFile.close();
    }
    else {
        std::cerr << "Unable to open file for writing." << std::endl;
    }

    // Open a file stream to write the merged order book output
    std::ofstream outMergedFile("merged_order_books.txt");

    if (outMergedFile.is_open()) {

        OrderBook mergedOrderBook;

        // Merge order books from different exchanges
        mergeOrderBooks(mergedOrderBook, coinbaseOrderBook);
        mergeOrderBooks(mergedOrderBook, geminiOrderBook);
        if (includeKraken) {
            mergeOrderBooks(mergedOrderBook, krakenOrderBook);
        }

        outMergedFile << "Merged Order Book:" << std::endl;
        mergedOrderBook.print(outMergedFile);


        double totalCostToBuy = mergedOrderBook.calculateTotalCostToBuy(quantity);
        double totalCostToSell = mergedOrderBook.calculateTotalCostToSell(quantity);

        if (totalCostToBuy >= 0) {
            outMergedFile << "Total Offers in Merged Order Book: " << mergedOrderBook.offers.size() << std::endl;
            outMergedFile << "Total cost to buy " << quantity << " Bitcoin across merged order book: " << totalCostToBuy << std::endl;
            outMergedFile << "Average execution price to buy " << quantity << " Bitcoin(s) across merged order book: " << (totalCostToBuy / quantity) << std::endl;
        }
        else {
            outMergedFile << "Not enough offers to buy " << quantity << " Bitcoin across merged order book." << std::endl;
        }

        if (totalCostToSell >= 0) {
            outMergedFile << "Total Bids in Merged Order Book: " << mergedOrderBook.bids.size() << std::endl;
            outMergedFile << "Total cost to sell " << quantity << " Bitcoin(s) across merged order book: " << totalCostToSell << std::endl;
            outMergedFile << "Average execution price to sell " << quantity << " Bitcoin(s) across merged order book: " << (totalCostToSell / quantity) << std::endl;
        }
        else {
            outMergedFile << "Not enough bids to sell " << quantity << " Bitcoin(s) across merged order book." << std::endl;
        }
    }
    else {
        std::cerr << "Unable to open file for writing." << std::endl;
    }
}
