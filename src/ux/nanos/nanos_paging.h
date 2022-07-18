#ifndef NANOS_PAGING_H
#define NANOS_PAGING_H

#define HELIUM_UX_MAX_CHARS 55
#define HELIUM_UX_MAX_TITLE 20
#define CHARS_PER_PAGE 12

typedef enum {
    FIRST,
    NEXT,
    PREV,
    LAST
} page_cmd_t;

void change_page(
        page_cmd_t page_cmd,
        void (*prev_menu)(page_cmd_t),
        void (*next_menu)(page_cmd_t)
);

void load_wallet(
        page_cmd_t page_cmd,
        void (*prev_menu)(page_cmd_t),
        void (*next_menu)(page_cmd_t)
);

#endif