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
#include "data_object.c"

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

  if(strcmp(argv[1],"get-data") == 0){    
    retval = get_data_object(bus_ctx_usp, argv[2]);
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

  if(strcmp(argv[1],"get-connected-stations") == 0){  
    retval = get_connected_stations(bus_ctx_usp);
    if (retval == 0) {
        goto leave;
    }
  }  

  leave:
    // clean amxrt configuration
    amxrt_delete();
    amxb_be_remove_all();
    return retval;
  
}
