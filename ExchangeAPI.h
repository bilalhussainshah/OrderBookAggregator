#ifndef EXCHANGEAPI_H
#define EXCHANGEAPI_H

#include "OrderBook.h"
#include <string>

class ExchangeAPI {
public:
    static OrderBook parseOrderBookCoinbase(const std::string& jsonData);
    static OrderBook parseOrderBookGemini(const std::string& jsonData);
    static OrderBook parseOrderBookKraken(const std::string& json); // New function

};

#endif // EXCHANGEAPI_H
