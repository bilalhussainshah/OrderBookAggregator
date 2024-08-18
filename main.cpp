#include <iostream>
#include "OrderBookAggregator.h"

int main() {
    try {
        // Create an instance of OrderBookAggregator
        OrderBookAggregator aggregator;

        // Fetch and process order books from both APIs
        aggregator.fetchAndProcessOrderBooks();

        return 0;
    }
    catch (const std::exception& ex) {
        std::cerr << "Exception: " << ex.what() << std::endl;
        return 1;
    }
}
