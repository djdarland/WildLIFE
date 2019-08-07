/* 	$Id: arity.c,v 1.2 1994/12/08 23:03:22 duchier Exp $	 */

#ifndef lint
static char vcid[] = "$Id: arity.c,v 1.2 1994/12/08 23:03:22 duchier Exp $";
#endif /* lint */

static int dummy;

#ifdef ARITY

#include "extern.h"
#include "trees.h"

FILE *features;
static int Aunif=0;
static int Amerge=0;
static int Aadd=0;
static int Atop=0;
static int Atoptop=0;
static int Auid=0;
static int Audiff=0;
static int Anil=0;
static int Anilnil=0;
static int Aident=0;
static int Adiff=0;
static int Aglb=0;


#define PERUNIF(X)  X,100.0*((double)X/(double)Aunif)
#define PERMERGE(X)  X,100.0*((double)X/(double)Amerge)


void arity_init()
{
  /*
  features=fopen("/udir/rmeyer/LIFE/MODULE/features","w");
  if(!features) {
    Errorline("Couldn't open feature log file\n");
    abort_life();
  }
  */
}


void arity_end()
{
  
  /* fclose(features); */
  
  features=fopen("/udir/rmeyer/LIFE/MODULE/profile","w");
  if(features) {
    fprintf(features,"---- Unifications and Features ----\n\n");
    
    fprintf(features,"add feature: %d\n\n",Aadd);
    
    fprintf(features,"unify: %d\n",Aunif);
    fprintf(features,"types:\n");
    fprintf(features,"\ttop-top: %d = %3.1lf\n",PERUNIF(Atoptop));
    fprintf(features,"\ttop-X: %d = %3.1lf\n",PERUNIF(Atop));
    fprintf(features,"\tX-X: %d = %3.1lf\n",PERUNIF(Auid));
    fprintf(features,"\tX-Y: %d = %3.1lf\n",PERUNIF(Audiff));
    fprintf(features,"\tGLB: %d = %3.1lf\n",PERUNIF(Aglb));

    fprintf(features,"merges: %d = %3.1lf\n",PERUNIF(Amerge));

    fprintf(features,"\tnil-nil: %d = %3.1lf\n",PERMERGE(Anilnil));
    fprintf(features,"\tnil-X: %d = %3.1lf\n",PERMERGE(Anil));
    fprintf(features,"\tX-X: %d = %3.1lf\n",PERMERGE(Aident));
    fprintf(features,"\tX-Y: %d = %3.1lf\n",PERMERGE(Adiff));
    
    fclose(features);
  }
}



void rec_print_feat(n)
     
     ptr_node n;
{
  if(n) {
    if(n->left) {
      rec_print_feat(n->left);
      fprintf(features,",");
    }
    
    fprintf(features,n->key);
    
    if(n->right) {
      fprintf(features,",");
      rec_print_feat(n->right);
    }
  }
}



void print_features(u)

     ptr_node u;
{
  fprintf(features,"(");
  rec_print_feat(u);
  fprintf(features,")");
}



int check_equal(u,v)

     ptr_node u;
     ptr_node v;
{
  int same=TRUE;
  
  if(u) {
    same=check_equal(u->left,v) &&
      find(featcmp,u->key,v) &&
	check_equal(u->right,v);
  }

  return same;
}



void arity_unify(u,v)
     ptr_psi_term u;
     ptr_psi_term v;
{
  Aunif++;

  if(u->type==top)
    if(v->type==top)
      Atoptop++;
    else
      Atop++;
  else
    if(v->type==top)
      Atop++;
    else
      if(u->type==v->type)
	Auid++;
      else
	if(u->type->children || v->type->children)
	  Aglb++;
	else
	  Audiff++;

  /*
    fprintf(features,
    "unify: %s %s\n",
    u->type->keyword->symbol,
    v->type->keyword->symbol);
    */
}



void arity_merge(u,v)
     ptr_node u;
     ptr_node v;
{
  Amerge++;
  
  if(u)
    if(v)
      if(check_equal(u,v))
	Aident++;
      else
	Adiff++;
    else
      Anil++;
  else
    if(v)
      Anil++;
    else
      Anilnil++;

  /*
    fprintf(features,"merge: ");
    print_features(u);
    fprintf(features," ");
    print_features(v);
    fprintf(features,"\n");
    */
}



void arity_add(u,s)

     ptr_psi_term u;
     char *s;
{
  Aadd++;

  /*
  fprintf(features,"add %s to %s",s,u->type->keyword->symbol);
  print_features(u->attr_list);
  fprintf(features,"\n");
  */
}

#endif
