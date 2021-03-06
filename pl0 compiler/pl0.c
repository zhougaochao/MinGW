// pl0 compiler source code

#pragma warning(disable:4996)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "pl0.h"
#include "set.c"

//////////////////////////////////////////////////////////////////////
// print error message.
void error(int n)
{
	int i;
	printf("      ");
	for (i = 1; i <= cc - 1; i++)
		printf(" ");
	printf("^\n");
	printf("Error %3d: %s\n", n, err_msg[n]);
	err++;
} // error

//////////////////////////////////////////////////////////////////////
void getch(void)
{
	if (cc == ll)
	{
		if (feof(infile))
		{
			printf("\nPROGRAM INCOMPLETE\n");
			exit(1);
		}
		ll = cc = 0;
		printf("%5d  ", cx);
		while ((!feof(infile)) && ((ch = getc(infile)) != '\n'))
		{
			// added & modified by alex 01-02-09
			printf("%c", ch);
			line[++ll] = ch;
		} // while
		printf("\n");
		line[++ll] = '\n';
	}
	ch = line[++cc];
} // getch

//////////////////////////////////////////////////////////////////////
// gets a symbol from input stream.
void getsym(void)
{
	int i, k, comment;
	char tmp_ch, a[MAXIDLEN + 1];
	while (ch == ' ' || ch == '\t' || ch == '\n')
		getch();
	while(ch == '/')
	{
		//ignore comments as if they are space, tab or enter marks
		tmp_ch = ch;
		getch();
		if (ch == '*')
		{
			//block comment
			comment = 1;
			do
			{
				do
					getch();
				while(ch != '*');
				getch();
			}
			while(ch != '/');
			comment = 0;
			getch();
			while (ch == ' ' || ch == '\t' || ch == '\n')
				getch();
		}
		else if(ch == '/')
		{
			//line comment
			comment = 1;
			do
				getch();
			while(ch != '\n');
			comment = 0;
			getch();
			while (ch == ' ' || ch == '\t' || ch == '\n')
				getch();
		}
		else
		{
			// this is not a comment, but a slash mark
			cc--;
			ch = tmp_ch;
			break;
		}
	}
	if (isalpha(ch))
	{
		// symbol is a reserved word or an identifier.
		k = 0;
		do
		{
			if (k < MAXIDLEN)
				a[k++] = ch;
			getch();
		}
		while (isalpha(ch) || isdigit(ch));
		a[k] = '\0';
		strcpy(id, a);
		word[0] = id;
		i = NRW;
		while (strcmp(id, word[i--]));
		if (++i)
			sym = wsym[i]; // symbol is a reserved word
		else
			sym = SYM_IDENTIFIER;   // symbol is an identifier
	}
	else if (isdigit(ch))
	{
		// symbol is a number.
		k = num = 0;
		sym = SYM_NUMBER;
		do
		{
			num = num * 10 + ch - '0';
			k++;
			getch();
		}
		while (isdigit(ch));
		if (k > MAXNUMLEN)
			error(25);     // The number is too great.
	}
	else if (ch == ':')
	{
		getch();
		if (ch == '=')
		{
			sym = SYM_BECOMES; // :=
			getch();
		}
		else
			sym = SYM_COLON;   // :, used in ?: mark and statement label (goto)
	}
	else if (ch == '>')
	{
		getch();
		if (ch == '=')
		{
			sym = SYM_GEQ;     // >=
			getch();
		}
		else if (ch == '>')
		{
			getch();
			if (ch == '=')
			{
				sym = SYM_RSHBEC;  // >>=
				getch();
			}
			else
				sym = SYM_RSHIFT;  // >>
		}
		else
			sym = SYM_GTR;     // >
	}
	else if (ch == '<')
	{
		getch();
		if (ch == '=')
		{
			sym = SYM_LEQ;     // <=
			getch();
		}
		else if (ch == '>')
		{
			sym = SYM_NEQ;     // <>
			getch();
		}
		else if (ch == '<')
		{
			getch();
			if (ch == '=')
			{
				sym = SYM_LSHBEC;  // <<=
				getch();
			}
			else
				sym = SYM_LSHIFT;  // <<

		}
		else
			sym = SYM_LES;     // <
	}
	else if (ch == '&')
	{
		getch();
		if (ch == '&')
		{
			sym = SYM_AND;     // &&
			getch();
		}
		else if (ch == '=')
		{
			sym = SYM_BANDBEC; // &=
			getch();
		}
		else
			sym = SYM_BAND;    // &
	}
	else if (ch == '^')
	{
		getch();
		if (ch == '^')
		{
			sym = SYM_XOR;     // ^^
			getch();
		}
		else if (ch == '=')
		{
			sym = SYM_BXORBEC; // ^=
			getch();
		}
		else
			sym = SYM_BXOR;    // ^
	}
	else if (ch == '|')
	{
		getch();
		if (ch == '|')
		{
			sym = SYM_OR;      // ||
			getch();
		}
		else if (ch == '=')
		{
			sym = SYM_BORBEC;  // |=
			getch();
		}
		else
			sym = SYM_BOR;     // |
	}
	else if (ch == '+')
	{
		getch();
		if (ch == '+')
		{
			sym = SYM_INC;      // ++
			getch();
		}
		else if (ch == '=')
		{
			sym = SYM_ADDBEC;  // +=
			getch();
		}
		else
			sym = SYM_PLUS;     // +
	}
	else if (ch == '-')
	{
		getch();
		if (ch == '-')
		{
			sym = SYM_DEC;      // --
			getch();
		}
		else if (ch == '=')
		{
			sym = SYM_SUBBEC;  // -=
			getch();
		}
		else
			sym = SYM_MINUS;   // -
	}
	else if (ch == '*')
	{
		getch();
		if (ch == '=')
		{
			sym = SYM_MULBEC;  // *=
			getch();
		}
		else
			sym = SYM_TIMES;   // *
	}
	else if (ch == '/')
	{
		getch();
		if (ch == '=')
		{
			sym = SYM_DIVBEC;   // /=
			getch();
		}
		else
			sym = SYM_SLASH;    // /
	}
	else if (ch == '%')
	{
		getch();
		if (ch == '=')
		{
			sym = SYM_MODBEC;   // %=
			getch();
		}
		else
			sym = SYM_MOD;      // %
	}
	else
	{
		// other tokens in csym
		i = NSYM;
		csym[0] = ch;
		while (csym[i--] != ch);
		if (++i)
		{
			sym = ssym[i];
			getch();
		}
		else
		{
			printf("Fatal Error: Unknown character.\n");
			exit(1);
		}
	}
} // getsym

//////////////////////////////////////////////////////////////////////
// generates (assembles) an instruction.
void gen(int x, int y, int z)
{
	if (cx > CXMAX)
	{
		printf("Fatal Error: Program too long.\n");
		exit(1);
	}
	code[cx].f = x;
	code[cx].l = y;
	code[cx++].a = z;
} // gen

//////////////////////////////////////////////////////////////////////
// tests if error occurs and skips all symbols that do not belongs to s1 or s2.
void test(symset s1, symset s2, int n)
{
	symset s;
	if (!inset(sym, s1))
	{
		error(n);
		s = uniteset(s1, s2);
		while(!inset(sym, s))
			getsym();
		destroyset(s);
	}
} // test

//////////////////////////////////////////////////////////////////////
int dx;  // data allocation index
int para_offset = -1;
int rtn_offset = -1;  //return offset

// enter object(constant, variable or procedre) into table.
void enter(int kind)
{
	mask *mk;
	tx++;
	strcpy(table[tx].name, id);
	table[tx].kind = kind;
	switch (kind)
	{
		case ID_CONSTANT:
			if (num > MAXADDRESS)
			{
				error(25); // The number is too great.
				num = 0;
			}
			table[tx].value = num;
			break;
		case ID_VARIABLE:
			mk = (mask *) &table[tx];
			mk -> level = level;
			mk -> address = dx++;
			break;
		case ID_FUNCTION:
			mk = (mask *) &table[tx];
			mk -> level = level;
			break;
	} // switch
} // enter

//////////////////////////////////////////////////////////////////////
void enter_para(char *tmp_id, int kind)
{
	mask* mk;
	tx++;
	strcpy(table[tx].name, tmp_id);
	table[tx].kind = kind;
	mk = (mask *) &table[tx];
	mk -> level = level;
	mk -> address = para_offset--;
}

//////////////////////////////////////////////////////////////////////
// locates identifier in symbol table.
int position(char* id)
{
	int i;
	strcpy(table[0].name, id);
	i = tx + 1;
	while (strcmp(table[--i].name, id) != 0);
	return i;
} // position

//////////////////////////////////////////////////////////////////////
void constdeclaration()
{
	if (sym == SYM_IDENTIFIER)
	{
		getsym();
		if (sym == SYM_EQU || sym == SYM_BECOMES)
		{
			if (sym == SYM_BECOMES)
				error(1); // Found ':=' when expecting '='.
			getsym();
			if (sym == SYM_NUMBER)
			{
				enter(ID_CONSTANT);
				getsym();
			}
			else
				error(2); // There must be a number to follow '='.
		}
		else
			error(3); // There must be an '=' to follow the identifier.
	}
	else
		error(4); // There must be an identifier to follow 'const', 'var', or 'function'.
} // constdeclaration

//////////////////////////////////////////////////////////////////////
void vardeclaration(void)
{
	if (sym == SYM_IDENTIFIER)
	{
		enter(ID_VARIABLE);
		getsym();
	}
	else
		error(4); // There must be an identifier to follow 'const', 'var', or 'function'.
} // vardeclaration

//////////////////////////////////////////////////////////////////////
void para_list()
{
	para_offset = -1;
	if(sym == SYM_IDENTIFIER)
	{
		char idTemp[MAXIDLEN + 1];
		strcpy(idTemp, id);
		getsym();
		if(sym == SYM_COMMA)
		{
			getsym();
			para_list();
		}
		enter_para(idTemp, ID_VARIABLE);
	}
} // para_list

//////////////////////////////////////////////////////////////////////
void listcode(int from, int to)
{
	int i;
	printf("\n");
	for (i = from; i < to; i++)
		printf("%5d %s\t%d\t%d\n", i, mnemonic[code[i].f], code[i].l, code[i].a);
	printf("\n");
} // listcode

//////////////////////////////////////////////////////////////////////
void argument_list(symset fsys)
{
	void expression(symset fsys);
	if(sym == SYM_RPAREN)
		return;
	expression(fsys);
	rtn_offset--;
	while (sym == SYM_COMMA)
	{
		getsym();
		expression(fsys);
		rtn_offset--;
	}
} // func_call

//////////////////////////////////////////////////////////////////////
void primary_expr(symset fsys)
{
	void expression(symset fsys);
	int i;
	symset set, set1;
	set1 = createset(SYM_IDENTIFIER, SYM_NUMBER, SYM_LPAREN, SYM_NULL);
	set = uniteset(fsys, set1);
	if (inset(sym, set1))
	{
		if (sym == SYM_IDENTIFIER)
		{
			if ((i = position(id)) == 0)
				error(11); // Undeclared identifier.
			else
			{
				switch (table[i].kind)
				{
						mask* mk;
					case ID_CONSTANT:
						gen(LIT, 0, table[i].value);
						getsym();
						break;
					case ID_VARIABLE:
						mk = (mask *) &table[i];
						gen(LOD, level - mk -> level, mk -> address);
						getsym();
						break;
					case ID_FUNCTION:
						mk = (mask *) &table[i];
						getsym();
						if(sym == SYM_LPAREN)
						{
							getsym();
							gen(LIT, 0, 0); //Stores return value
							para_offset = table[i].value;
							rtn_offset = -1;
							argument_list(fsys);
							if(sym == SYM_RPAREN)
								getsym();
							else
								error(22); //Missing ')'
							gen(CAL, level - mk -> level, mk -> address);
						}
						else
						    error(27); //Missing '('
						break;
				} // switch
			}
		}
		else if (sym == SYM_NUMBER)
		{
			if (num > MAXADDRESS)
			{
				error(25); // The number is too great.
				num = 0;
			}
			gen(LIT, 0, num);
			getsym();
		}
		else if (sym == SYM_LPAREN)
		{
			getsym();
			set = uniteset(set, createset(SYM_RPAREN, SYM_NULL));
			expression(set);
			if (sym == SYM_RPAREN)
				getsym();
			else
				error(22); // Missing ')'.
		}
	}
	else
		error(23);
	destroyset(set);
	destroyset(set1);
} // primary_expr

//////////////////////////////////////////////////////////////////////
void postfix_expr(symset fsys)
{
	int postop;
	symset set, set1;
	set1 = createset(SYM_INC, SYM_DEC, SYM_LPAREN, SYM_NULL);
	set = uniteset(fsys, set1);
	primary_expr(set);
	while (inset(sym, set1))
	{
		postop = sym;
		switch (postop)
		{
			case SYM_INC:
				if (code[cx - 1].f == LOD)//check if the increase oprand has the l-value attribute
				{
					//l-value check pass
					instruction previous_code = code[cx - 1];
					gen(INC, previous_code.l, previous_code.a);
				}
				else //l-value check failed
					error(26);
				break;
			case SYM_DEC:
				if (code[cx - 1].f == LOD) //check if the decrease oprand has the l-value attribute
				{
					//l-value check pass
					instruction previous_code = code[cx - 1];
					gen(DEC, previous_code.l, previous_code.a);
				}
				else //l-value check failed
					error(26);
				break;
			case SYM_LPAREN:
				if (code[cx - 1].f == CAL) //check if the primary expr is a function
				{
					instruction previous_code = code[cx - 1];
				}
				else
					error(31);
				break;
		}
		getsym();
	} // while
	destroyset(set);
	destroyset(set1);
} // postfix_expr

//////////////////////////////////////////////////////////////////////
void unary_expr(symset fsys)
{
	int i, unaryop;
	symset set, set1;
	set1 = createset(SYM_PLUS, SYM_MINUS, SYM_NOT, SYM_BNOT,
	                 SYM_ODD,  SYM_INC,   SYM_DEC, SYM_NULL);
	set = uniteset(fsys, set1);
	if (inset(sym, set1))
	{
		unaryop = sym;
		getsym();
		unary_expr(set);
		switch (unaryop)
		{
			case SYM_PLUS:
				break;
			case SYM_MINUS:
				gen(OPR, 0, OPR_NEG);
				break;
			case SYM_NOT:
				gen(OPR, 0, OPR_NOT);
				break;
			case SYM_BNOT:
				gen(OPR, 0, OPR_BNOT);
				break;
			case SYM_ODD:
				gen(OPR, 0, SYM_ODD);
				break;
			case SYM_INC:
				if (code[cx - 1].f == LOD) //check if the increase oprand has the l-value attribute
				{
					//l-value check pass
					instruction previous_code = code[cx - 1];
					gen(INC, previous_code.l, previous_code.a);
					gen(LOD, previous_code.l, previous_code.a);
				}
				else //l-value check failed
					error(26);
				break;
			case SYM_DEC:
				if (code[cx - 1].f == LOD) //check if the decrease oprand has the l-value attribute
				{
					//l-value check pass
					instruction previous_code = code[--cx];
					gen(DEC, previous_code.l, previous_code.a);
					gen(LOD, previous_code.l, previous_code.a);
				}
				else //l-value check failed
					error(26);
				break;
		}
	}
	else
		postfix_expr(set);
	destroyset(set);
	destroyset(set1);
} // unary_expr

//////////////////////////////////////////////////////////////////////
void multi_expr(symset fsys)
{
	int mulop;
	symset set;
	set = uniteset(fsys, createset(SYM_TIMES, SYM_SLASH, SYM_MOD, SYM_NULL));
	unary_expr(set);
	while (sym == SYM_TIMES || sym == SYM_SLASH || sym == SYM_MOD)
	{
		mulop = sym;
		getsym();
		unary_expr(set);
		if (mulop == SYM_TIMES)
			gen(OPR, 0, OPR_MUL);
		else if (mulop == SYM_SLASH)
			gen(OPR, 0, OPR_DIV);
		else
			gen(OPR, 0, OPR_MOD);
	} // while
	destroyset(set);
} // multi_expr

//////////////////////////////////////////////////////////////////////
void addi_expr(symset fsys)
{
	int addop;
	symset set;
	set = uniteset(fsys, createset(SYM_PLUS, SYM_MINUS, SYM_NULL));
	multi_expr(set);
	while (sym == SYM_PLUS || sym == SYM_MINUS)
	{
		addop = sym;
		getsym();
		multi_expr(set);
		if (addop == SYM_PLUS)
			gen(OPR, 0, OPR_ADD);
		else
			gen(OPR, 0, OPR_MIN);
	} // while
	destroyset(set);
} // addi_expr

//////////////////////////////////////////////////////////////////////
void algebra_expr(symset fsys)
{
	addi_expr(fsys);
} //dummy func algebra_expr

//////////////////////////////////////////////////////////////////////
void shift_expr(symset fsys)
{
	int shiftop;
	symset set;
	set = uniteset(fsys, createset(SYM_LSHIFT, SYM_RSHIFT, SYM_NULL));
	algebra_expr(set);
	while (sym == SYM_LSHIFT || sym == SYM_RSHIFT)
	{
		shiftop = sym;
		getsym();
		algebra_expr(set);
		if (shiftop == SYM_LSHIFT)
			gen(OPR, 0, OPR_LSHIFT);
		else
			gen(OPR, 0, OPR_RSHIFT);
	} // while
	destroyset(set);
} // shift_expr

//////////////////////////////////////////////////////////////////////
void comp_expr(symset fsys)
{
	int compop;
	symset set;
	set = uniteset(fsys, createset(SYM_LES, SYM_LEQ, SYM_GTR, SYM_GEQ, SYM_NULL));
	shift_expr(set);
	if (sym == SYM_LES || sym == SYM_LEQ || sym == SYM_GTR || sym == SYM_GEQ)
	{
		compop = sym;
		getsym();
		shift_expr(fsys);
		switch (compop)
		{
			case SYM_LES:
				gen(OPR, 0, OPR_LES);
				break;
			case SYM_LEQ:
				gen(OPR, 0, OPR_LEQ);
				break;
			case SYM_GTR:
				gen(OPR, 0, OPR_GTR);
				break;
			case SYM_GEQ:
				gen(OPR, 0, OPR_GEQ);
				break;
		} // switch
	} // if
	destroyset(set);
} // comp_expr

///////////////////////////////////////////////
void equ_expr(symset fsys)
{
	int equop;
	symset set;
	set = uniteset(fsys, createset(SYM_EQU, SYM_NEQ, SYM_NULL));
	comp_expr(set);
	if (sym == SYM_EQU || sym == SYM_NEQ)
	{
		equop = sym;
		getsym();
		comp_expr(fsys);
		switch (equop)
		{
			case SYM_EQU:
				gen(OPR, 0, OPR_EQU);
				break;
			case SYM_NEQ:
				gen(OPR, 0, OPR_NEQ);
				break;
		} // switch
	} // if
	destroyset(set);
} // equ_expr

//////////////////////////////////////////////////////////////////////
void rel_expr(symset fsys)
{
	equ_expr(fsys);
} // dummy func rel_expr

//////////////////////////////////////////////////////////////////////
void bit_and_expr(symset fsys)
{
	symset set;
	set = uniteset(fsys, createset(SYM_BAND, SYM_NULL));
	rel_expr(set);
	while (sym == SYM_BAND)
	{
		getsym();
		rel_expr(set);
		gen(OPR, 0, OPR_BAND);
	} // while
	destroyset(set);
} // bit_and_expr

//////////////////////////////////////////////////////////////////////
void bit_xor_expr(symset fsys)
{
	symset set;
	set = uniteset(fsys, createset(SYM_BXOR, SYM_NULL));
	bit_and_expr(set);
	while (sym == SYM_BXOR)
	{
		getsym();
		bit_and_expr(set);
		gen(OPR, 0, OPR_BXOR);
	} // while
	destroyset(set);
} // bit_xor_expr

//////////////////////////////////////////////////////////////////////
void bit_or_expr(symset fsys)
{
	symset set;
	set = uniteset(fsys, createset(SYM_BOR, SYM_NULL));
	bit_xor_expr(set);
	while (sym == SYM_BOR)
	{
		getsym();
		bit_xor_expr(set);
		gen(OPR, 0, OPR_BOR);
	} // while
	destroyset(set);
} // bit_or_expr

//////////////////////////////////////////////////////////////////////
void bit_expr(symset fsys)
{
	bit_or_expr(fsys);
} // dummy func bit_expr

//////////////////////////////////////////////////////////////////////
void and_expr(symset fsys)
{
	symset set;
	set = uniteset(fsys, createset(SYM_AND, SYM_NULL));
	bit_expr(set);
	while (sym == SYM_AND)
	{
		getsym();
		bit_expr(set);
		gen(OPR, 0, OPR_AND);
	} // while
	destroyset(set);
} // and_expr

//////////////////////////////////////////////////////////////////////
void xor_expr(symset fsys)
{
	symset set;
	set = uniteset(fsys, createset(SYM_XOR, SYM_NULL));
	and_expr(set);
	while (sym == SYM_XOR)
	{
		getsym();
		and_expr(set);
		gen(OPR, 0, OPR_XOR);
	} // while
	destroyset(set);
} // xor_expr

//////////////////////////////////////////////////////////////////////
void or_expr(symset fsys)
{
	symset set;
	set = uniteset(fsys, createset(SYM_OR, SYM_NULL));
	xor_expr(set);
	while (sym == SYM_OR)
	{
		getsym();
		xor_expr(set);
		gen(OPR, 0, OPR_OR);
	} // while
	destroyset(set);
} // or_expr

//////////////////////////////////////////////////////////////////////
void bool_expr(symset fsys)
{
	or_expr(fsys);
} // dummy func bool_expr

//////////////////////////////////////////////////////////////////////
void condition_expr(symset fsys)
{
	void expression(symset fsys);
	int cx1;
	symset set;
	set = uniteset(fsys, createset(SYM_QUESTION, SYM_COLON, SYM_NULL));
	bool_expr(set);
	if (sym == SYM_QUESTION)
	{
		cx1 = cx;
		gen(JPC, 0, 0);
		getsym();
		expression(set);
		code[cx1].a = cx + 1;
		cx1 = cx;
		if (sym == SYM_COLON)
		{
			getsym();
			gen(JMP, 0, 0);
			condition_expr(set);
			code[cx1].a = cx;
		}
		else
			error(28);
	}
	destroyset(set);
} //condition_expr

//////////////////////////////////////////////////////////////////////
void assign_expr(symset fsys)
{
	int assignop;
	symset set, set1;
	set1 = createset(SYM_BECOMES, SYM_LSHBEC, SYM_RSHBEC, SYM_BANDBEC, SYM_BXORBEC, SYM_BORBEC,
	                 SYM_ADDBEC,  SYM_SUBBEC, SYM_MULBEC, SYM_DIVBEC,  SYM_MODBEC,  SYM_NULL);
	set = uniteset(fsys, set1);
	condition_expr(set);
	if (inset(sym, set1))
	{
		if (code[cx - 1].f == LOD) //check if the left oprand has the l-value attribute
		{
			//l-value check pass
			instruction previous_code = code[--cx];
			assignop = sym;
			getsym();
			assign_expr(set); // self-recursion
			if (assignop == SYM_BECOMES)
				gen(STO, previous_code.l, previous_code.a);
			else if (assignop == SYM_LSHBEC)
			{
				gen(LOD, previous_code.l, previous_code.a);
				gen(XCHG, 0, 0);
				gen(OPR, 0, OPR_LSHIFT);
				gen(STO, previous_code.l, previous_code.a);
			}
			else if (assignop == SYM_RSHBEC)
			{
				gen(LOD, previous_code.l, previous_code.a);
				gen(XCHG, 0, 0);
				gen(OPR, 0, OPR_RSHIFT);
				gen(STO, previous_code.l, previous_code.a);
			}
			else if (assignop == SYM_BANDBEC)
			{
				gen(LOD, previous_code.l, previous_code.a);
				gen(XCHG, 0, 0);
				gen(OPR, 0, OPR_BAND);
				gen(STO, previous_code.l, previous_code.a);
			}
			else if (assignop == SYM_BXORBEC)
			{
				gen(LOD, previous_code.l, previous_code.a);
				gen(XCHG, 0, 0);
				gen(OPR, 0, OPR_BXOR);
				gen(STO, previous_code.l, previous_code.a);
			}
			else if (assignop == SYM_BORBEC)
			{
				gen(LOD, previous_code.l, previous_code.a);
				gen(XCHG, 0, 0);
				gen(OPR, 0, OPR_BOR);
				gen(STO, previous_code.l, previous_code.a);
			}
			else if (assignop == SYM_ADDBEC)
			{
				gen(LOD, previous_code.l, previous_code.a);
				gen(XCHG, 0, 0);
				gen(OPR, 0, OPR_ADD);
				gen(STO, previous_code.l, previous_code.a);
			}
			else if (assignop == SYM_SUBBEC)
			{
				gen(LOD, previous_code.l, previous_code.a);
				gen(XCHG, 0, 0);
				gen(OPR, 0, OPR_MIN);
				gen(STO, previous_code.l, previous_code.a);
			}
			else if (assignop == SYM_MULBEC)
			{
				gen(LOD, previous_code.l, previous_code.a);
				gen(XCHG, 0, 0);
				gen(OPR, 0, OPR_MUL);
				gen(STO, previous_code.l, previous_code.a);
			}
			else if (assignop == SYM_DIVBEC)
			{
				gen(LOD, previous_code.l, previous_code.a);
				gen(XCHG, 0, 0);
				gen(OPR, 0, OPR_DIV);
				gen(STO, previous_code.l, previous_code.a);
			}
			else if (assignop == SYM_MODBEC)
			{
				gen(LOD, previous_code.l, previous_code.a);
				gen(XCHG, 0, 0);
				gen(OPR, 0, OPR_MOD);
				gen(STO, previous_code.l, previous_code.a);
			}
		}
		else //l-value check failed
			error(26);
	}
	destroyset(set);
	destroyset(set1);
} // assign_expr

//////////////////////////////////////////////////////////////////////
void expression(symset fsys)
{
	assign_expr(fsys);
} //dummy func expression

//////////////////////////////////////////////////////////////////////
void statement(symset fsys)
{
	int i, cx1, cx2;
	symset set1, set;
	if (inset(sym, facbegsys))
	{
		expression(fsys);
		gen(POP, 0, 0);
		//all expressions have a return value and it should be poped when a statement is done.
	}
	else if (sym == SYM_RETURN)
	{
		getsym();
		if (inset(sym, facbegsys))
		{
			expression(fsys);
			if (sym == SYM_SEMICOLON)
				getsym();
			else
				error(10);
		}
		else if (sym == SYM_SEMICOLON)
		{
			gen(LIT, 0, 0);
			getsym();
		}
		else
			error(10);
		gen(RET, 0, 0);
		cx1 = cx;
		gen(JMP, 0, 0);
		statement(fsys);
		code[cx1].a = cx;
	}
	else if (sym == SYM_IF)
	{
		// if statement
		getsym();
		set1 = createset(SYM_ELSE, SYM_NULL);
		set = uniteset(set1, fsys);
		if (sym == SYM_LPAREN)
			expression(set);
		else
			error(27);
		cx1 = cx;
		gen(JPC, 0, 0);
		statement(set);
		code[cx1].a = cx + 1;
		cx1 = cx;
		if (sym == SYM_ELSE)
		{
			getsym();
			gen(JMP, 0, 0);
			statement(set);
			code[cx1].a = cx;
		}
		else
			gen(JMP, 0, cx1 + 1);
		destroyset(set1);
		destroyset(set);
	}
	else if (sym == SYM_BEGIN)
	{
		// block
		getsym();
		set1 = createset(SYM_SEMICOLON, SYM_END, SYM_NULL);
		set = uniteset(set1, fsys);
		statement(set);
		while (sym == SYM_SEMICOLON || inset(sym, statbegsys))
		{
			if (sym == SYM_SEMICOLON)
				getsym();
			else
				error(10);
			statement(set);
		} // while
		destroyset(set1);
		destroyset(set);
		if (sym == SYM_END)
			getsym();
		else
			error(17); // ';' or 'end' expected.
	}
	else if (sym == SYM_WHILE)
	{
		// while statement
		cx1 = cx;
		getsym();
		if (sym == SYM_LPAREN)
			expression(fsys);
		else
			error(27);
		cx2 = cx;
		gen(JPC, 0, 0);
		statement(fsys);
		gen(JMP, 0, cx1);
		code[cx2].a = cx;
	}
	test(fsys, phi, 19);
} // statement

//////////////////////////////////////////////////////////////////////
void block(symset fsys)
{
	int cx0; // initial code index
	mask* mk;
	int block_dx;
	int savedTx;
	symset set1, set;
	dx = 4;
	block_dx = dx;
	mk = (mask *) &table[tx];
	mk -> address = cx;
	gen(JMP, 0, 0);
	if (level > MAXLEVEL)
		error(32); // There are too many levels.
	if(sym == SYM_LPAREN)
	{
		int i;
		getsym();
		para_list();
		if (sym == SYM_RPAREN)
			getsym();
		else
			error(28);
		for (i = tx; table[i].kind != ID_FUNCTION; i--);
			table[i].value = para_offset;
	}
	do
	{
		if (sym == SYM_CONST)
		{
			// constant declarations
			getsym();
			do
			{
				constdeclaration();
				while (sym == SYM_COMMA)
				{
					getsym();
					constdeclaration();
				}
				if (sym == SYM_SEMICOLON)
					getsym();
				else
					error(5); // Missing ',' or ';'.
			}
			while (sym == ID_CONSTANT);
		} // if
		if (sym == SYM_VAR)
		{
			// variable declarations
			getsym();
			do
			{
				vardeclaration();
				while (sym == SYM_COMMA)
				{
					getsym();
					vardeclaration();
				}
				if (sym == SYM_SEMICOLON)
					getsym();
				else
					error(5); // Missing ',' or ';'.
			}
			while (sym == ID_VARIABLE);
		} // if
		block_dx = dx; //save dx before handling function call!
		while (sym == SYM_FUNCTION)
		{
			// function declarations
			getsym();
			if (sym == SYM_IDENTIFIER)
			{
				enter(ID_FUNCTION);
				getsym();
			}
			else
				error(4); // There must be an identifier to follow 'const', 'var', or 'function'.
			level++;
			savedTx = tx;
			set1 = createset(SYM_SEMICOLON, SYM_NULL);
			set = uniteset(set1, fsys);
			block(set);
			destroyset(set1);
			destroyset(set);
			tx = savedTx;
			level--;

			if (sym == SYM_SEMICOLON)
			{
				getsym();
				set1 = createset(SYM_IDENTIFIER, SYM_FUNCTION, SYM_NULL);
				set = uniteset(statbegsys, set1);
				test(set, fsys, 6);
				destroyset(set1);
				destroyset(set);
			}
			else
				error(5); // Missing ',' or ';'.
		} // while
		dx = block_dx; //restore dx after handling function call!
		set1 = createset(SYM_IDENTIFIER, SYM_NULL);
		set = uniteset(statbegsys, set1);
		test(set, declbegsys, 7);
		destroyset(set1);
		destroyset(set);
	}
	while (inset(sym, declbegsys));
	code[mk -> address].a = cx;
	mk -> address = cx;
	cx0 = cx;
	gen(INT, 0, block_dx);
	set1 = createset(SYM_SEMICOLON, SYM_END, SYM_NULL);
	set = uniteset(set1, fsys);
	statement(set);
	destroyset(set1);
	destroyset(set);
	gen(LEAVE, 0, 0); // leave
	test(fsys, phi, 8); // test for error: Follow the statement is an incorrect symbol.
	listcode(cx0, cx);
} // block

//////////////////////////////////////////////////////////////////////
int base(int stack[], int currentLevel, int levelDiff)
{
	int b = currentLevel;
	while (levelDiff--)
		b = stack[b];
	return b;
} // base

//////////////////////////////////////////////////////////////////////
// interprets and executes codes.
void interpret()
{
	int pc;        // program counter
	int stack[STACKSIZE] = {0};
	int top;       // top of stack
	int b;         // program, base, and top-stack register
	instruction i; // instruction register
	printf("Begin executing PL/0 program.\n");
	pc = 0;
	b = 1;
	top = 4;
	stack[4] = -1;
	do
	{
		i = code[pc++];
		switch (i.f)
		{
			case LIT:
				stack[++top] = i.a;
				break;
			case OPR:
				switch (i.a) // operator
				{
					case OPR_NEG:
						stack[top] = 0 - stack[top];
						break;
					case OPR_ADD:
						top--;
						stack[top] += stack[top + 1];
						break;
					case OPR_MIN:
						top--;
						stack[top] -= stack[top + 1];
						break;
					case OPR_MUL:
						top--;
						stack[top] *= stack[top + 1];
						break;
					case OPR_DIV:
						top--;
						if (stack[top + 1] == 0)
						{
							fprintf(stderr, "Runtime Error: Divisor is zero.\n");
							fprintf(stderr, "Program terminated.\n");
							continue;
						}
						stack[top] /= stack[top + 1];
						break;
					case OPR_MOD: // %��notice that MOD is like DIV
						top--;
						if (stack[top + 1] == 0)
						{
							fprintf(stderr, "Runtime Error: Module is zero.\n");
							fprintf(stderr, "Program terminated.\n");
							continue;
						}
						stack[top] %= stack[top + 1];
						break;
					case OPR_ODD:
						stack[top] %= 2;
						break;
					case OPR_LSHIFT:
						top--;
						stack[top] = stack[top] << stack[top + 1];
						break;
					case OPR_RSHIFT:
						top--;
						stack[top] = stack[top] >> stack[top + 1];
						break;
					case OPR_EQU:
						top--;
						stack[top] = stack[top] == stack[top + 1];
						break;
					case OPR_NEQ:
						top--;
						stack[top] = stack[top] != stack[top + 1];
						break;
					case OPR_LES:
						top--;
						stack[top] = stack[top] < stack[top + 1];
						break;
					case OPR_GEQ:
						top--;
						stack[top] = stack[top] >= stack[top + 1];
						break;
					case OPR_GTR:
						top--;
						stack[top] = stack[top] > stack[top + 1];
						break;
					case OPR_LEQ:
						top--;
						stack[top] = stack[top] <= stack[top + 1];
						break;

						//2017.9.29
					case OPR_NOT: // !
						stack[top] = !stack[top];
						break;
					case OPR_AND: // &&
						top--;
						stack[top] = stack[top] && stack[top + 1];
						break;
					case OPR_XOR: // ^^
						top--;
						stack[top] = !(stack[top]) && stack[top + 1] || !(stack[top + 1]) && stack[top];
						break;
					case OPR_OR: // ||
						top--;
						stack[top] = stack[top] || stack[top + 1];
						break;

						//2017.10.10
					case OPR_BNOT: // ~
						stack[top] = ~stack[top];
						break;
					case OPR_BAND: // &
						top--;
						stack[top] = stack[top] & stack[top + 1];
						break;
					case OPR_BXOR: // ^
						top--;
						stack[top] = stack[top] ^ stack[top + 1];
						break;
					case OPR_BOR: // |
						top--;
						stack[top] = stack[top] | stack[top + 1];
						break;
				} // switch
				break;
			case LOD:
				stack[++top] = stack[base(stack, b, i.l) + i.a];
				break;
			case STO:
				stack[base(stack, b, i.l) + i.a] = stack[top];
				printf("%d\n", stack[top]);
				break; // STO doesn't pop the top item of the stack any more.
			case INT:
				top += i.a;
				break;
			case JMP:
				pc = i.a;
				break;
			case JPC:
				if (stack[top--] == 0)
					pc = i.a;
				break;
			case POP: // pop the the top item to recover the stack 2017.10.26
				top--;
				break;
			case XCHG://exchange the top item with the second top item 2017.10.26
				if (stack[top] != stack[top - 1])
				{
					stack[top] ^= stack[top - 1];
					stack[top - 1] ^= stack[top];
					stack[top] ^= stack[top - 1];
				}
				break;
			case INC: // ++
				++stack[base(stack, b, i.l) + i.a];
				break;
			case DEC: // --
				--stack[base(stack, b, i.l) + i.a];
				break;
			case CAL:
				stack[top + 1] = base(stack, b, i.l);
				// generate new block mark
				stack[top + 2] = b;
				stack[top + 3] = pc;
				stack[top + 4] = rtn_offset;
				b = top + 1;
				pc = i.a;
				break;
   			case RET:
   				rtn_offset = stack[b + 3];
				stack[b + rtn_offset] = stack[top];
				break;
			case LEAVE:
				rtn_offset = stack[b + 3];
				top = b + rtn_offset;
				pc = stack[b + 2];
				b = stack[b + 1];
				break;
		} // switch

	}
	while (pc);
	printf("End executing PL/0 program.\n");
} // interpret

//////////////////////////////////////////////////////////////////////
int main (void)
{
	FILE* hbin;
	char s[80] = "example.txt";
	int i;
	symset set, set1, set2;
	/*printf("Please input source file name: "); // get file name to be compiled
	scanf("%s", s);*/
	if ((infile = fopen(s, "r")) == NULL)
	{
		printf("File %s can't be opened.\n", s);
		exit(1);
	}
	phi = createset(SYM_NULL);
	// create begin symbol sets
	declbegsys = createset(SYM_CONST, SYM_VAR, SYM_FUNCTION, SYM_NULL);
	statbegsys = createset(SYM_BEGIN, SYM_IF, SYM_WHILE, SYM_RETURN, SYM_NULL);
	facbegsys = createset(SYM_IDENTIFIER, SYM_NUMBER, SYM_LPAREN,
	                      SYM_PLUS, SYM_MINUS, SYM_NOT, SYM_BNOT, SYM_ODD,
	                      SYM_INC, SYM_DEC, SYM_NULL);
	err = cc = cx = ll = 0; // initialize global variables
	ch = ' ';
	kk = MAXIDLEN;
	getsym();
	set1 = createset(SYM_PERIOD, SYM_NULL);
	set2 = uniteset(declbegsys, statbegsys);
	set = uniteset(set1, set2);
	block(set);
	destroyset(set1);
	destroyset(set2);
	destroyset(set);
	destroyset(phi);
	destroyset(declbegsys);
	destroyset(statbegsys);
	destroyset(facbegsys);
	if (sym != SYM_PERIOD)
		error(9); // '.' expected.
	if (err == 0)
	{
		hbin = fopen("hbin.txt", "w");
		for (i = 0; i < cx; i++)
			fwrite(&code[i], sizeof(instruction), 1, hbin);
		fclose(hbin);
	}
	if (err == 0)
		interpret();
	else
		printf("There are %d error(s) in PL/0 program.\n", err);
	listcode(0, cx);
	system("pause");
	return 0;
} // main

//////////////////////////////////////////////////////////////////////
// eof pl0.c
