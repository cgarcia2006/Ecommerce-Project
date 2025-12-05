#pragma once

#include <string>
#include <vector>
#include "CartItem.h"

class User;

class Order
{
public:
    Order(const std::vector<CartItem>& prods, User* u);
    Order(const std::vector<CartItem>& prods, User* u, const std::string& oID);

    std::string orderID;
    std::vector<CartItem> products;
    std::string buyerID;
    double CalculateTotalPrice() const;
    void PrintInvoice() const;
    std::string getBuyerID() const;
    static int nextID;

private:

   
    User* user;
    double totalPrice = 0.0;
};