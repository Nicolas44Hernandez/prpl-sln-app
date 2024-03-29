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

#include "radio_stats.c"
#include "connected_stations.c"
#include "radio_feature.c"
#include "wifi_counters.c"

// usp endpoint to communicate with the host
#define DM_SOCK_URI_ENDPOINT "usp:/var/run/usp/endpoint_agent_path"
// amxb module to discuss in usp
#define DM_BACKEND_USP    "/usr/bin/mods/amxb/mod-amxb-usp.so"


// global bus variable
static amxb_bus_ctx_t *bus_ctx_usp = NULL;

static int app_check_args(int argc, char* argv[]) {
    int retval = 0;

    if(argc < 1) {
        printf("\n\nInvalid number of arguments\n");
        retval = 1;
    }
    return retval;
}

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


int main(int argc, char* argv[]) {
  printf("Smart Local Networks application start!\n");
  int retval = 0;
  retval = app_check_args(argc, argv);
  if (retval != 0) {
      goto leave;
  }

  // configure amx connection
  config_api_init();

  if(strcmp(argv[1],"get-radio-feature") == 0){    
    retval = get_radio_feature(bus_ctx_usp, argv[2]);
    if (retval == 0) {
        goto leave;
    }
  }

  if(strcmp(argv[1],"get-radio-stats") == 0){  
    retval = get_radio_stats(bus_ctx_usp);
    if (retval == 0) {
        goto leave;
    }
  }

  if(strcmp(argv[1],"counters-stations") == 0){  

    /*Init stations counters*/
    init_stations_counters();

    /* Loop for 25 samples (test)*/
    for(int i = 0; i < 25; i++){
      update_connected_stations_counters(bus_ctx_usp);
      print_stations_list();
      sleep(5);    
    }
  }  

  if(strcmp(argv[1],"wifi-counters") == 0){  
    
    /* Create structs*/
    BandCounters counters_2GHz;
    BandCounters counters_5GHz;
    // BandCounters counters_6GHz;
    Sample sample_2GHz;
    Sample sample_5GHz;
    // Sample sample_6GHz;

    /* Initialize structs*/
    init_counters(&counters_2GHz,0);
    init_counters(&counters_5GHz,1);
    // init_counters(&counters_6GHz,2);
    init_sample(&sample_2GHz);
    init_sample(&sample_5GHz);
    //init_sample(&sample_6GHz);
    
    /* Print values for test*/
    printf("*************************************************\n\n");
    printf("Before function call: \n");
    printBandCounters(&counters_2GHz);
    // printSample(&sample_2GHz);
    printBandCounters(&counters_5GHz);
    // printSample(&sample_5GHz);
    // printBandCounters(&counters_6GHz);
    // printSample(&sample_6GHz);
    printf("*************************************************\n\n");
    
    /* Loop for 20 samples (test)*/
    for(int i = 0; i < 20; i++){
      
      init_sample(&sample_2GHz);
      init_sample(&sample_5GHz);
      
      /*Call for getting counters samples*/
      getSampleForBandCounter(bus_ctx_usp, &sample_2GHz, counters_2GHz.band);
      getSampleForBandCounter(bus_ctx_usp, &sample_5GHz, counters_5GHz.band);
      
      /* Append sample to counters */
      appendNewSample(&counters_2GHz, &sample_2GHz);
      appendNewSample(&counters_5GHz, &sample_5GHz);
      
      /* Print for debug*/
      printf("------------- Iteration:%d\n", i);      
      printSample(&sample_2GHz);
      printBandCounters(&counters_2GHz);
      printSample(&sample_5GHz);
      printBandCounters(&counters_5GHz);
      printf("------------------------");
      sleep(5);
    }
  }  
  
  if(strcmp(argv[1],"counters-band") == 0){  

    /* Get band*/
    int band = 0;
    if(strcmp(argv[2],"1") == 0){ 
      band = 1;
    }
    if(strcmp(argv[2],"2") == 0){ 
      band = 2;
    }
    
    /* Create structs*/
    BandCounters counters;
    Sample sample;

    /* Initialize structs*/
    init_counters(&counters,band);
    init_sample(&sample);
    
    /* Loop for 20 samples (test)*/
    for(int i = 0; i < 20; i++){
      
      init_sample(&sample);
      
      /*Call for getting counters samples*/
      getSampleForBandCounter(bus_ctx_usp, &sample, counters.band);
      
      /* Append sample to counters */
      appendNewSample(&counters, &sample);
      
      /* Print for debug*/
      printf("------------- Iteration:%d\n", i);      
      printSample(&sample);
      printBandCounters(&counters);
      printf("------------------------");
      sleep(5);
    }
  }    

  leave:
    // clean amxrt configuration
    amxrt_delete();
    amxb_be_remove_all();
    return retval;
}
