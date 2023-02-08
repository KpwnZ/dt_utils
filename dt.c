#include "dt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

XNUDTNode *arm_parse_xnu_devicetree(uint8_t *blob) {
    XNUDTNode *root = arm_do_parse_xnu_devicetree(&blob);
    return root;
}

XNUDTNode *arm_do_parse_xnu_devicetree(uint8_t **blob) {
    XNUDTNode *node = calloc(sizeof(XNUDTNode), 1);
    memcpy(node, *blob, sizeof(XNUDTNode) - sizeof(List *) * 2);
    *blob += sizeof(XNUDTNode) - sizeof(List *) * 2;

    if (node->nprops == 0) {
        printf("[!] invalid node\n");
        abort();
    }
    for (int i = 0; i < node->nprops; ++i) {
        XNUDTProp *prop = arm_read_xnu_devicetree_prop(blob);
        node->properties = list_append(node->properties, prop);
    }
    for (int i = 0; i < node->nchildren; ++i) {
        XNUDTNode *child = arm_do_parse_xnu_devicetree(blob);
        node->children = list_append(node->children, child);
    }
    return node;
}

XNUDTProp *arm_read_xnu_devicetree_prop(uint8_t **blob) {
    XNUDTProp *prop = malloc(sizeof(XNUDTProp));
    memcpy(prop->name, *blob, kPropNameLength);
    *blob += kPropNameLength;
    prop->length = *(uint32_t *)*blob & ~0x80000000;
    *blob += sizeof(uint32_t);
    if (prop->length > 0) {
        prop->value = malloc(prop->length);
        memcpy(prop->value, *blob, prop->length);
        *blob += prop->length;
    }
    // well, this is not likely to happen
    *blob = (uint8_t *)(((uint64_t)*blob + 0x3) & ~0x3);
    return prop;
}

XNUDTProp *arm_search_xnu_devicetree_prop_by_name(XNUDTNode *node, const uint8_t *name) {
    assert(node != NULL);
    assert(name != NULL);
    XNUDTProp *prop = NULL;
    if ((prop = arm_get_xnu_devicetree_prop(node, name))) {
        return prop;
    }
    for (List *l = node->children; l != NULL; l = l->next) {
        if ((prop = arm_search_xnu_devicetree_prop_by_name((XNUDTNode *)l->data, name))) {
            return prop;
        }
    }
    return NULL;
}

XNUDTProp *arm_get_xnu_devicetree_prop(XNUDTNode *node, const uint8_t *name) {
    for (List *l = node->properties; l != NULL; l = l->next) {
        XNUDTProp *prop = (XNUDTProp *)l->data;
        if (strncmp((const char *)prop->name, (const char *)name, kPropNameLength) == 0) {
            return prop;
        }
    }
    return NULL;
}

void arm_print_xnu_devicetree_node(XNUDTNode *node, int depth) {
    assert(node != NULL);
    XNUDTProp *prop = NULL;
    for (List *l = node->properties; l != NULL; l = l->next) {
        if (strncmp((const char *)((XNUDTProp *)l->data)->name, "name", kPropNameLength) == 0) {
            for (int i = 0; i < depth; ++i) {
                printf("  ");
            }
            printf("%s: \n", ((XNUDTProp *)l->data)->value);
            break;
        }
    }
    for (List *l = node->properties; l != NULL; l = l->next) {
        prop = (XNUDTProp *)l->data;
        for (int i = 0; i < depth + 1; ++i) {
            printf("  ");
        }
        printf("%s (%d): ", prop->name, prop->length);
        style_print_data(prop->name, prop->value, prop->length);
        printf("\n");
    }
    for (List *l = node->children; l != NULL; l = l->next) {
        arm_print_xnu_devicetree_node((XNUDTNode *)l->data, depth + 1);
    }
}

int is_empty_string(uint8_t *data, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        if (data[i] != 0) {
            return 0;
        }
    }
    return 1;
}

int get_print_style(uint8_t *prop_name, uint8_t *data, size_t len) {
    // get print style based on the data
    // hexdump, string
    size_t printable = 0;
    if (prop_name[0] == '#') return PRINT_STYLE_INT;

    for (size_t i = 0; i < len; ++i) {
        if (data[i] >= 0x20 && data[i] <= 0x7e) {
            printable++;
        }
    }
    if (printable / (float)len > 0.6) {
        return PRINT_STYLE_STR;
    }
    return PRINT_STYLE_HEX;
}

void style_print_data(uint8_t *prop_name, uint8_t *data, size_t len) {
    int style = get_print_style(prop_name, data, len);
    switch (style) {
        case PRINT_STYLE_HEX:
            if (is_empty_string(data, len)) {
                printf("\"\"");
                break;
            }
            for (size_t i = 0; i < len; ++i) {
                printf("%02x ", (uint8_t)data[i]);
            }
            break;
        case PRINT_STYLE_STR:
            for (size_t i = 0; i < len; ++i) {
                if (data[i] >= 0x20 && data[i] <= 0x7e)
                    printf("%c", data[i]);
                else if (i == len - 1 && data[i] == 0)
                    break;
                else
                    printf("\\%02x", (uint8_t)data[i]);
            }
            break;
        case PRINT_STYLE_INT:
            printf("0x%llx", *(uint64_t *)data);
            break;
        default:
            break;
    }
}
