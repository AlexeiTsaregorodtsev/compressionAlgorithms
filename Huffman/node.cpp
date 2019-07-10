/*
 * Реализация класса узла(листа) дерева Node
 */

#include "node.h"

namespace Huffman {

	// Конструктор
	Node::Node(const char& key, const long long& amount)
		: key(key), amount(amount),
		left(nullptr), right(nullptr),
		parent(nullptr) {}

	// Деструктор
	Node::~Node()
	{
		// Если есть поддеревья - удаляем их
		if (left)
		{
			delete left;
		}
		if (right)
		{
			delete right;
		}
	}

	// gets и sets
	// Получить ключ
	char Node::getKey() const
	{
		return key;
	}

	// Получить значение
	long long Node::getAmount() const
	{
		return amount;
	}

	// Задать количество
	void Node::setValue(const long long& value)
	{
		amount = value;
	}

	// Получить левое поддерево
	Node* Node::getLeft() const
	{
		return left;
	}

	// Задать левое поддерево
	void Node::setLeft(Node* value)
	{
		// Если поддерево было не пустое - удалить его
		if (left)
			delete left;
		left = value;
	}

	// Получить правое поддерево
	Node* Node::getRight() const
	{
		return right;
	}

	// Задать правое поддерево
	void Node::setRight(Node* value)
	{
		// Если поддерево было не пустое - удалить его
		if (right)
			delete right;
		right = value;
	}

	// Получить родителя
	Node* Node::getParent() const
	{
		return parent;
	}

	// Задать родителя
	void Node::setParent(Node* value)
	{
		parent = value;
	}

	// Некоторые операции с узлом
	// Увеличить количество на 1
	void Node::increment()
	{
		amount++;
	}

}
