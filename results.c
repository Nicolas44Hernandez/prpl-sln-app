#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

int append_to_file(const char* str_to_append, const char* results_file){
  printf("Appending data to file: %s\n", results_file);
  FILE *fp;
  int wb_retval = 0;
  /*Opening the file in appending mode...*/
  fp = fopen(results_file, "a+");
  
  /*File open operation failed.*/
  if (fp == NULL) {
    printf("FILE: error opening the file");
    return -1;
  }
  /*Append to file*/
  wb_retval = fprintf(fp, str_to_append);
  /*Closing the file...*/
  fclose(fp);

  return wb_retval;
}