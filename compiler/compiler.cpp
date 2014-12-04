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
#define eqa 12 //==
#define ieq 13//!=
#define gt 14//>
#define gte 15//>=
#define lt 16//<
#define lte 17//<=
#define opn 18
#define fend 19

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
typedef struct da
{

}dag;


char ch = ' ';
char cht = ' ';
int sym;
string id;
int num, cc = 0, ll = -1, kk = 0, err=0, cx = 0, tcx = 0;
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
ofstream fout,x86c;
string fcode[20];
void expression(int, string, int);

void error(int n)
{
	cout << "error " << n << endl;
	err++;
}

void getch()
{
	string temp;
	if (cc > ll)
	{
		if (fileIn.eof())
		{
			cout << "program incomplete" << endl;
			fileIn.close();
			exit(0);
		}


		//	cout << cx << ' ' << endl;
		getline(fileIn, temp);
		fout << temp << endl;
		ll += temp.length()+1;
		line += temp+" ";
	}

	ch = line[cc];
	if (ch >= 'A' && ch <= 'Z') ch -= 'A' - 'a';
	cc++;

}

void getsym()
{
	int i, j, k;


	while (ch == ' '||ch=='\t')
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
		if (k>nmax) error(100);
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
				error(101);
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
		if (ch != '"') { error(102); sym = nul; }
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
	if (op == fend)
		cout << "end" << endl;
	else if (op != sig){
		code[cx].a = a;
		code[cx].b = b;
		code[cx].c = c;
		cout << fcode[op] << '	' << a << ',' << b << ',' << c << endl;
	}
	else
	{
		code[cx].signal = c;
		cout << c << ':' << endl;
	}
	
	cx++;

}
void test(set<int> s1, int n)
{
	if (s1.find(sym) == s1.end())
	{
		error(n);
		while (s1.find(sym) == s1.end() )
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
		(*tx)++;
	}
	if (type == constantc)
	{
		table[*tx].name = id;
		table[*tx].typ = type;
		table[*tx].val = cht;
		(*tx)++;
	}
	if (type == variablei || type == variablec)
	{
		table[*tx].name = id;
		table[*tx].typ = type;
		table[*tx].size = cnum;
		table[*tx].level = level;


		(*tx)++;

	}
	if (type == functionc || type == functioni || type == functionv)
	{
		table[*tx].name = id;
		table[*tx].typ = type;
		table[*tx].size = cnum;
		table[*tx].level = 1;

	

			(*tx)++;
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
	while (i <= tx)
	{
		if (table[i].typ >= functioni && table[i].typ <= functionv && table[i].name == id) return i;
		i++;
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
				error(103);
		}
		else
			error(104);
	}
	else
		error(105);
}
void vardeclaration(int type, int *tx, int level)
{
	if (sym == ident)
	{
		getsym();
		if (sym == comma|| sym==semicolon||sym==rparen)
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
					error(106);
			}
			else
				error(107);
		}
		else
			error(108);		
	}
	else
		error(105);
}
string convert(int num)
{
	strstream ss;
	string s="";

	ss << num;
	ss >> s;
	return s;
}
string getall(int num, string fname, int tx)
{
	string s = "";
	getsym();
	if (sym != lbracket)
		error(109); else getsym();
	expression(num, fname, tx);
	s = s+"[" + "$t" + convert(num) + "]";
	if (sym != rbracket)
		error(106); 
	return s;
}
void factor(int tnum,string fname,int tx,int flag)
{
	int i,j;

	if (sym == ident)
	{
		i = position(id, tx, fname);
		if (i == 0)
			error(110);
		else
		{
			if (table[i].typ == constantc || table[i].typ == constanti)
			{
				gen(mov, convert(table[i].val), "0", "$t" + convert(tnum));
				getsym();
			}
			else if ((table[i].typ == functionv && flag == 1)|| table[i].typ==functioni || table[i].typ==functionc)
			{
				getsym();
				if (sym != lparen) error(111); else getsym();
				for (j = 1; j <= table[i].size; j++)
				{
					expression(tnum, fname, tx);
					gen(mov, "$t" + convert(tnum), "0", "$c" + convert(table[i].size-j+1));
					
					if (j < table[i].size)
						if (sym != comma) error(112);
						else getsym();
					if (j == table[i].size)
						if (sym != rparen) error(113);
						else getsym();
				}
				gen(cal, "0", "0", table[i].name);
				gen(mov, "$r", "0", "$t" + convert(tnum));
			}
			else if (table[i].typ == variablec || table[i].typ == variablei)
			
				if (table[i].size > 0)
				{
					gen(mov, table[i].name + getall(tnum, fname, tx), "0", "$t" + convert(tnum));
					getsym();
				}
				else
				{
					gen(mov, table[i].name, "0", "$t" + convert(tnum));
					getsym();
				}
			
		
		}
	}
	else if (sym == lparen)
	{
		getsym();
		expression(tnum, fname, tx);
		if (sym != rparen)
			error(113); else getsym();
	}
	else if (sym == number)
	{
		gen(mov, convert(num), "0", "$t" + convert(tnum));
		getsym();
	}
	else if (sym == charc)
	{
		gen(mov, convert(cht), "0", "$t" + convert(tnum));
		getsym();
	}
	else
		error(108);
}
void term(int num,string fname,int tx)
{
	int mulop;
	factor(num++,fname,tx,0);
	while (sym == times || sym == slash)
	{
		mulop = sym;
		getsym();
		factor(num,fname,tx,0);
		if (mulop == times)
			gen(mul, "$t" + convert(num - 1), "$t" + convert(num), "$t" + convert(num - 1));
		else
			gen(div, "$t" + convert(num - 1), "$t" + convert(num), "$t" + convert(num - 1));

		
	}
}
void expression(int num,string fname,int tx)
{
	int addop;
	if (sym == plus || sym == minus)
	{
		addop = sym;
		getsym();
		term(num++,fname,tx);
		if (addop == minus)
			gen(opn, "$t" + convert(num - 1), 0, "$t" + convert(num - 1));
	}
	else term(num++,fname,tx);
	while (sym == plus || sym == minus)
	{
		addop = sym;
		getsym();
		term(num,fname,tx);
		if (addop == plus)
			gen(add, "$t" + convert(num - 1), "$t" + convert(num), "$t" + convert(num - 1));
		else
			gen(sub, "$t" + convert(num - 1), "$t" + convert(num), "$t" + convert(num - 1));

	}
}


void condition(int num,string fname,int tx,string x)
{
	int temp;
	expression(num,fname,tx);
	if (sym == rparen)
	{
		gen(mov, "0", "0", "$t2");
		gen(lte, "$t1", "$t2", x);
		getsym();
		return;
	}
	if (sym == eql)
		temp = ieq;
	else if (sym == neq)
		temp = eqa;
	else if (sym == gtr)
		temp = lte;
	else if (sym == geq)
		temp = lt;
	else if (sym == lss)
		temp = gte;
	else if (sym == leq)
		temp = gt;

		getsym();
		expression(num+1,fname,tx);

		gen(temp, "$t"+convert(num), "$t"+convert(num+1), x);
		if (sym != rparen)
			error(113); else getsym();
		return;
	
}

void statement(int *tx, string s, set<int> blockset, int flag)
{
	string numtemp, numtemp2;
	int i, k;
	int tnum = 1;
	do
	{
		if (flag == 1 && sym == rbrace) break;
		if (sym == ifsym)
		{
			getsym();
			if (sym != lparen) error(111); else getsym();
			
			numtemp = getnum();
			numtemp2 = getnum();
			condition(tnum, s, *tx,numtemp);
			
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
			if (sym != lparen) error(111); else getsym();
			
			numtemp2 = getnum();
			condition(tnum, s, *tx,numtemp2);
			
			statement(tx, s, blockset, 0);
			gen(jmp, "0", "0", numtemp);
			gen(sig, "0", "0", numtemp2);

		}
		else if (sym == lbrace)
		{
			getsym();
			statement(tx, s, blockset, 1);
			if (sym != rbrace) error(106); else getsym();
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
						numtemp += getall(tnum, s, *tx);

					getsym();
					if (sym == becomes)
						getsym(); else error(104);

					expression(tnum+1, s, *tx);

					gen(mov, "$t" + convert(tnum+1), "0", numtemp);


				}
				else if (table[i].typ >= functioni && table[i].typ <= functionv)
					factor(tnum, s, *tx, 1);
				else { error(114); getsym(); }
			}
			else { error(110); getsym(); }
			if (sym != semicolon) error(115); else getsym();
		}
		else if (sym == scanfsym)
		{
			getsym();
			if (sym != lparen) error(111); 
			do
			{
				getsym();
				if (sym == ident)
				{
					i = position(id, *tx, s);
					numtemp = id;
					if (i > 0)
					{
						if (table[i].typ == variablec || table[i].typ == variablei)
						{
							if (table[i].size > 0)
								numtemp += getall(tnum,s,*tx);
							gen(red, "0", "0", numtemp);
							getsym();
						}
						else error(116);
					}
					else error(110);
				}
				else error(105);

			} while (sym == comma);
			
			if (sym != rparen) error(113); else getsym();
			if (sym != semicolon) error(115); else getsym();
		}
		else if (sym == printfsym)
		{
			getsym();
			if (sym != lparen) error(111); else getsym();
			if (sym == stringc)
			{
				gen(prt, "0", "0", stable);
				getsym();
				if (sym == comma)
				{
					getsym();
					expression(tnum,s,*tx);
					gen(prt, "0", "0", "$t"+convert(tnum));

				}
				 if (sym == rparen)
					getsym();
				else error(113);		
			}
			else
			{
				expression(tnum,s,*tx);
				gen(prt, "0", "0", "$t" + convert(tnum));
				if (sym == rparen)
					getsym();
				else error(113);
			}
			if (sym != semicolon) error(115); else getsym();
		}
		else if (sym == switchsym)
		{
			getsym();
			if (sym != lparen) error(111); else getsym();
			expression(1,s,*tx);
			if (sym != rparen) error(113); else getsym();
			if (sym != lbrace) error(109); else getsym();
			numtemp = getnum();
			while (sym == casesym)
			{
				getsym();
				
				if (sym == number)
				{
					
					numtemp2 = getnum();
					gen(mov, convert(num), " ", "$t2");
					gen(ieq, "$t1", "$t2", numtemp2);
					

				}
				else if (sym == charc)
				{
					
					numtemp2 = getnum();
					gen(mov, convert(cht), " ", "$t2");
					gen(ieq, "$t1", "$t2", numtemp2);
				}
				else error(117);
				getsym();
				if (sym != colon) error(118); getsym();
				statement(tx, s, blockset, 0);
				gen(jmp, "0", "0", numtemp);
				gen(sig, "0", "0", numtemp2);
			}
			gen(sig, "0", "0", numtemp);
			if (sym != rbrace) error(106); else getsym();
		}
		else if (sym == returnsym)
		{
			getsym();
			if (sym == lparen)
			{
				getsym();
				expression(tnum, s, *tx);
				i = position(s,*tx,s);
				if (table[i].typ == functionv)
					error(119);
				else gen(ret, "0","0","$t" + convert(tnum));
				if (sym != rparen)
					error(113); else getsym();
				if (sym != semicolon) error(115); else getsym();


			}else if (sym == semicolon)
				gen(ret, "0", "0", "0");
			else error(108);
		

		}else
		{
			error(120);
			getsym();
		}



	} while (flag == 1 && sym != rbrace);
	
			


}
void blockcomp(int *tx, string s, set<int> blockset)
{
	int typetemp;
	int i,j;
	gen(sig, " ", " ", s);
	i = position(s, *tx, s);
	
	if (sym != lbrace) error(109); else getsym();
	while (sym == constsym)
	{
		getsym();
		if (sym == intsym)
			typetemp = constanti;
		else if (sym == charsym)
			typetemp = constantc;
		else error(121);
		getsym();
		
		while (sym == ident)
		{
			constdeclaration(typetemp, tx, 1);
			while (sym == comma)
			{
				getsym();
				constdeclaration(typetemp, tx, 1);
			}
			

		}
		if (sym == semicolon) getsym();
		else error(115);
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
		}
		if (sym == semicolon) 
			getsym();
		else error(115);

	}

	statement(tx, s, blockset, 1);
	if (sym != rbrace) error(106); else getsym();
	gen(fend, " ", " ", " ");

}
int startcomp(int tx, set<int>fsys)
{
	int tx0, cx0, typetemp,k;
	int temp, cnumtemp;
	set<int> blockset;
	blockset.insert(semicolon);
	blockset.insert(rbrace);
	tx0 = tx;

	gen(jmp, "0", "0", "main");
	getsym();
	test(fsys,  120);
	while (sym == constsym)
	{
		getsym();
		if (sym == intsym)
			typetemp = constanti;
		else if (sym == charsym)
			typetemp = constantc;
		else error(121);
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
			else error(115);

		}
		test(fsys, 121);
	}
	while (sym == intsym || sym == charsym)
	{
		k = sym;
		if (sym == intsym) typetemp = variablei;
		else if (sym == charsym) typetemp = variablec;

		temp = cc;
		getsym();
		getsym();
		if (sym == lparen)
		{
			cc = temp;
			ch = ' ';
			sym = k;
			break;
		}
		else{
			cc = temp;
			ch = ' ';
		}

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
			else error(115);

		}
		test(fsys, 121);
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
			if (sym != lparen) error(111);
			getsym();
			if (sym != rparen) error(113);
			getsym();
			blockcomp(&tx, "main", blockset);
			return tx;
		}
		if (sym == ident)

			if (position(id, tx, " ") != 0)
			{
			error(123);
			test(fsys,  121);
			continue;
			}
			else
			{
				cnumtemp = tx;
				enter(typetemp, &tx, 0, 1);
			}

		else { error(105); test(fsys, 121); continue; }

		getsym();
		if (sym != lparen) error(111);
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
		if (sym != rparen) error(113); else
			getsym();
		table[cnumtemp].size = tx - cnumtemp-1 ;
		blockcomp(&tx, table[cnumtemp].name, blockset);
	}
	
}
//===================================================优化================================================================================
//================================================转换为x86代码==========================================================================
int search(string fname, string s,int tx)
{
	int i,k,j,t=4;
	string t1, t2;
	i = position(fname,tx,fname);	
	for (k = 0; k < s.length; k++)
		if (s[k] == '[')break;
	if (k == s.length)
	{
		k = position(s, tx,fname);
		if (k < i) return -1;
	}
	else
	{
		t1 = s.substr(0, k);
		t2 = s.substr(k + 1, s.length - k - 2);
		t = atoi(t2.c_str)*4;
	}
	k = -4 * table[i].size;
	j = i + 1;
	while (table[j].name != t1)
	{
		if (table[j].size == 0)
			k += 4; else k += table[j].size * 4;
		j++;
	}
	return k + t;
}
int cachen(string s)
{
	return atoi(s.substr(2, s.length - 2).c_str);
}
void x86function(int tx,int *beg,string s)
{
	int i,cachenum=0,k;
	int j,temp=0;
	i = *beg+1;
	j = i;
	while (code[j].op != fend)
	{
		if (code[j].a[0] == '$' && cachen(code[j].a) > cachenum) cachenum = cachen(code[j].a);
		if (code[j].b[0] == '$' && cachen(code[j].b) > cachenum) cachenum = cachen(code[j].b);
		if (code[j].c[0] == '$' && cachen(code[j].c) > cachenum) cachenum = cachen(code[j].c);
		j++;
	}

	x86c << "sub esp," << 4 * cachenum << endl;

	while (1)
	{
		if (code[i].op == fend)
			break;
		else if (code[i].op == ret)
		{
			
			if (code[i + 1].op == fend) { i++; break; }
			x86c << "mov esp,ebp" << endl;
			x86c << "pop ebp" << endl;
			x86c << "ret" << endl;
		}
		else if (code[i].op == sig)
			x86c << "sig_" << code[i].signal << ':' ;
		else if (code[i].op == jmp)
			x86c << "jmp sig_" << code[i].signal << endl;
		else if (code[i].op == eqa)
		{
			
			x86c << "cmp eax,ebx" << endl;
			x86c << "je " << code[i].c << endl;
		}
		else if (code[i].op == ieq)
		{
			
			x86c << "cmp eax,ebx" << endl;
			x86c << "jne " << code[i].c << endl;
		}
		else if (code[i].op == gt)
		{
			
			x86c << "cmp eax,ebx" << endl;
			x86c << "jg " << code[i].c << endl;
		}
		else if (code[i].op == gte)
		{
			
			x86c << "cmp eax,ebx" << endl;
			x86c << "jge " << code[i].c << endl;
		}
		else if (code[i].op == lt)
		{
			
			x86c << "cmp eax,ebx" << endl;
			x86c << "jl " << code[i].c << endl;
		}
		else if (code[i].op == lte)
		{
			
			x86c << "cmp eax,ebx" << endl;
			x86c << "jle " << code[i].c << endl;
		}
		else if (code[i].op == prt)
		{
			
			x86c << "invoke crt_printf, CTXT(\"%x\"),eax" << endl;
		}
		else if (code[i].op == red)
		{
			k = search(s, code[i].c, tx);
			if (k = -1)
				x86c << "invoke crt_scanf,CTXT(\"%x\"),addr " << code[i].c << endl;
			else if (k <= 0)
				x86c << "invoke crt_scanf,CTXT(\"%x\"),addr dword ptr [ebp" << k - 8 << ']' << endl;
			else
				x86c << "invoke crt_scanf,CTXT(\"%x\"),addr dword ptr [ebp+" << k  << ']' << endl;
		}
		else if (code[i].op == cal)
		{
			x86c << "mov dword ptr [esp], eax" << endl;
			x86c << "mov dword ptr [esp+4], ecx" << endl;
			x86c << "cal " << code[i].c << endl;
			x86c << "mov dword ptr [esp+" << (cachen(code[i + 1].c) - 1) * 4 << "],eax" << endl;
			x86c << "mov eax,dword ptr [esp] " << endl;
			x86c << "mov ecx,dword ptr [esp+4]" << endl;
			i++;

		}
		else if (code[i].op == mov)
		{
			if (code[i].c[0] == '$' && code[i].c[1] == 'c')
			{
				k = cachen(code[i].c);
				if (k > temp) temp = k;
				if (cachen(code[i].a) > 2)
				{
					x86c << "mov edx,dword ptr [esp+" << (cachen(code[i].a) - 1) * 4 << ']' << endl;
					x86c << "mov dword ptr [esp-" << k * 4 << "],edx" << endl;
				}
				else if (cachen(code[i].a) == 2)
					x86c << "mov dword ptr [esp-" << k * 4 << "],ecx" << endl;

				else
					x86c << "mov dword ptr [esp-" << k * 4 << "],eax" << endl;

				if (k == 1)
				{
					x86c << "sub esp," << k * 4 << endl;
					temp = 0;
				}
			}
			else
			{
				if (code[i].a[0] == '$')
				{
					k = cachen(code[i].a);
					if (k > 2)
						x86c << "mov edx,dword ptr [esp+" << (k - 1) * 4 << ']' << endl;
					else if (k == 2)
						x86c << "mov edx,ecx" << endl;
					else x86c << "mov edx,eax" << endl;
				}
				else if (code[i].a[0] >= '0' && code[i].a[0] <= '9')
					x86c << "mov edx," << code[i].a[0] << endl;
				else
				{
					k = search(s, code[i].a, tx);
					if (k = -1)
						x86c << "mov edx," << code[i].a << endl;
					else if (k <= 0)
						x86c << "mov edx, dword ptr [ebp" << k - 8 << ']' << endl;
					else
						x86c << "mov edx,dword ptr [ebp+" << k << ']' << endl;
				}

				
			}
		}
				
		
		
		i++;
	}
	x86c << "mov esp,ebp" << endl;
	x86c << "pop ebp" << endl;
	x86c << "ret" << endl;
	*beg = i;
}
void x86convert(int tx)
{
	int i,j,k,total;
	int temp;
	x86c << ".386" << endl;
	x86c << ".Model Flat, StdCall" << endl;
	x86c << "Option Casemap :None" << endl;
	x86c << "Include \masm32\include\msvcrt.inc" << endl;
	x86c << "Include \masm32\include\windows.inc" << endl;
	x86c << "Include \masm32\include\masm32rt.inc" << endl;
	x86c << "IncludeLib \masm32\lib\msvcrt.lib" << endl;
	x86c << ".data" << endl;
	i = 1;
	while (i < tx)
	{
		if (table[i].typ >= functioni&& table[i].typ <= functionv) break;
		if (table[i].typ == variablec || table[i].typ == variablei)		
			if (table[i].size == 0)		
				x86c << table[i].name << " dword ?" << endl;		
			else		
				x86c << table[i].name << " dword " <<table[i].size<<" DUP(?)"<< endl;
			
		
		i++;
	}
	x86c << ".code" << endl;
	j = 1;
	while (j < cx)
	{
		if (code[j].op == sig)
		{
			
			k = position(code[j].signal, tx, code[j].signal);
			if (code[j].signal == "main")
				x86c << "strat:" << endl;
			else
			x86c << code[j].signal << " proc" << endl;
			x86c << "push ebp" << endl;
			x86c << "mov ebp,esp" << endl;
			i = k+1;
			total = -4 * table[k].size;
			while (i < tx && table[i].typ < functioni)
			{
				if (table[i].typ == variablec || table[i].typ == variablei)
					if (table[i].size == 0)
						total += 4; else total += 4 * table[i].size;

				i++;
			}
			
			x86c << "sub esp," << total << endl;
			temp = j;
			x86function(tx,&j,code[j].signal);

			if (code[temp].signal == "main")
				x86c << "end start" << endl;
			else
			x86c << code[temp].signal << " endp" << endl;
		}
		j++;
	}
}
int _tmain(int argc, _TCHAR* argv[])
{
	string s, s1;
	string den[42], den2[42];
	set <int> temp;
	int i;
	
	temp.insert(constsym);
	temp.insert(intsym);
	temp.insert(charsym);
	temp.insert(voidsym);

	cout << "Input source program file name" << endl;
	cin >> s;
	fileIn.open(s);
	fout.open("12061014_token.txt");
	x86c.open("x86code.asm");
	fcode[0] = "add"; fcode[1] = "sub"; fcode[2] = "mul";
	fcode[3] = "div"; fcode[4] = "mov"; fcode[5] = "jz";
	fcode[6] = "jmp"; fcode[7] = "cal"; fcode[8] = "ret";
	fcode[9] = "red"; fcode[10] = "prt"; fcode[11] = "sig";
	fcode[12] = "eqa"; fcode[13] = "leq"; fcode[14] = "gt";
	fcode[15] = "gte"; fcode[16] = "lt"; fcode[17] = "lte";
	fcode[18] = "opn";

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
	den[39] = "QMARK"; den[40] = "DQMARK"; den[41] = "COLON";

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
	den2[40] = "\""; den2[41] = ":";

	wsym[0] = ifsym;	wsym[1] = thensym;	wsym[2] = elsesym;
	wsym[3] = whilesym;	wsym[4] = constsym;	wsym[5] = scanfsym;
	wsym[6] = printfsym;	wsym[7] = returnsym;	wsym[8] = intsym;
	wsym[9] = charsym;	wsym[10] = switchsym;	wsym[11] = casesym;
	wsym[12] = voidsym;	wsym[13] = mainsym; 

	word[0] = "if";	word[1] = "then";	word[2] = "else";
	word[3] = "while";	word[4] = "const";	word[5] = "scanf";
	word[6] = "printf";	word[7] = "return";	word[8] = "int";
	word[9] = "char";	word[10] = "switch";	word[11] = "case";
	word[12] = "void";	word[13] = "main";

	ssym['+'] = plus;	ssym['-'] = minus;	ssym['*'] = times;
	ssym['/'] = slash;	ssym['('] = lparen;	ssym[')'] = rparen;
	ssym['='] = becomes;	ssym[','] = comma;	ssym['.'] = period;
	ssym['<'] = lss;	ssym['>'] = gtr;	ssym[';'] = semicolon;
	ssym['{'] = lbrace;	ssym['}'] = rbrace; ssym['['] = lbracket;
	ssym[']'] = rbracket; ssym['\''] = qmark; ssym['"'] = dqmark;
	ssym[':'] = colon;

	i=startcomp(1, temp);
	
	/*while (1)
	{
		getsym();
		if (sym == 1)
		{
			fout << "IDEN" << " " << id << endl;
			continue;
		}
		else if (sym == 2)
		{
			fout << "INTCON" << " " << num << endl;
			continue;

		}
		else if (sym == 37)
		{
			fout << "CHARCON" << " " << cht << endl;
			continue;
		}
		else if (sym == 38)
		{
			fout << "STRCON" << " " << stable << endl;
			continue;
		}
		fout << den[sym] << ' ' << den2[sym] << endl;
	}*/
	
	return 0;
}
