#ifndef OO_STACK_H
#define OO_STACK_H

#include "assert.h" 

namespace ke
{
	template <typename T>
	class Node
	{
		public:
			T data;
			Node* next;
	};

	template <typename T>
	class Stack
	{
	private:
		Node<T>* top;

	public:
		Stack() // constructor
		{
			top = nullptr;
		}

		~Stack() // destructor
		{
			clear();
		}

		void push(T item)
		{
			Node<T>* newNode = new Node<T>;
			newNode->data = item;
			newNode->next = top;
			top = newNode;
		}

		T pop()
		{
			assert(!isEmpty());
			Node<T>* temp = top;
			T data = temp->data;
			top = top->next;
			delete temp;
			return data;
		}

		T peek()
		{
			assert(!isEmpty());
			return top->data;
		}

		void clear()
		{
			while (top != nullptr)
			{
				Node<T>* temp = top;
				top = top->next;
				delete temp;
			}
		}

		bool isEmpty()
		{
			return top == nullptr;
		}
	};
}

#endif