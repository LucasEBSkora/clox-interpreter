#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "vm.h"

static void repl();
static void runFile(const char* path);


int main(int argc, const char* argv[]) {
  initVM();
  
  if (argc == 1) {
    repl();
  } else if (argc == 2) {
    runFile(argv[1]);
  } else {
    fprintf(stderr, "Usage: clor [path]\n");
    exit(64);
  }

  freeVM();
  return 0;
}

static void repl() {
  char line[1024];
  for (;;) {
    printf("> ");

    if (!fgets(line, sizeof (line), stdin)) {
      printf("\n");
      break;
    }

    interpret(line);
  }
}

static char* readFile(const char* path) {
  FILE* file = fopen(path, "rb");

  if (file == NULL) {
    fprintf(stderr, "Could not open file \"%s\".\n", path);
    exit(74);
  }
  //vai ate o final do arquivo
  fseek(file, 0L, SEEK_END);
  //ftell diz quantos bytes do inicio do arquivo estamos
  size_t fileSize = ftell(file);
  //rewind retorna o cursor para o inicio do arquivo
  rewind(file);

  //aloca memoria para o arquivo inteiro + o char nulo
  char* buffer = (char*)malloc(fileSize + 1);

  if (buffer == NULL) {
    fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
    exit(74);
  }

  //le de verdade o arquivo
  size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
  
  if (bytesRead < fileSize) {
    fprintf(stderr, "Could not read file \"%s\".\n", path);
    exit(74);
  }
  
  //adiciona o byte nulo
  buffer[bytesRead] = '\0';

  //fecha o arquivo
  fclose(file);
  return buffer;
}

static void runFile(const char* path) {
  char* source = readFile(path);
  InterpretResult result = interpret(source);
  free(source);

  if (result == INTERPRET_COMPILE_ERROR) exit(65);
  if (result == INTERPRET_RUNTIME_ERROR) exit(70);
}