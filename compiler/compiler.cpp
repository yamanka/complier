// author:yamanka
//

#include "stdafx.h"
#include <iostream>
#include <set>
#include <fstream>
#include <string>
#include "symbol.h"
#include<strstream>
using namespace std;


#define constanti 1
#define constantc 2
#define variablei 3
#define variablec 4
#define functioni 5
#define functionc 6
#define functionv 7

#define add 0
#define sub 1
#define mul 2
#define div 3
#define mov 4
#define jz 5
#define jmp 6
#define cal 7
#define ret 8
#define red 9
#define prt 10
#define sig 11

const int norw = 14, txmax = 100, nmax = 14, al = 10, amax = 2047, levmax = 2, cxmax = 1000;
typedef struct ins
{
	int op;
	string a, b, c;
	string signal;
}instruction;
typedef struct tx
{
	string name;
	int typ, dim, size, level, num, adr, val;

}txtable;



char ch = ' ';
char cht = ' ';
int sym;
string id;
int num, cc = 0, ll = -1, kk = 0, err, cx = 0, tcx = 0;
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
int signum = 0;
ifstream fileIn;


void error(int n)
{
	cout << "error " << n << " in line " << cc << endl;
}

void getch()
{
	string temp;
	if (cc > ll)
	{
		if (fileIn.eof())
		{
			//cout << "program incomplete" << endl;
			fileIn.close();
			exit(0);
		}


		//	cout << cx << ' ' << endl;
		getline(fileIn, temp);
		cout << temp << endl;
		ll += temp.length();
		line += temp;
	}

	ch = line[cc];
	if (ch >= 'A' && ch <= 'Z') ch -= 'A' - 'a';
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
		} while ((ch >= 'a' && ch <= 'z') || ch == '_' || (ch >= '0' && ch <= '9'));

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
	else if (ch == '\'')
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
			if (cc == ll) break;
			getch();	
		}
		if (ch != '"') { error(101); sym = nul; }
		else
		{
			sym = stringc;
			getch();
		}
	}
	else
	{
		sym = ssym[ch];
		getch();
	}

}
//===================================================ｴﾊｷｨｷﾖﾎ・================================================================================
string getnum()
{
	strstream ss;
	string s;
	signum++;
	ss << signum;
	ss >> s;
	return s;
}
void gen(int op, string a, string b, string c)
{
	code[cx].op = op;
	if (op != sig){
		code[cx].a = a;
		code[cx].b = b;
		code[cx].c = c;
	}
	else
		code[cx].signal = c;


	cx++;

}
void test(set<int> s1, set<int>s2, int n)
{
	if (s1.find(sym) == s1.end())
	{
		error(n);
		while (s1.find(sym) == s1.end() && s2.find(sym) == s2.end())
			getsym();
	}
}
void enter(int type, int *tx, int cnum, int level)
{

	if (type == constanti)
	{
		table[*tx].name = id;
		table[*tx].typ = type;
		table[*tx].val = num;
		*tx++;
	}
	if (type == constantc)
	{
		table[tcx].name = id;
		table[tcx].typ = type;
		table[tcx].num = cht;
		*tx++;
	}
	if (type == variablei || type == variablec)
	{
		table[*tx].name = id;
		table[*tx].typ = type;
		table[*tx].size = cnum;
		table[*tx].level = level;


		*tx++;

	}
	if (type == functionc || type == functioni || type == functionv)
	{
		table[*tx].name = id;
		table[*tx].typ = type;
		table[*tx].size = cnum;
		table[*tx].level = 1;

		if (type == functionc || type == functioni)

			*tx++;
	}
}
int position(string id, int tx, string fname)
{
	int i, j;
	for (i = 1; i < tx; i++)
		if (table[i].name == fname) break;


	while (i <= tx)
	{
		if (table[i].name == id) return i;
		i++;
		if (table[i].typ >= functioni && table[i].typ <= functionv) break;
	}
	i = 1;

	while (i <= tx)
	{
		if (table[i].name == id) return i;
		i++;
		if (table[i].typ >= functioni && table[i].typ <= functionv) break;
	}
	return 0;
}
void constdeclaration(int type, int *tx, int level)
{
	if (sym == ident)
	{
		getsym();
		if (sym == becomes)
		{
			getsym();
			if ((sym == number && type == constanti) || (sym == charc && type == constantc))
			{
				enter(type, tx, 0, level);
				getsym();
			}
			else
				error(110);
		}
		else
			error(109);
	}
	else
		error(108);
}
void vardeclaration(int type, int *tx, int level)
{
	if (sym == ident)
	{
		getsym();
		if (sym == comma)
		{
			enter(type, tx, 0, level);
			return;
		}
		if (sym == lbracket)
		{
			getsym();
			if (sym == number && num>0)
			{
				enter(type, tx, num, level);
				getsym();
				if (sym == rbracket)

					getsym();
				else
					error(111);
			}
			else
				error(113);
		}
		else
			error(112);
	}
	else
		error(108);
}
string convert(int num)
{
	strstream ss;
	string s="";

	ss << num;
	ss >> s;
	return s;
}
string getall()
{
	string s = "";
	getsym();
	if (sym != lbracket)
		error(117); else getsym();
	if (sym == number)
	{
		s= "[" + convert(num) + "]";
		getsym();
	}
	else error(118);
	if (sym != rbracket)
		error(119); else getsym();
	return s;
}
void condition()
{

}
void expression()
{

}
void statement(int *tx, string s, set<int> blockset, int flag)
{
	string numtemp, numtemp2;
	int i, k;
	do
	{
		if (sym == ifsym)
		{
			getsym();
			condition();
			numtemp = getnum();
			numtemp2 = getnum();
			gen(jz, "t1", "0", numtemp);
			statement(tx, s, blockset, 0);
			gen(jmp, "0", "0", numtemp2);
			gen(sig, "0", "0", numtemp);

			if (sym == elsesym)
			{
				getsym();
				statement(tx, s, blockset, 0);
			}
			gen(sig, "0", "0", numtemp2);
		}
		else if (sym == whilesym)
		{
			numtemp = getnum();
			gen(sig, "0", "0", numtemp);
			getsym();
			condition();
			numtemp2 = getnum();
			gen(jz, "t1", "0", numtemp2);
			statement(tx, s, blockset, 0);
			gen(jmp, "0", "0", numtemp);
			gen(sig, "0", "0", numtemp2);

		}
		else if (sym == lbrace)
		{
			getsym();
			statement(tx, s, blockset, 1);
		}
		else if (sym == ident)
		{
			i = position(id, *tx, s);
			numtemp = id;
			if (i > 0)
			{
				if (table[i].typ == variablei || table[i].typ == variablec)
				{
					if (table[i].size > 0)
						numtemp += getall();
										
					getsym();
					if (sym == becomes)
						getsym(); else error(116);

					expression();
					gen(mov, "t1", "0", numtemp);


				}
				else if (table[i].typ >= functioni && table[i].typ <= functionv)
					expression();
				else error(0);
			}
			else error(118);
			if (sym != semicolon) error(0); else getsym();
		}
		else if (sym == scanfsym)
		{
			getsym();
			if (sym != lbrace) error(0); 
			do
			{
				getsym();
				if (sym == ident)
				{
					i = position(id, *tx, s);
					numtemp = id;
					if (i > 0)
					{
						if (sym == variablec || sym == variablei)
						{
							if (table[i].size > 0)
								numtemp += getall();
							gen(red, "0", "0", numtemp);
							getsym();
						}
						else error(0);
					}
					else error(0);
				}
				else error(0);

			} while (sym != comma);
			if (sym != semicolon) error(0); else getsym();
		}
		else if (sym == printfsym)
		{
			getsym();
			if (sym != lbrace) error(0); else getsym();
			if (sym == stringc)
			{
				gen(prt, "0", "0", stable);
				getsym();
				if (sym == comma)
				{
					getsym();
					expression();
					gen(prt, "0", "0", "t1");

				}
				 if (sym == rbrace)
					getsym();
				else error(0);		
			}
			else
			{
				expression();
				gen(prt, "0", "0", "t1");
				if (sym == rbrace)
					getsym();
				else error(0);
			}
			if (sym != semicolon) error(0); else getsym();
		}



	} while (flag == 1 && sym != rbrace);
	if (flag == 0 && sym != semicolon) error(115); else
		if (flag == 1 && sym != rbrace) error(116); else
			getsym();


}
void blockcomp(int *tx, string s, set<int> blockset)
{
	int typetemp;
	gen(sig, " ", " ", s);
	if (sym != lbrace) error(114); else getsym();
	while (sym == constsym)
	{
		getsym();
		if (sym == intsym)
			typetemp = constanti;
		else if (sym == charsym)
			typetemp = constantc;
		else error(105);
		getsym();
		while (sym == ident)
		{
			constdeclaration(typetemp, tx, 1);
			while (sym == comma)
			{
				getsym();
				constdeclaration(typetemp, tx, 1);
			}
			if (sym == semicolon) getsym();
			else error(104);

		}

	}

	while (sym == intsym || sym == charsym)
	{

		if (sym == intsym) typetemp = variablei;
		else if (sym == charsym) typetemp = variablec;


		getsym();
		while (sym == ident)
		{
			vardeclaration(typetemp, tx, 1);
			while (sym == comma)
			{
				getsym();
				vardeclaration(typetemp, tx, 1);
			}
			if (sym == semicolon) getsym();
			else error(104);

		}

	}

	statement(tx, s, blockset, 1);

}
void startcomp(int tx, set<int>fsys)
{
	int tx0, cx0, typetemp;
	int temp, cnumtemp;
	set<int> blockset;
	blockset.insert(semicolon);
	blockset.insert(rbrace);
	tx0 = tx;

	gen(jmp, "0", "0", "main");
	test(fsys, fsys, 103);
	while (sym == constsym)
	{
		getsym();
		if (sym == intsym)
			typetemp = constanti;
		else if (sym == charsym)
			typetemp = constantc;
		else error(105);
		getsym();
		while (sym == ident)
		{
			constdeclaration(typetemp, &tx, 0);
			while (sym == comma)
			{
				getsym();
				constdeclaration(typetemp, &tx, 0);
			}
			if (sym == semicolon) getsym();
			else error(104);

		}
		test(fsys, fsys, 103);
	}
	while (sym == intsym || sym == charsym)
	{

		if (sym == intsym) typetemp = variablei;
		else if (sym == charsym) typetemp = variablec;

		temp = cc;
		getsym();
		getsym();
		if (sym == lparen)
		{
			cc = temp;
			break;
		}
		else
			cc = temp;

		getsym();
		while (sym == ident)
		{
			vardeclaration(typetemp, &tx, 0);
			while (sym == comma)
			{
				getsym();
				vardeclaration(typetemp, &tx, 0);
			}
			if (sym == semicolon) getsym();
			else error(104);

		}
		test(fsys, fsys, 103);
	}

	while (sym == intsym || sym == charsym || sym == voidsym)
	{
		if (sym == intsym) typetemp = functioni;
		else if (sym == charsym) typetemp = functionc;
		else typetemp = functionv;
		temp = cc;
		getsym();
		if (sym == mainsym)
		{
			enter(typetemp, &tx, 0, 1);
			getsym();
			if (sym != lparen) error(106);
			getsym();
			if (sym != rparen) error(107);
			getsym();
			blockcomp(&tx, "main", blockset);
		}
		if (sym == ident)

			if (position(id, tx, " ") != 0)
			{
			error(106);
			test(fsys, fsys, 103);
			continue;
			}
			else
			{
				cnumtemp = tx;
				enter(typetemp, &tx, 0, 1);
			}

		else { error(105); test(fsys, fsys, 103); continue; }

		getsym();
		if (sym != lparen) error(106);
		else getsym();
		while (sym == intsym || sym == charsym)
		{
			if (sym == intsym) typetemp = variablei;
			else typetemp = variablec;
			getsym();
			vardeclaration(typetemp, &tx, 1);
			if (sym != comma) break;
			getsym();
		}
		if (sym != rparen) error(107); else
			getsym();
		table[cnumtemp].size = tx - cnumtemp - 1;
		blockcomp(&tx, table[cnumtemp].name, blockset);
	}

}
int _tmain(int argc, _TCHAR* argv[])
{
	string s, s1;
	string den[42], den2[42];

	cout << "Input source program file name" << endl;
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
	wsym[12] = voidsym;	wsym[13] = mainsym; wsym[41] = elsesym;

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
		else if (sym == 2)
		{
			cout << "INTCON" << " " << num << endl;
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
