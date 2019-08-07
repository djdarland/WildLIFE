/*									tab:4
 *
 * lub.c - find least upper bound of the root sorts of two psi terms
 *
 * Copyright (c) 1992 Digital Equipment Corporation
 * All Rights Reserved.
 *
 * The standard digital prl copyrights exist and where compatible
 * the below also exists.
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  Copyright holder(s) make no
 * representation about the suitability of this software for
 * any purpose. It is provided "as is" without express or
 * implied warranty.
 *
 * Author: 			Seth Copen Goldstein
 * Version:			26
 * Creation Date:	Fri Jun  5 12:14:39 1992
 * Filename:		lub.c
 * History:
 */
/* 	$Id: lub.c,v 1.3 1995/08/25 21:34:37 duchier Exp $	 */

#ifndef lint
static char vcid[] = "$Id: lub.c,v 1.3 1995/08/25 21:34:37 duchier Exp $";
#endif /* lint */

#ifdef REV401PLUS
#include "defs.h"
#endif

ptr_int_list appendIntList(tail, more)
ptr_int_list tail;				/* attach copies of more to tail */
ptr_int_list more;
{
	while (more)
	{
		tail->next = STACK_ALLOC(int_list);
		tail= tail->next;
		tail->value_1 = more->value_1;
		tail->next = NULL;
		more = more->next;
	}
	return tail;
}

/* Set flags bit for all ancestors (i.e., higher up) of head */
void
mark_ancestors(def, flags)
     ptr_definition def;
     long *flags;
{
  ptr_int_list par;
  
  par=def->parents;
  while (par) {
    ptr_definition p;
    long len;
  
    p=(ptr_definition)par->value_1;
    len=bit_length(p->code);
    if (!flags[len]) {
      flags[len]=1;
      mark_ancestors(p, flags);
    }
    par=par->next;
  }
}

static long bfs(p, ans, pattern, flags)
ptr_definition p;
ptr_int_list ans;
ptr_int_list pattern;
long *flags;
{
	ptr_int_list head = STACK_ALLOC(int_list);
	ptr_int_list tail;
	ptr_int_list par;
	long len;
	long found = 0;
	
	if (p == top)
	{
	  or_codes(ans, (ptr_int_list)top);
	  return found;  // REV401PLUS -- added "found"
	}

/*	print_code(pattern);*/
/*	printf("\n");*/

	par = p->parents;
	if (par == NULL)
		return 0;				/* only parent is top */
	
	assert(par->value_1 != NULL);

	head->value_1 = par->value_1;
	head->next  = NULL;
	par = par->next;
	tail = appendIntList(head, par);

	while (head)
	{
/*		pc(head->value);*/
		len = bit_length(((ptr_definition )head->value_1)->code);
		if (!flags[len])
		{
			/* we havn't checked this type before */
			
			if (!((ptr_definition )head->value_1 == top) &&
				!((ptr_definition )head->value_1 == built_in) &&
				(sub_CodeType(pattern,((ptr_definition)head->value_1)->code)))
			{
				or_codes(ans, ((ptr_definition)head->value_1)->code);
/*				print_code(ans);*/
/*				printf("ans\n");*/
				found++;
				/* must set flags of ALL ancestors of head! */
				mark_ancestors((ptr_definition)head->value_1,flags);
			}
			else
				tail = appendIntList(tail,
									 ((ptr_definition )head->value_1)->parents);
			flags[len] = 1;
		}
		head = head->next;
	}
	return found;
}


/******************************************/
/* make a decoded type list from one type */
/******************************************/

static ptr_int_list makeUnitList(x)
ptr_definition x;
{
	ptr_int_list ans;

	ans = STACK_ALLOC(int_list);
	ans->value_1 = (GENERIC )x;
	ans->next = NULL;
	return ans;
}

/*****************************************************************************/
/* returns a decoded type list of the root sorts that make up the least upper
 * bound of the two terms, a &b.  Deals with  speacial cases of integers,
 * strings, etc.
 */
/*****************************************************************************/

ptr_int_list lub(a,b,pp)
ptr_psi_term a;
ptr_psi_term b;
ptr_psi_term *pp;
{
	extern long type_count;		/* the number of sorts in the hierarchy */
	ptr_definition ta;			/* type of psi term a */
	ptr_definition tb;			/* type of psi term b */
	long *flags;					/* set to 1 if this type has been checked in
								 * the lub search.
								 */
	ptr_int_list ans;
	ptr_int_list pattern;
	long found;
	
	ta = a->type;
	tb = b->type;
	
	/* special cases first */
	
	if (isValue(a) && isValue(b) && sub_type(ta,tb) && sub_type(tb,ta))
	{
		/* special case of two values being of same type.  Check that they
		 * might actually be same value before returning the type
		 */
		if (isSubTypeValue(a, b))
		{
			/* since we alreadyuu know they are both values, isSubTypeValue
			 * returns TRUE if they are same value, else false
			 */
			
			*pp = a;
			return NULL;
		}
	}
	
	if (sub_type(ta, tb)) return makeUnitList(tb);
	if (sub_type(tb, ta)) return makeUnitList(ta);

	/* ta has the lub of tb&ta without the high bit set, search upwards for a
	 * type that has the same lower bits as ta
	 */

	/* get the pattern to search for */
	
	pattern = copyTypeCode(ta->code);
	or_codes(pattern, tb->code);		/* pattern to search for */
	ans = copyTypeCode(pattern);		/* resulting pattern */
	
	/* initialize the table to be non-searched */
	
	flags = (long *)stack_alloc(sizeof(unsigned long) * type_count);
	memset(flags, 0, sizeof(unsigned long) * type_count);

	/* now do a breadth first search for each of arg1 and arg2 */

	found  = bfs(ta, ans, pattern, flags);
	found += bfs(tb, ans, pattern, flags);

	if (found)
		ans = decode(ans);
	else
		ans = makeUnitList(top);
	
	return ans;
}


