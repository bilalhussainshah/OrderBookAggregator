#include "ExchangeAPI.h"
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

OrderBook ExchangeAPI::parseOrderBookCoinbase(const std::string& jsonData) {
    OrderBook orderBook;
    try {
        auto data = json::parse(jsonData);

        for (const auto& bid : data["bids"]) {
            orderBook.bids.push_back({ std::stod(bid[0].get<std::string>()), std::stod(bid[1].get<std::string>()) });
        }
        for (const auto& offer : data["asks"]) {
            orderBook.offers.push_back({ std::stod(offer[0].get<std::string>()), std::stod(offer[1].get<std::string>()) });
        }

        orderBook.sortBids();
        orderBook.sortOffers();
    }
    catch (const json::parse_error& e) {
        std::cerr << "JSON Parse Error: " << e.what() << std::endl;
    }
    return orderBook;
}

OrderBook ExchangeAPI::parseOrderBookGemini(const std::string& jsonData) {
    OrderBook orderBook;
    try {
        auto data = json::parse(jsonData);

        for (const auto& bid : data["bids"]) {
            orderBook.bids.push_back({ std::stod(bid["price"].get<std::string>()), std::stod(bid["amount"].get<std::string>()) });
        }
        for (const auto& ask : data["asks"]) {
            orderBook.offers.push_back({ std::stod(ask["price"].get<std::string>()), std::stod(ask["amount"].get<std::string>()) });
        }

        orderBook.sortBids();
        orderBook.sortOffers();
    }
    catch (const json::parse_error& e) {
        std::cerr << "JSON Parse Error: " << e.what() << std::endl;
    }
    return orderBook;
}


OrderBook ExchangeAPI::parseOrderBookKraken(const std::string& jsonStr) {
    OrderBook orderBook;
    json root = json::parse(jsonStr);

    if (root.contains("result") && root["result"].contains("XXBTZUSD")) {
        const auto& result = root["result"]["XXBTZUSD"];

        // Parse asks (offers)
        const auto& asks = result["asks"];
        for (const auto& ask : asks) {
            Order order;
            order.price = std::stod(ask[0].get<std::string>());
            order.quantity = std::stod(ask[1].get<std::string>());
            orderBook.offers.push_back(order);
        }

        // Parse bids
        const auto& bids = result["bids"];
        for (const auto& bid : bids) {
            Order order;
            order.price = std::stod(bid[0].get<std::string>());
            order.quantity = std::stod(bid[1].get<std::string>());
            orderBook.bids.push_back(order);
        }

        // Sort the order book
        orderBook.sortBids();
        orderBook.sortOffers();
    }
    else {
        std::cerr << "Failed to parse Kraken order book JSON" << std::endl;
    }

    return orderBook;
}