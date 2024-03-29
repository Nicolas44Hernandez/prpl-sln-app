#include <time.h>

// specific prpl libraries
#include <amxrt/amxrt.h>
#include <amxc/amxc.h>
#include <amxp/amxp.h>
#include <amxd/amxd_dm.h>
#include <amxd/amxd_object.h>



/* Band counters struct */
struct bandCounters {
    double tx_MBs_0;
    double tx_MBs_1;
    double tx_MBs_2;
    double tx_MBs_3;
    double tx_MBs_4;
    double tx_MBs_5;
    double tx_MBs_6;
    double rx_MBs_0;
    double rx_MBs_1;
    double rx_MBs_2;
    double rx_MBs_3;
    double rx_MBs_4;
    double rx_MBs_5;
    double rx_MBs_6;
    int band;
    int nb_of_samples;
    uintmax_t last_sample_received_timestamp;
    int32_t last_sample_bytes_received;
    int32_t last_sample_bytes_sent;
};

/* Sample struct*/
struct sample {
    int32_t bytes_received;
    int32_t bytes_sent;
    uintmax_t timestamp;
};

typedef struct bandCounters BandCounters;
typedef struct sample Sample;

/* Functions declaration */
void printBandCounters (BandCounters* band_counters);
void init_counters(BandCounters* band_counters, int band);
void init_sample(Sample* sample);
void getSampleForBandCounter(amxb_bus_ctx_t *bus_ctx_usp, Sample* sample, int band);
void appendNewSample(BandCounters* band_counters, Sample* sample);


void printBandCounters(BandCounters* band_counters){
    /* Print band number */
    printf("---Band %d counters: \n", band_counters->band);

    /* Print number of samples */
    printf("nb_of_samples:%d\n", band_counters->nb_of_samples);

    /*Print last sample received values*/
    printf("last_sample_received_timestamp:%ju   ", band_counters->last_sample_received_timestamp);
    printf("last_sample_bytes_received:%d   ", band_counters->last_sample_bytes_received);
    printf("last_sample_bytes_sent:%d\n", band_counters->last_sample_bytes_sent);
    
    /*Print tx_MBs*/
    printf("tx_MBs:[");
    printf("%f,", band_counters->tx_MBs_0);
    printf("%f,", band_counters->tx_MBs_1);
    printf("%f,", band_counters->tx_MBs_2);
    printf("%f,", band_counters->tx_MBs_3);
    printf("%f,", band_counters->tx_MBs_4);
    printf("%f,", band_counters->tx_MBs_5);
    printf("%f,", band_counters->tx_MBs_6);
    printf("]\n");

    /*Print rx_MBs*/
    printf("rx_MBs:[");
    printf("%f,", band_counters->rx_MBs_0);
    printf("%f,", band_counters->rx_MBs_1);
    printf("%f,", band_counters->rx_MBs_2);
    printf("%f,", band_counters->rx_MBs_3);
    printf("%f,", band_counters->rx_MBs_4);
    printf("%f,", band_counters->rx_MBs_5);
    printf("%f,", band_counters->rx_MBs_6);
    printf("]\n");   
}

void printSample(Sample* sample){
    /*Print sample fields*/
    printf("Sample ");
    printf("bytes_received:%d   ", sample->bytes_received);
    printf("bytes_sent:%d   ", sample->bytes_sent);
    printf("timestamp:%ju\n", sample->timestamp);
}

void init_counters(BandCounters* band_counters, int band){
    printf("Counters initialization for [band:%d]\n", band);

    /* Healt check band*/
    if((band < 0) || (band > 2)){
        printf("Error: Invalid band in init counters [band:%d]", band);
        return -1;
    }

    /* Init tx_MBs, rx_MBs */
    band_counters->tx_MBs_0 = -1;
    band_counters->tx_MBs_1 = -1;
    band_counters->tx_MBs_2 = -1;
    band_counters->tx_MBs_3 = -1;
    band_counters->tx_MBs_4 = -1;
    band_counters->tx_MBs_5 = -1;
    band_counters->tx_MBs_6 = -1;
    band_counters->rx_MBs_0 = -1;
    band_counters->rx_MBs_1 = -1;
    band_counters->rx_MBs_2 = -1;
    band_counters->rx_MBs_3 = -1;
    band_counters->rx_MBs_4 = -1;
    band_counters->rx_MBs_5 = -1;
    band_counters->rx_MBs_6 = -1;

    /* Init last sample received values*/
    band_counters->last_sample_received_timestamp = 0;
    band_counters->last_sample_bytes_received = 0;
    band_counters->last_sample_bytes_sent = 0;

    /* Init number of samples*/
    band_counters->nb_of_samples = 0;

    /* Inint band*/
    band_counters->band=band;
}

void init_sample(Sample* sample){
    sample->bytes_sent = 0;
    sample->bytes_received = 0;
    sample->timestamp = 0;
}


void appendNewSample(BandCounters* band_counters, Sample* sample){   
    
    printf("Appending sample bytes_sent:%d, bytes_received:%d, timestamp:%ju\n", 
        sample->bytes_sent, 
        sample->bytes_received, 
        sample->timestamp
    );
    
    /*For first sample only take values for calculate in future samples*/
    printf("band_counters->nb_of_samples: %d \n", band_counters->nb_of_samples);
    if(band_counters->nb_of_samples == 0){
        printf("FIRST SAMPLE !!!! \n");
        /* Store last frame received byte counters and timestamp*/
        band_counters->last_sample_bytes_sent = sample->bytes_sent;
        band_counters->last_sample_bytes_received = sample->bytes_received;
        band_counters->last_sample_received_timestamp = sample->timestamp;        

        /*Increment nb of samples*/
        band_counters->nb_of_samples = 1; 
        return 0;     
    }
    /*If counter array is already filled*/
    else{

        /* Calculate deltas */
        int32_t deltatime = sample->timestamp - band_counters->last_sample_received_timestamp;
        int32_t delta_tx_bytes = sample->bytes_sent - band_counters->last_sample_bytes_sent;
        int32_t delta_rx_bytes = sample->bytes_received - band_counters->last_sample_bytes_received;
        printf("DELTATIME:%d  DELTA_TX_BYTES:%d   DELTA_RX_BYTES:%d\n", deltatime, delta_tx_bytes, delta_rx_bytes);

        /* Calculate traffic from sample*/
        //TODO: Mbps
        double tx_MBs_sample = (double)(delta_tx_bytes * 8 )/ (deltatime * 1000000); 
        double rx_MBs_sample = (double)(delta_rx_bytes * 8 ) / (deltatime * 1000000);
        printf("CALCULATED TX_MBs_SAMPLE:%f  RX_MBs_SAMPLE:%f\n", tx_MBs_sample, rx_MBs_sample);
        
        /* Append calculated values to counters*/

        switch ( band_counters->nb_of_samples ) {
            case 1:
                band_counters->tx_MBs_0 = tx_MBs_sample;
                band_counters->rx_MBs_0 = rx_MBs_sample;
                break;
            case 2:
                band_counters->tx_MBs_1 = tx_MBs_sample;
                band_counters->rx_MBs_1 = rx_MBs_sample;
                break;
            case 3:
                band_counters->tx_MBs_2 = tx_MBs_sample;
                band_counters->rx_MBs_2 = rx_MBs_sample;
                break;
            case 4:
                band_counters->tx_MBs_3 = tx_MBs_sample;
                band_counters->rx_MBs_3 = rx_MBs_sample;
                break;
            case 5:
                band_counters->tx_MBs_4 = tx_MBs_sample;
                band_counters->rx_MBs_4 = rx_MBs_sample;
                break;
            case 6:
                band_counters->tx_MBs_5 = tx_MBs_sample;
                band_counters->rx_MBs_5 = rx_MBs_sample;
                break;
            case 7:
                band_counters->tx_MBs_6 = tx_MBs_sample;
                band_counters->rx_MBs_6 = rx_MBs_sample;
                break;
            default:
                /*Left shift tx_MBs counters */
                band_counters->tx_MBs_0 = band_counters->tx_MBs_1;
                band_counters->tx_MBs_1 = band_counters->tx_MBs_2;
                band_counters->tx_MBs_2 = band_counters->tx_MBs_3;
                band_counters->tx_MBs_3 = band_counters->tx_MBs_4;
                band_counters->tx_MBs_4 = band_counters->tx_MBs_5;
                band_counters->tx_MBs_5 = band_counters->tx_MBs_6;
                band_counters->tx_MBs_6 = tx_MBs_sample;

                /*Left shift tx_MBs counters */
                band_counters->rx_MBs_0 = band_counters->rx_MBs_1;
                band_counters->rx_MBs_1 = band_counters->rx_MBs_2;
                band_counters->rx_MBs_2 = band_counters->rx_MBs_3;
                band_counters->rx_MBs_3 = band_counters->rx_MBs_4;
                band_counters->rx_MBs_4 = band_counters->rx_MBs_5;
                band_counters->rx_MBs_5 = band_counters->rx_MBs_6;
                band_counters->rx_MBs_6 = rx_MBs_sample;
        }

        /* Update last sample received counters*/
        band_counters->last_sample_bytes_sent = sample->bytes_sent;
        band_counters->last_sample_bytes_received = sample->bytes_received;
        band_counters->last_sample_received_timestamp = sample->timestamp; 

        /*Increment nb of samples*/
        if(band_counters->nb_of_samples < 8){
            band_counters->nb_of_samples = band_counters->nb_of_samples + 1;
        }

        return 0;
    }
    
}

void getSampleForBandCounter(amxb_bus_ctx_t *bus_ctx_usp, Sample* sample, int band)
{   
    printf("Updating WiFi.Radio.%d counters\n", band);

    /* Declare vars*/
    amxc_var_t args;
    amxc_var_t ret;
    int retval = 0;
    time_t rawtime;
    int32_t bytes_sent; 
    int32_t bytes_received;

    /*Init vars*/
    amxc_var_init(&args);
    amxc_var_init(&ret);
    bytes_sent = 0;
    bytes_received = 0;

    /* Healt check band*/
    if((band < 0) || (band > 2)){
        printf("Error: Invalid band in counters struct [band:%d]", band);
        return -1;
    }

    const char *bands_cmd_arr[] = { 
        "WiFi.Radio.1.",  // 2.4 GHz
        "WiFi.Radio.2.",  // 5 GHz
        "WiFi.Radio.3."   // 6 GHz
    }; 

    /*Get Timestamp*/
    rawtime = time(NULL);
    uintmax_t timestamp = (uintmax_t)rawtime;

    /*Get Radio Stats*/
    retval = amxb_call(bus_ctx_usp, bands_cmd_arr[band], "getRadioStats", &args, &ret, 5);
    if (retval != 0) {
        printf("RPC Error: Error in RPC execution\n");
        amxc_var_clean(&args);
        amxc_var_clean(&ret);
        return -1;
    }


    /*Extract result*/
    amxc_var_for_each(var, &ret) {

        char *var_str = amxc_var_dyncast(cstring_t, var);
        if(strlen(var_str) < 10){
            continue;
        }  

        // Get the value
        bytes_sent = GET_INT32(var, "BytesSent");
        bytes_received = GET_INT32(var, "BytesReceived");
        
        // printf("BytesSent: %d\n", bytes_sent);
        // printf("BytesReceived: %d\n", bytes_received);        
    }   

    /* Assign values to sample */
    sample->bytes_received = bytes_received;
    sample->bytes_sent = bytes_sent;
    sample->timestamp = timestamp;
    
    /* Clean vars*/
    amxc_var_clean(&args);
    amxc_var_clean(&ret);
}
