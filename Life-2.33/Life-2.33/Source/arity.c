/*! \file arity.c
  \brief arity - NOT USED

  currently (12/11/2016) DHD ARITY not defined - so ifdef'ed out
*/

#ifdef ARITY

#include "defs.h"

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

/*! \fn void arity_init()
  \brief arity_init

*/

void arity_init()
{
  /*
    features=fopen("features.log","w");
    if(!features) {
    Errorline("Couldn't open feature log file\n");
    abort_life(TRUE);  // djd added TRUE
    }
  */ // RECOMMENTED 2.16 DJD
}

/*! \fn void arity_end()
  \brief void arity_end()

*/

void arity_end()
{
  
  fclose(features); 
  
  features=fopen("profile.log","w");
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

/*! \fn void rec_print_feat(ptr_node n)
  \brief rec_print_feat
  \param ptr_node n

*/

void rec_print_feat(ptr_node n)
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

/*! \fn void print_features(ptr_node u)
  \brief print_features
  \param ptr_node u

*/

void print_features(ptr_node u)
{
  fprintf(features,"(");
  rec_print_feat(u);
  fprintf(features,")");
}

/*! \fn int check_equal(ptr_node u,ptr_node v)
  \brief check_equal
  \param ptr_node u
  \param ptr_node v

*/

int check_equal(ptr_node u,ptr_node v)
{
  int same=TRUE;
  
  if(u) {
    same=check_equal(u->left,v) &&
      find(FEATCMP,u->key,v) &&
      check_equal(u->right,v);
  }

  return same;
}

/*! \fn void arity_unify(ptr_psi_term u,ptr_psi_term v)
  \brief arity_unify
  \param ptr_psi_term u
  \param ptr_psi_term v

*/

void arity_unify(ptr_psi_term u,ptr_psi_term v)
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

/*! \fn void arity_merge(ptr_node u,ptr_node v)
  \brief arity_merge
  \param ptr_node u
  \param ptr_node v

*/

void arity_merge(ptr_node u,ptr_node v)
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

/*! \fn void arity_add(ptr_psi_term u,char *s)
  \brief arity_add
  \param  ptr_psi_term u
  \param char *s

*/

void arity_add(ptr_psi_term u,char *s)
{
  Aadd++;

  /*
    fprintf(features,"add %s to %s",s,u->type->keyword->symbol);
    print_features(u->attr_list);
    fprintf(features,"\n");
  */
}

#endif
