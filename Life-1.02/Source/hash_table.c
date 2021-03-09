/******************************* KEYWORDS *************************************
  RM: Feb 3 1993
  
  New version of the keyword table and related routines.

  The keyword table will not NOT be sorted, however, access will be hashed.

  Each module has its own hash table of symbols.

  All definition are stores in a linked list starting at first_definition.
  */
/* 	$Id: hash_table.c,v 1.2 1994/12/08 23:24:09 duchier Exp $	 */

#ifndef lint
static char vcid[] = "$Id: hash_table.c,v 1.2 1994/12/08 23:24:09 duchier Exp $";
#endif /* lint */


#include "extern.h"


ptr_definition first_definition=NULL;

int rand_array[256];



/******** HASH_CREATE(size)
  Create a hash-table for max size keywords.
  */
  
ptr_hash_table hash_create(size)

     int size;
{
  ptr_hash_table new;
  int i;
  
  new=(ptr_hash_table)malloc(sizeof(struct wl_hash_table));
  new->size=size;
  new->used=0;
  new->data=(ptr_keyword *)malloc(size*sizeof(ptr_keyword));
  for(i=0;i<size;i++)
    new->data[i]=NULL;
  return new;
}



/******** HASH_EXPAND(table,new_size)
  Allocate a bigger hash table.
  */

void hash_expand(table,new_size)

     ptr_hash_table table;
     int new_size;
{
  ptr_keyword *old_data;
  int old_size;
  int i;

  
  old_data=table->data;
  old_size=table->size;

  table->size=new_size; /* Must be power of 2 */
  table->used=0;
  table->data=(ptr_keyword *)malloc(new_size*sizeof(ptr_keyword));
  
  for(i=0;i<new_size;i++)
    table->data[i]=NULL;
  
  for(i=0;i<old_size;i++)
    if(old_data[i])
      hash_insert(table,old_data[i]->symbol,old_data[i]);

  free(old_data);
}



/******** HASH_CODE(table,symbol)
  Return the hash code for a symbol
  */

int hash_code(table,symbol)
     
     ptr_hash_table table;
     char *symbol;
{
  int n=0;
  
  /* printf("code of %s ",symbol); */
  
  while(*symbol) {
    n ^= rand_array[*symbol]+rand_array[n&255];
    n++;
    symbol++;
  }
  
  n &= (table->size-1);
  
  
  /* printf("=%d\n",n); */
  
  return n;
}



int hash_find(table,symbol)

     ptr_hash_table table;
     char *symbol;

{
  int n;
  int i=1;
  
  n=hash_code(table,symbol);
  
  while(table->data[n] && strcmp(table->data[n]->symbol,symbol)) {
    /* Not a direct hit... */
    n+= i*i;
    /* i++; */
    n &= table->size-1;
  }
  
  return n;
}



/******** HASH_LOOKUP(table,symbol)
  Look up a symbol in the symbol table.
  */

ptr_keyword hash_lookup(table,symbol)
     
     ptr_hash_table table;
     char *symbol;

{
  int n;

  
  n=hash_find(table,symbol);
  
  /* printf("found %s at %d keyword %x\n",symbol,n,table->data[n]); */
  
  return table->data[n];
}



/******** HASH_INSERT(table,symbol,keyword)
  Add a symbol and data to a table. Overwrite previous data.
  */

void hash_insert(table,symbol,keyword)
     
     ptr_hash_table table;
     char *symbol;
     ptr_keyword keyword;
{
  int n;


  n=hash_find(table,symbol);

  /* printf("inserting %s at %d keyword %x\n",symbol,n,keyword); */
  
  if(!table->data[n])
    table->used++;
  table->data[n]=keyword;
  
  if(table->used*2>table->size)
    hash_expand(table,table->size*2);
}



/******** HASH_DISPLAY(table)
  Display a symbol table (for debugging).
  */

void hash_display(table)
     
     ptr_hash_table table;
     
{
  int i;
  int n;
  char *s;
  int c=0;
  int t=0;
  
  printf("*** Hash table %x:\n",table);
  printf("Size: %d\n",table->size);
  printf("Used: %d\n",table->used);
  
  for(i=0;i<table->size;i++)
    if(table->data[i]) {
      t++;
      s=table->data[i]->symbol;
      n=hash_code(table,s);
      
      printf("%4d %4d %s %s\n",
	     i,
	     n,
	     i==n?"ok   ":"*bad*",
	     s);
      
      if(i!=n)
	c++;
    }
  
  printf("Really used: %d\n",t);
  printf("Collisions: %d = %1.3f%%\n",
	 c,
	 100.0*c/(double)t);
}
