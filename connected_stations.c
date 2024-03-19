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

#define BUFF_SIZE 10000
#define BUFF_CONNETED_STATIONS_SIZE 100

int connected_stations_2GHz;
int connected_stations_5GHz;
int connected_stations_6GHz;

#define CONNECTED_STATIONS_RESULTS_FILE "connected_stations_result.txt"

int count_active_stations(const char *stations_str){
    const char *needle = "Active:true";
    int count = 0;
    const char *tmp = stations_str;
    while(tmp = strstr(tmp, needle))
    {   
        count++;
        tmp++;
    }
    return count;
}

int count_nb_stations(const char *stations_str){
    const char *needle = "MACAddress:";
    int count = 0;
    const char *tmp = stations_str;
    while(tmp = strstr(tmp, needle))
    {   
        count++;
        tmp++;
    }
    return count;
}

int dump_results_for_single_station(
    const char* station, 
    const char* band, 
    const char *timestmp
){  

    /*Variables*/
    char station_str[BUFF_SIZE];
    station_str[0] =  '\0';
    int retval = 0;

    /*Append timestamp header*/
    char *ts_header = "{timestamp:";
    strcat(station_str, ts_header);
    strcat(station_str, timestmp);

    /*Append band*/ 
    char *band_header = "band:";
    strcat(station_str, band_header);
    strcat(station_str, band);
    strcat(station_str, ",");


    /*Append station*/ 
    strcat(station_str, station);

    /*Append end char*/
    strcat(station_str, "}\n");

    /*Extract station MAC address*/
    const char *needle = "MACAddress:";
    char station_mac[30];
    station_mac[0] =  '\0';
    char *station_tmp = station;
    while(station_tmp = strstr(station_tmp, needle))
    {      
        station_tmp=station_tmp+11;
        station_tmp[17] =  '\0';        
        strcat(station_mac, station_tmp);
        break;
    }

    /*Use MAC to construct station result file name*/
    for(int i = 2; i<16; i=i+3){
        station_mac[i] =  '_'; 
    }
    char station_stats_result_file[40];
    station_stats_result_file[0] =  '\0';
    strcat(station_stats_result_file, "station_");
    strcat(station_stats_result_file, station_mac);
    strcat(station_stats_result_file, ".txt");
    printf("RESULTS FOR STATION IN: %s \n", station_stats_result_file);

    /*Append station to results file*/    
    retval = append_to_file(station_str, station_stats_result_file);
    if (retval == 0) {
        printf("Error when writting to file\n");
    }

    /*Print station results for debug*/
    //printf("STATION TO APPEND TO FILE: %s \n\n\n", station_str);

    return 0;
}

int dump_connected_stations_nb(const char *timestmp){
    int total_connected_stations = connected_stations_2GHz + connected_stations_5GHz + connected_stations_6GHz;
    printf("Conneted stations - Total:%d,2.4GHZ:%d,5GHz:%d,6GHz:%d\n", total_connected_stations,connected_stations_2GHz,connected_stations_5GHz, connected_stations_6GHz);
        
    /*Variables*/
    char connected_stations_str[BUFF_CONNETED_STATIONS_SIZE];
    connected_stations_str[0] =  '\0';

    /*Append timestamp header*/
    char *ts_header = "{timestamp:";
    strcat(connected_stations_str, ts_header);
    strcat(connected_stations_str, timestmp);

    /*Append connected statios info*/
    char entry[50];
    snprintf(entry, sizeof entry, "Total:%d,2.4GHZ:%d,5GHz:%d,6GHz:%d", total_connected_stations,connected_stations_2GHz,connected_stations_5GHz, connected_stations_6GHz);
    strcat(connected_stations_str, entry);
    
    /*Append end char*/
    strcat(connected_stations_str, "}\n");

    /*Append station to results file*/    
    int retval = append_to_file(connected_stations_str, CONNECTED_STATIONS_RESULTS_FILE);
    if (retval == 0) {
        printf("Error when writting to file\n");
        return -1;
    }

    return 0;
}

int dump_connected_stations_results(const char* stations_str, const char* band, const char *timestmp){
    
    /*Variables*/
    char connected_stations_str[BUFF_SIZE];
    connected_stations_str[0] =  '\0';

    /*Append timestamp header*/
    char *ts_header = "{timestamp:";
    strcat(connected_stations_str, ts_header);
    strcat(connected_stations_str, timestmp); 
    
    /* Get number of stations*/
    int nb_active_stations = count_active_stations(stations_str);
    int nb_stations_in_list = count_nb_stations(stations_str);
    if(!strcmp(band,"2.4GHz")){
        connected_stations_2GHz = nb_active_stations;
    }
    if(!strcmp(band,"5GHz")){
        connected_stations_5GHz = nb_active_stations;
    }
    if(!strcmp(band,"6GHz")){
        connected_stations_6GHz = nb_active_stations;
    }

    printf(
        "UPDATED SATIONS NB -   2.4GHz:%d    5GHz:%d   6GHz:%d \n", 
        connected_stations_2GHz, 
        connected_stations_5GHz, 
        connected_stations_6GHz
    );  

    /*loop over stations*/
    const char *needle = "AuthenticationState";
    char *station_tmp = stations_str;
    
    //printf("ALL THE STATIONS: %s \n\n\n", stations_str);
    int stations_done = 0;
    while(stations_done<nb_stations_in_list){
        int current_total_len = strlen(station_tmp);

        char last_station_str[BUFF_SIZE];
        last_station_str[0] =  '\0';

        while(station_tmp = strstr(station_tmp, needle))
        {      
            last_station_str[0] =  '\0';
            strcat(last_station_str, station_tmp);
            station_tmp++;
        }
                
        /*Get current station len*/
        int station_str_len;
        station_str_len = strlen(last_station_str);

        /*Calculate new len*/
        int new_len;
        new_len = current_total_len - station_str_len;
        
        /*Get new pointer*/
        if(stations_done<nb_stations_in_list){
            station_tmp = stations_str;
            station_tmp[new_len-1] = '\0';
        }

        /*Increment statios done*/
        stations_done++;

        /*Dump results for station*/
        dump_results_for_single_station(last_station_str, band, timestmp); 
    }
        
    return 0;
}

int get_connected_stations(amxb_bus_ctx_t *bus_ctx_usp){
    printf("GET CONNECTED STATIONS\n");

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
    const char *bands[] = { 
        "5GHz",
        "2.4GHz",        
        "6GHz",
    };   

    /*Commands bands array*/
    const char *bands_cmd_arr[] = { 
        "WiFi.AccessPoint.1.", 
        "WiFi.AccessPoint.2.", 
        "WiFi.AccessPoint.3."
    }; 
    
    /*Init args*/
    amxc_var_init(&args);
    amxc_var_init(&ret); 
    connected_stations_2GHz=0;
    connected_stations_5GHz=0;
    connected_stations_6GHz=0;

    /*Get Current time*/   
    time ( &rawtime );
    timeinfo = localtime( &rawtime );
    char* timeinfo_str = asctime(timeinfo);
    timeinfo_str[strlen(timeinfo_str) -1] = '\0';
    char *timestmp = strcat(timeinfo_str, ",");

    /*Append timestamp header*/
    char *ts_header = "{timestamp:";
    strcat(stats_str, ts_header);

    /*Loop over the wifi bands*/
    for(int i=0; i<3; i++){
    
        /*Get Station Stats*/
        retval = amxb_call(bus_ctx_usp, bands_cmd_arr[i], "getStationStats", &args, &ret, 5);
        if (retval != 0) {
            printf("RPC Error: Error in RPC execution\n");
            amxb_free_invoke(&invoke_ctx);
            amxc_var_clean(&args);
            amxc_var_clean(&ret);
            return -1;
        }

        /*Print returned values for debug*/
        //amxc_var_dump(&ret, STDOUT_FILENO);

        amxc_var_for_each(var, &ret) {
            
            if(amxc_var_type_of(var) != AMXC_VAR_ID_LIST) {
                continue;
            }

            char *stations_list_str = amxc_var_dyncast(cstring_t, var);            
            if(strlen(stations_list_str) > 10){
                retval = dump_connected_stations_results(stations_list_str, bands[i], timestmp);
                if (retval != 0) {
                    printf("Error in reults dump\n");
                }
            }
        }        

        /*Clean variables*/
        amxb_free_invoke(&invoke_ctx);
        amxc_var_clean(&args);
        amxc_var_clean(&ret);
        retval = 0;
    }

    retval = dump_connected_stations_nb(timestmp);
    if (retval != 0) {
        printf("Error in number of connected stations reults dump\n");
    }
}
