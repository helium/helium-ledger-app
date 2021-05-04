#include "bolos_target.h"

#ifdef HAVE_UX_FLOW

#include "ux.h"

ux_state_t G_ux;
bolos_ux_params_t G_ux_params;

////////////////
// MENU ABOUT:
const ux_flow_step_t * const menu_main[];

UX_FLOW_DEF_NOCB(
  menu_about_version,
  bn,
  {
    "Version",
     APPVERSION
  });

UX_FLOW_DEF_VALID(
  menu_about_back,
  nnn,
  ux_flow_init(0, menu_main, NULL),
  {
    NULL,
    "Back",
    NULL,
  });

UX_DEF(menu_about,
       &menu_about_version,
       &menu_about_back
       );

////////////////
// MENU :
UX_FLOW_DEF_NOCB(
  menu_main_waiting_commands_step,
  nn,
  {
    "Waiting for",
    "commands..."
  });

UX_FLOW_DEF_VALID(
  menu_main_about_step,
  nnn,
  ux_flow_init(0, menu_about, NULL),
  {
    NULL,
    "About",
    NULL,
  });

UX_FLOW_DEF_VALID(
  menu_main_quit_step,
  pb,
  os_sched_exit(-1),
  {
    &C_icon_dashboard,
    "Quit",
  });


UX_DEF(menu_main,
       &menu_main_waiting_commands_step,
       &menu_main_about_step,
       &menu_main_quit_step
       );

// ui_idle displays the main menu. Note that your app isn't required to use a
// menu as its idle screen; you can define your own completely custom screen.
void ui_idle(void) {
    if(G_ux.stack_count == 0) {
	ux_stack_push();
      }
    ux_flow_init(0, menu_main, NULL);
}



#endif
