#pragma once

#include <iostream>

#include "User.h"
#include "Product.h"
#include "Database.h"

class Admin
{
public:
    static void ManageUsers(User& user);
    static void ManageProducts(Product& product);
    static void AddNewProduct(const Product& product);
    static void RemoveProduct(const Product& product);
    static void DeleteUser(const User& user);
    static std::string code;
private:
    Admin() = delete;
};