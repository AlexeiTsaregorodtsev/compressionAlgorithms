/*
 * Класс кодирования файла по методу Хаффмана
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
		const unsigned long bufferSize = 1024;  // Размер буфера
		long long unzippingFileSize = -1;       // Размер несжатого файла
		long long zippingFileSize = -1;         // Размер сжатого файла
		double progress = 0;                    // Прогресс в %
		long long position = 0;                 // Позиция в файле, который обрабатывается
		std::string errors = "";                // Ошибка
		std::string unzippingFileName;          // Имя несжатого файла
		std::string zippingFileName;            // Имя сжатого файла
		char* buffRead;                         // Буфер чтения файла
		char* buffSave;                         // Буфер записи в файл

		// Скрываем конструктор копирования оператор присваивания
		Huffman(const Huffman& huffman);
		Huffman& operator=(const Huffman& huffman);

		// Создание вектора Хаффмана
		void genHuffVec(const Node* node,
			std::vector<std::tuple<char, int, long long>>& huffVec,
			long long& charCode, int& lenCharCode);
		// Создание дерева Хаффмана, возвращает корень дерева
		Node* createTree(std::vector<Node*> nodes);
		// Очистка переменных
		void clear();
		// Сохранение 8-битных позиций буфера
		void save8bit(int& lenCharCode, long long& charCode,
			unsigned long& buffSavePosition, std::ofstream& fOut);
	public:
		// Конструктор
		Huffman(const std::string& unzippingFileName = "", const std::string& zippingFileName = "");
		// Деструктор
		~Huffman();

		// gets и sets
		long long getUnzippingFileSize() const;                 // Вернуть размер несжатого файла
		long long getZippingFileSize() const;                   // Вернуть размер сжатого файла
		double getProgress() const;                             // Вернуть прогресс обратоки
		long long getPosition() const;                          // Вернуть позицию в файле
		std::string getUnzippingFileName() const;               // Вернуть имя несжатого файла
		std::string getZippingFileName() const;                 // Вернуть имя сжатого файла

		bool zipping();                                         // Архивация
		bool unzipping();                                       // Разархивация
	};

}
#endif // HUFFMAN_H