#pragma once 
#include <iostream>
#include <windows.h>
#include <queue>
#include <vector>
using namespace std;

template<class W>
struct HuffmanTreeNode
{
	HuffmanTreeNode<W>* _left;
	HuffmanTreeNode<W>* _right;
	HuffmanTreeNode<W>* _parent;
	W _w;

	HuffmanTreeNode(const W& w)
		:_w(w)
		, _left(NULL)
		, _right(NULL)
		, _parent(NULL)
	{}
};

template<class W>
class HuffmanTree
{
	typedef HuffmanTreeNode<W> Node;
public:
	struct NodeCompare
	{
		bool operator()(Node* l, Node* r)
		{
			return l->_w > r->_w;
		}
	};

	HuffmanTree(W* a, size_t n, const W& invalid)
		:_root(NULL)
	{
		//优先级队列构造小堆，每次拿出最小元素去构造huffman结点
		priority_queue<Node*, vector<Node*>, NodeCompare>minheap;

		for (size_t i = 0; i < n; ++i)
		{
			if (a[i] != invalid)
				minheap.push(new Node(a[i]));
		}//小堆构建完成

 		if (minheap.size() == 0)
		{
			cout << "无效文件" << endl;
			exit(0);
		}
		else if (minheap.size() == 1)
		{
			Node* right = minheap.top();
			_root = new Node(right->_w);
			right->_parent = _root;
			_root->_right = right;
		}
		else
		{
			while (minheap.size() > 1)//每次拿到最小的元素后调整堆 
			{
				Node* left = minheap.top();
				minheap.pop();
				Node* right = minheap.top();//huffman模型 right比left大
				minheap.pop();
				Node* parent = new Node(left->_w + right->_w);
				//父节点等于孩子节点之和，然后将父节点丢回堆中，再取最小两个
				parent->_left = left;
				parent->_right = right;
				left->_parent = parent; //三叉链 构造最小树
				right->_parent = parent;
				minheap.push(parent);
			}
			_root = minheap.top();
		}

		_root = minheap.top();	//Huffman树构建完成
	}

	~HuffmanTree()
	{
		Destory(_root);
		_root = NULL;
	}

	void Destory(Node* root)
	{
		if (NULL == root)
			return;
		Destory(root->_left);
		Destory(root->_right);

		delete root;
	}

	Node* GetRoot()
	{
		return _root;
	}
private:
	HuffmanTree(const HuffmanTree<W>&);
	HuffmanTree<W>&  operator=(const HuffmanTree<W>&);
private:
	Node* _root;
};


void TestHuffmanTree()
{
	int a[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	HuffmanTree<int> t(a, sizeof(a)/sizeof(a[0]), 0);
}

