#pragma once

#include <iostream>
#include <string>
#include <vector>

class Product
{
public:
	Product(std::string n, double p, int q);
	Product(std::string n, double p, std::string ID);
	Product(std::string n, double p, int q, bool d, std::string ID);
	std::string getProductID() const;
	std::string getName() const;
	double getPrice() const;
	int getStockQuantity() const;
	bool isOnDisplay() const;
	void toDisplay();
	void outOfDisplay();
	void setPrice(double p);
	void setStockQuantity(int q);
	void setName(std::string& n);
	static bool ProductNameExist(const std::string& n);
	static int nextID;
	
private:

	
	std::string productID;
	std::string name;
	double price;
	int stockQuantity;
	bool onDisplay;

};