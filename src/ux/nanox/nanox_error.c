#include "bolos_target.h"

#ifdef HAVE_UX_FLOW

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <os.h>
#include <os_io_seproxyhal.h>
#include <cx.h>
#include "helium.h"
#include "helium_ux.h"
#include "save_context.h"

UX_FLOW_DEF_VALID(
    ux_display_error, 
    bnnn_paging,
    ui_idle(),
    {
      .title = "ADPU Payload Error",
      .text = "companion app sent wrong size"
    });


UX_DEF(ux_display_error_flow,
  &ux_display_error
);

void ui_displayError(void)
{
  if(G_ux.stack_count == 0) {
    ux_stack_push();
  }
  ux_flow_init(0, ux_display_error_flow, NULL);
}

#endif