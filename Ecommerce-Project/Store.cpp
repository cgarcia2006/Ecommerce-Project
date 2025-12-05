#include <iostream>
#include <string>
#include <vector>

#include "Store.h"
#include "Product.h"
#include "Database.h"
#include "Admin.h"

void Store::ViewProducts() {
    int count = 1;

    do {
        count = 1;
        std::cout << "********** Store Products **********" << std::endl;
        for (const auto& product : Database::allProducts) {
            if (product->isOnDisplay() && product->getStockQuantity() != 0) {
                std::cout << "[" << count << "] Product ID : " << product->getProductID() << ", Name : " << product->getName() << ", Price : $" << product->getPrice() << ", Amount: " << product->getStockQuantity() << std::endl;
                count++;
            }
        }
        return;
    } while (true);

}

Product* Store::IdentifyProduct(const std::string& p) {

    for (auto& item : Database::allProducts) {
        if (p == item->getName()) {
            return item.get();
        }
    }
    std::cout << "Item requested not found\n";
    return nullptr;
}

Product* Store::IdentifyProductByID(const std::string& p) {

    for (auto& item : Database::allProducts) {
        if (p == item->getProductID()) {
            return item.get();
        }
    }
    std::cout << "Item requested not found\n";
    return nullptr;
}

CartItem* Store::IdentifyItem(const std::string& p, User& u) {
    // Drop expired products first
    u.cart.CleanExpiredItems();

    for (auto& item : u.cart.products) {
        if (auto sp = item.product.lock()) {
            if (p == sp->getName()) {
                return &item;
            }
        }
    }
    std::cout << "Item requested not found in cart\n";
    return nullptr;
}


bool Store::UserExist(const std::string& ID) {
    for (auto& u : Database::allUsers) {
        if (ID == u->getUserID()) {
            return true;
        }
    }
    return false;
}

User* Store::IdentifyUser(const std::string& ID) {
    for (auto& u : Database::allUsers) {
        if (ID == u->getUserID()) {
            return u.get();
        }
    }

    return nullptr;
}


void Store::ChangeQuantity(CartItem& i) {
    int quantity;

    std::cout << "Enter new quantity: ";
    std::cin >> quantity;

    if (auto p = i.product.lock()) {
        if (Database::AvailableAmount(*p, quantity)) {
            i.ChangeAmount(quantity);
            std::cout << "Amount changed succesfully.\n";
            return;
        }
        else {
            std::cout << "Amount requested not available\n";
            return;
        }
    }

    std::cout << "This product no longer exists in the store. Removing it from cart.\n";
}




void Store::AddToCartStore(User& u, Product* p, int amount) {
    if (!Database::AvailableAmount(*p, amount)) {
        std::cout << "Amount requested not avaialble\n";
        return;
    }

    u.cart.AddToCart(p, amount);
}

User* Store::AdminLogin(const std::string& email, const std::string& password, const std::string& adminPassword) {
    for (auto& user : Database::allUsers) {
        if (user->getEmail() == email && user->getPassword() == password && adminPassword == Admin::code) {
            return user.get();
        }
    }
    return nullptr;
}

User* Store::Login(const std::string& email, const std::string& password) {
    for (auto& user : Database::allUsers) {
        if (user->getEmail() == email && user->getPassword() == password) {
            return user.get();
        }

    }
    return nullptr;
}

void Store::Logout(User* currentUser) {
    if (currentUser) {
        std::cout << "User " << currentUser->getName() << " logged out successfully." << std::endl;
    }
    currentUser = nullptr;
}

int Store::LoginPhase() {
    int choice;
    do {
        std::cout << "1. Login as User\n2. Login as Admin\n3. Login as a Guest\n4. Register\n5. Admin register\n6. Exit\nChoose an option: ";
        std::cin >> choice;
        if (choice < 1 || choice > 6) {
            std::cout << "Invalid choice. Please try again." << std::endl;
        }
    } while (choice < 1 || choice > 6);
    return choice;
}

User* Store::LoginSecondPhase(int choice, std::string& status) {
    std::string email;
    std::string password;
    std::string adminPassword;
    switch (choice) {
    case 1:
    {
        int decision;
        do {

            
            std::cout << "Enter email: ";
            std::cin >> email;
            std::cout << "Enter password: ";
            std::cin >> password;

            if (Login(email, password) != nullptr) {
                status = "user";
                return Login(email, password);
    
            }

            do {
                std::cout << "Login failed\n";
                std::cout << "Continue as a guest: [1]Yes/[2]No\n";
                std::cin >> decision;

                if (decision == 1) {
                    status = "guest";
                    return nullptr;
                }
                else if (decision == 2) {
                    break;
                }
                else {
                    std::cout << "Enter a valid input (1 - 2)\n";
                }
            } while (true);

        } while (true);
    }
    case 2:
    {
        
        do {
            std::cout << "Enter admin email: ";
            std::cin >> email;
            std::cout << "Enter admin password: ";
            std::cin >> password;
            std::cout << "Enter system admin code: ";
            std::cin >> adminPassword;

            if (AdminLogin(email, password, adminPassword) != nullptr) {
                status = "admin";
                return AdminLogin(email, password, adminPassword); 
            }

            do {
                int decision;
                std::cout << "Login failed\n";
                std::cout << "Continue as a guest: [1]Yes/[2]No\n";
                std::cin >> decision;

                if (decision == 1) {
                    std::cout << "Continuing as guest user." << std::endl;
                    status = "guest";
                    return nullptr;
                    break;
                }
                else if (decision == 2) {
                    break;
                }
                else {
                    std::cout << "Enter a valid input (1 - 2)\n";
                }
            } while (true);
            
        } while (true); 
        break;
    }
    case 3:
    {
        std::cout << "Continuing as guest user." << std::endl;

        status = "guest";

        return nullptr;
    }
    case 4:
    {
        User::Register();
        break;
    }
    case 5:
    {
        User::AdminRegister();
        break;
    }
    case 6:
    {
        std::cout << "Exiting the program." << std::endl;
        exit(0);
    }
    }
    return nullptr;
}

int Store::DisplayStoreOptions(const std::string& stat) {
    int choice;

    if (stat == "admin") {

        std::cout << std::endl;
        do {
            std::cout << std::endl;
            std::cout << "[1] Manage Products\n";
            std::cout << "[2] Manage Users\n";
            std::cout << "[3] Log out\n";

            std::cin >> choice;
            if (choice >= 1 && choice <= 3) {
                return choice;
            }
            std::cout << "Enter a valid option (1 - 3)\n";
        } while (true);
    }
    else if (stat == "user" || stat == "guest") {
        do {
            std::cout << std::endl;
            std::cout << "[1] View Store Products\n";
            std::cout << "[2] Add Item to Cart\n";
            std::cout << "[3] View Shoppingcart\n";
            std::cout << "[4] Buy Cart\n";
            std::cout << "[5] View Order History\n";
            std::cout << "[6] Log out\n";

            std::cin >> choice;
            if (choice >= 1 && choice <= 6) {
                return choice;
            }
            std::cout << "Enter a valid option (1 - 6)\n";
        } while (true);
    }
    return 0;
}



void Store::DisplayStoreOptionPhaseTwo(int choice, User* user, const std::string& stat, bool& loop) {
    if (stat == "user" || stat == "guest") {
        switch (choice) {
        case 1:
        {
            ViewProducts();
            break;
        }
        case 2:
        {
            if (stat == "guest") {
                std::cout << "Create an account to perform this action\n";
                break;
            }
            int amount;
            std::string p;
            std::cout << "What Product do you want to add: ";
            std::cin >> p;
            std::cout << "Enter amount: :";
            std::cin >> amount;
            Product* prod = IdentifyProduct(p);
            if (prod && user) AddToCartStore(*user, prod, amount);
            break;
        }
        case 3:
        {
            if (stat == "guest") {
                std::cout << "Create an account to perform this action\n";
                break;
            }
            else if (user->cart.CartIsEmpty()) {
                std::cout << "Shopping cart is empty!\n";
                break;
            }
            int cartChoice;
            do {
                user->cart.ViewCart();
                std::cout << "[1] Remove items from cart\n";
                std::cout << "[2] Update quantity\n";
                std::cout << "[3] Exit\n";
                std::cin >> cartChoice;

                if (cartChoice >= 1 && cartChoice <= 3) {
                    break;
                }
                std::cout << "Enter a valid option (1 - 3)\n";
            } while (true);

            switch (cartChoice) {
                case 1:
                {
                    std::string i;
                    std::cout << "Enter item to remove from cart: ";
                    std::cin >> i;

                    CartItem* it = IdentifyItem(i, *user);
                    if (it) user->cart.RemoveFromCart(*it);
                    break;
                }
                case 2:
                {
                    std::string i;
                    std::cout << "Enter item to update quantity: ";
                    std::cin >> i;

                    CartItem* it = IdentifyItem(i, *user);
                    if (it) Store::ChangeQuantity(*it);
                    break;
                }
                case 3:
                    break;
            }
            break;
        }
        case 4:
        {
            if (stat == "guest") {
                std::cout << "Create an account to perform this action\n";
                break;
            }
            if (user) user->cart.BuyCart(*user);
            break;
        }
        case 5:
        {
            if (stat == "guest") {
                std::cout << "Create an account to perform this action\n";
                break;
            }
            if (user) user->ViewOrderHistory();
            break;
        }
        case 6:
        {
            Database::SaveUsers();
            Database::SaveProducts();
            loop = false;
            Logout(user);
            break;
        }
        }
    }
    else if (stat == "admin") {
        switch (choice) {
        case 1:
        {   
            bool loop = true;
            do {
                int choice1;
                std::string name;
                int quantity;
                double price;
                bool onDisplay;
                std::cout << "[1] Add product\n";
                std::cout << "[2] Manage product\n";
                std::cin >> choice1;

                if (choice1 == 1) {
                    do {
                        std::cout << "Enter name of product: ";
                        std::cin >> name;
                        if (!Product::ProductNameExist(name)) {
                            break;
                        }
                        std::cout << "Product name already exist\n";
                    } while (true);
                    
                    std::cout << "Enter price: ";
                    std::cin >> price;
                    std::cout << "Enter quantity:";
                    std::cin >> quantity;


                    Product p1(name, price, quantity);
                    Database::AddProductToDatabase(p1);
                    loop = false;
                }
                else if (choice1 == 2) {
                    if (Database::allProducts.empty()) {
                        std::cout << "No products yet!\n";
                        continue;
                    }
                    std::string ID;
                    std::cout << "Enter ID of the product: ";
                    std::cin >> ID;
                    Product* p = IdentifyProductByID(ID);
                    if (p) Admin::ManageProducts(*p);
                    loop = false;
                }
                else {
                    std::cout << "Etner a valid input\n";
                }
            } while (loop);
            break;
            
        }
        case 2:
        {
            if (Database::allUsers.empty()) {
                std::cout << "No users yet!\n";
                break;
            }
            std::string ID;
            std::cout << "Enter ID of the user: ";
            std::cin >> ID;

            if (UserExist(ID)) {
                User* u = IdentifyUser(ID);
                if (u) Admin::ManageUsers(*u);
                break;
            }
            std::cout << "User does not exist\n";
            break;
            
        }
        case 3:
        {
            loop = false;
            Logout(user);
            break;
        }
        }
    }
}