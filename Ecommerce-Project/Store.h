#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "Product.h"
#include "User.h"
#include "CartItem.h"

class Store
{
public:
    static void ViewProducts();
    static void Logout(User* currentUser);
    static User* Login(const std::string& email, const std::string& password);
    static User* AdminLogin(const std::string& email, const std::string& password, const std::string& adminPassword);
    static int LoginPhase();
    static User* LoginSecondPhase(int choice, std::string& status);
    static int DisplayStoreOptions(const std::string& stat);
    static void DisplayStoreOptionPhaseTwo(int choice, User* user, const std::string& stat, bool& loop);
    static Product* IdentifyProduct(const std::string& p);
    static void AddToCartStore(User& u, Product* p, int amount);
    static CartItem* IdentifyItem(const std::string& p, User& u);
    static User* IdentifyUser(const std::string& ID);
    static Product* IdentifyProductByID(const std::string& p);
    static void ChangeQuantity(CartItem& i);
    static bool UserExist(const std::string& ID);
private:
};