#include <iostream>

int	main() {
	try {
		throw "unwind section";
	} catch (char *err) {
		std::cout << "Error: %s\n" << err;
	}
}