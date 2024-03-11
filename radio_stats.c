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

int get_radio_air_stats(amxb_bus_ctx_t *bus_ctx_usp){
    printf("GET RADIO STATS\n");
  
    /*Vars declaration*/
    FILE *fp;
    amxc_var_t args;
    amxc_var_t ret;
    amxb_invoke_t* invoke_ctx = NULL;
    int retval = 0;
    const char* file_name = "results.txt";

    /*Init args*/
    amxc_var_init(&args);
    amxc_var_init(&ret);

    /*RPC call*/
    retval = amxb_call(bus_ctx_usp, "WiFi.Radio.1.", "getRadioAirStats", &args, &ret, 5);
    if (retval != 0) {
        printf("RPC Error: Error in RPC execution\n");
        amxb_free_invoke(&invoke_ctx);
        amxc_var_clean(&args);
        amxc_var_clean(&ret);
        return -1;
    }
    /*Opening the file in appending mode...*/
    //fp = fopen(file_name, "a+");

    /*Print returned values for debug*/
    amxc_var_dump(&ret, STDOUT_FILENO);
    //amxc_var_dump(&ret, fp);

    /*Clean vars*/
    amxb_free_invoke(&invoke_ctx);
    amxc_var_clean(&args);
    amxc_var_clean(&ret);  

    /*Closing the file...*/
    //fclose(fp);
}


int get_radio_stats(amxb_bus_ctx_t *bus_ctx_usp){
    printf("GET RADIO AIR STATS\n");
    /*Vars declaration*/
    amxc_var_t args;
    amxc_var_t ret;
    amxb_invoke_t* invoke_ctx = NULL;
    int retval = 0;
    const char* file_name = "results.txt";

    FILE* fp = NULL;
    
    /*Init args*/
    amxc_var_init(&args);
    amxc_var_init(&ret);

    /*RPC call*/
    retval = amxb_call(bus_ctx_usp, "WiFi.Radio.1.", "getRadioStats", &args, &ret, 5);
    if (retval != 0) {
        printf("RPC Error: Error in RPC execution\n");
        amxb_free_invoke(&invoke_ctx);
        amxc_var_clean(&args);
        amxc_var_clean(&ret);
        return -1;
    }
    
    /*Print returned values for debug*/
    amxc_var_dump(&ret, STDOUT_FILENO);

    /*Opening the file in appending mode...*/
    fp = fopen(file_name, "a+");

    /*Append to results file*/
    amxc_var_dump(&ret, fp);

    /* Iterate on result object*/
    // amxc_var_for_each(var, &ret) {
    //     //amxc_var_t* objectvar = GET_ARG(var, "WiFi.Radio.1.");
    //     // get the friendlyname value
    //     //const char* val = GET_CHAR(objectvar, "FriendlyName");
    //     const int* val = GET_INT32(var, "Noise");
    //     // print the value
    //     printf("Noise: %s", val);
    //     printf("\n");
    // }


    /*Clean vars*/
    amxb_free_invoke(&invoke_ctx);
    amxc_var_clean(&args);
    amxc_var_clean(&ret);  

    /*Closing the file...*/
    fclose(fp);
}
