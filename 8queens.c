/*
 *		8-queens.c
 *			Solve the eight queens problem using backtracking
 *
 *				begun: March 1, 2002
 *					by: Steven Skiena
 *					*/

/*
 * Copyright 2003 by Steven S. Skiena; all rights reserved.
 *
 * Permission is granted for use in non-commerical applications
 * provided this copyright notice remains intact and unchanged.
 *
 * This program appears in my book:
 *
 * "Programming Challenges: The Programming Contest Training Manual"
 * by Steven Skiena and Miguel Revilla, Springer-Verlag, New York 2003.
 *
 * See our website www.programming-challenges.com for additional information.
 *
 * This book can be ordered from Amazon.com at
 *
 * http://www.amazon.com/exec/obidos/ASIN/0387001638/thealgorithmrepo/
 *
 * */

// #include <stdio.h>

typedef int bool;

#define TRUE 1
#define FALSE 0
#define MAXCANDIDATES 100 /* max possible next extensions */
#define NMAX 100          /* maximum solution size */

typedef char *data; /* type to pass data to backtrack */

int solution_count; /* how many solutions are there? */

bool finished = FALSE; /* found all solutions yet? */

int abs(int n) {
  if (n > 0)
    return n;
  return -n;
}

void construct_candidates(int a[], int k, int n, int c[], int *ncandidates);
int is_a_solution(int a[], int k, int n);
void process_solution(int a[], int k);


void backtrack(int a[], int k, int input) {
  int c[MAXCANDIDATES]; /* candidates for next position */
  int ncandidates;      /* next position candidate count */
  int i;                /* counter */

  if (is_a_solution(a, k, input))
    process_solution(a, k);
  else {
    k = k + 1;
    construct_candidates(a, k, input, c, &ncandidates);
    for (i = 0; i < ncandidates; i++) {
      a[k] = c[i];
      backtrack(a, k, input);
      if (finished)
        return; /* terminate early */
    }
  }
}

void process_solution(int a[], int k) {
  int i; /* counter */

  solution_count++;
}

int is_a_solution(int a[], int k, int n) { return (k == n); }

/*	What are possible elements of the next slot in the 8-queens
 *		problem?
 *		*/

void construct_candidates(int a[], int k, int n, int c[], int *ncandidates) {
  int i, j;        /* counters */
  bool legal_move; /* might the move be legal? */

  *ncandidates = 0;
  for (i = 1; i <= n; i++) {
    legal_move = TRUE;
    for (j = 1; j < k; j++) {
      if (abs((k)-j) == abs(i - a[j])) /* diagonal threat */
        legal_move = FALSE;
      if (i == a[j]) /* column threat */
        legal_move = FALSE;
    }
    if (legal_move == TRUE) {
      c[*ncandidates] = i;
      *ncandidates = *ncandidates + 1;
    }
  }
}

// extern void _sim_exit();

int _start() {
  int a[NMAX]; /* solution vector */
  int i;       /* counter */
  int compare[] = {1, 0, 0, 2, 10, 4, 40, 92};

  for (i = 1; i <= 9; i++) {
    solution_count = 0;
    backtrack(a, 0, i);
    // printf("n=%d  solution_count=%d\n",i,solution_count);
    // if (compare[i - 1] != solution_count)
    //   return 1;
      // _sim_exit();
      // __asm__("ecall\n\t");
  }
  return 0;
  // _sim_exit();
  // __asm__("ecall\n\t");
}
