/*
 * Описание класса узла(листа) дерева Node
 */

#ifndef NODE_H
#define NODE_H

namespace Huffman {

	class Node
	{
		char key;           // Ключ
		long long amount;   // Количество
		Node* left;         // Ссылка на левое поддерево
		Node* right;        // Ссылка на правое поддерево
		// Скрываем конструктор копирования оператор присваивания
		Node(const Node& node);
		Node& operator=(const Node& node);
	public:
		// Коструктор
		Node(const char& key, const long long& amount);
		// Деструктор
		~Node();

		// gets и sets
		char getKey() const;                    // Получить ключ
		long long getAmount() const;            // Получить количество
		void setValue(const long long& value);  // Задать количество
		Node* getLeft() const;                  // Получить левое поддерево
		void setLeft(Node* value);              // Задать левое поддерево
		Node* getRight() const;                 // Получить правое поддерево
		void setRight(Node* value);             // Задать правое поддерево

		// Некоторые операции с узлом
		void increment();       // Увеличить количество на 1
	};

}
#endif // NODE_H