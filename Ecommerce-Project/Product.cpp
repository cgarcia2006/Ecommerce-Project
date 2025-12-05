#include <iostream>
#include <string>

#include "Product.h"
#include "Database.h"

int Product::nextID;

Product::Product(std::string n, double p, int q)
	: name(n), price(p), stockQuantity(q)
{
	productID = std::to_string(nextID++);
	onDisplay = false;
	Database::SaveProducts();
}

Product::Product(std::string n, double p, std::string ID)
	: name(n), price(p), productID(ID)
{
}

Product::Product(std::string n, double p, int q, bool d, std::string ID)
	: name(n), price(p), stockQuantity(q), onDisplay(d), productID(ID) {
}


std::string Product::getProductID() const {
	return productID;
}
std::string Product::getName() const {
	return name;
}
double Product::getPrice() const {
	return price;
}
int Product::getStockQuantity() const {
	return stockQuantity;
}

bool Product::isOnDisplay() const {
	return onDisplay;
}

void Product::toDisplay() { 
	onDisplay = true;
}
void Product::outOfDisplay() { 
	onDisplay = false; 
}
void Product::setPrice(double p) {
	price = p;
}
void Product::setStockQuantity(int q) {
	stockQuantity = q;
}
void Product::setName(std::string& n) {
	name = n;
}
bool Product::ProductNameExist(const std::string& n) {
	for (auto& p : Database::allProducts) {
		if (n == p->getName()) {
			return true;
		}
	}
	return false;
}