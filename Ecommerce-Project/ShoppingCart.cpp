#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>

#include "ShoppingCart.h"
#include "Order.h"
#include "Product.h"
#include "User.h"
#include "Database.h"

void ShoppingCart::AddToCart(Product* p, int quantity) {
    if (!p) return;

    auto sp = Database::GetProductByID(p->getProductID());
    if (!sp) {
        std::cout << "Error: product not found in database.\n";
        return;
    }

    products.emplace_back(sp, quantity);  // shared_ptr -> weak_ptr inside CartItem
}


void ShoppingCart::CleanExpiredItems() {
    products.erase(
        std::remove_if(products.begin(), products.end(),
            [](const CartItem& item) {
                return item.product.expired();
            }),
        products.end()
    );
}



void ShoppingCart::RemoveFromCart(CartItem& p) {
    CleanExpiredItems();

    auto targetPtr = p.product.lock();
    if (!targetPtr) {
        std::cout << "Product no longer exists in store. Removing from cart.\n";
        // just clean again; nothing else to do
        CleanExpiredItems();
        return;
    }

    bool deleted = false;
    for (auto it = products.begin(); it != products.end(); ) {
        if (auto sp = it->product.lock()) {
            if (sp->getProductID() == targetPtr->getProductID()) {
                it = products.erase(it);
                deleted = true;
            }
            else {
                ++it;
            }
        }
        else {
            // product expired, drop it
            it = products.erase(it);
        }
    }

    if (deleted) {
        std::cout << "Product removed from cart." << std::endl;
    }
    else {
        std::cout << "Product not found in cart." << std::endl;
    }
}


void ShoppingCart::ViewCart() {
    CleanExpiredItems();

    if (products.empty()) {
        std::cout << "Shopping cart is empty!\n";
        return;
    }

    std::cout << "********** Shopping Cart **********" << std::endl;
    for (const auto& item : products) {
        if (auto p = item.product.lock()) {
            std::cout << "Product ID: " << p->getProductID()
                << ", Name: " << p->getName()
                << ", Price: $" << p->getPrice()
                << ", Quantity: " << item.Quantity << std::endl;
        }
    }
}


void ShoppingCart::BuyCart(User& user) {
    CleanExpiredItems();

    if (CartIsEmpty()) {
        std::cout << "Shopping cart is empty!\n";
        return;
    }

    bool doable = true;
    for (const auto& item : products) {
        auto p = item.product.lock();
        if (!p) continue; // product vanished, effectively ignored by cleaner

        if (!Database::InStock(*p) || !Database::AvailableAmount(*p, item.Quantity)) {
            doable = false;
            break;
        }
    }

    if (!doable) {
        std::cout << "One or more items in your cart are out of stock or exceed available quantity." << std::endl;
        std::cout << "Purchase could not be completed." << std::endl;
        return;
    }

    // update stock
    for (auto& item : products) {
        if (auto p = item.product.lock()) {
            p->setStockQuantity(p->getStockQuantity() - item.Quantity);
        }
    }

    std::cout << "Purchase successful!" << std::endl;

    auto orderPtr = std::make_unique<Order>(products, &user);
    user.orderHistory.push_back(*orderPtr);             // copy for user
    Database::allOrders.push_back(std::move(orderPtr)); // unique record in DB

    products.clear();
    Database::SaveOrders();
    Database::SaveProducts();
}


void ShoppingCart::ClearCart() {
    products.clear();
    std::cout << "Shopping cart cleared." << std::endl;
}

void ShoppingCart::UpdateQuantity(CartItem* item, int newQuantity) {
    if (item) item->Quantity = newQuantity;
}

void ShoppingCart::ManageCart() {
    CleanExpiredItems();

    if (products.empty()) {
        std::cout << "Shopping cart is empty!\n";
        return;
    }

    std::string itemEdit;
    int newQuantity;

    std::cout << "Current items in your cart:" << std::endl;
    for (const auto& items : products) {
        if (auto p = items.product.lock()) {
            std::cout << "Product: " << p->getName()
                << ", Quantity: " << items.Quantity << std::endl;
        }
    }

    std::cout << "Enter the name of the product you want to update the quantity for: ";
    std::cin >> itemEdit;
    std::cout << "Enter the new quantity: ";
    std::cin >> newQuantity;

    for (auto& items : products) {
        if (auto p = items.product.lock()) {
            if (p->getName() == itemEdit) {
                UpdateQuantity(&items, newQuantity);
                std::cout << "Quantity updated." << std::endl;
                return;
            }
        }
    }

    std::cout << "Product not found.\n";
}


bool ShoppingCart::CartIsEmpty() {
    CleanExpiredItems();
    return products.empty();
}
