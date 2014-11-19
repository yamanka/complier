// author:yamanka
//

#include "stdafx.h"
#include <iostream>
#include <set>
#include <fstream>
#include <string>
#include "symbol.h"
using namespace std;


#define constant 1
#define variable 2
#define function 3

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
typedef struct ct
{
	string name;
	int typ, val;
	char c;
}ctable;


char ch=' ';
char cht = ' ';
int sym;
string id;
int num, cc=1, ll=0, kk=0, err, cx;
string line;
string a;
instruction code[cxmax];
string word[norw];
string stable;
int wsym[norw];
int ssym[256];
string mnemonic[10];
set<int> declbegsys, statbegsys, facbegsys;
txtable table[txmax];
ctable cxtable[txmax];
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
			//cout << "program incomplete" << endl;
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
	if (ch >= 'A' && ch <= 'Z') ch -= 'A' - 'a';
	cc++;

}
void getsym()
{
	int i, j, k;
	while (ch == ' '||ch=='\0')
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
		} while ((ch >= 'a' && ch <= 'z') || ch == '_'||(ch>='0' && ch<='9'));
		
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
	else if (ch=='\'')
	{
		getch();
		cht = ch;
		if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || (ch >= '0'&&ch <= '9') || (ch >= 'a'&&ch <= 'z') || (ch >= 'A'&&ch <= 'Z'))
		{
			getch();
			if (ch == '\'')
			{
				sym = charc;
				getch();
			}
			else
			{
				error(102);
				sym = nul;
			}
			
		}
		else sym = nul;

	}
	else if (ch == '"')
	{
		
		stable = "";
		getch();
		while (ch != '"')
		{
			stable += ch;
			
			getch();
			if (ch=='\0') break;
		}
		if (ch=='\0') { error(101); sym = nul;}
		else
		{
			sym = stringc;
			getch();
		}
	}else
	{
		sym = ssym[ch];
		getch();
	}
	
}
int _tmain(int argc, _TCHAR* argv[])
{
	string s,s1;
	string den[41],den2[41];
	cout << "Input source program file name"<< endl;
	cin >> s;
	fileIn.open(s);   

	den[0] = "NODEFINE";	den[1] = "IDEN";	den[2] = "INTCON";
	den[3] = "PLUS";	den[4] = "MIUN";	den[5] = "MULT";
	den[6] = "DIV";	den[7] = "EQL";	den[8] = "NEQ";
	den[9] = "LSS";	den[10] = "LEQ";	den[11] = "GRE";
	den[12] = "GEQ";	den[13] = "LPARENT";	den[14] = "RPARENT";
	den[15] = "COMMA";	den[16] = "SEMICN";	den[17] = "PERIOD";
	den[18] = "ASSIGN";	den[19] = "LBRACE";	den[20] = "RBRACE";
	den[21] = "LBRACK";	den[22] = "RBRACK";	den[23] = "IFTK";
	den[24] = "THENTK";	den[25] = "ELSETK"; den[26] = "WHILETK";
	den[27] = "CONSTTK"; den[28] = "SCANFTK";	den[29] = "PRINTTK";
	den[30] = "RETURNTK"; den[31] = "INTTK"; den[32] = "CHARTK";
	den[33] = "SWITCHTK";	den[34] = "CASETK"; den[35] = "VOIDTK";
	den[36] = "MAINTK"; den[37] = "CHARCON"; den[38] = "STRCON";
	den[39] = "QMARK"; den[40] = "DQMARK";
	
	den[0] = "null"; 
	den2[3] = "+";	den2[4] = "-"; den2[5] = "*"; den2[6] = "/";
	den2[7] = "=="; den2[8] = "!="; den2[9] = "<"; den2[10] = "<=";
	den2[11] = ">"; den2[12] = ">="; den2[13] = "("; den2[14] = ")";
	den2[15] = ","; den2[16] = ";"; den2[17] = "."; den2[18] = "=";
	den2[19] = "{"; den2[20] = "}";	den2[21] = "[";	den2[22] = "]";
	den2[23] = "if"; den2[24] = "then"; den2[25] = "else";
	den2[26] = "while"; den2[27] = "const"; den2[28] = "scanf";
	den2[29] = "printf"; den2[30] = "return"; den2[31] = "int";
	den2[32] = "char"; den2[33] = "switch"; den2[34] = "case";
	den2[35] = "void"; den2[36] = "main"; den2[39] = "'";
	den2[40] = "\"";
	
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
	ssym[']'] = rbracket; ssym['\''] = qmark; ssym['"'] = dqmark;
		
	while (1)
	{
		getsym();
		if (sym == 1)
		{
			cout << "IDEN" << " " << id << endl;
			continue;
		}
		else if (sym==2)
		{
			cout << "INTCON" << " " << num<<endl;
			continue;

		}
		else if (sym == 37)
		{
			cout << "CHARCON" << " " << cht << endl;
			continue;
		}
		else if (sym == 38)
		{
			cout << "STRCON" << " " << stable << endl;
			continue;
		}
		cout << den[sym] << ' ' << den2[sym] << endl;
	}
	s = "abc";
	s[1] = 'a';
	cout << s.length() << endl;
	return 0;
}

