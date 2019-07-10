/*
 * ����� ����������� ����� �� ������ ��������
 */
#ifndef HUFFMAN_H
#define HUFFMAN_H

#include "node.h"

#include <string>
#include <vector>
#include <tuple>

namespace Huffman
{

	class Huffman
	{
		enum huffCharEnum {
			HUFFCHAR_CHAR = 0,
			HUFFCHAR_LENGTH,
			HUFFCHAR_CODE
		};
		const unsigned long bufferSize = 1024;  // ������ ������
		long long unzippingFileSize = -1;       // ������ ��������� �����
		long long zippingFileSize = -1;         // ������ ������� �����
		double progress = 0;                    // �������� � %
		long long position = 0;                 // ������� � �����, ������� ��������������
		std::string errors = "";                // ������
		std::string unzippingFileName;          // ��� ��������� �����
		std::string zippingFileName;            // ��� ������� �����
		char* buffRead;                         // ����� ������ �����
		char* buffSave;                         // ����� ������ � ����

		// �������� ����������� ����������� �������� ������������
		Huffman(const Huffman& huffman);
		Huffman& operator=(const Huffman& huffman);

		// �������� ������� ��������
		void genHuffVec(const Node* node,
			std::vector<std::tuple<char, int, long long>>& huffVec,
			long long& charCode, int& lenCharCode);
		// �������� ������ ��������, ���������� ������ ������
		Node* createTree(std::vector<Node*> nodes);
		// ������� ����������
		void clear();
		// ���������� 8-������ ������� ������
		void save8bit(int& lenCharCode, long long& charCode,
			unsigned long& buffSavePosition, std::ofstream& fOut);
	public:
		// �����������
		Huffman(const std::string& unzippingFileName = "", const std::string& zippingFileName = "");
		// ����������
		~Huffman();

		// gets � sets
		long long getUnzippingFileSize() const;                 // ������� ������ ��������� �����
		long long getZippingFileSize() const;                   // ������� ������ ������� �����
		double getProgress() const;                             // ������� �������� ��������
		long long getPosition() const;                          // ������� ������� � �����
		std::string getUnzippingFileName() const;               // ������� ��� ��������� �����
		std::string getZippingFileName() const;                 // ������� ��� ������� �����

		bool zipping();                                         // ���������
		bool unzipping();                                       // ������������
	};

}
#endif // HUFFMAN_H