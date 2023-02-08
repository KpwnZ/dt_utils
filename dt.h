#ifndef DT_H
#define DT_H

#include <stdint.h>
#include <stdlib.h>
#define kPropNameLength 32

struct List {
    void *data;
    struct List *next;
};
typedef struct List List;

typedef struct __attribute__((packed, aligned(1))) {
    uint8_t name[kPropNameLength];
    uint32_t length;
    uint8_t *value;
} XNUDTProp;

typedef struct {
    uint32_t nprops;
    uint32_t nchildren;
    List *properties;
    List *children;
} XNUDTNode;

#define ALIGN(a) (((uint64_t)a + 0x3) & ~0x3)

#define PRINT_STYLE_INT 0
#define PRINT_STYLE_HEX 1
#define PRINT_STYLE_STR 2

List *list_append(List *list, void *data);
void style_print_data(uint8_t *prop_name, uint8_t *data, size_t len);
XNUDTProp *arm_read_xnu_devicetree_prop(uint8_t **blob);
XNUDTNode *arm_do_parse_xnu_devicetree(uint8_t **blob);
XNUDTProp *arm_search_xnu_devicetree_prop_by_name(XNUDTNode *node, const uint8_t *name);
XNUDTProp *arm_get_xnu_devicetree_prop(XNUDTNode *node, const uint8_t *name);
XNUDTProp *arm_set_xnu_devicetree_prop(XNUDTProp *n, const uint8_t *name, uint32_t size, uint8_t *val);
void arm_print_xnu_devicetree_node(XNUDTNode *node, int depth);
void arm_add_xnu_devicetree_prop(XNUDTNode *root, const char *name, uint32_t len, const char *value, const char *path);
void arm_save_devicetree(XNUDTNode *root, const char *path);
XNUDTNode *arm_parse_xnu_devicetree(uint8_t *blob);
#endif
