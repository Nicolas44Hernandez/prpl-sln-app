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

#define DM_WIFI_OBJ "WiFi.Radio.1."
#define DM_WIFI_METHOD "getRadioStats"

int get_radio_feature(amxb_bus_ctx_t *bus_ctx_usp, const char* feature){
    printf("GET RADIO FEATURE OBJECT\n");
    printf("Getting radio feature %s from : %s.%s()\n", feature, DM_WIFI_OBJ, DM_WIFI_METHOD);
    
    /*Vars declaration*/
    amxc_var_t args;
    amxc_var_t ret;
    int retval = 0;
    
    /*Get Radio Stats*/
    retval = amxb_call(bus_ctx_usp, DM_WIFI_OBJ, DM_WIFI_METHOD, &args, &ret, 5);
    if (retval != 0) {
        printf("RPC Error: Error in RPC execution\n");
        amxc_var_clean(&args);
        amxc_var_clean(&ret);
        return -1;
    }

    /*Extract result*/
    amxc_var_for_each(var, &ret) {

        /* Print var type*/
        const char *type_of_var = amxc_var_type_name_of(var);
        printf("type of var: %s \n", type_of_var);   

        char *var_str = amxc_var_dyncast(cstring_t, var);
        if(strlen(var_str) < 10){
            continue;
        }  

        /*Print table size*/
        int htable_size = amxc_htable_size(&var);
        printf("htable size: %d\n", htable_size); 

        // Get the value
        const int val = GET_INT32(var, feature);
        printf("Found value - %s: %d\n", feature, val);

    }


    printf("\n\n\nDUMP ENTYRE RESPONSE:\n");
    /*Print returned values for debug*/
    amxc_var_dump(&ret, STDOUT_FILENO);

    /*Clean variables*/
    amxc_var_clean(&args);
    amxc_var_clean(&ret);
    retval = 0;
}
