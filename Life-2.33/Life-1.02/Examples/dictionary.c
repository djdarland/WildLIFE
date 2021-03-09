/* Copyright 1992 Digital Equipment Corporation
   All Rights Reserved
*/

/* Dictionary program in C. */
/* Author: Peter Van Roy */

#include <stdio.h>
#define FALSE 0
#define TRUE 1

typedef char *string;

typedef struct _base_tree *tree;

typedef struct _base_tree {
    string name, def;
    tree left, right;
} base_tree;

int contains(t,name1,def1)
tree *t;
string *name1, *def1;
{
    if (*t) {
        if (*name1) {
	    int c=strcmp((*t)->name,*name1);
	    if (c<0)
	        return contains(&((*t)->left),  name1, def1);
	    else if (c>0)
	        return contains(&((*t)->right), name1, def1);
	    else {
		if (*def1) {
		    if ((*t)->def)
		        return (strcmp((*t)->def,*def1)==0);
	            else
			(*t)->def = *def1;
	        } else {
                    if ((*t)->def)
			*def1 = (*t)->def;
		    else
			printf("Error: can't handle var-var case\n");
                }
	    }
        } else {
	    *name1= (*t)->name;
	    *def1 = (*t)->def;
        }
    } else {
	(*t) = (tree) calloc(1, sizeof(struct _base_tree));
	(*t)->name = *name1;
	(*t)->def  = *def1;
    }
    return TRUE;
}

main() {
    tree t=NULL;
    string cn="cat", cd="furry feline";
    string dn="dog", dd="furry canine";
    string def=NULL;
    contains(&t, &cn, &cd);
    contains(&t, &dn, &dd);
    contains(&t, &cn, &def);
    printf("A %s is a %s\n",cn,def);
}
