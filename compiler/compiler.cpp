// author:yamanka
//

#include "stdafx.h"
#include <iostream>
#include <set>
#include <fstream>
#include <string>
#include "symbol.h"
#include<strstream>
#include<algorithm>
#include<memory.h>
#include<iterator>
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
	int op;
	int lchild, rchild, father;
	int sign;
	int portal;
} dag;
typedef struct v
{
	int offset;
	string name;
	int name2;
}vv;


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
int yes[1000],f[1000];
dag point[1000];
vv varname[1000];
int ptop = 1, vtop = 1;
int char_or_int_flag=0;
int linenum = 0;
void expression(int, string, int);
int cachen(string);

void error(int n)
{
	cout << "error " << n <<" at line"<<linenum<< endl;
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
		linenum++;
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
			else if (sym == plus || sym == minus)
			{
				int t;
				t = sym;
				getsym();
				if (sym == number)
				{
					if (t == minus)
						num = -num;
					enter(type, tx, 0, level);
					getsym();
				}
				else error(103);
			}else
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
string getall(int num_, string fname, int tx,int *rett)
{
	string s = "";
	int temp,k;
	char c;
	temp = cc;
	c = ch;

	getsym();
	getsym();
	k = sym;
	if (sym == plus || sym == minus)
	{
		int temp1;
		getsym();
		temp1 = sym;
		getsym();
		if (sym == rbracket && temp1 == number)
			if (k == plus)
				*rett = -num;
			else *rett = num;
		else
			*rett = 0;

	}
	else if (sym == number)
	{
		getsym();
		if (sym == rbracket)
			*rett = num;
		else *rett = 0;
	}
	else
		*rett = 0;

	cc = temp;
	ch = c;
	
	getsym();
	if (sym != lbracket)
		error(109); else getsym();
	expression(num_, fname, tx);
	s = s+"[" + "$t" + convert(num_) + "]";
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
				if (char_or_int_flag == 1 && table[i].typ == constanti)
					char_or_int_flag = 0;

				gen(mov, convert(table[i].val), "0", "$t" + convert(tnum));
				getsym();
			}
			else if ((table[i].typ == functionv && flag == 1)|| table[i].typ==functioni || table[i].typ==functionc)
			{
				if (char_or_int_flag > 0)
					char_or_int_flag++;
				getsym();
				if (sym != lparen) error(111); else getsym();
				for (j = 1; j <= table[i].size; j++)
				{
					expression(tnum, fname, tx);
					gen(mov, "$t" + convert(tnum), "0", "$c" + convert(table[i].size-j+1));
					
					if (j < table[i].size)
						if (sym != comma) error(112);
						else getsym();
					
				}
				if (sym != rparen) error(113);
				else getsym();

				gen(cal, "0", "0", table[i].name);
				gen(mov, "$r", "0", "$t" + convert(tnum));
				if (char_or_int_flag > 0)
					char_or_int_flag--;
				if (char_or_int_flag == 1 && table[i].typ == functioni)
					char_or_int_flag = 0;
			}
			else if (table[i].typ == variablec || table[i].typ == variablei)
			{
				if (char_or_int_flag == 1 && table[i].typ == variablei)
					char_or_int_flag = 0;

				if (table[i].size > 0)
				{
					int temp;
					gen(mov, table[i].name + getall(tnum, fname, tx,&temp), "0", "$t" + convert(tnum));
					if (temp >= table[i].size || temp < 0)
						error(135);
					getsym();
				}
				else
				{
					gen(mov, table[i].name, "0", "$t" + convert(tnum));
					getsym();
				}
			}
		
		}
	}
	else if (sym == plus || sym == minus)
	{
		int addop;
		addop = sym;
		getsym();
		if (sym == number)
		
			if (addop == plus)
			{
				gen(mov, convert(num), "0", "$t" + convert(tnum));
				getsym();
			}
			else
			{
				gen(mov, "-" + convert(num), "0", "$t" + convert(tnum));
				getsym();
			}
		
		else
			error(134);
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
		{
			string ss;
			ss = "$t" + convert(num - 1);
			gen(opn, "$t" + convert(num - 1), "0", "$t" + convert(num - 1));
			
		}
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
		tnum = 1;
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
					{
						int temp;
						numtemp += getall(tnum++, s, *tx,&temp);
						if (temp >= table[i].size || temp < 0)
							error(135);
					}

					getsym();
					if (sym == becomes)
						getsym(); else error(104);

					expression(tnum, s, *tx);

					gen(mov, "$t" + convert(tnum), "0", numtemp);


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
							{
								int temp;
								numtemp += getall(tnum, s, *tx,&temp);
								if (temp >= table[i].size || temp <0 )
									error(135);
							}
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
					char_or_int_flag = 1;
					expression(tnum,s,*tx);
					if (char_or_int_flag==0)
					gen(prt, "0", "0", "$t"+convert(tnum));
					else
						gen(prt, "1", "0", "$t" + convert(tnum));
					char_or_int_flag = 0;

				}
				 if (sym == rparen)
					getsym();
				else error(113);		
			}
			else
			{
				char_or_int_flag = 1;
				expression(tnum,s,*tx);
				if (char_or_int_flag == 0)
					gen(prt, "0", "0", "$t" + convert(tnum));
				else
					gen(prt, "1", "0", "$t" + convert(tnum));
				char_or_int_flag = 0;

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
					gen(mov, convert(num), "0", "$t2");
					gen(ieq, "$t1", "$t2", numtemp2);
					

				}
				else if (sym == charc)
				{
					
					numtemp2 = getnum();
					gen(mov, convert(cht), "0", "$t2");
					gen(ieq, "$t1", "$t2", numtemp2);
				}
				else if (sym == minus)
				{
					getsym();
					if (sym == number)
					{
						num = -num;
						numtemp2 = getnum();
						gen(mov, convert(num), "0", "$t2");
						gen(ieq, "$t1", "$t2", numtemp2);
					}
					else error(117);
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



	} while (flag == 1 );
	
			


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
	if (sym == constsym)
		error(136);

	statement(tx, s, blockset, 1);
	if (sym != rbrace) error(106); 
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

	if (sym == constsym)
	{	
		error(136);
		getsym();
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
		getsym();
	}
	
}
//===================================================局部公共子表达式删除================================================================================
int stringFind(string s)
{
	int k;
	for (k = 0; k < s.length(); k++)
		if (s[k] == '[')break;
	return k;
}
void check(int i)
{
	int j, k;
	string s,s2;
	s = varname[i].name;
	k = stringFind(s);
	if (k < s.length())
	{
		varname[i].name = s.substr(0, k);
		s2 = s.substr(k + 1, s.length() - k - 2);
		for (j = 1; j < vtop; j++)
			if (varname[j].name == s2)
				break;
		if (j < vtop)
			varname[i].name2 = varname[j].offset;
		f[varname[j].offset] = ++(point[varname[j].offset].father);
		point[varname[j].offset].sign = 1;
	}
	else
		varname[i].name2 = 0;
}
void release(int i)
{
	int j;
	for (j = 1; j < vtop;j++)
		if (varname[j].name[0] == '$'&& varname[j].offset == i)
		{
		varname[j].name2 = 0;
		varname[j].offset = 0;
		yes[cachen(varname[j].name)] = 0;
		}
}
string apply()
{
	int i;
	i = 1;
	while (1)
	{
		if (yes[i] == 0)
		{
			yes[i] = 1;
			return "$t" + convert(i);
		}
		i++;
	}
}
void optimize(int start, int end)
{
	int i, j, k,m,n;
	int temp, temp1,temp2, temp3;
	int stack[1000],pan[1000];
	int top ,start1;
	if (start >= end)
		return;
	start1 = start;
	top = 0;
	for (i = 0; i < 1000; i++)
	{
		pan[i] = 0;
		yes[i] = 0;
	}
	vtop = 1;
	ptop = 1;
	for (i = start; i <= end; i++)
	{

		temp = 0;
		temp1 = 0;
		temp2 = 0;
		temp3 = 0;

		if (stringFind(code[i].a) < code[i].a.length())
		{
			string s1,s2;
			int flag;
			flag = stringFind(code[i].a);
			s1 = code[i].a.substr(0, flag);
			s2 = code[i].a.substr(flag + 1, code[i].a.length()-flag - 2);
			for (j = 1; j < vtop; j++)
				if (varname[j].name == s2)
				{
				flag = varname[j].offset;
				break;
				}
			for (j = 1; j < vtop; j++)
				if (varname[j].name == s1 && varname[j].name2 == flag)
					temp1 = j;

		}
		if (stringFind(code[i].b) < code[i].b .length())
		{
			string s1, s2;
			int flag;
			flag = stringFind(code[i].b);
			s1 = code[i].b.substr(0, flag);
			s2 = code[i].b.substr(flag + 1, code[i].b.length() - flag - 2);
			for (j = 1; j < vtop; j++)
				if (varname[j].name == s2)
				{
				flag = varname[j].offset;
				break;
				}
			for (j = 1; j < vtop; j++)
				if (varname[j].name == s1 && varname[j].name2 == flag)
					temp2 = j;

		}
		if (stringFind(code[i].c) < code[i].c.length())
		{
			string s1, s2;
			int flag;
			flag = stringFind(code[i].c);
			s1 = code[i].c.substr(0, flag);
			s2 = code[i].c.substr(flag + 1, code[i].c.length() - flag - 2);
			for (j = 1; j < vtop; j++)
				if (varname[j].name == s2)
				{
				flag = varname[j].offset;
				break;
				}
			for (j = 1; j < vtop; j++)
				if (varname[j].name == s1 && varname[j].name2 == flag && point[varname[j].offset].lchild>0)
					temp3 = j;

		}

		for (j = 1; j < vtop; j++)
		{
			if (varname[j].name == code[i].a)
				temp1 = j;
			if (varname[j].name == code[i].b)
				temp2 = j;
			if (varname[j].name == code[i].c)
				temp3 = j;
		}
		if (temp1 == 0)
		{
			point[ptop].op = -1;
			point[ptop].lchild = 0;
			point[ptop].rchild = 0;
			point[ptop].father = 0;
			point[ptop].sign = 0;
			point[ptop].portal = vtop;
			varname[vtop].name = code[i].a;
			check(vtop);
			varname[vtop].offset = ptop;
			temp1 = vtop;
			ptop++;
			vtop++;
		}

		if (temp2 == 0)
		{
			point[ptop].op = -1;
			point[ptop].lchild = 0;
			point[ptop].rchild = 0;
			point[ptop].father = 0;
			point[ptop].sign = 0;
			point[ptop].portal = vtop;
			varname[vtop].name = code[i].b;
			check(vtop);
			varname[vtop].offset = ptop;
			temp2 = vtop;
			ptop++;
			vtop++;
		}
		for (j = 1; j < ptop; j++)
			if (point[j].op == code[i].op && point[j].lchild == varname[temp1].offset && point[j].rchild == varname[temp2].offset)
			{
			temp = j;
			break;
			}
				
		if (temp == 0)
			if (code[i].op == mov && point[varname[temp1].offset].lchild>0)
			{
			temp = varname[temp1].offset;
			}else
		{
			point[ptop].op = code[i].op;
			point[ptop].lchild = varname[temp1].offset;
			point[ptop].rchild = varname[temp2].offset;
			point[ptop].father = 0;
			point[ptop].sign = 0;
			f[point[ptop].lchild]=++(point[point[ptop].lchild].father);
			f[point[ptop].rchild]=++(point[point[ptop].rchild].father);
			temp = ptop;
			ptop++;
		}

		if (temp3 == 0)
		{
			varname[vtop].name = code[i].c;
			check(vtop);
			varname[vtop].offset = temp;
			vtop++;
		}
		else
			varname[temp3].offset = temp;
	}

	for (i = 1; i < vtop; i++)
		if (varname[i].offset == varname[i].name2)
			(point[varname[i].offset].father)--;
	//建树完成
	while (1)
	{
		for (i = 1; i <ptop; i++)
			if (point[i].father == 0 && pan[i]==0)
				break;
		if (i == ptop) break;
		pan[i] = 1;
		if (point[i].lchild > 0)
		{
			stack[top] = i;
			top++;
			(point[point[i].lchild].father)--;
			(point[point[i].rchild].father)--;
			
			for (k = 0; k < vtop; k++)
				if (varname[k].name2>0 && varname[k].offset == i)
					(point[varname[k].name2].father)--;
		}
		if (point[point[i].lchild].op==-1 )         //数组在叶子节点
		{
			i = point[i].lchild;
			for (k = 0; k < vtop; k++)
				if (varname[k].name2>0 && varname[k].offset == i)
					(point[varname[k].name2].father)--;
		}
		if (point[point[i].rchild].op==-1 )
		{
			i = point[i].rchild;
			for (k = 0; k < vtop; k++)
				if (varname[k].name2>0 && varname[k].offset == i)
					(point[varname[k].name2].father)--;
		}
	}
	for (i = 1; i < vtop; i++)
		if (varname[i].name[0] == '$')
			yes[cachen(varname[i].name)] = 1;
	//构造新的表达式
	while (top > 0)
	{
		top--;
		k = stack[top];
		temp1 = point[k].lchild;
		temp2 = point[k].rchild;
		if (point[temp1].op == -1)
		{
			temp1 = point[temp1].portal;
		}
		else
		{
			for (i = 1; i < vtop; i++)
				if (varname[i].offset == temp1 && varname[i].name[0] != '$')
				{
				f[temp1]--;
				if (f[temp1] == 0)
					release(temp1);
				temp1 = i;
				break;
				}
			if (i == vtop)
				for (i = 1; i < vtop; i++)
					if (varname[i].offset == temp1)
					{
				f[temp1]--;
				if (f[temp1] == 0)
					release(temp1);
				temp1 = i;
				break;
					}
		}
		if (point[temp2].op == -1)
		{
			temp2 = point[temp2].portal;
		}
		else
		{
			for (i = 1; i < vtop; i++)
				if (varname[i].offset == temp2 && varname[i].name[0] != '$')
				{
				f[temp2]--;
				if (f[temp2] == 0)
					release(temp2);
				temp2 = i;
				break;
				}
			if (i == vtop)
				for (i = 1; i < vtop; i++)
					if (varname[i].offset == temp2)
					{
				f[temp2]--;
				if (f[temp2] == 0)
					release(temp2);
				temp2 = i;
				break;
					}
		}
		//op处理
		m = 0; n = 0;
		for (i = 1; i < vtop; i++)		
			if (varname[i].offset == k)
				if (varname[i].name[0] == '$')
					m = i;
				else
					n = i;

		if ((m == 0 && point[k].sign == 1)||(n==0 && m==0))
		{
			string s = apply();
			for (i = 1; i < vtop; i++)
				if (varname[i].name == s)
					break;
			if (i < vtop)
				varname[i].offset = k;
			else
			{
				varname[vtop].name = s;
				varname[vtop].name2 = 0;
				varname[vtop].offset = k;
				vtop++;
			}
		}
		
		for (i = 1; i < vtop;i++)
			if (varname[i].name[0] == '$' && varname[i].offset == k)
			{ 
			code[start].op = point[k].op;
			if (varname[temp1].name2 == 0)
				code[start].a = varname[temp1].name;
			else
				for (j = 1; j < vtop; j++)
					if (varname[j].name[0] == '$' && varname[j].offset == varname[temp1].name2)
					{
				code[start].a = varname[temp1].name + '[' + varname[j].name + ']';

				break;
					}
			
			if (varname[temp2].name2 == 0)
				code[start].b = varname[temp2].name;
			else
				for (j = 1; j < vtop; j++)
					if (varname[j].name[0] == '$' && varname[j].offset == varname[temp2].name2)
					{
				code[start].b = varname[temp2].name + '[' + varname[j].name + ']';
				break;
					}

			code[start].c = varname[i].name;
			start++;
			
		}
	
		for (i = 1; i < vtop;i++)
			if (varname[i].name[0] != '$' && varname[i].offset == k)
			{
			code[start].op = point[k].op;

			if (varname[temp1].name2 == 0)
				code[start].a = varname[temp1].name;
			else
				for (j = 1; j < vtop; j++)
					if (varname[j].name[0] == '$' && varname[j].offset == varname[temp1].name2)
					{
				code[start].a = varname[temp1].name + '[' + varname[j].name + ']';

				break;
					}

			if (varname[temp2].name2 == 0)
				code[start].b = varname[temp2].name;
			else
				for (j = 1; j < vtop; j++)
					if (varname[j].name[0] == '$' && varname[j].offset == varname[temp2].name2)
					{
				code[start].b = varname[temp2].name + '[' + varname[j].name + ']';
				break;
					}

			if (varname[i].name2 == 0)
				code[start].c = varname[i].name;
			else		
				for (j = 1; j < vtop; j++)
					if (varname[j].name[0] == '$' && varname[j].offset == varname[i].name2)
					{
					code[start].c = varname[i].name + '[' + varname[j].name + ']';
					break;
					}
			start++;
			}
			
	}
	for (i = start1; i < start;i++)
		cout << fcode[code[i].op] << ' ' << code[i].a << ',' << code[i].b << ',' << code[i].c << endl;
	cout << endl;
	for (i = start; i <= end; i++)
		code[i].op = -1;
}
//================================================冲突图建立=============================================================================
int op_check(string name,int tx,string fname)
{
	int i,n,be;
	if (name[0] == '$') return 0;
	if (stringFind(a) < a.length()) return 0;

	for (i = 1; i < tx; i++)
		if (table[i].name == fname) break;
	n = table[i].size;
	i = i + 1 + n;

	while (i <= tx)
	{
		if (table[i].name == name ) return 1;
		
		if (table[i].typ >= functioni && table[i].typ <= functionv) break;
		i++;
	}
	return 0;
}
void conflict_map(int tx)
{
	int l[100], r[100], behind1[100], behind2[100];
	set<int> def[100], use[100], in[100], out[100];
	int so_called[100];
	string s;
	int i, j, k, n = 0;
	i = 1;
	while (i < cx)
	{
		
		int fop, fen;
		n = 0;
		memset(so_called, 0, sizeof(so_called));

		while (code[i].op != sig) i++;
		j = i;
		while (code[j].op != fend) j++;
		s = code[i].signal;
		fop = i;
		fen = j;
		//划分基本块
		j = fop + 1;
		for (i = fop + 1; i <= fen; i++)
		{
			if ((code[i].op >= 0 && code[i].op <= 4) || code[i].op == opn || code[i].op == red || code[i].op == prt)
				continue;
			if (i == j) continue;
			if (code[i].op == sig)
			{
				l[n] = j;
				r[n] = i - 1;
				j = i;
				n++;
			}
			else
				if (code[i].op == cal)
				{
				l[n] = j;
				r[n] = i - 1;
				n++;
				l[n] = i;
				r[n] = i;
				j = i + 1;
				n++;
				}
				else
				{
					l[n] = j;
					r[n] = i;
					j = i + 1;
					n++;
				}
		}
		//确定后继基本块
		for (i = 0; i < n; i++)
		{
			if (code[r[i]].op == jmp)
			{
				for (j = 0; j < n; j++)
					if (code[l[j]].signal == code[r[i]].c)
						break;
				behind1[i] = j;
				behind2[i] = -1;
			}
			else if (code[r[i]].op >= 12 && code[r[i]].op <= 17)
			{
				for (j = 0; j < n; j++)
					if (code[l[j]].signal == code[r[i]].c)
						break;
				behind1[i] = j;
				if (i + 1 < n)
					behind2[i] = i + 1;
				else behind2[i] = -1;
			}
			else if (code[r[i]].op == ret || code[r[i]].op == fend)
			{
				behind1[i] = -1;
				behind2[i] = -1;
			}
			else
			{
				behind1[i] = -1;
				behind2[i] = i + 1;
			}
		}
		//确定use 和 def集合
		for (i = 0; i < n; i++)
		{
			for (j = l[i]; j <= r[i]; j++)
			{
				if (code[j].op >= 0 && code[j].op <= 3)
				{
					if (op_check(code[j].a, tx, s) == 1 && use[i].find(position(code[j].a, tx, s)) == use[i].end() && def[i].find(position(code[j].a, tx, s)) == def[i].end())
					{
						use[i].insert(position(code[j].a, tx, s));
						so_called[position(code[j].a, tx, s)] = 1;
					}
					if (op_check(code[j].b, tx, s) == 1 && use[i].find(position(code[j].b, tx, s)) == use[i].end() && def[i].find(position(code[j].b, tx, s)) == def[i].end())
					{
						use[i].insert(position(code[j].b, tx, s));
						so_called[position(code[j].b, tx, s)] = 1;
					}
					if (op_check(code[j].c, tx, s) == 1 && use[i].find(position(code[j].c, tx, s)) == use[i].end() && def[i].find(position(code[j].c, tx, s)) == def[i].end())
					{
						def[i].insert(position(code[j].c, tx, s));
						so_called[position(code[j].c, tx, s)] = 1;
					}
				}
				else if (code[j].op == mov || code[j].op == opn)
				{
					if (op_check(code[j].a, tx, s) == 1 && use[i].find(position(code[j].a, tx, s)) == use[i].end() && def[i].find(position(code[j].a, tx, s)) == def[i].end())
					{
						use[i].insert(position(code[j].a, tx, s));
						so_called[position(code[j].a, tx, s)] = 1;
					}
					if (op_check(code[j].c, tx, s) == 1 && use[i].find(position(code[j].c, tx, s)) == use[i].end() && def[i].find(position(code[j].c, tx, s)) == def[i].end())
					{
						def[i].insert(position(code[j].c, tx, s));
						so_called[position(code[j].c, tx, s)] = 1;
					}
				}
				else if (code[j].op == red)
					if (op_check(code[j].c, tx, s) == 1 && use[i].find(position(code[j].c, tx, s)) == use[i].end() && def[i].find(position(code[j].c, tx, s)) == def[i].end())
					{
					def[i].insert(position(code[j].c, tx, s));
					so_called[position(code[j].c, tx, s)] = 1;
					}
					else if (code[j].op >= 12 && code[j].op <= 17)
					{
						if (op_check(code[j].a, tx, s) == 1 && use[i].find(position(code[j].a, tx, s)) == use[i].end() && def[i].find(position(code[j].a, tx, s)) == def[i].end())
						{
							use[i].insert(position(code[j].a, tx, s));
							so_called[position(code[j].a, tx, s)] = 1;
						}
						if (op_check(code[j].b, tx, s) == 1 && use[i].find(position(code[j].b, tx, s)) == use[i].end() && def[i].find(position(code[j].b, tx, s)) == def[i].end())
						{
							use[i].insert(position(code[j].b, tx, s));
							so_called[position(code[j].b, tx, s)] = 1;
						}
					}
					

			}
		}
		//迭代求in out集合
		while (1)
		{
			int flag;
			flag = 1;
			for (i = n - 1; i >= 0; i--)
			{
				set <int> c;
				int temp;
				if (behind2[i] > 0)
					c = in[behind2[i]];
				if (behind1[i] > 0)
					set_union(c.begin(), c.end(), in[behind1[i]].begin(), in[behind1[i]].end(), inserter(out[i], out[i].begin()));
				set_difference(out[i].begin(), out[i].end(), def[i].begin(), def[i].end(), inserter(c, c.begin()));
				temp = in[i].size();
				set_union(c.begin(), c.end(), use[i].begin(), use[i].end(), inserter(in[i], in[i].begin()));
				if (temp != in[i].size())
					flag = 0;

			}
			if (flag == 1) break;
		}
		int color[100];
		
		int map[100][100];
		int stack[100];
		int top = 0;
		set <int> c;
		memset(color, 0, sizeof(color));
	
		memset(map, 0, sizeof(map));
		for (i = 0; i < n; i++)
		{
			set_union(def[i].begin(), def[i].end(), in[i].begin(), in[i].end(), inserter(c, c.begin()));
			in[i] = c;
		}

		for (i = 0; i < n; i++)
			for (set<int>::iterator j = in[i].begin(); j != in[i].end(); j++)
			{
			so_called[*j] = 1;
			for (set<int>::iterator k = in[i].begin(); k != in[i].end(); k++)
				if (j != k)
					map[*j][*k] = 1;
			}
		while (1)
		{
			int flag;
			flag = 0;
			for (i = 0; i < 100; i++)
			{
				int so_num = 0;
				if (so_called[i] == 1)
				{
					if (color[i] != 0) continue;
					flag = 1;
					for (j = 0; j < 100; j++)
						if (map[i][j] == 1 && color[j] == 0)
							so_num++;
					if (so_num < 3)
					{
						color[i] = -1;
						stack[top++] = i;
					}
				}
				
			}
			if (i == 100 && flag == 1)
				for (i = 0; i < 100; i++)
					if (so_called[i] == 1 && color[i] == 0)
					{
				      color[i] = -2;
					  break;
					}
					
			if (i == 100 && flag != 1) break;
		}
		
		while (top > 0)
		{
			int c;
			int te;
			te = stack[--top];
			c = 1;
			for (i = 1; i < 100; i++)
				if (so_called[i] == 1 && map[te][i] == 1 && color[i] == c)
				{
				c++;
				break;
				}
			for (i = 1; i < 100; i++)
				if (so_called[i] == 1 && map[te][i] == 1 && color[i] == c)
				{
				c++;
				break;
				}

			color[te] = c;
		}
		for (i = 1; i < 100; i++)
			if (color[i]>0)
		{
			string s,s1;
			s = table[i].name;
			if (color[i] == 1)
				s1 = "EBX";
			else if (color[i] == 2)
				s1 = "ESI";
			else s1 = "EDI";
			for (j = fop; j <= fen; j++)
			{
				if (code[j].a == s)code[j].a = s1;
				if (code[j].b == s)code[j].b = s1;
				if (code[j].c == s)code[j].c = s1;
			}

		}
		i = fen + 1;
	}
}
//================================================转换为x86代码==========================================================================
int search(string fname, string s,int tx,int *flag)
{
	int i,k,j,t=4;
	string t1;
	i = position(fname,tx,fname);	
	

	for (k = 0; k < s.length(); k++)
		if (s[k] == '[')break;
	
	if (k == s.length())
	{
		*flag = 0;
		t1 = s;
	}
	else
	{
		*flag = k;
		t1 = s.substr(0, k);
	
	}
	k = position(t1, tx, fname);
	if (k < i) return -1;

	k = -4 * table[i].size;
	j = i + 1;
	while (table[j].name != t1)
	{
		if (table[j].size == 0)
			k += 4; else k += table[j].size * 4;
		j++;
	}
	return k + 4;
}
int cachen(string s)
{
	string t;
	t = s.substr(2,s.length() - 2);
	return atoi(t.c_str());
}
void cache_a_c(int tx,string s,string coden,string cache,int total)
{
	int k,flag;
	string t1;
	if (coden == "EBX" || coden == "ESI" || coden == "EDI")
		x86c << "mov  " << cache << ',' << coden << endl;

	else if (coden[0] == '$')
	{
		k = cachen(coden);
		x86c << "mov  " << cache << ',' <<" dword ptr[ebp - " <<total<<'-'<< k  * 4 << ']' << endl;

	}
	else if ((coden[0] >= '0' && coden[0] <= '9')||coden[0]=='-')
		x86c << "mov "<<cache<<',' << coden << endl;
	else
	{
		k = search(s, coden, tx, &flag);
		if (k == -1)
		{ if (flag==0)
			x86c << "mov " << cache << ',' << "var_"<<coden << endl;
		else
		{
			t1 = coden.substr(flag + 1, coden.length() - flag - 2);
			x86c << "mov edx,dword ptr[ebp-" << total<<'-'<<cachen(t1) * 4 << ']' << endl;
			x86c << "sal edx,2" << endl;
			x86c << "mov " << cache << ',' <<"var_"<< coden.substr(0,flag+1) <<"edx]" << endl;
		}
		}
		else if (k <= 0)
			x86c << "mov " << cache << ',' <<" dword ptr[ebp+" <<  20-k << ']' << endl;
		else
		{
			if (flag == 0)
				x86c << "mov " << cache << ',' << "dword ptr[ebp- " << k << ']' << endl;
			else
			{
				t1 = coden.substr(flag + 1, coden.length() - flag - 2);
				x86c << "mov edx,dword ptr[ebp-" << total<<'-'<<cachen(t1) * 4 << ']' << endl;
				x86c << "sal edx,2" << endl;
				x86c << "sub edx,ebp" << endl;
				x86c << "neg edx" << endl;
				x86c << "mov " << cache << ',' << "dword ptr[edx- " << k << "]"<< endl;
			}
		}
	}
}
void mov_d(int i,int tx,string s,int total)
{
	int k, flag;
	string t1;
	if (code[i].c == "EBX" || code[i].c == "ESI" || code[i].c == "EDI")
		x86c << "mov  " << code[i].c << ',' << "eax" << endl;
	else if (code[i].c[0] == '$')
	{
		k = cachen(code[i].c);
		x86c << "mov dword ptr [ebp-" << total<<'-'<<k  * 4 << "],eax" << endl;

	}
	else
	{
		k = search(s, code[i].c, tx, &flag);
		if (k == -1)
		{
			if (flag == 0)
				x86c << "mov " << "var_"<<code[i].c << ",eax" << endl;
			else
			{
				t1 = code[i].c.substr(flag + 1, code[i].c.length() - flag - 2);
				x86c << "mov edx,dword ptr[ebp-" <<total<<'-'<< cachen(t1) * 4 << ']' << endl;
				x86c << "sal edx,2" << endl;
				x86c << "mov var_" << code[i].c.substr(0, flag + 1) << "edx],eax" << endl;
			}
		}
		else if (k <= 0)
			x86c << "mov dword ptr [ebp+" <<  20-k << "],eax" << endl;
		else
		{
			if (flag == 0)
				x86c << "mov dword ptr[ebp-" << k << "],eax" << endl;
			else
			{
				t1 = code[i].c.substr(flag + 1, code[i].c.length() - flag - 2);
				x86c << "mov edx,dword ptr[ebp-" <<total<<'-'<< cachen(t1) * 4 << ']' << endl;
				x86c << "sal edx,2" << endl;
				x86c << "sub edx,ebp" << endl;
				x86c << "neg edx" << endl;
				x86c << "mov dword ptr [edx-" << k << "],eax" << endl;
			}
		}

	}
}
void x86function(int tx,int *beg,string s,int total)
{
	int i,cachenum=0,k,flag;
	string t1, t2;
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
			x86c << "pop edi" << endl;
			x86c << "pop esi" << endl;
			x86c << "pop ebx" << endl;
			j = position(s, tx, s);
			x86c << "ret " <<table[j].size*4<< endl;
		}
		else if (code[i].op == sig)
			x86c << "sig_" << code[i].signal << ':'<<endl ;
		else if (code[i].op == jmp)
			x86c << "jmp sig_" << code[i].c << endl;
		else if (code[i].op == eqa)
		{
			x86c << "mov eax,dword ptr[ebp-4-"<<total<<']' << endl;
			x86c << "mov ecx,dword ptr[ebp-8-"<<total<<']' << endl;
			x86c << "cmp eax,ecx" << endl;
			x86c << "je sig_" << code[i].c << endl;
		}
		else if (code[i].op == ieq)
		{
			x86c << "mov eax,dword ptr[ebp-4-" << total  << ']' << endl;
			x86c << "mov ecx,dword ptr[ebp-8-" << total  << ']' << endl;
			x86c << "cmp eax,ecx" << endl;
			x86c << "jne sig_" << code[i].c << endl;
		}
		else if (code[i].op == gt)
		{
			x86c << "mov eax,dword ptr[ebp-4-" << total  << ']' << endl;
			x86c << "mov ecx,dword ptr[ebp-8-" << total  << ']' << endl;
			x86c << "cmp eax,ecx" << endl;
			x86c << "jg sig_" << code[i].c << endl;
		}
		else if (code[i].op == gte)
		{
			x86c << "mov eax,dword ptr[ebp-4-" << total  << ']' << endl;
			x86c << "mov ecx,dword ptr[ebp-8-" << total  << ']' << endl;
			x86c << "cmp eax,ecx" << endl;
			x86c << "jge sig_" << code[i].c << endl;
		}
		else if (code[i].op == lt)
		{
			x86c << "mov eax,dword ptr[ebp-4-" << total  << ']' << endl;
			x86c << "mov ecx,dword ptr[ebp-8-" << total  << ']' << endl;
			x86c << "cmp eax,ecx" << endl;
			x86c << "jl sig_" << code[i].c << endl;
		}
		else if (code[i].op == lte)
		{
			x86c << "mov eax,dword ptr[ebp-4-" << total  << ']' << endl;
			x86c << "mov ecx,dword ptr[ebp-8-" << total  << ']' << endl;
			x86c << "cmp eax,ecx" << endl;
			x86c << "jle sig_" << code[i].c << endl;
		}
		else if (code[i].op == prt)
		{
			if (code[i].c[0] == '$')
				if (code[i].a=="0")
				x86c << "invoke crt_printf, addr literal(\"%d\"),dword ptr [ebp-4-"<<total<<']' << endl;
				else
					x86c << "invoke crt_printf, addr literal(\"%c\"),dword ptr [ebp-4-"<<total<<']' << endl;
			else
				x86c << "invoke crt_printf,addr literal(\"" << code[i].c << "\")" << endl;
		}
		else if (code[i].op == red)
		{
			if (code[i].c == "EBX" || code[i].c == "ESI" || code[i].c == "EDI")
			{ 
				x86c << "invoke crt_scanf,CTXT(\"%d\")," << "addr [esp-4]"<< endl;
				x86c << "mov " << code[i].c << ",[esp-4]"<<endl;
			}
			else{
				k = search(s, code[i].c, tx, &flag);
				if (k == -1)
				{
					if (flag == 0)
						x86c << "invoke crt_scanf,CTXT(\"%d\"),addr " << "var_" << code[i].c << endl;
					else
					{
						t1 = code[i].c.substr(flag + 1, code[i].c.length() - flag - 2);
						x86c << "mov edx,dword ptr[ebp-" << total << '-' << cachen(t1) * 4 << ']' << endl;
						x86c << "sal edx,2" << endl;
						x86c << "invoke crt_scanf,CTXT(\"%d\"),addr var_" << code[i].c.substr(0, flag + 1) << "edx]" << endl;
					}
				}
				else if (k <= 0)
					x86c << "invoke crt_scanf,CTXT(\"%d\"),addr dword ptr [ebp+" << 20 - k << ']' << endl;
				else{
					if (flag == 0)
						x86c << "invoke crt_scanf,CTXT(\"%d\"),addr dword ptr [ebp-" << k << ']' << endl;
					else
					{
						t1 = code[i].c.substr(flag + 1, code[i].c.length() - flag - 2);
						x86c << "mov edx,dword ptr[ebp-" << total << '-' << cachen(t1) * 4 << ']' << endl;
						x86c << "sal edx,2" << endl;
						x86c << "sub edx,ebp" << endl;
						x86c << "neg edx" << endl;
						x86c << "invoke crt_scanf,CTXT(\"%d\"),addr dword ptr [edx-" << k << endl;
					}
				}
			}
		}
		else if (code[i].op == cal)
		{
			
			x86c << "call " << '_'<<code[i].c << endl;
			x86c << "mov dword ptr [ebp-" <<total<<'-'<< cachen(code[i + 1].c) * 4 << "],eax" << endl;
			i++;

		}
		else if (code[i].op == mov)
		{
			if (code[i].c[0] == '$' && code[i].c[1] == 'c')
			{
							
					x86c << "push dword ptr [ebp-" <<total<<'-'<< cachen(code[i].a)  * 4 << ']' << endl;
					
							
			}
			else
			{//源操作数
				cache_a_c(tx, s, code[i].a, "eax",total);		
				//目标操作数
				mov_d(i,tx,s,total);
			}
		
		}
		else if (code[i].op == add || code[i].op==sub )
		{
			cache_a_c(tx, s, code[i].a, "eax",total);
			cache_a_c(tx, s, code[i].b, "ecx",total);
			if (code[i].op == add)
				x86c << "add eax,ecx" << endl;
			else
				x86c << "sub eax,ecx" << endl;
			mov_d(i, tx, s,total);
		}
		else if (code[i].op == opn)
		{
			cache_a_c(tx, s, code[i].a, "eax",total);
			x86c << "neg eax" << endl;
			mov_d(i, tx, s,total);
		}
		else if (code[i].op == mul)
		{
			cache_a_c(tx, s, code[i].a, "eax",total);
			cache_a_c(tx, s, code[i].b, "ecx",total);
			x86c << "imul ecx" << endl;
			mov_d(i, tx, s,total);
		}
		else if (code[i].op == div)
		{
			cache_a_c(tx, s, code[i].a, "eax",total);
			cache_a_c(tx, s, code[i].b, "ecx",total);
			x86c << "cdq" << endl;
			x86c << "idiv ecx" << endl;
			mov_d(i, tx, s,total);
		}
		

		
				
		
		
		i++;
	}
	x86c << "mov esp,ebp" << endl;
	x86c << "pop ebp" << endl;
	x86c << "pop edi" << endl;
	x86c << "pop esi" << endl;
	x86c << "pop ebx" << endl;
	j = position(s, tx, s);
	x86c << "ret " << table[j].size * 4 << endl;
	*beg = i;
}
void x86convert(int tx)
{
	int i,j,k,total;
	int temp;
	x86c << ".386" << endl;
	x86c << ".Model Flat, StdCall" << endl;
	x86c << "Option Casemap :None" << endl;
	x86c << "Include \\masm32\\include\\msvcrt.inc" << endl;
	x86c << "Include \\masm32\\include\\windows.inc" << endl;
	x86c << "Include \\masm32\\include\\masm32rt.inc" << endl;
	x86c << "IncludeLib \\masm32\\lib\\msvcrt.lib" << endl;
	x86c << ".data" << endl;
	i = 1;
	while (i < tx)
	{
		if (table[i].typ >= functioni&& table[i].typ <= functionv) break;
		if (table[i].typ == variablec || table[i].typ == variablei)		
			if (table[i].size == 0)		
				x86c << "var_"<<table[i].name << " dword ?" << endl;		
			else		
				x86c << "var_"<<table[i].name << " dword " <<table[i].size<<" DUP(?)"<< endl;
			
		
		i++;
	}
	x86c << ".code" << endl;
	j = 1;
	while (j < cx)
	{
		if (code[j].op == sig)
		{
			
			k = position(code[j].signal, tx, code[j].signal);
			
			
			x86c << '_'<<code[j].signal << " proc" << endl;
			x86c << "push ebx" << endl;
			x86c << "push esi" << endl;
			x86c << "push edi" << endl;
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
			x86function(tx,&j,code[j].signal,total);

			
			x86c << '_'<<code[temp].signal <<" endp" << endl;
		}
		j++;
	}
}
void testtest()
{
	set <int> c1, c2,c3;
	c1.insert(1);
	c2.insert(2);
	set_union(c1.begin(), c1.end(), c2.begin(), c2.end(), inserter(c3,c3.begin()));
	cout << c3.size() << endl;
	exit(0);
}
int _tmain(int argc, _TCHAR* argv[])
{
	string s, s1;
	string den[42], den2[42];
	set <int> temp;
	int i,pre,k,j;
	
	temp.insert(constsym);
	temp.insert(intsym);
	temp.insert(charsym);
	temp.insert(voidsym);
	//testtest();
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
	//c0转四元式
	k=startcomp(1, temp);

	//四元式删除局部公共子表达式
	//pre = 0;
	//for (i = 1; i <= cx; i++)
	//	if ((code[i].op > 5 && code[i].op < 18) || code[i].op == 19||(code[i].op==mov && code[i].c[0]=='$' && code[i].c[1]=='c'))
	//	{
	//	optimize(pre + 1, i - 1);
	//	
	//			
	//	pre = i;
	//	if (code[i].op == cal)
	//		pre++;
	//	}

	//for (i = 1; i < cx;i++)
	//	if (code[i].op == -1)
	//	{
	//	j = i;
	//	while (code[j].op == -1 && j <= cx)
	//		j++;
	//	if (j >= cx)
	//	{
	//		pre = i ;
	//		break;
	//	}
	//	code[i].op = code[j].op;
	//	code[i].a = code[j].a;
	//	code[i].b = code[j].b;
	//	code[i].c = code[j].c;
	//	code[i].signal = code[j].signal;
	//	code[j].op = -1;
	//	
	//	}
	//cx = pre;
	///*for (i = 1; i < cx; i++)
	//	cout << code[i].a << ',' << code[i].b << ',' << code[i].c << endl;*/
	//	//建立冲突图并分配全局寄存器
	//conflict_map(k);
		//四元式转x86汇编
	if (err == 0)
		x86convert(k);
	x86c << "end _main" << endl;

	return 0;
}
