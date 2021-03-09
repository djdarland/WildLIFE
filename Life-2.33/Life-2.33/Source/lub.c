/*! \file lub.c
  \brief least upper bound of the root sorts of two psi terms

  *
  * lub.c - find least upper bound of the root sorts of two psi terms
  *
  */

/*
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

#include "defs.h"

/*! \fn ptr_int_list appendIntList(ptr_int_list tail, ptr_int_list more)
  \brief appendIntList
  \param tail - ptr_int_list tail
  \param more - ptr_int_list more
  
  attach copies of more to tail 
*/

ptr_int_list appendIntList(ptr_int_list tail, ptr_int_list more)
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
/*! \fn void mark_ancestors(ptr_definition def, long *flags)
  \brief mark_ancestors
  \param def - ptr_definition def
  \param flags - long *flags
  
  Set flags bit for all ancestors (i.e., higher up) of head 
*/

void mark_ancestors(ptr_definition def, long *flags)
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

/*! \fn static long bfs(ptr_definition p, ptr_int_list ans, ptr_int_list pattern, long *flags)
  \brief bfs
  \param p - ptr_definition p
  \param ans - ptr_int_list ans
  \param pattern - ptr_int_list pattern
  \param flags - long *flags)
  
*/

static long bfs(ptr_definition p, ptr_int_list ans, ptr_int_list pattern, long *flags)
{
  ptr_int_list head = STACK_ALLOC(int_list);
  ptr_int_list tail;
  ptr_int_list par;
  long len;
  long found = 0;
	
  if (p == top)
    {
      or_codes((ptr_int_list)ans, (ptr_int_list)top);
      return 0; // ADDED 0 DJD 2.05
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

/*! \fn static ptr_int_list makeUnitList(ptr_definition x)
  \brief  makeUnitList
  \param x - ptr_definition x
  
  make a decoded type list from one type 
*/

static ptr_int_list makeUnitList(ptr_definition x)
{
  ptr_int_list ans;

  ans = STACK_ALLOC(int_list);
  ans->value_1 = (GENERIC )x;
  ans->next = NULL;
  return ans;
}

/*! \fn ptr_int_list lub(ptr_psi_term a,ptr_psi_term b,ptr_psi_term pp)
  \brief lub
  \param a - ptr_psi_term a
  \param b - ptr_psi_term b
  \param pp - ptr_psi_term pp
  
  returns a decoded type list of the root sorts that make up the least upper
  bound of the two terms, a &b.  Deals with  speacial cases of integers,
  strings, etc.
*/

ptr_int_list lub(ptr_psi_term a,ptr_psi_term b,ptr_psi_term *pp)
{
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


