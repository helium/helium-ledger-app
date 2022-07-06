#include <stdint.h>
#include <string.h>
#include "nanos_paging.h"
#include "save_context.h"

void change_page(
        page_cmd_t page_cmd,
        void (*prev_menu)(page_cmd_t),
        void (*next_menu)(page_cmd_t)) {
    int page;
    switch (page_cmd) {
        case FIRST:
            page = (global.fullStr_len/CHARS_PER_PAGE - ((global.fullStr_len%CHARS_PER_PAGE) ? 0 : 1));
            global.title[global.title_len-3] = page + 1 + 48;
            global.title[global.title_len-5] = '1';
            if(page == 0) {
                global.title[global.title_len-7] = '\0';
            }
            global.displayIndex = 0;
            if (global.fullStr_len >= CHARS_PER_PAGE) {
                memcpy(global.partialStr, &global.fullStr[global.displayIndex], CHARS_PER_PAGE);
                (global.partialStr)[CHARS_PER_PAGE]='\0';
            } else {
                memcpy(global.partialStr, &global.fullStr[global.displayIndex], global.fullStr_len);
                (global.partialStr)[global.fullStr_len]='\0';
            }
            break;
        case LAST:
            if (global.fullStr_len <= CHARS_PER_PAGE) {
                change_page(
                        FIRST,
                        prev_menu,
                        next_menu);
            } else {
                page = (global.fullStr_len / CHARS_PER_PAGE - ((global.fullStr_len % CHARS_PER_PAGE) ? 0 : 1));
                global.displayIndex = page * CHARS_PER_PAGE;
                global.title[global.title_len - 3] = page + 1 + 48;
                global.title[global.title_len - 5] = page + 1 + 48;
                int diff = global.fullStr_len - global.displayIndex;
                memcpy(global.partialStr, &global.fullStr[global.displayIndex], diff);
                global.partialStr[diff] = '\0';
            }
            break;
        case PREV:
            if (global.displayIndex == 0) {
                if (prev_menu != 0){
                    (*prev_menu)(LAST);
                }
            } else {
                global.displayIndex -= CHARS_PER_PAGE;
                page = (global.displayIndex/CHARS_PER_PAGE);
                global.title[global.title_len-5] = page + 1 + 48;
                memcpy(global.partialStr, &global.fullStr[global.displayIndex], CHARS_PER_PAGE);
                global.partialStr[CHARS_PER_PAGE]='\0';
            }
            break;
        case NEXT:
            global.displayIndex += CHARS_PER_PAGE;
            page = (global.displayIndex/CHARS_PER_PAGE);
            global.title[global.title_len - 5] = page + 48;

            uint8_t last_page = (global.fullStr_len / CHARS_PER_PAGE - ((global.fullStr_len % CHARS_PER_PAGE) ? 0 : 1));
            global.title[global.title_len - 5] = last_page + 1 + 48;
            if (page >last_page) {
                if (next_menu != 0){
                    (*next_menu)(FIRST);
                }
            } else {
                global.title[global.title_len-5] = page + 1 + 48;

                if (global.displayIndex + CHARS_PER_PAGE > global.fullStr_len) {
                    int diff = global.fullStr_len - global.displayIndex;
                    memcpy(global.partialStr, &global.fullStr[global.displayIndex], diff);
                    global.partialStr[diff] = '\0';
                } else {
                    memcpy(global.partialStr, &global.fullStr[global.displayIndex], CHARS_PER_PAGE);
                    global.partialStr[CHARS_PER_PAGE] = '\0';
                }
            }
            break;
    }
}
