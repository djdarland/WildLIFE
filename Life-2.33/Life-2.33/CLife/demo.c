/* Example C program calling Wild-LIFE */
#include "../Source/c_life.h"



main(argc,argv)

     int argc;
     char *argv[];

{
  int ans;
  
  PsiTerm a;
  PsiTerm sum;
  char **features;
  int i;
  double value;
  int ok;
  

  
  /*** Initialize Wild-LIFE ***/
  WFInit(argc,argv); /* Currently doesn't really use the arguments */


  
  /*** Submit a query ***/
  /* \042 is a quote sign (") */
  ans=WFInput("write(\042Welcome to Wild-LIFE!!\042),nl?");
  
  /* Deal with Wild-LIFE response */
  switch(ans) {

  case WFno:
    printf("WFInput failed\n");
    break;
    
  case WFyes :
    printf("WFInput succeeded\n");
    break;

  case WFmore:
    printf("WFInput succeeded and there may be more answers\n");
    break;
  }


  
  /*** This MACRO displays an error message if the query fails ***/
  WFProve("true=false?");


  
  /*** Solve a simple constraint ***/
  WFProve("A=B+C?");
  WFProve("B=1?");
  WFProve("A=5?");
  WFProve("write('C=',C),nl?");
  WFProve(".");


  
  /*** Back-track over 4 solutions ***/
  WFProve("A={1;2;3;4},write(A),nl?");
  WFProve(";");
  WFProve(";");
  WFProve(";");
  WFProve(";"); /* No more at this point */

  
  
  /*** Back-track over only the first 2 solutions ***/
  WFProve("A={6;7;8},write(A),nl?");
  WFProve(";");
  WFProve("."); /* Return to top level */


  
  /*** Build a psi-term and query it ***/
  WFProve("A=message(\042three+four\042,equals => 3+4),write(A),nl?");

  /* Read the variable 'A' */
  a=WFGetVar("A");
  if(!a) { /* Error checking, here for demonstration only */
    fprintf(stderr,"Couldn't read variable 'A'\n");
    exit(1);
  }

  /* Print the type of 'A' */
  printf("The type of A is %s\n",WFType(a));

  /* Get the number of features of 'A' */
  printf("A has %d features\n",WFFeatureCount(a));

  /* Get the feature 'equals' */
  sum=WFGetFeature(a,"equals");
  if(!sum) { /* Error checking, here for demonstration only */
    fprintf(stderr,"Couldn't read feature 'equals'\n");
    exit(1);
  }

  /* Get the value of 'sum' */
  value=WFGetDouble(sum,&ok);
  if(!ok) { /* Error checking, here for demonstration only */
    fprintf(stderr,"'sum' is not a real number\n");
    exit(1);
  }
  printf("sum=%lg\n",value);

  /* Get the first feature */
  /* You can use NULL in WFGetDouble and WFGetString if you are sure the
     psi-term effectively contains a value of the correct type. */
  printf("the first feature is: '%s'\n",
	 WFGetString(WFGetFeature(a,"1"),NULL));
  
  /* Get the features as a NULL terminated string array */
  features=WFFeatures(a);
  if(features) {
    for(i=0;features[i];i++) {
      printf("feature %s => %s\n",
	     features[i],
	     WFType(WFGetFeature(a,features[i])));
    }
    free(features); /* Recommended */
  }
  else { /* Error checking, here for demonstration only */
    fprintf(stderr,"'A' has no features\n");
    exit(1);
  }
  
  
  /* Run the queens program */
  WFProve("import(\042queens\042)?");
  WFProve("queens?");

  /* Loop over each solution */
  do {
    sleep(1);
    printf("retrying\n");
    ans=WFInput(";");
    printf("ans=%d\n",ans);
  } while(ans);
}
