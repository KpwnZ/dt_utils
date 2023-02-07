#include "dt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

int main(int argc, const char **argv) {
    const char *name = NULL;
    
    if (argc < 2) {
        printf("usage: %s <dtb> [-s name]\n", argv[0]);
        return 1;
    }

    if (argc == 4 && strcmp(argv[2], "-s") == 0) {
        name = argv[3];
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
    if (name) {
        XNUDTProp *n = arm_search_xnu_devicetree_prop_by_name(root, (uint8_t *)name);
        if (n) {
            printf("%s: ", n->name);
            for (int i = 0; i < n->length; ++i) {
                printf("%02x ", ((uint8_t *)n->value)[i]);
            }
        } else {
            printf("error: could not find node %s\n", name);
        }
    } else {
        arm_print_xnu_devicetree_node(root, 0);
    }
    return 0;
}
