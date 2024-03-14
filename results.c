#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

// usp endpoint to communicate with the host
#define RADIO_STATS_RESULTS_FILE "radio_stats_result.txt"

int append_to_file(const char* str_to_append){
  printf("Appending data to file: %s\n", RADIO_STATS_RESULTS_FILE);
  FILE *fp;
  int wb_retval = 0;
  /*Opening the file in appending mode...*/
  fp = fopen(RADIO_STATS_RESULTS_FILE, "a+");
  
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