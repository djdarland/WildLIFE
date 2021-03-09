/* Copyright by Denys Duchier, Dec 1994
   Simon Fraser University

   All new system utilities and extensions to Wild LIFE 1.02
   are implemented using the new call_primitive interface
   */
/*	$Id: sys.h,v 1.2 1995/07/27 20:16:30 duchier Exp $	*/
#ifndef _LIFE_SYS_H_
#define _LIFE_SYS_H_
#include "extern.h"

/********************************************************************
  When calling a primitive, you always need to process the arguments
  according to the same protocol.  The call_primitive procedure does
  all this work for you.  It should be called as follows:

		       call_primitive(f,n,args,info)

  where f is the primitive implementing the actual functionality, n
  is the number of arguments described in args, and args is an array
  of argument descriptions, and info is a pointer to extra info to be
  passed to f.  Each argument is described by a psi_arg structure
  whose 1st field is a string naming the feature, 2nd field is a type
  restriction, and 3rd field describes processing options, e.g.:

		  { "1" , quoted_string , REQUIRED }

  describes a required argument on feature 1, that must be a string.
  The 3rd field is a mask of boolean flags and is constructed by
  ORing some constants chosen from the set:

  OPTIONAL	for an optional argument
  REQUIRED	for a required argument (i.e. residuate on it if not
  		present
  UNEVALED	if the argument should not be evaluated
  JUSTFAIL	to just fail is the argument does not meet its type
		restriction
  POLYTYPE	sometimes you want to permit several particular sorts
		in that case the 2nd psi_arg field is interpreted as
		a pointer to a NULL terminated array of ptr_definitions
  MANDATORY	like REQUIRED, but it is an error for it not to be
  		present; don't residuate.  This is useful for
		predicates since it doesn't make sense for them to
		residuate.
  NOVALUE	no value required for this argument.

  The primitive must be defined to take the following arguments
  		f(argl,result,funct[,info])
  where argl is an array containing the arguments obtained by call_
  primitive, result is the result in case we are implementing a
  function, and info (optional) is extra information, typically a
  pointer to a structure.
  *******************************************************************/

#define OPTIONAL  0
#define REQUIRED  1
#define UNEVALED  (1<<1)
#define JUSTFAIL  (1<<2)
#define POLYTYPE  (1<<3)
#define MANDATORY (1<<4)
#define NOVALUE   (1<<5)

typedef struct {
  char *feature;
  ptr_definition type;
  unsigned int options;
} psi_arg;

#define SETARG(args,i,the_feature,the_type,the_options) \
  { int j = i; \
    args[j].feature = the_feature; \
    args[j].type    = the_type; \
    args[j].options = the_options; }

#define NARGS(args) (sizeof(args)/sizeof(psi_arg))

extern long call_primitive(long (*fun)(),int num,psi_arg argi[],GENERIC info);

#endif /* _LIFE_SYS_H_ */
