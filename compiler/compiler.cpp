// author:yamanka
//

#include "stdafx.h"
#include <iostream>
#include <set>
#include <fstream>
#include <string>
using namespace std;

#include "symbol.h"
#define constant 0
#define variable 1
#define function 2

#define lit 0
#define opr 1
#define lod 2
#define sto 3
#define cal 4
#define intop 5
#define jmp 6
#define jpc 7
#define red 8
#define wrt 9

const int norw = 14, txmax = 100, nmax = 14, al = 10, amax = 2047, levmax = 2, cxmax = 200;
typedef struct ins
{
	int f, l, a;
}instruction;
typedef struct tx
{
	string name;
	int kind, val, level, adr;

}txtable;


char ch=' ';
int sym;
string id;
int num, cc=1, ll=0, kk=0, err, cx;
string line;
string a;
instruction code[cxmax];
string word[norw];
int wsym[norw];
int ssym[256];
string mnemonic[10];
set<int> declbegsys, statbegsys, facbegsys;
txtable table[txmax];
ifstream fileIn;
char nam[30];

void error(int n)
{
	cout << "error " << n << " in line " << cc << endl;
}

void getch()
{
	if (cc > ll) 
	{
		if (fileIn.eof())
		{
			cout << "program incomplete" << endl;
			fileIn.close();
			exit(0);
		}
		ll = 0;
		cc = 0;
		
		cout << cx << ' ' << endl;
		getline(fileIn, line);
		ll = line.length();
	}
	
	ch = line[cc];
	cc++;

}
void getsym()
{
	int i, j, k;
	while (ch == ' ')
		getch();
	if ((ch >= 'a' && ch <= 'z') || ch == '_')
	{
		k = 0;
		do
		{
			if (k < al)
			{
				k++;
				a[k] = ch;
			}
			getch;
		} while ((ch >= 'a' && ch <= 'z') || ch == '_');
		if (k >= kk) k = kk;
		else	
			do
			{
				a[kk] = ' ';
				kk--;

			} while (kk != k);
			id = a;
			for (i = 0; i < norw; i++)
				if (word[k] == id) break;
			if (i == norw) sym = ident;
			else sym = wsym[k];

		
	}
	else if (ch >= '0' && ch <= '9')
	{
		k = 0;
		num = 0;
		sym = number;
		do
		{
			num = num * 10 + ch - '0';
			k++;
			getch;
		} while (ch >= '0' && ch <= '9');
		if (k>nmax) error(30);
	}
	else if (ch == '=')
	{
		getch;
		if (ch == '=')
		{
			sym = eql;
			getch;
		}
		else sym = becomes;
	}
	else if (ch == '<')
	{
		getch;
		if (ch == '=')
		{
			sym = leq;
			getch;
		}
		else sym = lss;
	}
	else if (ch == '>')
	{
		getch;
		if (ch == '=')
		{
			sym = geq;
			getch;
		}
		else
			sym = gtr;
	}
	else if (ch == '!')
	{
		getch;
		if (ch == '=')
		{
			sym = neq;
			getch;
		}
		else sym = nul;
	}
	else
	{
		sym = ssym[ch];
		getch;
	}
	
}
int _tmain(int argc, _TCHAR* argv[])
{
	string s,s1;
	
	cout << "Input source program file name"<< endl;
	cin >> s;
	fileIn.open(s);   

	while (1)
	{
		getsym();
	}
	s = "abc";
	s[1] = 'a';
	cout << s.length() << endl;
	return 0;
}

