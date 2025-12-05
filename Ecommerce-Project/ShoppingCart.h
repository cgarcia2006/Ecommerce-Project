#pragma once

#include <iostream>
#include <vector>
#include "CartItem.h"
#include "Product.h"

class User;

class ShoppingCart
{
public:
    std::vector<CartItem> products;

    void BuyCart(User& user);


    void ViewCart();

    void AddToCart(Product* p, int quantity);
    void RemoveFromCart(CartItem& p);
    void ClearCart();
    void UpdateQuantity(CartItem* item, int newQuantity);
    void ManageCart();
    bool CartIsEmpty();


    void CleanExpiredItems();

};
