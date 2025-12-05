#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "Product.h"
#include "ShoppingCart.h"
#include "Order.h"

class User
{
public:
	User(std::string n, std::string e, std::string p, bool isAdmin);
	User(std::string n, std::string e, std::string p, bool admin, std::string ID);

	void ViewOrderHistory();
	std::string getName() const;
	std::string getEmail() const;
	std::string getUserID() const;
	std::string getPassword() const;
	bool getIsAdmin() const;
	void ResetPassword(const std::string& newPassword);
	void ResetEmail(const std::string& newEmail);
	void ResetUsername(const std::string& newName);
	static void Register();
	static void AdminRegister();
	static bool UserNameExist(const std::string& name);
	ShoppingCart cart;
	std::vector<Order> orderHistory;
	static int nextID;

private:

	
	std::string name;
	std::string email;
	std::string password;
	std::string userID;
	bool isAdmin = false;
};