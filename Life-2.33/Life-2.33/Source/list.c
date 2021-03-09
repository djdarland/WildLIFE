/*! \file list.c
  \brief list handling functions

  list.c contains the functions to manage double link list
  with 2 entries (first and last element)
  Links belongs to each atom
*/

/* 
 * Copyright 1991 Digital Equipment Corporation.
 * All Rights Reserved.
 */

#include "defs.h"

/*! \fn void List_SetLinkProc (RefListHeader header, RefListGetLinksProc getLinks)
  \brief List_SetLinkProc 
  \param header - RefListHeader header
  \param getLinks - RefListGetLinksProc getLinks

  Set functions					       
*/

void List_SetLinkProc (RefListHeader header, RefListGetLinksProc getLinks)
{
    header->First = NULL;
    header->Last = NULL;

#ifdef prlDEBUG
    header->Lock = 0;
#endif

    header->GetLinks = getLinks;
}

/*! \fn void List_InsertAhead (RefListHeader header, Ref atom) 
  \brief List_InsertAhead 
  \param header - RefListHeader header
  \param atom - Ref atom 

  List functions					     
*/

void List_InsertAhead (RefListHeader header, Ref atom) 
{
    RefListGetLinksProc  getLinks = header->GetLinks;

        /* Update links of atom to insert */

    (*getLinks)(atom)->Next = header->First;
    (*getLinks)(atom)->Prev = NULL;

        /* Link to the head of list */

    if (header->First != NULL)
      (*getLinks)(header->First)->Prev = atom;

    else	/* The list is empty */
      header->Last  = atom;

    header->First = atom;
}

/*! \fn void List_Append (RefListHeader header, Ref atom) 
  \brief void List_Append 
  \param header - RefListHeader header
  \param atom - Ref atom

*/

void List_Append (RefListHeader header, Ref atom) 
{
    RefListGetLinksProc  getLinks = header->GetLinks;

	        /* Link to the end of list */

    if (header->Last != NULL)
      (*getLinks)(header->Last)->Next = atom;

    else	/* The list is empty */
      header->First = atom;

	        /* Update links of atom to insert */

    (*getLinks)(atom)->Prev = header->Last;
    (*getLinks)(atom)->Next = NULL;

		/* Update last element of header */

    header->Last  = atom;
}

/*! \fn void List_InsertBefore (RefListHeader header, Ref atom, Ref mark)
  \brief List_InsertBefore 
  \param header - RefListHeader header
  \param atom - Ref atom
  \param mark - Ref mark

*/

void List_InsertBefore (RefListHeader header, Ref atom, Ref mark)
{
    RefListGetLinksProc  getLinks = header->GetLinks;

    if (mark != NULL)
    {
        (*getLinks)(atom)->Next = mark;

        if (mark != header->First)
        {
            (*getLinks)(atom)->Prev = (*getLinks)(mark)->Prev;
            (*getLinks)((*getLinks)(mark)->Prev)->Next = atom;
        }
        else	/* Insert ahead the list */
        {
            (*getLinks)(atom)->Prev = NULL;
            header->First = atom;
        }

        (*getLinks)(mark)->Prev = atom;
    }
    else        /* Append to the list */
      List_Append (header, atom);
} 

/*! \fn void List_InsertAfter (RefListHeader header, Ref atom, Ref mark)
  \brief List_InsertAfter 
  \param header - RefListHeader header
  \param atom - Ref atom
  \param mark - Ref mark)
  
*/

void List_InsertAfter (RefListHeader header, Ref atom, Ref mark)
{
    RefListGetLinksProc  getLinks = header->GetLinks;

#ifdef prlDEBUG
    if (header->Lock > 1)
      OS_PrintMessage ("List_InsertAfter: Warning insert after on recursive List_Enum call !!\n");
#endif

    if (mark != NULL)
    {
        (*getLinks)(atom)->Prev = mark;

        if (mark != header->Last)
        {
            (*getLinks)(atom)->Next = (*getLinks)(mark)->Next;
            (*getLinks)((*getLinks)(mark)->Next)->Prev = atom;
        }
        else	/* Insert at the end of the list */
        {
            (*getLinks)(atom)->Next = NULL;
            header->Last = atom;
        }

        (*getLinks)(mark)->Next = atom;
    }
    else        /* Insert ahead the list */
      List_InsertAhead (header, atom);
} 

/*! \fn void List_Swap (RefListHeader header, Ref first, Ref second)
  \brief List_Swap 
  \param header - RefListHeader header
  \param first - Ref first
  \param second - Ref second)
  
*/

void List_Swap (RefListHeader header, Ref first, Ref second)
{
    RefListGetLinksProc	getLinks = header->GetLinks;

	/* Don't swap if the input is wrong */

    if ((*getLinks)(first)->Next != second)
    {
#ifdef prlDEBUG
	OS_PrintMessage ("List_Swap: WARNING wrong input data, swap not done..\n");
#endif
	return;
    }

        /* Special Cases */

    if (header->First == first)
      header->First = second;
    else
      (*getLinks)((*getLinks)(first)->Prev)->Next = second;
    
    if (header->Last == second)
      header->Last = first;
    else
      (*getLinks)((*getLinks)(second)->Next)->Prev = first;

    	/* Swap the atoms */

    (*getLinks)(second)->Prev = (*getLinks)(first)->Prev;
    (*getLinks)(first)->Next  = (*getLinks)(second)->Next;
    (*getLinks)(first)->Prev  = second;
    (*getLinks)(second)->Next = first;
}

/*! \fn static long List_SwapLinks (RefListHeader header, Ref atom)
  \brief List_SwapLinks 
  \param header - RefListHeader header
  \param atom - Ref atom)

*/

static long List_SwapLinks (RefListHeader header, Ref atom)
{
    Ref	save;

    save = (*header->GetLinks)(atom)->Next;
    (*header->GetLinks)(atom)->Next = (*header->GetLinks)(atom)->Prev;
    (*header->GetLinks)(atom)->Prev = save;

    return TRUE;
}

/*! \fn void List_Reverse (RefListHeader header)
\brief List_Reverse 
\param header - RefListHeader header

*/

void List_Reverse (RefListHeader header)
{
    Ref			cur, next;
    RefListGetLinksProc	getLinks = header->GetLinks;

    /* This traverse cannot be done with function List_Enum() */

    cur = header->First;

    /* Swap the headers */
    header->First = header->Last;
    header->Last  = cur;

    while (cur != NULL)
    {
	next = (*getLinks)(cur)->Next;
	(void)List_SwapLinks (header, cur);
	cur = next;
    }
}

/*! \fn void List_Remove (RefListHeader header, Ref atom)
  \brief List_Remove 
  \param header - RefListHeader header
  \param atom - Ref atom)
  
*/

void List_Remove (RefListHeader header, Ref atom)
{
/*-----------------------------------------------------------------------------

WARNING
	- The container is 'updated' two times if the first and last atom
	  of list is the only one to remove.

-----------------------------------------------------------------------------*/

    RefListGetLinksProc  getLinks = header->GetLinks;

#ifdef prlDEBUG
    if (header->Lock > 1)
      OS_PrintMessage ("List_Remove: Warning remove on recursive List_Enum call !!\n");
#endif

        /* Update the DownStream links */

    if ((*getLinks)(atom)->Prev != NULL)
    {
        (*getLinks)((*getLinks)(atom)->Prev)->Next = 
            (*getLinks)(atom)->Next;
    }
    else            /* Atom is the first of list */
      header->First = (*getLinks)(atom)->Next;

        /* Update the UpStream links */

    if ((*getLinks)(atom)->Next != NULL)
    {
        (*getLinks)((*getLinks)(atom)->Next)->Prev = 
            (*getLinks)(atom)->Prev;
    }
    else            /* Atom is the last of list */
      header->Last = (*getLinks)(atom)->Prev;

    	/* Reset the atom links */

    (*getLinks)(atom)->Prev = NULL;
    (*getLinks)(atom)->Next = NULL;
} 

/*! \fn void List_Concat (RefListHeader header1, RefListHeader header2)
  \brief List_Concat 
  \param header1 - RefListHeader header1
  \param header2 - RefListHeader header2
  
*/

void List_Concat (RefListHeader header1, RefListHeader header2)
{
    RefListGetLinksProc  getLinks = header1->GetLinks;

    if (header1->GetLinks == header2->GetLinks)
    {
#ifdef prlDEBUG
	OS_PrintMessage ("List_Concat: ERROR concat different lists\n");
#endif
	return;
    }

	/* Concatenate only if the second list is not empty */

    if (header2->First != NULL)
    {
	/* Obvious concatenate when the first list is empty */

        if (header1->First == NULL)
            header1->First = header2->First;

        else	/* Concatenate the two non empty lists */
        {
            (*getLinks)(header1->Last)->Next  = header2->First;
            (*getLinks)(header2->First)->Prev = header1->Last;
        }
        header1->Last = header2->Last;
    }
} 

/*! \fn long List_EnumFrom (RefListHeader header, Ref atom, RefListEnumProc proc, Ref closure)
  \brief List_EnumFrom 
  \param header -  RefListHeader header
  \param atom - Ref atom
  \param proc - RefListEnumProc proc
  \param closure - Ref closure 

*/

long List_EnumFrom (RefListHeader header, Ref atom, RefListEnumProc proc, Ref closure)
{
    Ref	cur, next;
    int	notInterrupted = TRUE;

#ifdef prlDEBUG
    header->Lock += 1;
#endif

    cur = atom;
    while (cur != NULL && notInterrupted)
    {
	next = List_Next (header, cur);
	notInterrupted = (*proc)(cur, closure);
	cur = next;
    }

#ifdef prlDEBUG
    header->Lock -=1;
#endif
    
    return (notInterrupted);
}

/*! \fn long List_Enum (RefListHeader header, RefListEnumProc proc, Ref closure)
  \brief List_Enum 
  \param header - RefListHeader header
  \param proc - RefListEnumProc proc
  \param closure - Ref closure

*/

long List_Enum (RefListHeader header, RefListEnumProc proc, Ref closure)
/*-----------------------------------------------------------------------------

(NO) SIDE EFFECTS
	The current atom can be modified by the function RemoveAtom () during
	the traversing of the list. This is the reason why the current pointer
	is managed on the header.

-----------------------------------------------------------------------------*/
{
    return (List_EnumFrom (header, header->First, proc, closure));
}

/*! \fn long List_EnumBackFrom (RefListHeader header, Ref atom, RefListEnumProc	proc, Ref closure)
  \brief List_EnumBackFrom
  \param header - RefListHeader header
  \param atom - Ref atom
  \param proc - RefListEnumProc proc
  \param closure - Ref closure)
  
*/

long List_EnumBackFrom (RefListHeader header, Ref atom, RefListEnumProc	proc, Ref closure)
{
    Ref	cur, prev;
    int	notInterrupted = TRUE;

#ifdef prlDEBUG
    header->Lock += 1;
#endif

    cur = atom;
    while (cur != NULL && notInterrupted)
    {
	prev = List_Prev (header, cur);
	notInterrupted = (*proc)(cur, closure);
	cur = prev;
    }

#ifdef prlDEBUG
    header->Lock -=1;
#endif
    
    return (notInterrupted);
}

/*! \fn long List_EnumBack (RefListHeader header, RefListEnumProc proc, Ref closure)
  \brief List_EnumBack 
  \param header - RefListHeader header
  \param proc - RefListEnumProc proc
  \param closure - Ref closure
  
*/

long List_EnumBack (RefListHeader header, RefListEnumProc proc, Ref closure)
{
    return (List_EnumBackFrom (header, header->Last, proc, closure));
}

/*! \fn static long List_CountAtom (Ref p, Ref nbR)
  \brief List_CountAtom 
  \param p - Ref p
  \param nbR - Ref nbR
  
*/

/*ARGSUSED*/
static long List_CountAtom (Ref p, Ref nbR)
{
    long *nb = (long *)nbR;
    
    ++*nb;
    return TRUE;
}

/*! \fn long List_Card (RefListHeader header)
  \brief List_Card 
  \param header - RefListHeader header

*/


long List_Card (RefListHeader header)
{
    long n = 0;
    
    (void)List_Enum (header,(RefListEnumProc) List_CountAtom, &n);
    return n;
}

/*! \fn long List_IsUnlink (RefListLinks links)
  \brief List_IsUnlink 
  \param links - RefListLinks links

*/

long List_IsUnlink (RefListLinks links)
{
    return (links->Next == NULL && links->Prev == NULL);
}

/*! \fn void List_Cut (RefListHeader header, Ref atom, RefListHeader newHeader)
  \brief List_Cut 
  \param header - RefListHeader header
  \param atom - Ref atom
  \param newHeader - RefListHeader newHeader

*/

void List_Cut (RefListHeader header, Ref atom, RefListHeader newHeader)
{
    RefListGetLinksProc  getLinks = header->GetLinks;

    if (atom != List_Last (header))
    {
	newHeader->First = List_Next (header, atom);
	newHeader->Last  = header->Last;

	header->Last = atom;

	/* Update the links */
	(*getLinks)(atom)->Next = NULL;
	(*getLinks)(newHeader->First)->Prev = NULL;
    }
}

/*==============================================================================*/
