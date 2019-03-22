#include <rhino/udi/udi.h>

linked_list_t* graphics_list;
linked_list_t* disk_list;

udi_graphics_driver_t* active_driver;


void udi_initialize(){
    graphics_list = linked_list_create();
    disk_list = linked_list_create();
}

void udi_register_graphics_driver(udi_graphics_driver_t* driver){
    debug_log_number_hex((uint32_t)driver);
    linked_list_insert(graphics_list, driver);
}

void udi_register_disk_driver(udi_disk_driver_t* driver){
    linked_list_insert(disk_list, driver);
}   

udi_graphics_driver_t* udi_get_active_graphics_driver(){
    return active_driver;
}

udi_graphics_driver_t* udi_get_graphics_driver_by_name(char* name){
    for(linked_list_node_t* node = graphics_list->head; node != NULL; node = node->next){
        udi_graphics_driver_t* drv = node->data;

        if(strcmp(drv->name, name) == 0) return drv;
    }

    return NULL;
}

void udi_set_active_graphics_driver(udi_graphics_driver_t* driver){
    if(active_driver) active_driver->disable_driver();
    if(linked_list_find(graphics_list, driver)){
        active_driver = driver;
    } else {

        linked_list_insert(graphics_list, driver);

        active_driver = driver;
    }

    active_driver->enable_driver();

}

udi_disk_driver_t* udi_get_disk_driver_by_name(char* name){
    for(linked_list_node_t* node = disk_list->head; node != NULL; node = node->next){
        udi_disk_driver_t* dsk = node->data;

        if(strcmp(dsk->name, name) == 0) return dsk;
    }

    return NULL;
}

udi_disk_driver_t* udi_get_disk_driver_by_index(uint32_t index){
    uint32_t i = 0;
    for(linked_list_node_t* node = disk_list->head; node != NULL; node = node->next, i++){
        if(i == index){
            return node->data;
        }
    }

    return NULL;
}