#ifndef SET_H
#define SET_H

typedef struct snode
{
	int elem;
	struct snode *next;
} snode, *symset;

symset phi, declbegsys, statbegsys, facbegsys;

symset createset(int elem, .../* SYM_NULL */);
void destroyset(symset s);
symset uniteset(symset s1, symset s2);
void setinsert(symset s, int elem);
int inset(int elem, symset s);

#endif
// EOF set.h
