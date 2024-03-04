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
#define DM_DEVICE_KEY  "DeviceInfo."

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


int get_dm_friendly_name()
{
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
  // amxc_var_dump(&results, STDOUT_FILENO);

  // helper to iterate on result object
  amxc_var_for_each(var, &results) {
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


int main() {
  printf("olis world!\n");

  // configure amx connection
  config_api_init();
  // get the FriendlyName from host
  get_dm_friendly_name();

  // clean amxrt configuration
  amxrt_delete();
  return 0;
}
