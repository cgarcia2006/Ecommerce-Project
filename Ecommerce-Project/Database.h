#pragma once
#pragma once

#include <string>
#include <vector>
#include <memory>

class User;
class Product;
class Order;

class Database
{
public:
    static std::vector<std::unique_ptr<User>> allUsers;
    static std::vector<std::shared_ptr<Product>> allProducts;  
    static std::vector<std::unique_ptr<Order>> allOrders;
    static void SaveUsers();
    static void SaveProducts();

    static void loadUsers();
    static void loadProducts();

    static bool InStock(const Product& product);
    static bool AvailableAmount(const Product& product, int requestedQuantity);
    static void AddProductToDatabase(const Product& product);
    static void RemoveProductFromDatabase(const Product& product);
    static void DeleteUserFromDatabase(const User& user);

    static std::shared_ptr<Product> GetProductByID(const std::string& ID);
};
