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


int get_data_object(amxb_bus_ctx_t *bus_ctx_usp, const char* data_obj_path){
    printf("GET DATA OBJECT\n");
    printf("Getting data object in path: %s\n", data_obj_path);
    amxc_var_t values;
    amxc_var_init(&values);  

    int rv = amxb_get(bus_ctx_usp, data_obj_path, 0, &values, 1);
    amxc_var_dump(&values, STDOUT_FILENO); 

    /*Clean vars*/
    amxc_var_clean(&values);
}
