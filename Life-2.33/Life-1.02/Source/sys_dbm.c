/* Copyright by Denys Duchier, Jul 1995
   Simon Fraser University

  DBM INTERFACE
  */
/*	$Id: sys_dbm.c,v 1.4 1996/01/17 00:34:34 duchier Exp $	*/

#include "extern.h"
#include "built_ins.h"
#include "modules.h"
#include "sys.h"
#include <dbm.h>

static long
dbminit_internal(args,result,funct)
     ptr_psi_term args[],result,funct;
{
  if (dbminit((char*)args[0]->value)<0) return FALSE;
  else {
    unify_bool_result(result,TRUE);
    return TRUE;
  }
}

static long
c_dbminit()
{
  psi_arg args[1];
  SETARG(args,0,"1",quoted_string,REQUIRED);
  return call_primitive(dbminit_internal,NARGS(args),args,0);
}

static long
dbmfetch_internal(args,result,funct)
     ptr_psi_term args[],result,funct;
{
  datum d;
  d.dptr  = (char*)args[0]->value;
  d.dsize = strlen(d.dptr);
  d = fetch(d);
  if (d.dptr == NULL) return FALSE;
  else {
    ptr_psi_term bytes = stack_bytes(d.dptr,d.dsize);
    push_goal(unify,bytes,result,NULL);
    return TRUE;
  }
}

static long
c_dbmfetch()
{
  psi_arg args[1];
  SETARG(args,0,"1",quoted_string,REQUIRED);
  return call_primitive(dbmfetch_internal,NARGS(args),args,0);
}

static long
dbmstore_internal(args,result,funct)
     ptr_psi_term args[],result,funct;
{
  datum key,content;
  key.dptr  = (char*)args[0]->value;
  key.dsize = strlen(key.dptr);
  content.dptr  = (char*)args[1]->value;
  content.dsize = strlen(content.dptr);
  if (store(key,content)<0) return FALSE;
  else return TRUE;
}

static long
c_dbmstore()
{
  psi_arg args[2];
  SETARG(args,0,"1",quoted_string,REQUIRED);
  SETARG(args,1,"2",quoted_string,REQUIRED);
  return call_primitive(dbmstore_internal,NARGS(args),args,0);
}

static long
dbmdelete_internal(args,result,funct)
     ptr_psi_term args[],result,funct;
{
  datum key;
  key.dptr  = (char*)args[0]->value;
  key.dsize = strlen(key.dptr);
  if (delete(key)<0) return FALSE;
  else return TRUE;
}

static long
c_dbmdelete()
{
  psi_arg args[1];
  SETARG(args,0,"1",quoted_string,REQUIRED);
  return call_primitive(dbmdelete_internal,NARGS(args),args,0);
}

static long
dbmfirstkey_internal(args,result,funct)
     ptr_psi_term args[],result,funct;
{
  datum key;
  key=firstkey();
  if (key.dptr==NULL) return FALSE;
  else {
    ptr_psi_term bytes = stack_bytes(key.dptr,key.dsize);
    push_goal(unify,result,bytes,NULL);
    return TRUE;
  }
}

static long
c_dbmfirstkey()
{
  return call_primitive(dbmfirstkey_internal,0,NULL,0);
}

static long
dbmnextkey_internal(args,result,funct)
     ptr_psi_term args[],result,funct;
{
  datum key;
  key.dptr  = (char*)args[0]->value;
  key.dsize = strlen(key.dptr);
  key = nextkey(key);
  if (key.dptr==NULL) return FALSE;
  else  {
    ptr_psi_term bytes = stack_bytes(key.dptr,key.dsize);
    push_goal(unify,result,bytes,NULL);
    return TRUE;
  }
}

static long
c_dbmnextkey()
{
  psi_arg args[1];
  SETARG(args,0,"1",quoted_string,REQUIRED);
  return call_primitive(dbmnextkey_internal,NARGS(args),args,0);
}

void
insert_dbm_builtins()
{
  new_built_in(sys_module,"dbm_init",function,c_dbminit);
  new_built_in(sys_module,"dbm_fetch",function,c_dbmfetch);
  new_built_in(sys_module,"dbm_store",predicate,c_dbmstore);
  new_built_in(sys_module,"dbm_delete",predicate,c_dbmdelete);
  new_built_in(sys_module,"dbm_firstkey",function,c_dbmfirstkey);
  new_built_in(sys_module,"dbm_nextkey",function,c_dbmnextkey);
}
