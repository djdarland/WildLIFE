/* Copyright 1992 Digital Equipment Corporation
   All Rights Reserved
*/

/* Hamming sequence problem in C. */
/* Author: Peter Van Roy */

#include <stdio.h>
#define FALSE 0
#define TRUE  1

#define MAX 1000

#define TWO   0
#define THREE 1
#define FIVE  2

int seq[MAX];
int i,i2,i3,i5; 

/* Generate the Hamming sequence from 1 to n. */
void hamming(n)
int n;
{
   int min,whichmin;
   int e2,e3,e5;

   i=i2=i3=i5=1;
   seq[1]=1;

   while (seq[i]<=n) {
      /* Get the candidate integers */
      e2=2*seq[i2];
      e3=3*seq[i3];
      e5=5*seq[i5];

      /* Find the minimum candidate. */
      min=e2; whichmin=TWO;
      if (e3<min) { min=e3; whichmin=THREE; }
      if (e5<min) { min=e5; whichmin=FIVE; }

      /* Insert if bigger. */
      if (min>seq[i]) {
         i++;
         seq[i]=min;
      }

      /* Move to next element. */
      if (whichmin==TWO)   i2++;
      if (whichmin==THREE) i3++;
      if (whichmin==FIVE)  i5++;
   }
}

main()
{
   int j;

   hamming(1000);
   for (j=1; j<i; j++)
      printf("%d ",seq[j]);
   printf("\n");
}
