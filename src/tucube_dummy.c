#include <dlfcn.h>
#include <err.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <tucube/tucube_Module.h>
#include <tucube/tucube_IModule.h>
#include <tucube/tucube_ICore.h>
#include <libgenc/genc_Tree.h>
#include "tucube_IDummy.h"

struct tucube_dummy_Interface {
    TUCUBE_IDUMMY_FUNCTION_POINTERS;
};

struct tucube_dummy_LocalModule {
    const char* message;
    int interval;
};

TUCUBE_IMODULE_FUNCTIONS;
TUCUBE_ICORE_FUNCTIONS;
TUCUBE_IDUMMY_FUNCTIONS;

int tucube_IModule_init(struct tucube_Module* module, struct tucube_Config* config, void* args[]) {
    warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    module->localModule.pointer = malloc(1 * sizeof(struct tucube_dummy_LocalModule));

    struct tucube_dummy_LocalModule* localModule = module->localModule.pointer;
    TUCUBE_CONFIG_GET(config, module->id, "tucube_dummy.message", string, &(localModule->message), "I HAVE NOTHING TO SAY");
    TUCUBE_CONFIG_GET(config, module->id, "tucube_dummy.interval", integer, &(localModule->interval), 1);

    struct tucube_Module_Ids childModuleIds;
    GENC_ARRAY_LIST_INIT(&childModuleIds);
    TUCUBE_CONFIG_GET_CHILD_MODULE_IDS(config, module->id, &childModuleIds);
    GENC_TREE_NODE_FOR_EACH_CHILD(module, index) {
        struct tucube_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, index);
        childModule->interface = malloc(1 * sizeof(struct tucube_dummy_Interface));
        struct tucube_dummy_Interface* moduleInterface = childModule->interface;
        if((moduleInterface->tucube_IDummy_service = dlsym(childModule->dlHandle, "tucube_IDummy_service")) == NULL) {
            warnx("%s: %u: Unable to find tucube_IDummy_service()", __FILE__, __LINE__);
            return -1;
        }
    }
    return 0;
}

int tucube_ICore_service(struct tucube_Module* module, void* args[]) {
    struct tucube_dummy_LocalModule* localModule = module->localModule.pointer;
    struct tucube_Module* parentModule = GENC_TREE_NODE_GET_PARENT(module);
    while(true) {
        warnx("Module message: %s", localModule->message);
        warnx("ID of my parent module is %s", parentModule->id); 
        GENC_TREE_NODE_FOR_EACH_CHILD(module, index) {
            struct tucube_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, index);
            struct tucube_dummy_Interface* moduleInterface = childModule->interface;
            moduleInterface->tucube_IDummy_service(childModule);
        }
        sleep(localModule->interval);
    }
    return 0;
}

int tucube_IDummy_service(struct tucube_Module* module) {
    struct tucube_dummy_LocalModule* localModule = module->localModule.pointer;
    struct tucube_Module* parentModule = GENC_TREE_NODE_GET_PARENT(module);
    warnx("Module message: %s", localModule->message);
    warnx("ID of my parent module is %s", parentModule->id); 
    GENC_TREE_NODE_FOR_EACH_CHILD(module, index) {
        struct tucube_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, index);
        struct tucube_dummy_Interface* moduleInterface = childModule->interface;
        moduleInterface->tucube_IDummy_service(childModule);
    }
    
    return 0;
}

int tucube_IModule_destroy(struct tucube_Module* module) {
struct tucube_dummy_LocalModule* localModule = module->localModule.pointer;
    warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    GENC_TREE_NODE_FOR_EACH_CHILD(module, index) {
        struct tucube_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, index);
        free(childModule->interface);
    }
    free(module->tlModuleKey);
    free(module->localModule.pointer);
    free(module);
//    dlclose(module->dl_handle);
    return 0;
}
