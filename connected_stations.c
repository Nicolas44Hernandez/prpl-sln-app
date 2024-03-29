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

int total_connected_stations;
uintmax_t last_sample_timestamp;

#define CONNECTED_STATIONS_RESULTS_FILE "connected_stations_result.txt"

#define MAX_NB_CONNECTED_STATIONS 20

struct station
{   
    char* mac;
    int32_t last_sample_tx_bytes;
    int32_t last_sample_rx_bytes;
    uint32_t signal_strength;
    char* band;
};

struct station connected_stations_list[MAX_NB_CONNECTED_STATIONS];

void init_stations_counters(){
    /*Init countesrs variables*/
    total_connected_stations = 0;
    for(int i = 0; i< MAX_NB_CONNECTED_STATIONS; i++){
        connected_stations_list[i].mac = malloc(20);
        connected_stations_list[i].band = malloc(20);
        strcpy(connected_stations_list[i].mac, "");
        strcpy(connected_stations_list[i].band, "");
        connected_stations_list[i].last_sample_tx_bytes = -1;
        connected_stations_list[i].last_sample_rx_bytes = -1;
        connected_stations_list[i].signal_strength = -1;
    }
    return 0;  
}


void reset_station_counters(int station_nb){
    /*Reset countes for station */    
    strcpy(connected_stations_list[station_nb].mac, "");
    strcpy(connected_stations_list[station_nb].band, "");
    connected_stations_list[station_nb].last_sample_tx_bytes = -1;
    connected_stations_list[station_nb].last_sample_rx_bytes = -1;
    connected_stations_list[station_nb].signal_strength = -1;   
    
    /* Decrement connected stations counter*/
    total_connected_stations -= 1;     
    return 0;  
}


void update_station(char* mac_address, int32_t rx_bytes, int32_t tx_bytes, int32_t signal_strength, char* band ){
    /*Update station atributes or create if not pressent*/
    // printf("*****Updating station - MACAddress:%s  RxBytes:%d   TxBytes:%d SignalStrength:%d\n", 
    //     mac_address, 
    //     rx_bytes, 
    //     tx_bytes,
    //     signal_strength
    // ); 

    /* Iterate over active stations list to find station*/
    for(int i =0; i < MAX_NB_CONNECTED_STATIONS; i++){
        /* Check if station is not empty*/
        if( strcmp(connected_stations_list[i].mac, "") !=  0 ){
            /* Check if station is not empty*/
            if( strcmp(connected_stations_list[i].mac, mac_address) ==  0 ){
                //printf("The station is already present in active stations list, updating counters...\n");
                connected_stations_list[i].last_sample_tx_bytes = tx_bytes;
                connected_stations_list[i].last_sample_rx_bytes = rx_bytes;
                connected_stations_list[i].signal_strength = signal_strength;
                strcpy(connected_stations_list[i].band, band); 
                return 0;
            } 
        }
    }

    /*Create station in active stations list*/
    //printf("Appendig station to active stations list...\n");
    for(int i =0; i < MAX_NB_CONNECTED_STATIONS; i++){
        /*If station is empty overwrite*/
        if( strcmp(connected_stations_list[i].mac, "") ==  0 ){
            strcpy(connected_stations_list[i].mac, mac_address);
            connected_stations_list[i].last_sample_tx_bytes = tx_bytes;
            connected_stations_list[i].last_sample_rx_bytes = rx_bytes;
            connected_stations_list[i].signal_strength = signal_strength;
            strcpy(connected_stations_list[i].band, band); 
            total_connected_stations += 1;
            return 0;
        }
    }    

    /*Error ?*/
    return -1;
}


void print_stations_list(){
    /*Loop over connected stations*/
    printf("-----Connected stations list: \n");
    for(int i =0; i < MAX_NB_CONNECTED_STATIONS; i++){
        if( strcmp(connected_stations_list[i].mac, "") !=  0 ){
            printf("STATION: MACAddress:%s  RxBytes:%d   TxBytes:%d   SignalStrength:%d   Band:%s\n",
                connected_stations_list[i].mac,
                connected_stations_list[i].last_sample_rx_bytes,
                connected_stations_list[i].last_sample_tx_bytes,
                connected_stations_list[i].signal_strength,
                connected_stations_list[i].band
            );
        }        
    } 
}


void update_connected_stations_list(amxc_var_t * var, char* band){
    /*Iterate over station list*/
    amxc_var_for_each(station, var) {
        char* mac_address = GET_CHAR(station, "MACAddress");
        int32_t rx_bytes = GET_INT32(station, "RxBytes");
        int32_t tx_bytes = GET_INT32(station, "TxBytes");
        int32_t signal_strength = GET_INT32(station, "SignalStrength");
        bool active = GET_BOOL(station, "Active");
        if(active){
            /*Update or append station counters*/
            update_station(mac_address, rx_bytes, tx_bytes, signal_strength, band);
        }        
    }
}

void purge_inactive_stations(amxc_var_t * var, char* band){
    /*Purge stations from active stations list if not present in sample*/
    //printf("Purging inactive stations from list\n");

    int stations_to_delete_idx[MAX_NB_CONNECTED_STATIONS];
    int nb_of_stations_to_delete = 0;

    /*Iterate over acive stations list*/
    for(int i =0; i < MAX_NB_CONNECTED_STATIONS; i++){
        if( (strcmp(connected_stations_list[i].mac, "") !=  0) && strcmp(connected_stations_list[i].band, band) ==  0 ){
            //printf("Checking if station idx %d mac: %s is currently active\n", i, connected_stations_list[i].mac);

            /*Iterate over stations list in current sample*/
            bool station_in_sample = false;
            amxc_var_for_each(station, var) {
                char* mac_address = GET_CHAR(station, "MACAddress");
                bool active = GET_BOOL(station, "Active");
                /*Validate if station is active */
                if(active){
                    if( strcmp(connected_stations_list[i].mac, mac_address) ==  0 ){
                        //printf("The station is present in sample\n");
                        station_in_sample = true;
                    } 
                }
            }
            if(!station_in_sample){
                stations_to_delete_idx[nb_of_stations_to_delete] = i;
                nb_of_stations_to_delete ++;
            }        
        }       
    }

    // printf("Stations to delete [");
    // for(int i =0; i < nb_of_stations_to_delete; i++){ 
    //     printf("%d,", stations_to_delete_idx[i]);
    // }
    // printf("]\n");

    /*Remove stations to delete from list*/
    for(int i =0; i < nb_of_stations_to_delete; i++){ 
        reset_station_counters(stations_to_delete_idx[i]);
    }
}


int update_connected_stations_counters(amxb_bus_ctx_t *bus_ctx_usp){
    printf("GET CONNECTED STATIONS\n");

    /*Vars declaration*/
    amxc_var_t args;
    amxc_var_t ret;
    time_t rawtime;
    struct tm * timeinfo;
    int retval = 0;

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

    /*Get Current time*/   
    time ( &rawtime );
    timeinfo = localtime( &rawtime );
    char* timeinfo_str = asctime(timeinfo);
    timeinfo_str[strlen(timeinfo_str) -1] = '\0';
    char *timestmp = strcat(timeinfo_str, ",");

    /*Loop over the wifi bands*/
    for(int i=0; i<3; i++){
    
        /*Get Station Stats*/
        retval = amxb_call(bus_ctx_usp, bands_cmd_arr[i], "getStationStats", &args, &ret, 5);
        if (retval != 0) {
            printf("RPC Error: Error in RPC execution\n");
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

            /*Remove non active stations from list*/
            purge_inactive_stations(var, bands[i]);

            /*Append connected stations to list*/
            update_connected_stations_list(var, bands[i]);
        }        

        /*Clean variables*/
        amxc_var_clean(&args);
        amxc_var_clean(&ret);
        retval = 0;
    }    
}
