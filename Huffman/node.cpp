/*
 * ���������� ������ ����(�����) ������ Node
 */

#include "node.h"

namespace Huffman {

	// �����������
	Node::Node(const char& key, const long long& amount)
		: key(key), amount(amount),
		left(nullptr), right(nullptr),
		parent(nullptr) {}

	// ����������
	Node::~Node()
	{
		// ���� ���� ���������� - ������� ��
		if (left)
		{
			delete left;
		}
		if (right)
		{
			delete right;
		}
	}

	// gets � sets
	// �������� ����
	char Node::getKey() const
	{
		return key;
	}

	// �������� ��������
	long long Node::getAmount() const
	{
		return amount;
	}

	// ������ ����������
	void Node::setValue(const long long& value)
	{
		amount = value;
	}

	// �������� ����� ���������
	Node* Node::getLeft() const
	{
		return left;
	}

	// ������ ����� ���������
	void Node::setLeft(Node* value)
	{
		// ���� ��������� ���� �� ������ - ������� ���
		if (left)
			delete left;
		left = value;
	}

	// �������� ������ ���������
	Node* Node::getRight() const
	{
		return right;
	}

	// ������ ������ ���������
	void Node::setRight(Node* value)
	{
		// ���� ��������� ���� �� ������ - ������� ���
		if (right)
			delete right;
		right = value;
	}

	// �������� ��������
	Node* Node::getParent() const
	{
		return parent;
	}

	// ������ ��������
	void Node::setParent(Node* value)
	{
		parent = value;
	}

	// ��������� �������� � �����
	// ��������� ���������� �� 1
	void Node::increment()
	{
		amount++;
	}

}
