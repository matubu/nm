#include <iostream>

int	main() {
	try {
		try {
			throw "maybe unwind section ?";
		} catch (void *err) {
			std::cout << "Error void: " << err << std::endl;
		}
	} catch (char *err) {
		std::cout << "Error: " << err << std::endl;
	}
}
