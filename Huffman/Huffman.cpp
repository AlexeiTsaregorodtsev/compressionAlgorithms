#include <fstream>
#include <algorithm>

#include "huffman.h"

namespace Huffman {

	// Конструктор
	Huffman::Huffman(const std::string& unzippingFileName, const std::string& zippingFileName)
		:unzippingFileName(unzippingFileName), zippingFileName(zippingFileName)
	{
		buffRead = new char[bufferSize];
		buffSave = new char[bufferSize];
	}

	// Деструктор
	Huffman::~Huffman()
	{
		delete[] buffRead;
		delete[] buffSave;
	}

	// Очистка переменных
	void Huffman::clear()
	{
		unzippingFileSize = -1;
		zippingFileSize = -1;
		progress = 0;
		position = 0;
		errors = "";
	}

	// Рекурсивная функция создания таблицы Хаффамана
	void Huffman::genHuffVec(const Node* node,
		std::vector<std::tuple<char, int, long long>>& huffVec,
		long long& charCode, int& lenCharCode)
	{
		// Если дошли до листка
		if (node->getLeft() == nullptr && node->getRight() == nullptr)
		{
			// Сохранияем в таблицу символ, длину кода и код
			huffVec.push_back(std::make_tuple(node->getKey(), lenCharCode, charCode));
			return;
		}
		// Если левое поддерево не пустое
		if (node->getLeft() != nullptr)
		{
			// Увеличиваем длину кода
			lenCharCode++;
			// Добавляем ноль в конец кода
			charCode *= 2;
			// Вызываем функцию рекурсивно
			genHuffVec(node->getLeft(), huffVec, charCode, lenCharCode);
			// Уменьшаем буфер
			lenCharCode--;
			// Сдвигаем вправо код
			charCode /= 2;
		}
		// Если есть правое поддерево
		if (node->getRight() != nullptr) {
			// Увеличиваем длину кода
			lenCharCode++;
			// Добавляем единицу в конец кода
			charCode = charCode * 2 + 1;
			// Вызываем функцию рекурсивно
			genHuffVec(node->getRight(), huffVec, charCode, lenCharCode);
			// Уменьшаем буфер
			lenCharCode--;
			// Сдвигаем вправо код
			charCode /= 2;
		}
	}

	// Создание дерева Хаффмана
	Node* Huffman::createTree(std::vector<Node*> nodes)
	{
		// Сортируем вектор частоты символов
		sort(nodes.begin(),
			nodes.end(),
			[](const Node* first, const Node* second)
			{
				return first->getAmount() > second->getAmount();
			}
		);

		// Создаем бинарное дерево
		// Пока не останется один узел
		while (nodes.size() != 1)
		{
			// Берем два узла с минимальными счетчиками
			Node* firstNode = nodes.back();
			nodes.pop_back();
			Node* secondNode = nodes.back();
			nodes.pop_back();

			// Создаем новый узел, который будет родителем для двух узлов
			Node* parent = new Node('\0', firstNode->getAmount() + secondNode->getAmount());

			// Выясняем, какой узел левый, какой правый
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
			// Родительский узел вставляем в вектор узлов с сортировкой
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

		// Оставшийся узел будет корнем дерева
		return nodes[0];
	}

	// Сохранение 8-битных позиций буфера
	void Huffman::save8bit(int& lenCharCode, long long& charCode,
		unsigned long& buffSavePosition, std::ofstream& fOut)
	{
		while (lenCharCode > 7)
		{
			// Код для сохранения
			char c = 0;
			// Маска для удаления кода из буфера
			long long mask = 0;
			// Читаем код
			c = static_cast<char>(charCode >> (lenCharCode - 8));
			// Создаем маску
			for (int i = 0; i < lenCharCode - 8; ++i) {
				mask = (mask << 1) | 1;
			}
			// Удаляем код из буфера
			charCode &= mask;
			// Уменьшаем размер буфера
			lenCharCode -= 8;
			// Добавляем код буфер для записи
			buffSave[buffSavePosition] = c;
			buffSavePosition++;
			// Если размер буфера bufferSize, то сохраняем его в файл
			if (buffSavePosition == bufferSize)
			{
				fOut.write(buffSave, static_cast<long>(sizeof(char) * bufferSize));
				buffSavePosition = 0;
			}
		}
	}

	// gets и sets
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

	// Архивирование
	bool Huffman::zipping()
	{
		// Чистим переменные
		clear();
		// Открываем файл
		std::ifstream fIn(unzippingFileName, std::ios_base::binary | std::ios_base::in);
		// Если ошибка открытия - выход
		if (!fIn.is_open())
		{
			errors = "Ошибка открытия файла " + unzippingFileName;
			return false;
		}
		// Считываем размер файла
		fIn.seekg(0, std::ios_base::end);
		unzippingFileSize = fIn.tellg();
		fIn.seekg(0);
		// Вектор, в котором будут все символы, встречающиеся в файле.
		std::vector<Node*> dic;
		// Так как код символа может состоять, меньше, чем 8 бит, нужно создать
		// Код завершения. Пусть он будет длиной 0
		dic.push_back(new Node('\0', 0));

		// Читаем весь файл.
		while (true)
		{
			// Считываем очередной фрагмен файла
			fIn.read(buffRead, static_cast<long>(sizeof(char) * bufferSize));
			// Узнаем, сколько считано из файла
			long lenBuffRead = fIn.gcount();

			// Читаем посимвольно буфер
			for (long i = 0; i < lenBuffRead; ++i)
			{
				// Ищем символ в векторе
				std::vector<Node*>::iterator it =
					std::find_if(dic.begin(),
						dic.end(),
						[this, &i](const Node* node)
						{
							return node->getKey() == buffRead[i] && node->getAmount() != 0;
						});
				// Если символ найден, то увеличиваем счетчик
				if (it != dic.end())
					(*it)->increment();
				// иначе создаем новый узел
				else
					dic.push_back(new Node(buffRead[i], 1));
			}
			// Считываем позицию в файле
			position = fIn.tellg();
			// Вычисляем прогресс
			progress = 50.0 * position / unzippingFileSize;
			// Если считан весь файл - выходим
			if (fIn.eof())
				break;
		}

		// Половина пройдена
		progress = 50.0;

		// Создаем дерево
		Node* root = createTree(dic);

		// Создаем список хаффмана через рекурсивную функцию
		std::vector<std::tuple<char, int, long long>> huffChars;

		long long charCode = 0;     // Код для символа
		int lenCharCode = 0;       // Длина кода символа
		genHuffVec(root, huffChars, charCode, lenCharCode);

		// Сортируем список хаффамана
		sort(std::begin(huffChars),
			std::end(huffChars),
			[](auto &first, auto &second)
			{
				if (std::get<HUFFCHAR_LENGTH>(first) == std::get<HUFFCHAR_LENGTH>(second))
					return std::get<HUFFCHAR_CHAR>(first) > std::get<HUFFCHAR_CHAR>(second);
				return std::get<HUFFCHAR_LENGTH>(first) < std::get<HUFFCHAR_LENGTH>(second);
			}
		);

		// Позиция в буфере записи
		unsigned long buffSavePosition = 0;

		// Буффер, который содержит код из списка хаффмана
		charCode = 0;
		lenCharCode = 0;

		// Чистим ошибки вв/выв и переходим в начало файла
		fIn.clear();
		fIn.seekg(0);

		// Создаем файл для записи
		std::ofstream fOut(zippingFileName, std::ios_base::binary | std::ios_base::trunc);
		// Если ошибка создания файла - выходим
		if (!fOut.is_open())
		{
			fIn.close();
			errors = "Ошибка создания файла " + zippingFileName;
			return false;
		}

		// Записываем заголовок файла, содержит таблицу частоты символов
		// Размер заголовка
		size_t titleSize = dic.size() * (sizeof(char) + sizeof(long long));
		fOut.write(reinterpret_cast<const char*>(&titleSize), sizeof(size_t));

		// Сохраняем список частоты встречи кодов
		for (size_t i = 0; i < dic.size(); ++i) {
			char key = dic[i]->getKey();
			long long count = dic[i]->getAmount();
			fOut.write(reinterpret_cast<const char*>(&key), sizeof(key));
			fOut.write(reinterpret_cast<const char*>(&count), sizeof(count));
		}

		// Читаем весь файл
		while (true)
		{
			fIn.read(buffRead, static_cast<long>(sizeof(char) * bufferSize));
			// Читаем посимвольно буфер
			long lenBuffRead = fIn.gcount();
			for (int i = 0; i < lenBuffRead; ++i)
			{
				// Ищем считанный код в списке хаффмана
				std::vector<std::tuple<char, int, long long>>::iterator it =
					std::find_if(huffChars.begin(),
						huffChars.end(),
						[this, &i](const std::tuple<char, int, long long>& value)
						{
							return std::get<HUFFCHAR_CHAR>(value) == buffRead[i];
						}
				);

				// Если код не найден, то выходим
				if (it == huffChars.end())
				{
					fIn.close();
					fOut.close();
					errors = "Входящий символ не найден в таблице Хаффмана.";
					return false;
				}

				// Увеличиваем размер буфера
				lenCharCode += std::get<HUFFCHAR_LENGTH>(*it);
				// Если размер буфера больше 64, то выходим с ошибкой
				if (lenCharCode > 64)
				{
					fIn.close();
					fOut.close();
					errors = "Переполнение буффера.";
					return false;
				}

				// Добавляем код в буфер
				charCode = (charCode << std::get<HUFFCHAR_LENGTH>(*it)) | std::get<HUFFCHAR_CODE>(*it);
				// Сохраняем все 8-битные позиции буфера
				save8bit(lenCharCode, charCode, buffSavePosition, fOut);
			}
			// Считываем позицию в файле
			position = fIn.tellg();
			// Вычисляем прогресс
			progress = 50.0 + 50.0 * position / unzippingFileSize;
			// Если конец файла - выходим из цикла
			if (fIn.eof())
				break;
		}
		// Добавляем код конца файла
		lenCharCode += std::get<HUFFCHAR_LENGTH>(huffChars.back());
		charCode = (charCode << std::get<HUFFCHAR_LENGTH>(huffChars.back())) |
			std::get<HUFFCHAR_CODE>(huffChars.back());
		save8bit(lenCharCode, charCode, buffSavePosition, fOut);
		// Если в буфере что то осталось
		if (lenCharCode != 0)
		{
			charCode = charCode << (8 - lenCharCode);
			buffSave[buffSavePosition] = static_cast<char>(charCode);
			buffSavePosition++;
		}

		// Если в буфере записи что то осталось
		if (buffSavePosition != 0)
		{
			fOut.write(buffSave, static_cast<long>(buffSavePosition));
		}

		// Считываем размер сжатого файла
		zippingFileSize = fOut.tellp();

		// Прогресс 100%
		progress = 100.0;
		// Закрываем файлы
		fOut.close();
		fIn.close();

		// Удаляем дерево
		delete root;

		return true;
	}

	// Разархивирование
	bool Huffman::unzipping()
	{
		// Чистим переменные
		clear();

		// Открываем файл
		std::ifstream fIn(zippingFileName, std::ios_base::binary | std::ios_base::in);
		// Если ошибка открытия - выход
		if (!fIn.is_open())
		{
			errors = "Ошибка открытия файла " + zippingFileName;
			return false;
		}

		// Считываем размер файла
		fIn.seekg(0, std::ios_base::end);
		zippingFileSize = fIn.tellg();
		fIn.seekg(0);

		// Размер заголовка
		size_t titleSize = 0;
		// Считываем размер заголовка
		fIn.read(reinterpret_cast<char*>(&titleSize), sizeof(size_t));
		// Вычисляем количество элементов
		long dicCount = titleSize / (sizeof(char) + sizeof(long long));
		// Вектор, в котором будут все символы, встречающиеся в файле
		std::vector<Node*> dic;
		// Загружаем список частоты встречи кодов
		for (long i = 0; i < dicCount; ++i)
		{
			char key;
			long long count;
			fIn.read(reinterpret_cast<char*>(&key), sizeof(key));
			fIn.read(reinterpret_cast<char*>(&count), sizeof(count));
			dic.push_back(new Node(key, count));
		}

		// Создаем дерево
		Node* root = createTree(dic);

		// Создаем файл для записи
		std::ofstream fOut(unzippingFileName, std::ios_base::binary | std::ios_base::trunc);
		// Если ошибка создания файла - выходим
		if (!fOut.is_open())
		{
			errors = "Ошибка создания файла " + unzippingFileName;
			fIn.close();
			delete root;
			return false;
		}

		Node* node = root;              // Узел, который хранит указатель движения по дереву
		char ch;                        // Считанный символ
		unsigned long buffPos = 0;      // Позиция в буфуре записи

		// Читаем остаток файла
		while (node->getAmount() != 0 && fIn.get(ch))
		{
			// Обходим биты в считанном символе
			for (int i = 7; i >= 0; --i)
			{
				// В зависимости от считанного бита двигаемся по дереву
				node = (((ch >> i) & 1) == 1) ? node->getRight() : node->getLeft();
				// Если это листок
				if (node->getLeft() == nullptr && node->getRight() == nullptr)
				{
					// Если это не код код конца файла
					if (node->getAmount() != 0)
					{
						// То записываем в буфер полученный символ
						buffSave[buffPos] = node->getKey();
						buffPos++;
						// Возвращаемся к корню дерева
						node = root;
						// Если буфер полный, записываем его в файл
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
			// Считываем позицию в файле
			position = fIn.tellg();
			// Вычисляем прогресс
			progress = 100.0 * position / zippingFileSize;
		}

		// Если в буфере остались данные - записываем их
		if (buffPos > 0)
			fOut.write(buffSave, static_cast<long>(buffPos));

		progress = 100.0;

		// Считываем размер сжатого файла
		unzippingFileSize = fOut.tellp();

		// Закрываем файлы
		fOut.close();
		fIn.close();

		// Удаляем динамические переменные
		delete root;

		return true;
	}

}