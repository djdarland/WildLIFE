/* 
 * Copyright 1991 Digital Equipment Corporation.
 * All Rights Reserved.
 */
/* 	$Id: list.h,v 1.2 1994/12/08 23:28:39 duchier Exp $	 */


/*
** list.h contains the functions to manage double link list
** with 2 entries (first and last element)
** Links belongs to each atom
*/


#ifndef NULL
#define NULL 0
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

typedef void *			Ref;
typedef struct wl_ListLinks *	RefListLinks;
typedef struct wl_ListHeader *	RefListHeader;
typedef RefListLinks		(*RefListGetLinksProc)	( );
typedef int			(*RefListEnumProc)	( );



/*
  "First", "Last" are pointers to the first and last element of the list
  respectively.
  
  "Current" points to the current processed element of the list. Used when
  applying a function to each element of the list.
  
  "GetLinks" is a function to get the list links on the object.
  
  "Lock" is the number of recursive enum calls on the list. Used only in
  debugging mode.
  */


typedef struct wl_ListHeader
{
  Ref First, Last;

#ifdef prlDEBUG
    Int32			Lock;
#endif

    RefListGetLinksProc		GetLinks;
} ListHeader;



typedef struct wl_ListLinks
{
    Ref Next, Prev;
} ListLinks;



extern void List_SetLinkProc (RefListHeader,RefListGetLinksProc );
extern void List_InsertAhead (RefListHeader, Ref );
extern void List_Append (RefListHeader,Ref );
extern void List_InsertBefore (RefListHeader,Ref,Ref );
extern void List_InsertAfter (RefListHeader,Ref,Ref );
extern void List_Swap (RefListHeader,Ref,Ref );
extern void List_Reverse (RefListHeader);
extern void List_Remove (RefListHeader,Ref );
extern void List_Concat (RefListHeader,RefListHeader );
extern long List_EnumFrom (RefListHeader,Ref,RefListEnumProc,Ref );
extern long List_Enum (RefListHeader,RefListEnumProc,Ref );
extern long List_EnumBackFrom (RefListHeader,Ref,RefListEnumProc,Ref );
extern long List_EnumBack (RefListHeader,RefListEnumProc,Ref );
extern long List_Card (RefListHeader );
extern long List_IsUnlink (RefListLinks );
extern void List_Cut (RefListHeader,Ref,RefListHeader );

/*=============================================================================*/
/*			Get functions	(macros)                               */
/*=============================================================================*/

#define List_First(header) ((header)->First)
#define List_Last(header) ((header)->Last)
#define List_Next(header,RefAtom) ((*(header)->GetLinks)(RefAtom)->Next)
#define List_Prev(header,RefAtom) ((*(header)->GetLinks)(RefAtom)->Prev)
#define List_IsEmpty(header) (List_First(header)==NULL)
