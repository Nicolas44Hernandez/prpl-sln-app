#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

// specific prpl libraries
#include <amxrt/amxrt.h>
#include <amxc/amxc.h>
#include <amxp/amxp.h>
#include <amxd/amxd_dm.h>
#include <amxd/amxd_object.h>

// program libraries
#include "results.c"

#define BUFF_SIZE 4000

// Result files
#define RADIO_STATS_2GHZ_RESULTS_FILE "radio_stats_2GHz_result.txt"
#define RADIO_STATS_5GHZ_RESULTS_FILE "radio_stats_5GHz_result.txt"
#define RADIO_STATS_6GHZ_RESULTS_FILE "radio_stats_6GHz_result.txt"


int get_radio_stats(amxb_bus_ctx_t *bus_ctx_usp){
    printf("GET RADIO STATS\n");

    /*Vars declaration*/
    amxc_var_t args;
    amxc_var_t ret;
    amxb_invoke_t* invoke_ctx = NULL;
    time_t rawtime;
    struct tm * timeinfo;
    int retval = 0;
    char stats_str[BUFF_SIZE];
    stats_str[0] =  '\0';

    /*Result files array*/
    const char *result_files_arr[] = { 
        RADIO_STATS_5GHZ_RESULTS_FILE,
        RADIO_STATS_2GHZ_RESULTS_FILE, 
        RADIO_STATS_6GHZ_RESULTS_FILE,
    };   

    /*Commands bands array*/
    const char *bands_cmd_arr[] = { 
        "WiFi.Radio.1.", 
        "WiFi.Radio.2.", 
        "WiFi.Radio.3."
    };   

    /*Init args*/
    amxc_var_init(&args);
    amxc_var_init(&ret);

    /*Get Current time*/   
    time ( &rawtime );
    timeinfo = localtime( &rawtime );
    char* timeinfo_str = asctime(timeinfo);
    timeinfo_str[strlen(timeinfo_str) -1] = '\0';
    char *timestmp = strcat(timeinfo_str, ",");
    //printf( "Current local time and date: %s\n",timestmp);

    
    /*Loop over the wifi bands*/
    for(int i=0; i<3; i++){
        
        /*Append timestamp header*/
        char *ts_header = "{timestamp:";
        strcat(stats_str, ts_header);

        /*Append timestamp to result str*/    
        strcat(stats_str, timestmp); 

        /*Get Radio Stats*/
        retval = amxb_call(bus_ctx_usp, bands_cmd_arr[i], "getRadioStats", &args, &ret, 5);
        if (retval != 0) {
            printf("RPC Error: Error in RPC execution\n");
            amxb_free_invoke(&invoke_ctx);
            amxc_var_clean(&args);
            amxc_var_clean(&ret);
            return -1;
        }

        /*Extract result*/
        amxc_var_for_each(var, &ret) {
            //cast the entire list
            char *list_str = amxc_var_dyncast(cstring_t, var);
            if(strlen(list_str) > 10){
                strcat(stats_str, list_str);    
            }  
        }

        /*Print returned values for debug*/
        //printf("%s:\n", bands_cmd_arr[i]);
        //amxc_var_dump(&ret, STDOUT_FILENO);

        /*Clean variables*/
        amxb_free_invoke(&invoke_ctx);
        amxc_var_clean(&args);
        amxc_var_clean(&ret);
        retval = 0;

        /*Get Radio Air Stats*/
        retval = amxb_call(bus_ctx_usp, bands_cmd_arr[i], "getRadioAirStats", &args, &ret, 5);
        if (retval != 0) {
            printf("RPC Error: Error in RPC execution\n");
            amxb_free_invoke(&invoke_ctx);
            amxc_var_clean(&args);
            amxc_var_clean(&ret);
            return -1;
        }

        /*Extract result*/
        amxc_var_for_each(var, &ret) {
            //cast the entire list
            char *list_air_str = amxc_var_dyncast(cstring_t, var);
            if(strlen(list_air_str) > 10){
                strcat(stats_str, list_air_str);              
            }  
        }
        
        /*Print returned values for debug*/
        //printf("%s:\n", bands_cmd_arr[i]);
        //amxc_var_dump(&ret, STDOUT_FILENO);

        
        /*Append end char*/
        strcat(stats_str, "}\n");

        /*Print stats values for debug*/
        //printf("STATS TO PRINT: %s \n", stats_str);

        /*Append to results file*/
        retval = append_to_file(stats_str, result_files_arr[i]);
        if (retval == 0) {
            printf("Error when writting to file\n");
        }

        /*Clean vars*/
        amxb_free_invoke(&invoke_ctx);
        amxc_var_clean(&args);
        amxc_var_clean(&ret);  
        stats_str[0] =  '\0';
        
    }
}
