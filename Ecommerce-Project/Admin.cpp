#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>

#include "Admin.h"
#include "Product.h"

std::string Admin::code = "54321";

void Admin::ManageProducts(Product& product) {
    
    int choice;
    std::string newName;
    while (true) {
        std::cout << "Managing product: " << product.getName() << std::endl;
        std::cout << "1. Change name " << std::endl;
        std::cout << "2. Add product to display " << std::endl;
        std::cout << "3. Remove product from display " << std::endl;
        std::cout << "4. Change product price " << std::endl;
        std::cout << "5. Change stock quantity " << std::endl;
        std::cout << "6. Delete product " << std::endl;
        std::cout << "7. Exit " << std::endl;
        std::cin >> choice;

        switch (choice) {
        case 1:
            std::cout << "New product name: ";
            std::cin >> newName;
            product.setName(newName);
            std::cout << "Product name changed." << std::endl;
            Database::SaveProducts();
            break;
        case 2:
            product.toDisplay();
            std::cout << "Product added to display." << std::endl;
            Database::SaveProducts();
            break;
        case 3:
            product.outOfDisplay();
            std::cout << "Product removed from display." << std::endl;
            Database::SaveProducts();
            break;
        case 4: {
            double newPrice;
            std::cout << "Enter new price: ";
            std::cin >> newPrice;
            product.setPrice(newPrice);
            std::cout << "Product price updated." << std::endl;
            Database::SaveProducts();
            break;
        }
        case 5: {
            int newQuantity;
            std::cout << "Enter new stock quantity: ";
            std::cin >> newQuantity;
            product.setStockQuantity(newQuantity);
            std::cout << "Product stock quantity updated." << std::endl;
            Database::SaveProducts();
            break;
        }
        case 6: {
            Database::RemoveProductFromDatabase(product);
        }
        case 7:
            return;
        default:
            std::cout << "Invalid choice. Exiting management." << std::endl;
            return;
        }
    }
}

void Admin::ManageUsers(User& user) {
    if (Database::allUsers.empty()) {
        std::cout << "No users yet!\n";
        return;
    }
    int choice;
    while (true) {
        std::cout << "Managing user: " << user.getName() << std::endl;
        std::cout << "1. View Order History " << std::endl;
        std::cout << "2. Ban User " << std::endl;
        std::cout << "3. Exit User Management " << std::endl;
        std::cin >> choice;
        switch (choice) {
        case 1:
            user.ViewOrderHistory();
            break;
        case 2:
            DeleteUser(user);
            std::cout << "User banned and removed from database." << std::endl;
            Database::SaveUsers();
            return;
        case 3:
            std::cout << "Exiting user management." << std::endl;
            return;
        default:
            std::cout << "Invalid choice. Try again." << std::endl;
        }
    }
}

void Admin::AddNewProduct(const Product& product) {
    Database::AddProductToDatabase(product);
    Database::SaveProducts();
}
void Admin::RemoveProduct(const Product& product) {
    Database::RemoveProductFromDatabase(product);
    Database::SaveProducts();
}
void Admin::DeleteUser(const User& user) {
    Database::DeleteUserFromDatabase(user);
    Database::SaveUsers();
}




