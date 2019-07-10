#include <iostream>
#include <clocale>
#include <thread>

#include "huffman.h"

int main(int argc, char* argv[])
{
	std::setlocale(LC_ALL, "");
	Huffman::Huffman huffman("c:\\temp\\gently_1.txt",
		"c:\\temp\\gently_1.txt.arh");

	
	std::thread myThread(&Huffman::Huffman::zipping, &huffman);
	while (huffman.getProgress() < 100)
	{
		std::cout << huffman.getProgress() << "%                 \r";
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
	
	myThread.join();

	std::cout << huffman.getProgress() << "%          \n";

	/*if (huffman.isError())
	{
		std::wcout << huffman.getErrors();
	}*/
	std::cout << std::endl;

	return 0;
}
