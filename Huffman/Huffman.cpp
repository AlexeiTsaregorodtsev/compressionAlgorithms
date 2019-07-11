#include <fstream>
#include <algorithm>

#include "huffman.h"

namespace Huffman {

	// �����������
	Huffman::Huffman(const std::string& unzippingFileName, const std::string& zippingFileName)
		:unzippingFileName(unzippingFileName), zippingFileName(zippingFileName)
	{
		buffRead = new char[bufferSize];
		buffSave = new char[bufferSize];
	}

	// ����������
	Huffman::~Huffman()
	{
		delete[] buffRead;
		delete[] buffSave;
	}

	// ������� ����������
	void Huffman::clear()
	{
		unzippingFileSize = -1;
		zippingFileSize = -1;
		progress = 0;
		position = 0;
		errors = "";
	}

	// ����������� ������� �������� ������� ���������
	void Huffman::genHuffVec(const Node* node,
		std::vector<std::tuple<char, int, long long>>& huffVec,
		long long& charCode, int& lenCharCode)
	{
		// ���� ����� �� ������
		if (node->getLeft() == nullptr && node->getRight() == nullptr)
		{
			// ���������� � ������� ������, ����� ���� � ���
			huffVec.push_back(std::make_tuple(node->getKey(), lenCharCode, charCode));
			return;
		}
		// ���� ����� ��������� �� ������
		if (node->getLeft() != nullptr)
		{
			// ����������� ����� ����
			lenCharCode++;
			// ��������� ���� � ����� ����
			charCode *= 2;
			// �������� ������� ����������
			genHuffVec(node->getLeft(), huffVec, charCode, lenCharCode);
			// ��������� �����
			lenCharCode--;
			// �������� ������ ���
			charCode /= 2;
		}
		// ���� ���� ������ ���������
		if (node->getRight() != nullptr) {
			// ����������� ����� ����
			lenCharCode++;
			// ��������� ������� � ����� ����
			charCode = charCode * 2 + 1;
			// �������� ������� ����������
			genHuffVec(node->getRight(), huffVec, charCode, lenCharCode);
			// ��������� �����
			lenCharCode--;
			// �������� ������ ���
			charCode /= 2;
		}
	}

	// �������� ������ ��������
	Node* Huffman::createTree(std::vector<Node*> nodes)
	{
		// ��������� ������ ������� ��������
		sort(nodes.begin(),
			nodes.end(),
			[](const Node* first, const Node* second)
			{
				return first->getAmount() > second->getAmount();
			}
		);

		// ������� �������� ������
		// ���� �� ��������� ���� ����
		while (nodes.size() != 1)
		{
			// ����� ��� ���� � ������������ ����������
			Node* firstNode = nodes.back();
			nodes.pop_back();
			Node* secondNode = nodes.back();
			nodes.pop_back();

			// ������� ����� ����, ������� ����� ��������� ��� ���� �����
			Node* parent = new Node('\0', firstNode->getAmount() + secondNode->getAmount());

			// ��������, ����� ���� �����, ����� ������
			if (firstNode->getAmount() < secondNode->getAmount())
			{
				parent->setLeft(firstNode);
				parent->setRight(secondNode);
			}
			else
			{
				parent->setRight(firstNode);
				parent->setLeft(secondNode);
			}
			// ������������ ���� ��������� � ������ ����� � �����������
			std::vector<Node*>::iterator it = nodes.begin();
			while (true)
			{
				if (it == nodes.end() || (*it)->getAmount() < parent->getAmount())
				{
					nodes.insert(it, parent);
					break;
				}
				it++;
			}
		}

		// ���������� ���� ����� ������ ������
		return nodes[0];
	}

	// ���������� 8-������ ������� ������
	void Huffman::save8bit(int& lenCharCode, long long& charCode,
		unsigned long& buffSavePosition, std::ofstream& fOut)
	{
		while (lenCharCode > 7)
		{
			// ��� ��� ����������
			char c = 0;
			// ����� ��� �������� ���� �� ������
			long long mask = 0;
			// ������ ���
			c = static_cast<char>(charCode >> (lenCharCode - 8));
			// ������� �����
			for (int i = 0; i < lenCharCode - 8; ++i) {
				mask = (mask << 1) | 1;
			}
			// ������� ��� �� ������
			charCode &= mask;
			// ��������� ������ ������
			lenCharCode -= 8;
			// ��������� ��� ����� ��� ������
			buffSave[buffSavePosition] = c;
			buffSavePosition++;
			// ���� ������ ������ bufferSize, �� ��������� ��� � ����
			if (buffSavePosition == bufferSize)
			{
				fOut.write(buffSave, static_cast<long>(sizeof(char) * bufferSize));
				buffSavePosition = 0;
			}
		}
	}

	// gets � sets
	long long Huffman::getUnzippingFileSize() const
	{
		return unzippingFileSize;
	}

	long long Huffman::getZippingFileSize() const
	{
		return zippingFileSize;
	}

	double Huffman::getProgress() const
	{
		return progress;
	}

	long long Huffman::getPosition() const
	{
		return position;
	}

	std::string Huffman::getUnzippingFileName() const
	{
		return unzippingFileName;
	}

	std::string Huffman::getZippingFileName() const
	{
		return zippingFileName;
	}

	// �������������
	bool Huffman::zipping()
	{
		// ������ ����������
		clear();
		// ��������� ����
		std::ifstream fIn(unzippingFileName, std::ios_base::binary | std::ios_base::in);
		// ���� ������ �������� - �����
		if (!fIn.is_open())
		{
			errors = "������ �������� ����� " + unzippingFileName;
			return false;
		}
		// ��������� ������ �����
		fIn.seekg(0, std::ios_base::end);
		unzippingFileSize = fIn.tellg();
		fIn.seekg(0);
		// ������, � ������� ����� ��� �������, ������������� � �����.
		std::vector<Node*> dic;
		// ��� ��� ��� ������� ����� ��������, ������, ��� 8 ���, ����� �������
		// ��� ����������. ����� �� ����� ������ 0
		dic.push_back(new Node('\0', 0));

		// ������ ���� ����.
		while (true)
		{
			// ��������� ��������� ������� �����
			fIn.read(buffRead, static_cast<long>(sizeof(char) * bufferSize));
			// ������, ������� ������� �� �����
			long lenBuffRead = fIn.gcount();

			// ������ ����������� �����
			for (long i = 0; i < lenBuffRead; ++i)
			{
				// ���� ������ � �������
				std::vector<Node*>::iterator it =
					std::find_if(dic.begin(),
						dic.end(),
						[this, &i](const Node* node)
						{
							return node->getKey() == buffRead[i] && node->getAmount() != 0;
						});
				// ���� ������ ������, �� ����������� �������
				if (it != dic.end())
					(*it)->increment();
				// ����� ������� ����� ����
				else
					dic.push_back(new Node(buffRead[i], 1));
			}
			// ��������� ������� � �����
			position = fIn.tellg();
			// ��������� ��������
			progress = 50.0 * position / unzippingFileSize;
			// ���� ������ ���� ���� - �������
			if (fIn.eof())
				break;
		}

		// �������� ��������
		progress = 50.0;

		// ������� ������
		Node* root = createTree(dic);

		// ������� ������ �������� ����� ����������� �������
		std::vector<std::tuple<char, int, long long>> huffChars;

		long long charCode = 0;     // ��� ��� �������
		int lenCharCode = 0;       // ����� ���� �������
		genHuffVec(root, huffChars, charCode, lenCharCode);

		// ��������� ������ ���������
		sort(std::begin(huffChars),
			std::end(huffChars),
			[](auto &first, auto &second)
			{
				if (std::get<HUFFCHAR_LENGTH>(first) == std::get<HUFFCHAR_LENGTH>(second))
					return std::get<HUFFCHAR_CHAR>(first) > std::get<HUFFCHAR_CHAR>(second);
				return std::get<HUFFCHAR_LENGTH>(first) < std::get<HUFFCHAR_LENGTH>(second);
			}
		);

		// ������� � ������ ������
		unsigned long buffSavePosition = 0;

		// ������, ������� �������� ��� �� ������ ��������
		charCode = 0;
		lenCharCode = 0;

		// ������ ������ ��/��� � ��������� � ������ �����
		fIn.clear();
		fIn.seekg(0);

		// ������� ���� ��� ������
		std::ofstream fOut(zippingFileName, std::ios_base::binary | std::ios_base::trunc);
		// ���� ������ �������� ����� - �������
		if (!fOut.is_open())
		{
			fIn.close();
			errors = "������ �������� ����� " + zippingFileName;
			return false;
		}

		// ���������� ��������� �����, �������� ������� ������� ��������
		// ������ ���������
		size_t titleSize = dic.size() * (sizeof(char) + sizeof(long long));
		fOut.write(reinterpret_cast<const char*>(&titleSize), sizeof(size_t));

		// ��������� ������ ������� ������� �����
		for (size_t i = 0; i < dic.size(); ++i) {
			char key = dic[i]->getKey();
			long long count = dic[i]->getAmount();
			fOut.write(reinterpret_cast<const char*>(&key), sizeof(key));
			fOut.write(reinterpret_cast<const char*>(&count), sizeof(count));
		}

		// ������ ���� ����
		while (true)
		{
			fIn.read(buffRead, static_cast<long>(sizeof(char) * bufferSize));
			// ������ ����������� �����
			long lenBuffRead = fIn.gcount();
			for (int i = 0; i < lenBuffRead; ++i)
			{
				// ���� ��������� ��� � ������ ��������
				std::vector<std::tuple<char, int, long long>>::iterator it =
					std::find_if(huffChars.begin(),
						huffChars.end(),
						[this, &i](const std::tuple<char, int, long long>& value)
						{
							return std::get<HUFFCHAR_CHAR>(value) == buffRead[i];
						}
				);

				// ���� ��� �� ������, �� �������
				if (it == huffChars.end())
				{
					fIn.close();
					fOut.close();
					errors = "�������� ������ �� ������ � ������� ��������.";
					return false;
				}

				// ����������� ������ ������
				lenCharCode += std::get<HUFFCHAR_LENGTH>(*it);
				// ���� ������ ������ ������ 64, �� ������� � �������
				if (lenCharCode > 64)
				{
					fIn.close();
					fOut.close();
					errors = "������������ �������.";
					return false;
				}

				// ��������� ��� � �����
				charCode = (charCode << std::get<HUFFCHAR_LENGTH>(*it)) | std::get<HUFFCHAR_CODE>(*it);
				// ��������� ��� 8-������ ������� ������
				save8bit(lenCharCode, charCode, buffSavePosition, fOut);
			}
			// ��������� ������� � �����
			position = fIn.tellg();
			// ��������� ��������
			progress = 50.0 + 50.0 * position / unzippingFileSize;
			// ���� ����� ����� - ������� �� �����
			if (fIn.eof())
				break;
		}
		// ��������� ��� ����� �����
		lenCharCode += std::get<HUFFCHAR_LENGTH>(huffChars.back());
		charCode = (charCode << std::get<HUFFCHAR_LENGTH>(huffChars.back())) |
			std::get<HUFFCHAR_CODE>(huffChars.back());
		save8bit(lenCharCode, charCode, buffSavePosition, fOut);
		// ���� � ������ ��� �� ��������
		if (lenCharCode != 0)
		{
			charCode = charCode << (8 - lenCharCode);
			buffSave[buffSavePosition] = static_cast<char>(charCode);
			buffSavePosition++;
		}

		// ���� � ������ ������ ��� �� ��������
		if (buffSavePosition != 0)
		{
			fOut.write(buffSave, static_cast<long>(buffSavePosition));
		}

		// ��������� ������ ������� �����
		zippingFileSize = fOut.tellp();

		// �������� 100%
		progress = 100.0;
		// ��������� �����
		fOut.close();
		fIn.close();

		// ������� ������
		delete root;

		return true;
	}

	// ����������������
	bool Huffman::unzipping()
	{
		// ������ ����������
		clear();

		// ��������� ����
		std::ifstream fIn(zippingFileName, std::ios_base::binary | std::ios_base::in);
		// ���� ������ �������� - �����
		if (!fIn.is_open())
		{
			errors = "������ �������� ����� " + zippingFileName;
			return false;
		}

		// ��������� ������ �����
		fIn.seekg(0, std::ios_base::end);
		zippingFileSize = fIn.tellg();
		fIn.seekg(0);

		// ������ ���������
		size_t titleSize = 0;
		// ��������� ������ ���������
		fIn.read(reinterpret_cast<char*>(&titleSize), sizeof(size_t));
		// ��������� ���������� ���������
		long dicCount = titleSize / (sizeof(char) + sizeof(long long));
		// ������, � ������� ����� ��� �������, ������������� � �����
		std::vector<Node*> dic;
		// ��������� ������ ������� ������� �����
		for (long i = 0; i < dicCount; ++i)
		{
			char key;
			long long count;
			fIn.read(reinterpret_cast<char*>(&key), sizeof(key));
			fIn.read(reinterpret_cast<char*>(&count), sizeof(count));
			dic.push_back(new Node(key, count));
		}

		// ������� ������
		Node* root = createTree(dic);

		// ������� ���� ��� ������
		std::ofstream fOut(unzippingFileName, std::ios_base::binary | std::ios_base::trunc);
		// ���� ������ �������� ����� - �������
		if (!fOut.is_open())
		{
			errors = "������ �������� ����� " + unzippingFileName;
			fIn.close();
			delete root;
			return false;
		}

		Node* node = root;              // ����, ������� ������ ��������� �������� �� ������
		char ch;                        // ��������� ������
		unsigned long buffPos = 0;      // ������� � ������ ������

		// ������ ������� �����
		while (node->getAmount() != 0 && fIn.get(ch))
		{
			// ������� ���� � ��������� �������
			for (int i = 7; i >= 0; --i)
			{
				// � ����������� �� ���������� ���� ��������� �� ������
				node = (((ch >> i) & 1) == 1) ? node->getRight() : node->getLeft();
				// ���� ��� ������
				if (node->getLeft() == nullptr && node->getRight() == nullptr)
				{
					// ���� ��� �� ��� ��� ����� �����
					if (node->getAmount() != 0)
					{
						// �� ���������� � ����� ���������� ������
						buffSave[buffPos] = node->getKey();
						buffPos++;
						// ������������ � ����� ������
						node = root;
						// ���� ����� ������, ���������� ��� � ����
						if (buffPos == bufferSize)
						{
							fOut.write(buffSave, static_cast<long>(bufferSize));
							buffPos = 0;
						}
					}
					else
					{
						break;
					}
				}
			}
			// ��������� ������� � �����
			position = fIn.tellg();
			// ��������� ��������
			progress = 100.0 * position / zippingFileSize;
		}

		// ���� � ������ �������� ������ - ���������� ��
		if (buffPos > 0)
			fOut.write(buffSave, static_cast<long>(buffPos));

		progress = 100.0;

		// ��������� ������ ������� �����
		unzippingFileSize = fOut.tellp();

		// ��������� �����
		fOut.close();
		fIn.close();

		// ������� ������������ ����������
		delete root;

		return true;
	}

}