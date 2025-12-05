
#if defined(_MSC_VER) && _MSC_VER < 1914
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#else
#include <filesystem>
namespace fs = std::filesystem;
#endif

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <limits>
#include <sstream>   
#include <cctype>


#include "Database.h"
#include "User.h"
#include "Product.h"
#include "Order.h"
#include "CartItem.h"

std::vector<std::unique_ptr<User>> Database::allUsers;
std::vector<std::shared_ptr<Product>> Database::allProducts;
std::vector<std::unique_ptr<Order>> Database::allOrders;

static std::vector<std::string> split(const std::string& s, char delim = '|') {
    std::vector<std::string> parts;
    std::string token;
    std::istringstream iss(s);
    while (std::getline(iss, token, delim)) parts.push_back(token);
    return parts;
}

std::shared_ptr<Product> Database::GetProductByID(const std::string& ID) {
    for (auto& p : allProducts) {
        if (ID == p->getProductID()) {
            return p;
        }
    }
    return nullptr;
}

static std::string sanitize(const std::string& s) {
    std::string t = s;
    for (char& c : t) {
        if (c == '<' || c == '>' || c == ':' || c == '"' || c == '/' || c == '\\' || c == '|' || c == '?' || c == '*')
            c = '_';
    }
    return t;
}

void Database::SaveUsers() {
    const fs::path root = "C:/Users/Cesar/source/repos/Ecommerce-Project/Users";
    fs::create_directories(root);

    for (const auto& uptr : allUsers) {
        const User& u = *uptr;
        fs::path userDir = root / sanitize(u.getName());
        fs::create_directories(userDir);
        fs::path filePath = userDir / "user.txt";

        std::ofstream out(filePath);
        if (!out) {
            std::cerr << "Failed to open " << filePath.string() << "\n";
            continue;
        }

        out << "INFO\n";
        out << u.getUserID() << '|' << u.getName() << '|' << u.getEmail()
            << '|' << u.getPassword() << '|' << (u.getIsAdmin() ? 1 : 0) << "\n";

        // CART
   
        std::vector<const CartItem*> validCartItems;
        for (const auto& item : u.cart.products) {
            if (auto p = item.product.lock()) {
                validCartItems.push_back(&item);
            }
        }
        out << "CART|" << validCartItems.size() << "\n";
        for (const auto* itemPtr : validCartItems) {
            if (auto p = itemPtr->product.lock()) {
                out << p->getProductID() << '|'
                    << p->getName() << '|'
                    << p->getPrice() << '|'
                    << itemPtr->Quantity << "\n";
            }
        }

        // ORDERS
        for (const auto& ord : u.orderHistory) {
            std::vector<const CartItem*> validItems;
            for (const auto& item : ord.products) {
                if (auto p = item.product.lock()) {
                    validItems.push_back(&item);
                }
            }

            out << "ORDER|" << ord.orderID << '|' << validItems.size() << "\n";
            for (const auto* itemPtr : validItems) {
                if (auto p = itemPtr->product.lock()) {
                    out << p->getProductID() << '|'
                        << p->getName() << '|'
                        << p->getPrice() << '|'
                        << itemPtr->Quantity << "\n";
                }
            }
            out << "TOTAL|" << ord.CalculateTotalPrice() << "\n";
        }

    }
}

void Database::SaveProducts() {
    const std::string productPath = "C:/Users/Cesar/source/repos/Ecommerce-Project/products.txt";
    std::ofstream out(productPath);
    if (!out) {
        std::cerr << "Error opening product file for writing: " << productPath << "\n";
        return;
    }

    out << "PRODUCTS|" << allProducts.size() << "\n";

    for (const auto& pPtr : allProducts) {
        const Product& p = *pPtr;
        out << p.getProductID() << '|'
            << p.getName() << '|'
            << p.getPrice() << '|'
            << p.getStockQuantity() << '|'
            << (p.isOnDisplay() ? 1 : 0) << "\n";
    }
}

bool Database::InStock(const Product& p) {
    return p.getStockQuantity() > 0;
}

bool Database::AvailableAmount(const Product& p, int requestedQuantity) {
    return p.getStockQuantity() >= requestedQuantity;
}

void Database::AddProductToDatabase(const Product& product) {
    allProducts.emplace_back(std::make_shared<Product>(product));
    SaveProducts();
    std::cout << "Product added to database and saved.\n";
}



void Database::RemoveProductFromDatabase(const Product& product) {
    auto target = product.getProductID();
    bool deleted = false;

    for (size_t i = 0; i < allProducts.size(); ++i) {
        if (allProducts[i]->getProductID() == target) {
            allProducts.erase(allProducts.begin() + i);
            deleted = true;
            std::cout << "Product removed from in-memory database.\n";
            break;
        }
    }

    if (!deleted) {
        std::cout << "Product not found in database.\n";
        return;
    }

    SaveProducts();
    std::cout << "Products file updated.\n";
}


void Database::DeleteUserFromDatabase(const User& u) {
    auto target = u.getUserID();
    auto userNameSanitized = sanitize(u.getName());
    bool deleted = false;

    for (size_t i = 0; i < allUsers.size(); ++i) {
        if (allUsers[i]->getUserID() == target) {
            std::cout << "Removing user \"" << allUsers[i]->getName()
                << "\" from in-memory database.\n";
            allUsers.erase(allUsers.begin() + i);
            deleted = true;
            break;
        }
    }

    if (!deleted) {
        std::cout << "User not found in database.\n";
        return;
    }

    const fs::path root = "C:/Users/Cesar/source/repos/Ecommerce-Project/Users";
    fs::path userDir = root / userNameSanitized;
    std::error_code ec;
    fs::remove_all(userDir, ec);  
    if (ec) {
        std::cerr << "Warning: failed to remove user directory "
            << userDir.string() << ": " << ec.message() << "\n";
    }
    else {
        std::cout << "User directory removed: " << userDir.string() << "\n";
    }
}

void Database::loadUsers() {
    int maxID = 0;
    int maxIDo = 0;
    allUsers.clear();
    const fs::path root = "C:/Users/Cesar/source/repos/Ecommerce-Project/Users";
    if (!fs::exists(root)) return;

    for (const auto& dirEntry : fs::directory_iterator(root)) {
        if (!dirEntry.is_directory()) continue;

        fs::path filePath = dirEntry.path() / "user.txt";
        if (!fs::exists(filePath)) continue;

        std::cout << "Loading file: " << filePath.string() << "\n";
        std::ifstream in(filePath);
        if (!in) {
            std::cerr << "Failed to open " << filePath.string() << "\n";
            continue;
        }

        std::string line;

        // ---- INFO header ----
        if (!std::getline(in, line) || line != "INFO") {
            std::cerr << "Bad format (expected INFO) in "
                << filePath.string() << "\n";
            continue;
        }

        // ---- INFO data ----
        if (!std::getline(in, line)) continue;
        auto info = split(line);
        if (info.size() != 5) {
            std::cerr << "Bad INFO line in " << filePath.string() << "\n";
            continue;
        }
        std::string id = info[0];
        std::string name = info[1];
        std::string email = info[2];
        std::string password = info[3];
        bool isAdmin = (info[4] == "1" || info[4] == "true" || info[4] == "True");

        maxID = std::max(maxID, std::stoi(id));

        allUsers.emplace_back(std::make_unique<User>(name, email, password, isAdmin, id));
        User* u = allUsers.back().get();

        // ---- CART header ----
        if (!std::getline(in, line)) continue;
        if (line.rfind("CART|", 0) != 0) {
            std::cerr << "Bad CART header in " << filePath.string() << "\n";
            continue;
        }
        int cartCount = std::stoi(line.substr(5));

        // ---- CART items ----
        u->cart.products.clear();
        for (int i = 0; i < cartCount; ++i) {
            if (!std::getline(in, line)) break;
            auto f = split(line);
            if (f.size() != 4) continue;
            std::string itemID = f[0];
            std::string itemName = f[1];
            double      price = std::stod(f[2]);
            int         qty = std::stoi(f[3]);
            auto prodPtr = Database::GetProductByID(itemID);
            if (prodPtr) {
                u->cart.products.emplace_back(prodPtr, qty);
            }
        }

        // ---- ORDERS header ----

        u->orderHistory.clear();

        while (std::getline(in, line)) {
            if (line.rfind("ORDER|", 0) != 0)
                break; 

            auto head = split(line);
            if (head.size() != 3) break;

            std::string orderID = head[1];
            int k = std::stoi(head[2]);

            std::vector<CartItem> prods;
            prods.reserve(k);

            for (int j = 0; j < k; ++j) {
                if (!std::getline(in, line)) break;
                auto f = split(line);
                if (f.size() != 4) continue;

                std::string itemID = f[0];
                int qty = std::stoi(f[3]);

                auto prodPtr = Database::GetProductByID(itemID);
                if (prodPtr)
                    prods.emplace_back(prodPtr, qty);
            }

            // Read TOTAL
            if (std::getline(in, line)) {
                if (line.rfind("TOTAL|", 0) != 0)
                    std::cerr << "Warning: expected TOTAL line\n";
            }

            u->orderHistory.emplace_back(prods, u, orderID);
        }

    }
    Order::nextID = maxIDo + 1;
    User::nextID = maxID + 1;
}


void Database::loadProducts() {
    int maxID = 0;

    const std::string productPath = "C:/Users/Cesar/source/repos/Ecommerce-Project/products.txt";
    std::ifstream in(productPath);
    if (!in) {
        std::cerr << "No products file found at " << productPath << " (first run?).\n";
        return;
    }

    std::string line;

    if (!std::getline(in, line)) return;
    if (line.rfind("PRODUCTS|", 0) != 0) {
    }

    allProducts.clear();

    while (std::getline(in, line)) {
        if (line.empty()) continue;
        auto f = split(line);

        if (f.size() != 5) {
            std::cerr << "Bad product line: \"" << line << "\"\n";
            continue;
        }

        std::string id = f[0];
        std::string name = f[1];
        double      price = std::stod(f[2]);
        int         stock = std::stoi(f[3]);
        bool        onDisplay = (f[4] == "1" || f[4] == "true" || f[4] == "True");

        maxID = std::max(maxID, std::stoi(id));

        allProducts.emplace_back(
            std::make_shared<Product>(name, price, stock, onDisplay, id)
        );
        
    }
    Product::nextID = maxID + 1;

    std::cout << "Loaded " << allProducts.size() << " products from file.\n";
}








       
