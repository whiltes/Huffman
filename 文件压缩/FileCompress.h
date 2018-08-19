#pragma once
#include <string>
#include <stdio.h>
#include <assert.h>
#include "HuffmanTree.h"

#pragma warning(disable:4996)

using namespace std;

typedef long long LongType;

struct CharInfo
{
	char _ch;		//字符
	LongType _count;	//次数
	string _code; //权值

	CharInfo operator+(const CharInfo& info)
	{
		CharInfo ret;
		ret._count = _count + info._count;
		return ret;
	}
	bool operator>(const CharInfo& info)
	{
		return _count > info._count;
	}
	bool operator<(const CharInfo& info)
	{
		return _count < info._count;
	}
	bool operator!=(const CharInfo& info)
	{
		return _count != info._count;
	}
};

class FileCompress
{
	typedef HuffmanTreeNode<CharInfo> Node;

	struct TmpInfo
	{
		char _ch;		
		LongType _num;
	};

public:
	FileCompress()
	{
		for (size_t i = 0; i < 256; ++i)
		{
			_infos[i]._ch = i;
			_infos[i]._count = 0;
		}
	}

	

/***********************压缩*******************************************/
	void Compress(const char* file)	//压缩
	{
		//1.统计字符出现的次数

		FILE* fout = fopen(file, "r");
		assert(fout);

		char character = fgetc(fout);

		while (character != EOF)
		{
			_infos[(unsigned char)character]._count++;
			character = fgetc(fout);
		}

		//2、构建Huffman tree
		CharInfo invalid;
		invalid._count = 0;	//无效字符
		HuffmanTree<CharInfo> tree(_infos, 256, invalid);

		//3、生成code

		GenerateHuffmanCode(tree.GetRoot());


		//4、压缩
		string compressFile = file;
		compressFile += ".huffman";

		FILE* fin = fopen(compressFile.c_str(), "wb");

		for (size_t i = 0; i < 256; ++i)
		{
			if (_infos[i]._count>0)
			{
				TmpInfo tmpinfo;
				tmpinfo._ch = _infos[i]._ch;
				tmpinfo._num = _infos[i]._count;

				fwrite(&tmpinfo, sizeof(TmpInfo), 1, fin);
			}
		}

		TmpInfo tmpinfo;
		tmpinfo._num = -1;
		fwrite(&tmpinfo, sizeof(TmpInfo), 1, fin);



		fseek(fout, 0, SEEK_SET);

		character = fgetc(fout);

		size_t CeffectiveBit = 0;
		char value = 0;
		int pos = 0;

		while (character != EOF)
		{
			string& code = _infos[(unsigned char)character]._code;
			for (size_t i = 0; i < code.size(); ++i)
			{
				if (code[i] == '0')
				{
					value &= ~(1 << pos);
				}
				else if (code[i] == '1')
				{
					value |= (1 << pos);
				}
				else
				{
					assert(false);
				}
				++pos;
				if (pos == 8)
				{
					fputc(value, fin);
					value = 0;
					pos = 0;
				}
				++CeffectiveBit;
			}
			character = fgetc(fout);
		}
		cout << "压缩Huffman编码长度:" << CeffectiveBit << endl;

		if (pos > 0)
		{
			fputc(value, fin);
		}
		fclose(fout);
		fclose(fin);
	}



	//生成huffman code
	void GenerateHuffmanCode(Node* root)
	{
		if (root == NULL)
			return;
		if (root->_left == NULL && root->_right == NULL)
		{
			string code;
			Node* cur = root;
			Node* parent = cur->_parent;
			while (parent)
			{
				if (cur == parent->_left)
				{
					code.push_back('0');
				}
				else
				{
					code.push_back('1');
				}

				cur = parent;
				parent = cur->_parent;
			}

			reverse(code.begin(), code.end());
			_infos[(unsigned char)root->_w._ch]._code = code;
			return;
		}

		GenerateHuffmanCode(root->_left);
		GenerateHuffmanCode(root->_right);
	}
/*******************解压缩*********************************************/

	void Uncompress(const char* file)
	{
		assert(file);

		string uncompressFile = file;

		size_t pos = uncompressFile.rfind('.');

		assert(pos != string::npos);
		uncompressFile.erase(pos, uncompressFile.size() - pos);

		uncompressFile += ".unhuffman";

		FILE* fout = fopen(file, "rb");
		assert(fout);

		FILE* fin = fopen(uncompressFile.c_str(), "w");
		assert(fin);

		TmpInfo info;
		fread(&info, sizeof(TmpInfo), 1, fout);
		while (info._num != -1)
		{
			_infos[(unsigned char)info._ch]._ch = info._ch;
			_infos[(unsigned char)info._ch]._count = info._num;

			fread(&info, sizeof(TmpInfo), 1, fout);
		}

		//1、重建huffman tree
		size_t n = 0;
		CharInfo invalid;
		invalid._count = 0;
		HuffmanTree<CharInfo> tree(_infos, 256, invalid);
		Node* root = tree.GetRoot();
		Node* cur = root;
		LongType fileSize = root->_w._count;//跟等于所有叶子之和
		cout << "文件中ASCII个数:" << fileSize << endl;

		//2、解压
		char value = fgetc(fout);
		while (1)
		{
			for (size_t pos = 0; pos < 8; ++pos)
			{
				if (value&(1 << pos))
					cur = cur->_right;
				else
					cur = cur->_left;

				++n;

				if (cur->_left == NULL&&cur->_right == NULL)
				{
					fputc(cur->_w._ch, fin);
					cur = root;

					if (--fileSize == 0)//？？？？？？？
					{
						break;
					}
				}
			}
			if (fileSize == 0)
			{
				break;
			}
			value = fgetc(fout);
		}

		cout << "解压Huffman编码长度:" << n << endl;
		fclose(fout);
		fclose(fin);

	}

private:
	CharInfo _infos[256];
};


void TestCompress()
{
	FileCompress fc;
	size_t start = GetTickCount();
	fc.Compress("Input.txt");
	size_t end = GetTickCount();
	cout << "UseTime:" << end - start << endl;


	fc.Uncompress("Input.txt.huffman");

}