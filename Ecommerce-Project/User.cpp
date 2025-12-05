#include <iostream>
#include <string>
#include <vector>

#include "User.h"
#include "Product.h"
#include "ShoppingCart.h"
#include "Database.h"

int User::nextID = 0;

User::User(std::string n, std::string e, std::string p, bool admin)
	: name(n), email(e), password(p), isAdmin(admin), userID(std::to_string(nextID++))
{

	Database::allUsers.push_back(std::make_unique<User>(*this));
	Database::SaveUsers();
}

User::User(std::string n, std::string e, std::string p, bool admin, std::string ID)
	: name(n), email(e), password(p), isAdmin(admin), userID(ID) 
{}

void User::ViewOrderHistory() {
	if (orderHistory.empty()) {
		std::cout << "No orders yet!\n";
		return;
	}
	for (auto& order : orderHistory) {
		std::cout << order.orderID << std::endl;
		for (auto& item : order.products) {
			if (auto p = item.product.lock()) {
				std::cout << "Product ID: " << p->getProductID()
					<< ", Name: " << p->getName()
					<< ", Quantity: " << item.Quantity
					<< ", Price: $" << p->getPrice() << std::endl;
			}
			else {
				std::cout << "Product: [no longer available]"
					<< ", Quantity: " << item.Quantity << std::endl;
			}
		}
	}
}

std::string User::getName() const {
	return name;
}
std::string User::getEmail() const {
	return email;
}
std::string User::getPassword() const {
	return password;
}
std::string User::getUserID() const {
	return userID;
}

bool User::getIsAdmin() const {
	return isAdmin;
}

void User::ResetEmail(const std::string& newEmail) {
	email = newEmail;
}

void User::ResetPassword(const std::string& newPassword) {
	password = newPassword;
}

void User::ResetUsername(const std::string& newName) {
	name = newName;
}
bool User::UserNameExist(const std::string& name) {
	for (auto& u : Database::allUsers) {
		if (name == u->getName()) {
			return true;
		}
	}
	return false;
}

void User::Register() {
	std::string n, e, p;
	do {
		std::cout << "Input name: ";
		std::cin >> n;

		if (!UserNameExist(n)) {
			break;
		}
		std::cout << "Username already exist\n";
	} while (true);
	
	std::cout << "Input email: ";
	std::cin >> e;
	std::cout << "Input password: ";
	std::cin >> p;

	User newUser(n, e, p, false);
}

void User::AdminRegister() {
	std::string n, e, p;
	do {
		std::cout << "Input name: ";
		std::cin >> n;

		if (!UserNameExist(n)) {
			break;
		}
		std::cout << "Username already exist\n";
	} while (true);

	std::cout << "Input email: ";
	std::cin >> e;
	std::cout << "Input password: ";
	std::cin >> p;
	User newUser(n, e, p, true);
}
