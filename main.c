#include "dt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

int main(int argc, const char **argv) {
    const char *path = NULL;
    const char *name = NULL;
    const char *data = NULL;
    int len = 0;
    int op = 0;
    if (argc < 2) {
        printf(
            "usage: %s <dtb> \n"
            "-s <name>                                   search for a property with name\n"
            "   -s prop_name\n"
            "-a <nodepath> <name> <len> <data> <output>  add new property to node with nodepath\n"
            "   -a /device-tree/chosen prop_name 4 `echo '\\x12\\x34\\x56'` output.dtb\n"
            "-d <nodepath> <name> <output>               delete property from node with nodepath\n"
            "   -d /device-tree/chosen prop_name output.dtb\n",
            argv[0]);
        return 1;
    }

    if (argc == 4 && strcmp(argv[2], "-s") == 0) {
        name = argv[3];
        op = 1;
    } else if (argc == 8 && strcmp(argv[2], "-a") == 0) {
        path = argv[3];
        name = argv[4];
        len = atoi(argv[5]);
        data = argv[6];
        op = 2;
    } else if (argc == 6 && strcmp(argv[2], "-d") == 0) {
        path = argv[3];
        name = argv[4];
        op = 3;
    }

    // read dtb into memory
    FILE *fp = fopen(argv[1], "rb");
    if (fp == NULL) {
        printf("error: could not open %s\n", argv[1]);
        return 1;
    }
    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *blob = malloc(size);
    fread(blob, 1, size, fp);
    fclose(fp);

    XNUDTNode *root = arm_parse_xnu_devicetree((uint8_t *)blob);
    if (op == 1) {
        XNUDTProp *n = arm_search_xnu_devicetree_prop_by_name(root, (uint8_t *)name);
        if (n) {
            printf("%s: ", n->name);
            for (int i = 0; i < n->length; ++i) {
                printf("%02x ", ((uint8_t *)n->value)[i]);
            }
        } else 
            printf("error: could not find node %s\n", name);
    } else if (op == 2) {
        arm_add_xnu_devicetree_prop(root, name, len, data, path);
        arm_save_devicetree(root, argv[7]);
    } else if (op == 3) {
        arm_remove_xnu_devicetree_prop(root, name, path);
        arm_save_devicetree(root, argv[5]);
    } else {
        arm_print_xnu_devicetree_node(root, 0);
    }
    return 0;
}
