#include "OrderBook.h"
#include <iostream>
#include <fstream>  // Include for file operations





void OrderBook::sortBids() {
    std::sort(bids.begin(), bids.end(), [](const Order& a, const Order& b) {
        return a.price > b.price; // Higher prices first
        });
}

void OrderBook::sortOffers() {
    std::sort(offers.begin(), offers.end(), [](const Order& a, const Order& b) {
        return a.price < b.price; // Lower prices first
        });
}


// Overloaded print method to write to a file
void OrderBook::print(std::ofstream& outFile) const {
    outFile << "Bids:" << std::endl;
    for (const auto& bid : bids) {
        outFile << "Price: " << bid.price << ", Quantity: " << bid.quantity << std::endl;
    }
    outFile << "Offers:" << std::endl;
    for (const auto& offer : offers) {
        outFile << "Price: " << offer.price << ", Quantity: " << offer.quantity << std::endl;
    }
}



void OrderBook::print() const {

    std::cout << "Bids:" << std::endl;
    for (const auto& bid : bids) {
        std::cout << "Price: " << bid.price << ", Quantity: " << bid.quantity << std::endl;
    }
    std::cout << "Offers:" << std::endl;
    for (const auto& offer : offers) {
        std::cout << "Price: " << offer.price << ", Quantity: " << offer.quantity << std::endl;
    }
}

double OrderBook::calculatePriceToBuy(double quantity) const {
    double totalCost = 0.0;
    double remainingQuantity = quantity;

    for (const auto& offer : offers) {
        double quantityAtLevel = std::min(remainingQuantity, offer.quantity);
        totalCost += quantityAtLevel * offer.price;
        remainingQuantity -= quantityAtLevel;

        if (remainingQuantity <= 0) break;
    }

    // If not enough offers are available to buy the desired quantity, return -1
    if (remainingQuantity > 0) {
        return -1.0;
    }

    return totalCost;
}

double OrderBook::calculatePriceToSell(double quantity) const {
    double totalRevenue = 0.0;
    double remainingQuantity = quantity;

    for (const auto& bid : bids) {
        double quantityAtLevel = std::min(remainingQuantity, bid.quantity);
        totalRevenue += quantityAtLevel * bid.price;
        remainingQuantity -= quantityAtLevel;

        if (remainingQuantity <= 0) break;
    }

    // If not enough bids are available to sell the desired quantity, return -1
    if (remainingQuantity > 0) {
        return -1.0;
    }

    return totalRevenue;
}

double OrderBook::calculateTotalCostToBuy(double quantity) const {
    double totalCost = 0.0;
    double remainingQuantity = quantity;

    for (const auto& ask : offers) {
        double availableQuantity = ask.quantity;
        double tradeQuantity = std::min(remainingQuantity, availableQuantity);
        totalCost += tradeQuantity * ask.price;
        remainingQuantity -= tradeQuantity;
        if (remainingQuantity <= 0) break;
    }

    if (remainingQuantity > 0) {
        return -1.0;  // Not enough liquidity to fulfill the order
    }

    return totalCost;
}

double OrderBook::calculateTotalCostToSell(double quantity) const {
    double totalCost = 0.0;
    double remainingQuantity = quantity;

    for (const auto& bid : bids) {
        double availableQuantity = bid.quantity;
        double tradeQuantity = std::min(remainingQuantity, availableQuantity);
        totalCost += tradeQuantity * bid.price;
        remainingQuantity -= tradeQuantity;
        if (remainingQuantity <= 0) break;
    }

    if (remainingQuantity > 0) {
        return -1.0;  // Not enough liquidity to fulfill the order
    }

    return totalCost;
}
