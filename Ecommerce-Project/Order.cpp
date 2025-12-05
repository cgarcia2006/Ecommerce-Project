#include <iostream>
#include <string>
#include <vector>

#include "Order.h"
#include "User.h"
#include "Database.h"

int Order::nextID = 0;

Order::Order(const std::vector<CartItem>& prods, User* u)
    : products(prods), user(u), buyerID(u ? u->getUserID() : std::string())
{
    orderID = "ORD" + std::to_string(nextID++);
}

Order::Order(const std::vector<CartItem>& prods, User* u, const std::string& oID)
    : products(prods), user(u), orderID(oID), buyerID(u ? u->getUserID() : std::string())
{}

void Order::PrintInvoice() const {
    std::cout << "Order ID: " << orderID << std::endl;
    std::cout << "Products:" << std::endl;
    for (const auto& item : products)
    {
        if (auto p = item.product.lock()) {
            std::cout << "- " << p->getName()
                << " (Amount: " << item.Quantity
                << "): $" << p->getPrice() << std::endl;
        }
        else {
            std::cout << "- [Product no longer available]"
                << " (Amount: " << item.Quantity << ")\n";
        }
    }
    std::cout << "Total Price: $" << CalculateTotalPrice() << std::endl;
}

double Order::CalculateTotalPrice() const {
    double sum = 0.0;
    for (const auto& item : products) {
        if (auto p = item.product.lock()) {
            sum += p->getPrice() * item.Quantity;
        }
    }
    return sum;
}

std::string Order::getBuyerID() const {
    return buyerID;
}