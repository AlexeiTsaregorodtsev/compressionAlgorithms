/*
 * �������� ������ ����(�����) ������ Node
 */

#ifndef NODE_H
#define NODE_H

namespace Huffman {

	class Node
	{
		char key;           // ����
		long long amount;   // ����������
		Node* left;         // ������ �� ����� ���������
		Node* right;        // ������ �� ������ ���������
		// �������� ����������� ����������� �������� ������������
		Node(const Node& node);
		Node& operator=(const Node& node);
	public:
		// ����������
		Node(const char& key, const long long& amount);
		// ����������
		~Node();

		// gets � sets
		char getKey() const;                    // �������� ����
		long long getAmount() const;            // �������� ����������
		void setValue(const long long& value);  // ������ ����������
		Node* getLeft() const;                  // �������� ����� ���������
		void setLeft(Node* value);              // ������ ����� ���������
		Node* getRight() const;                 // �������� ������ ���������
		void setRight(Node* value);             // ������ ������ ���������

		// ��������� �������� � �����
		void increment();       // ��������� ���������� �� 1
	};

}
#endif // NODE_H