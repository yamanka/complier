// author:yamanka
//

#include "stdafx.h"
#include <iostream>
#include <set>
#include <fstream>
#include <string>
#include "symbol.h"
using namespace std;


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
	int kind, typ, dim, size, level, num, adr;

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
		
	//	cout << cx << ' ' << endl;
		getline(fileIn, line);
		cout << line << endl;
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
		a = "";
		do
		{
			if (k < al)
			{
				a = a + ' ';
				a[k] = ch;
				k++;
			}
			getch();
		} while ((ch >= 'a' && ch <= 'z') || ch == '_');
		
			id = a;
			for (i = 0; i < norw; i++)
				if (word[i] == id) break;
			if (i == norw) sym = ident;
			else sym = wsym[i];

		
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
			getch();
		} while (ch >= '0' && ch <= '9');
		if (k>nmax) error(30);
	}
	else if (ch == '=')
	{
		getch();
		if (ch == '=')
		{
			sym = eql;
			getch();
		}
		else sym = becomes;
	}
	else if (ch == '<')
	{
		getch();
		if (ch == '=')
		{
			sym = leq;
			getch();
		}
		else sym = lss;
	}
	else if (ch == '>')
	{
		getch();
		if (ch == '=')
		{
			sym = geq;
			getch();
		}
		else
			sym = gtr;
	}
	else if (ch == '!')
	{
		getch();
		if (ch == '=')
		{
			sym = neq;
			getch();
		}
		else sym = nul;
	}
	else
	{
		sym = ssym[ch];
		getch();
	}
	
}
int _tmain(int argc, _TCHAR* argv[])
{
	string s,s1;
	string den[30],den2[30];
	cout << "Input source program file name"<< endl;
	cin >> s;
	fileIn.open(s);   

	den[0] = "nul";	den[1] = "ident";	den[2] = "number";
	den[3] = "plus";	den[4] = "minus";	den[5] = "times";
	den[6] = "times";	den[7] = "eql";	den[8] = "neq";
	den[9] = "lss";	den[10] = "leq";	den[11] = "gtr";
	den[12] = "geq";	den[13] = "lparen";	den[14] = "rparen";
	den[15] = "comma";	den[16] = "semicolon";	den[17] = "period";
	den[18] = "becomes";	den[19] = "lbrace";	den[20] = "rbrace";
	den[21] = "lbracket";	den[22] = "rbracket";
	
	den2[3] = "+";	den2[4] = "-"; den2[5] = "*"; den2[6] = "/";
	den2[7] = "=="; den2[8] = "!="; den2[9] = "<"; den2[10] = "<=";
	den2[11] = ">"; den2[12] = ">="; den2[13] = "("; den2[14] = ")";
	den2[15] = ","; den2[16] = ";"; den2[17] = "."; den2[18] = "=";
	den2[19] = "{"; den2[20] = "}";	den2[21] = "[";	den2[22] = "]";

	wsym[0] = ifsym;	wsym[1] = thensym;	wsym[2] = elsesym;
	wsym[3] = whilesym;	wsym[4] = constsym;	wsym[5] = scanfsym;
	wsym[6] = printfsym;	wsym[7] = returnsym;	wsym[8] = intsym;
	wsym[9] = charsym;	wsym[10] = switchsym;	wsym[11] = casesym;
	wsym[12] = voidsym;	wsym[13] = mainsym;

	word[0] = "if";	word[1] = "then";	word[2] = "else";
	word[3] = "while";	word[4] = "const";	word[5] = "scanf";
	word[6] = "printf";	word[7] = "return";	word[8] = "int";
	word[9] = "char";	word[10] = "switch";	word[9] = "case";
	word[12] = "void";	word[13] = "main";

	ssym['+'] = plus;	ssym['-'] = minus;	ssym['*'] = times;
	ssym['/'] = slash;	ssym['('] = lparen;	ssym[')'] = rparen;
	ssym['='] = becomes;	ssym[','] = comma;	ssym['.'] = period;
	ssym['<'] = lss;	ssym['>'] = gtr;	ssym[';'] = semicolon;
	ssym['{'] = lbrace;	ssym['}'] = rbrace; ssym['['] = lbracket;
	ssym[']'] = rbracket;

	while (1)
	{
		getsym();
		if (sym >= 21)
		{
			cout << word[sym-21] << "  " << word[sym - 21] + "symble" << endl;
			continue;
		}
		else if (sym==1)
		{
			cout << id << "  " << "ident"<<endl;
			continue;

		}
		else if (sym == 2)
		{
			cout << num << "  " << "number" << endl;
			continue;
		}
		else if (sym >= 3)
		{
			cout << den2[sym] << "  " << den[sym] << endl;
			continue;
		}
	}
	s = "abc";
	s[1] = 'a';
	cout << s.length() << endl;
	return 0;
}

