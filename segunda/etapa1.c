/*
  Código para resolver o desafio da
  segunda etapa da Advent of Code.

  @rahvax | 8bitzone
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* Encurtar os tipos */
typedef unsigned long long ull;
typedef __uint128_t u128;
typedef struct {
  ull a, b;
} Interval;


void printTotal(u128 number) {
  char buffer[64];
  int index=0;
  if (number == 0) {
    putchar('0');
    return;
  }
  while (number > 0) {
    int digit = (int)(number % 10);
    buffer[index++] = '0' + digit;
    number/=10;
  }
  while (index--) putchar(buffer[index]);
}

int composeInterval(const void *pointer1, const void *pointer2) {
  const Interval *i1 = pointer1;
  const Interval *i2 = pointer2;
  if (i1->a < i2->a)
    return -1;
  else if (i1->a > i2->a)
    return 1;
  else if (i1->b < i2->b)
    return -1;
  else if (i1->b > i2->b)
    return 1;
  return 0;
}

int main(void) {
  char *line = NULL, *token=NULL;
  size_t len = 0, indexCount = 0, indexAlloc=0, mCount = 0;
  ssize_t read = getline(&line, &len, stdin);
  Interval *iVals = NULL, *merged;
  u128 pow10[41];
  ull maxr = 0, total=0;

  if (read <= 0) {
    fprintf(stderr, "Erro: não foi possível ler o STDIN.\n");
    return -1;
  }

  while (read > 0 && (line[read - 1] == '\n' || line[read - 1] == '\r'))
    line[--read] = '\0';
  token = strtok(line, ",");
  while (token) {
    char *dash = strchr(token, '-');
    char *s1=NULL, *s2=NULL;
    ull a, b;
    
    if (!dash) {
      token = strtok(NULL, ",");
      continue;
    }
    s1 = token;
    s2 = dash + 1;
    a = strtoull(s1, NULL, 10);
    b = strtoull(s2, NULL, 10);

    if (a > b) {
      ull t = a;
      a = b;
      b = t;
      if (indexCount == indexAlloc) {
        indexAlloc = indexAlloc ? indexAlloc * 2 : 16;
        iVals = realloc(iVals, indexAlloc * sizeof(Interval));
        if (!iVals) {
          fprintf(stderr, "Memória estourada\n");
          return 1;
        }
      }
    }
    iVals[indexCount].a = a;
    iVals[indexCount].b = b;
    indexCount++;
    token=strtok(NULL, ",");
  }
  if (indexCount == 0) {
    printf("0");
    free(iVals);
    free(line);
    return 0;
  }

  qsort(iVals, indexCount, sizeof(Interval), composeInterval);

  merged = malloc(indexCount * sizeof(Interval));
  if (!merged) {
    fprintf(stderr, "Memória estourada\n");
    return 1;
  }

  merged[0] = iVals[0];
  mCount = 1;
  for (size_t i = 1; i < indexCount; ++i) {
    if (iVals[i].a <= merged[mCount - 1].b + 1) {
      if (iVals[i].b > merged[mCount - 1].b)
        merged[mCount - 1].b = iVals[i].b;
    }
    else
      merged[mCount++] = iVals[i];
  }

  pow10[0] = 1;
  for (register int i = 1; i <= 40; ++i)
    if (merged[i].b > maxr)
      maxr = merged[i].b;
  for (register int i = 1;; ++i) {
    int kMinus = 2 * i - 1;
    u128 factor, baseMin, baseMax;
    
    if (kMinus > 40)
      break;

    factor = pow10[i - 1];
    baseMin = pow10[i - 1];
    baseMax = pow10[i] - 1;

    for (size_t i = 0; i < mCount; ++i) {
      u128 L = merged[i].a, R = merged[i].b, high, cnt, sum;
      u128 low = (L + factor - 1) / factor;
      if (low < baseMin)
        low = baseMin;
      high = R / factor;
      if (high > baseMax)
        high = baseMax;
      if (low > high)
        continue;
      cnt = high - low + 1;
      sum = (low + high) * cnt / 2;
      total+=sum*factor;
    }
  }

  printTotal(total);
  putchar('\n');

  free(iVals);
  free(merged);
  free(line);
  return 0;
}
