#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>


int append_to_file(const char* file_name){
  printf("Appending data to file: %s\n", file_name);
  FILE *fp;
  /*Opening the file in appending mode...*/
  fp = fopen(file_name, "a+");
  
  /*File open operation failed.*/
  if (fp == NULL) {
    printf("FILE: error opening the file");
    return -1;
  }
  /*Append to file*/
  fprintf(fp, "Holis in file\n");
  /*Closing the file...*/
  fclose(fp);

  return 0;
}