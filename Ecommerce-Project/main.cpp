#include <iostream>

#include "Store.h"
#include "Database.h"

int main() {
	Database::loadProducts();
	Database::loadUsers();

	do {
		bool loop = true;
		std::string status = "";
		User* currentUser = nullptr;

		while (status != "user" && status != "admin" && status != "guest") {
			int loginPhaseResult = Store::LoginPhase();
			currentUser = Store::LoginSecondPhase(loginPhaseResult, status);
		}

		do {
			Store::DisplayStoreOptionPhaseTwo(Store::DisplayStoreOptions(status), currentUser, status, loop);
		} while (loop);
	} while (true);

	Database::SaveUsers();
	Database::SaveProducts();

	return 0;
}