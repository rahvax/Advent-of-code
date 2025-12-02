/* (2)x
  Como é um desafio de lógica,
  e é algo bem simples, eu optei
  por não usar header personalizado.

  by: @rahvaz | 8bitzone
*/

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

/* Abrir um arquivo pelo seu caminho,
   e retornar seu ponteiro. */
FILE *openFile (const char *filePath);

/* Realizar parser e verificar
   a entrada (filestream ou input)
   para realizar a soma solicitada. */
void parser (FILE *fileStream);

int main(int argc, char **argv) {
  FILE *fp=stdin;
  
  if (argc > 1) {
    if (!(fp = openFile(argv[1]))) {
      fprintf(stderr, "Exit: arquivo inexistente ou sem permissão.\n");
      return -1;
    }
  }
  
  parser(fp);
  return 0;
}


FILE *openFile(const char *filePath) {
  FILE *fp;
  if (!(fp = fopen(filePath, "r"))) {
    fprintf(stderr, "Erro: arquivo inexistente ou sem permissão!\n");
    return NULL;
  }
  return fp;
}


void parser(FILE *fileStream) {
  char line[256];
  int position = 50;
  long long total=0, dmod=0; // um double charmoso

  while (fgets(line, sizeof(line), fileStream)) {
    char *pointer=line, *endPointer=NULL, dir;
    long long dist; // double charmoso PT. 2

    /* Avançar espaços */
    while (*pointer && isspace((unsigned char)*pointer))
      pointer++;
    if (!*pointer)
      continue;

    /* Avançar até digito/número */
    dir = *pointer;
    while (*pointer && !isdigit((unsigned char)*pointer) && *pointer != '-')
      pointer++;
    if (!*pointer)
      continue;

    errno = 0; // vem do errno.h
    dist = strtol(pointer, &endPointer, 10);
    if (errno != 0)
      continue;

    if (dist > 0) {
      long long k0;
      if (dir == 'R') {
        k0 = (100 - position) % 100;
        if (k0 == 0)
          k0 = 100;
      }
      else if (dir == 'L') {
        k0 = position % 100;
        if (k0 == 0) k0=100;
      }
      else
        k0 = -1;

      if (k0 > 0 && dist >= k0)
	total+=1 + (dist-k0)/100;
    }
    else;

    dmod = dist % 100;
    if (dir == 'R')
      position = (int)((position + dmod) % 100);
    else if (dir == 'L') {
      position = (int)((position - dmod) % 100);
      if (position < 0)
        position += 100;
      else;
    }
  }
  if (fileStream != stdin) fclose(fileStream);
  printf("%lld\n", total);

}
