#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include <vector>
#include <algorithm>
#include <fstream>  // Include for file operations


struct Order {
    double price;
    double quantity;
};

class OrderBook {
public:
    std::vector<Order> bids;
    std::vector<Order> offers;

    void sortBids();
    void sortOffers();
    void print() const; // Add this method to print the order book
    void print(std::ofstream&) const;
    double calculatePriceToBuy(double quantity) const;
    double calculatePriceToSell(double quantity) const;


};

#endif // ORDERBOOK_H
