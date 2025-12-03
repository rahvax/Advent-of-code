/*
  Segundo dia do Advent of Code,
  fiz esse bem mais desorganizado
  e antes de postar dei uma
  refatorada T_T
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <errno.h>

/* Definir tipos para encurtar */
typedef unsigned long long ull;
typedef __uint128_t u128;

#define INITIAL_CAPACITY 16
/* 10^20 > 2^64 */
#define MAX_K 20

typedef struct {
  ull a, b;
} Interval;

/* Imprime u128 em base 10 */
void printU128(u128 value) {
  char buf[64];
  int i = 0;

  if (value == 0) {
    putchar('0');
    return;
  }
  while (value > 0) {
    unsigned digit = (unsigned)(value % 10);
    buf[i++] = '0' + digit;
    value /= 10;
  }
  while (i--)
    putchar(buf[i]);
}

/* Remove espaços  */
void trimSpace(char *s) {
  char *p = s;
  size_t len;

  while (isspace((unsigned char)*p))
    p++;
  if (p != s)
    memmove(s, p, strlen(p) + 1);
  len = strlen(s);
  while (len && isspace((unsigned char)s[len - 1]))
    s[--len] = '\0';
}

/* 
  Faz o parsing da entrada (",") e 
  retorna ponteiro alocado e coloca
  a contagem em outCount */
Interval *parseIntervals(char *line, size_t *outCount) {
  size_t cap = INITIAL_CAPACITY, count = 0;
  Interval *arr = malloc(cap * sizeof(Interval));
  char *token;
  if (!arr)
    return NULL;

  token = strtok(line, ",");
  while (token) {
    char *dash, *s1, *s2, *endp1,*endp2;
    ull a, b;
    Interval *tmp;

    trimSpace(token);
    if (*token == '\0') {
      token = strtok(NULL, ",");
      continue;
    }
    dash = strchr(token, '-');
    if (!dash) {
      token = strtok(NULL, ",");
      continue;
    }

    *dash = '\0';
    s1 = token;
    s2 = dash + 1;
    errno = 0;
    endp1 = NULL;
    endp2 = NULL;
    a = strtoull(s1, &endp1, 10);
    b = strtoull(s2, &endp2, 10);

    if (errno != 0 || *endp1 != '\0' || *endp2 != '\0') {
      token = strtok(NULL, ",");
      continue;
    }
    if (a > b) {
      ull t = a;
      a = b;
      b = t;
    }
    if (count == cap) {
      cap *= 2;
      tmp = realloc(arr, cap * sizeof(Interval));
      if (!tmp) {
        free(arr);
        return NULL;
      }
      arr = tmp;
    }
    arr[count].a = a;
    arr[count].b = b;
    count++;

    token = strtok(NULL, ",");
  }
  *outCount = count;
  return arr;
}

/* Comparator para qsort */
int compInterval(const void *p1, const void *p2) {
  const Interval *i1 = p1;
  const Interval *i2 = p2;
  if (i1->a < i2->a) return -1;
  if (i1->a > i2->a) return 1;
  if (i1->b < i2->b) return -1;
  if (i1->b > i2->b) return 1;
  return 0;
}

/*
 Recebe um array de intervals com inCount e
 junta intervals sobrepostos
 Retorna um novo array com a contagem
 */
Interval *mergeIntervals(Interval *inArr, size_t inCount, size_t *outCount) {
  Interval *merged;
  size_t m,i;
  if (inCount == 0) {
    *outCount = 0;
    return NULL;
  }

  qsort(inArr, inCount, sizeof(Interval), compInterval);
  merged = malloc(inCount * sizeof(Interval));
  if (!merged)
    return NULL;

  m = 0;
  merged[0] = inArr[0];
  m = 1;
  i = 1;
  for (; i < inCount; ++i) {
    if (inArr[i].a <= merged[m-1].b + 1ULL) {
      if (inArr[i].b > merged[m-1].b)
        merged[m-1].b = inArr[i].b;
    } else
      merged[m++] = inArr[i];
  }
  *outCount = m;
  return merged;
}

/* Preenche um array de ^10 */
void compPow10(u128 pow10[]) {
  int i;
  pow10[0] = 1;
  i = 1;
  for (; i <= MAX_K; ++i)
    pow10[i] = pow10[i-1] * 10ULL;
}

/*
 Recebe os intervals e retorna a soma dos IDs */
u128 sumInvalidIds(const Interval *merged, size_t mCount) {
  u128 pow10Arr[MAX_K + 1], maxR, total;
  int k;
  size_t i;

  if (mCount == 0)
    return (u128)0;
  compPow10(pow10Arr);
  maxR = 0;
  i = 0;
  for (; i < mCount; ++i)
    if ((u128)merged[i].b > maxR)
      maxR = merged[i].b;
  total = (u128)0;
  k = 1;
  for (; k <= MAX_K; ++k) {
    u128 factor,multiplier,baseMin,baseMax;
    factor = pow10Arr[k];
    multiplier = factor + (u128)1;
    baseMin = (k == 1) ? (u128)1 : pow10Arr[k-1];
    baseMax = factor - 1;

    if (baseMin > baseMax)
      continue;
    if (baseMin * multiplier > maxR)
      break;

    i = 0;
    for (; i < mCount; ++i) {
      u128 L, R,xLow,xHigh,cnt,sumX;
      u128 add;
      L = merged[i].a;
      R = merged[i].b;
      xLow = (L + multiplier - 1) / multiplier;

      if (xLow < baseMin)
        xLow = baseMin;
      xHigh = R / multiplier;
      if (xHigh > baseMax)
        xHigh = baseMax;
      if (xLow > xHigh)
        continue;

      cnt = xHigh - xLow + 1;
      sumX = (xLow + xHigh) * cnt / 2;
      add = sumX * multiplier;
      total += add;
    }
  }
  return total;
}

int main(void) {
  char *line = NULL;
  size_t inCount = 0, len = 0, mCount = 0;
  ssize_t read;
  Interval *inArr = NULL, *merged = NULL;
  u128 total;
  read = getline(&line, &len, stdin);

  if (read <= 0) {
    fprintf(stderr, "Erro: não foi possível ler STDIN\n");
    free(line);
    return 1;
  }
  while (read > 0 && (line[read-1] == '\n' || line[read-1] == '\r'))
    line[--read] = '\0';
  inArr = parseIntervals(line, &inCount);
  free(line);

  if (!inArr) {
    fprintf(stderr, "Erro de memória ou input inválido\n");
    return 1;
  }
  merged = mergeIntervals(inArr, inCount, &mCount);
  free(inArr);
  if (!merged && mCount != 0) {
    fprintf(stderr, "Erro de memória\n");
    return 1;
  }

  total = sumInvalidIds(merged, mCount);
  printU128(total);
  putchar('\n');

  free(merged);
  return 0;
}

