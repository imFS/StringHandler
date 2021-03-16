#include <iostream>
#include "StringHandler.h"


int main()
{
	std::cout << "Test..." << std::endl;

	StringHandler handler;

	// Setup filters
	handler.on("PM", [](std::string p) {
		std::cout << "PM Detected!" << std::endl;
		});
	handler.add_filter("PM", "pm ");

	handler.on("GM", [](std::string p) {
		std::cout << "GM Detected!" << std::endl;
		});
	handler.add_filter("GM", "gmjoined ");

	// Some random strings to trigger them
	handler.search("Hello world!");
	handler.search("pm Hi");
	handler.search("gmjoined HiThere");

	system("pause");
	return 0;
}