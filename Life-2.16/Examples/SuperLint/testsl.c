/*
 *  This C program is not intended to mean anything. But it contains all the
 *  stylistic problems treated by the rules of the file 'rules.lf'.
 *  Moreover it contains some odd syntactic constructions in order to test the
 *  reliability of the C parser.
 *  The main function contains somewhat more coherent code in order to test the
 *  rules checking for uninitialized variables.
 */


typedef unsigned UNSIGNED;


typedef UNSIGNED ((**PTR));


extern int external_name;


int a[3][~~~0] = {{1, 2, 3},
		  {1, 2, 3},
		  {0, ~~~1, 1},
		 };

struct _strange {
  unsigned int :1&~3;
  struct {
    float **f;
    int tag:3;
  }  ** field;
} strange;

typedef struct _strange *(*(strange_ptr));


float **my_function(int i, long double **d)
{
  int a,
      b = ~~0,
      c,
      A_very_explicit_variable_name;






  /* that's a big scope */

















  a=++--b;
  if(a>=++b){
    int i;
    UNSIGNED *j;

    typedef long PTR;

    **a = (PTR)(++b);
    for(i= 0; i < *++a; i++) {
      enum _ee {PTR, PTR1};

      float *(*F);

      **F+= 0.3E12L;
    }
    return 1 + i;
  }
  else  /* another indentation style */
  {
    typedef float ((((((((Nested_type))))))));

    typedef int PTR;

    PTR new_c;





   /* another big scope */





     Nested_type *s;

     new_c+=3;
    {
      const int PTR;

      PTR =0;
    }
  }
}


main(int argc, char **argv)
{
  int i, j, k, m;
  float **l;

  if(1) {
    i = j;
    if(0)
      k = 1;
    else {
      l = my_function(k, (long double **) 0);
      k = 2;
    };
    for(i=0; i<10; i++) {
      j += 1;
      m = 3;
      j *= m;
    }
    l = k + m;
  }
  else
    i = m;
  l = i * k;
  argc = argv[3];
}
