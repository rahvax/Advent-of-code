/*
  Como é um desafio de lógica,
  e é algo bem simples, eu optei
  por não usar header personalizado.

  by: @rahvaz | 8bitzone
*/

#include <ctype.h>
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
  FILE *fp=NULL;
  if (argc < 2) {
    fprintf(stderr, "Erro: passe a saída de um arquivo ou seu path!\n");
    return -1;
  }

  if (!(fp = openFile(argv[1]))) {
    fprintf(stderr, "Exit: programa encerrado.\n");
    return -1;
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
  long count=0;

  while (fgets(line, sizeof(line), fileStream)) {
    char *pointer=line, *endPointer=NULL, dir;
    long dist;

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

    /* Realizar o parser
       R eleva o número e L reduz
       Ao chegar em 99 é zerado e ao chegar
       em <0 curva para 99. */
    dist = strtol(pointer, &endPointer, 10);
    if (dir == 'R')
      position = (int)(((position + dist) % 100 + 100) % 100);
    else if (dir == 'L')
      position = (int)(((position - dist) % 100 + 100) % 100);
    else
      continue;

    if (position == 0) count++;
  }

  if (fileStream != stdin) fclose(fileStream);
  printf("%ld\n", count);
}
