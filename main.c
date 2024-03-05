#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

// specific prpl libraries
#include <amxrt/amxrt.h>
#include <amxc/amxc.h>
#include <amxp/amxp.h>
#include <amxd/amxd_dm.h>
#include <amxd/amxd_object.h>


// usp endpoint to communicate with the host
#define DM_SOCK_URI_ENDPOINT "usp:/var/run/usp/endpoint_agent_path"
// amxb module to discuss in usp
#define DM_BACKEND_USP    "/usr/bin/mods/amxb/mod-amxb-usp.so"

// key of datamodel to search
//#define DM_DEVICE_KEY  "DeviceInfo."
#define DM_DEVICE_KEY  "WiFi.Radio.1.Stats."

// key of datamodel to WiFi
#define DM_WIFI_KEY "WiFi."

// global bus variable
static amxb_bus_ctx_t *bus_ctx_usp = NULL;


int config_api_init() {
    // arg use to pass command option to amxrt
    char *argv[] = { (char*)"c-getdm", "-A",
                     "-B", DM_BACKEND_USP,
                     "-u", DM_SOCK_URI_ENDPOINT};
    int index = 0;
    int retval;

    // init of amxrt
    amxrt_new();
    // configuration with command line options
    amxrt_config_init(sizeof(argv)/sizeof(argv[0]), argv, &index, NULL);

    // connect to bus
    retval = amxrt_connect();
    if (retval != 0) {
        printf("CONFIG DM: amxrt_connect failed");
        return -1;
    }
    // keep the bus object of the connexion
    bus_ctx_usp = amxb_find_uri(DM_SOCK_URI_ENDPOINT);

    return 0;
}


int get_dm_friendly_name() {
  amxc_var_t results;

  // init variable
  amxc_var_init(&results);

  // get datamodel key from usp endpoint
  int rv = amxb_get(bus_ctx_usp, DM_DEVICE_KEY, 0, &results, 1);
  if(rv != 0) {
      printf("CONFIG DM: failed to get config");
      amxc_var_clean(&results);
      return -1;
  }
  // dump all datamodel info under the key for debug
  amxc_var_dump(&results, STDOUT_FILENO);

  // helper to iterate on result object
  amxc_var_for_each(var, &results) {
    //print var
    printf("var:");
    printf("\n");
    printf(var);
    // get the datamodel
    amxc_var_t* objectvar = GET_ARG(var, DM_DEVICE_KEY);
    // get the friendlyname value
    const char* val = GET_CHAR(objectvar, "FriendlyName");
    // print the value
    printf(val);
    printf("\n");
  }
  // release the object result
  amxc_var_clean(&results);
  return 0;
}

int get_wifi_stats() {
  amxc_var_t results;

  // init variable
  amxc_var_init(&results);

  // get datamodel key from usp endpoint
  int rv = amxb_get(bus_ctx_usp, DM_WIFI_KEY, 0, &results, 1);
  if(rv != 0) {
      printf("CONFIG DM: failed to get config");
      amxc_var_clean(&results);
      return -1;
  }
  // dump all datamodel info under the key for debug
  amxc_var_dump(&results, STDOUT_FILENO);

  // helper to iterate on result object
  // amxc_var_for_each(var, &results) {
  //   // get the datamodel
  //   amxc_var_t* objectvar = GET_ARG(var, DM_WIFI_KEY);
  //   // get the friendlyname value
  //   const char* val = GET_CHAR(objectvar, "MulticastPacketsSent");
  //   // print the value
  //   printf("MulticastPacketsSent:");
  //   printf(val);
  //   printf("\n");
  // }
  // release the object result
  amxc_var_clean(&results);
  return 0;
}

int get_wifi_radio_stats(){
  amxc_var_t values;
  amxc_var_init(&values);  

  //int rv = amxb_get(bus_ctx_usp, "WiFi.Radio.*.Stats.", 0, &values, 1);
  int rv = amxb_get(bus_ctx_usp, "WiFi.Radio.*.getRadioStats()", 0, &values, 1);
  amxc_var_dump(&values, STDOUT_FILENO); 

  amxc_var_clean(&values);
}


int write_to_file(){


  /*Define file name*/
  char file_name[] = "/data/test.txt";
  /*Define string to append*/
  char string_to_append[] = "Nico";


  /*Opening the file in appending mode...*/
  fp = fopen(file_name, "a+");
  /*File open operation failed.*/
  if (fp == NULL) {
    printf("RESULTS FILE [ERROR]: failed to open results file\n");
    return -1;
  }

  /*Appending string to the file...*/
  printf("Appending string...\n");
  fprintf(fp, "Your string: %s\n", string_to_append);
  
  /*Closing the file...*/
  fclose(fp);
  
  printf("Successfully appended\n");
  return 0;

}


int main() {
  //printf("Smart Local Networks application start!\n");

  // configure amx connection
  config_api_init();
  // get the FriendlyName from host
  //get_dm_friendly_name();

  // get WiFi stats
  //get_wifi_stats();

  // get wifi radio stats
  //get_wifi_radio_stats();

  // write to file
  write_to_file();

  // clean amxrt configuration
  amxrt_delete();
  return 0;
}
