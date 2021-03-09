#ifndef C_LIFE_H
#define C_LIFE_H
/* 	$Id: c_life.h,v 1.2 1994/12/08 23:21:00 duchier Exp $	 */

#include <stdio.h>


/* Type for psi-terms, hidden from users */
typedef void *PsiTerm;


/* Replies to queries */

/* Input failed */
#define WFno 0

/* Input succeeded */
#define WFyes  1

/* Input succeeded with possibly more answers */
#define WFmore 2




/* A useful macro for goals which should succeed */

#define WFProve(A) { char *c=(A);if(!WFInput(c)) \
  fprintf(stderr,"%s failed (%s, line %d)\n",c,__FILE__,__LINE__); }



/* Function declarations */

/* Submit a query */
int WFInput(/* char *query */);

/* Get a variable's value */
PsiTerm WFGetVar(/* char *name */);

/* Get the type of a psi-term */
char *WFType(/* PsiTerm psi */);

/* Get the value of a psi-term if it's a double */
double WFGetDouble(/* PsiTerm psi, int *ok */);

/* Get the value of a psi-term if it's a string */
char *WFGetString(/* PsiTerm psi, int *ok */);

/* Count the features of a psi-term */
int WFFeatureCount(/* PsiTerm psi */);

/* Get the value of a feature */
PsiTerm WFGetFeature(/* PsiTerm psi, char *featureName */);

/* Get all the feature names as a NULL-terminated array of strings */
char **WFFeatures(/* PsiTerm psi */);



#endif /* C_LIFE_H */
