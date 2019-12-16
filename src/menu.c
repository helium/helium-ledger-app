#include "menu.h"
#include "os.h"

volatile uint8_t dummy_setting_1;
volatile uint8_t dummy_setting_2;

void display_settings(void);
void switch_dummy_setting_1_data(void);
void switch_dummy_setting_2_data(void);

//////////////////////////////////////////////////////////////////////
const char* settings_submenu_getter(unsigned int idx);
void settings_submenu_selector(unsigned int idx);

//////////////////////////////////////////////////////////////////////////////////////
// Enable contract data submenu:

void dummy_setting_1_data_change(unsigned int enabled) {
    nvm_write((void *)&N_storage.dummy_setting_1, &enabled, 1);
    ui_idle();
}

const char* const dummy_setting_1_data_getter_values[] = {
  "No",
  "Yes",
  "Back"
};

const char* dummy_setting_1_data_getter(unsigned int idx) {
  if (idx < ARRAYLEN(dummy_setting_1_data_getter_values)) {
    return dummy_setting_1_data_getter_values[idx];
  }
  return NULL;
}

void dummy_setting_1_data_selector(unsigned int idx) {
  switch(idx) {
    case 0:
      dummy_setting_1_data_change(0);
      break;
    case 1:
      dummy_setting_1_data_change(1);
      break;
    default:
      ux_menulist_init(0, settings_submenu_getter, settings_submenu_selector);
  }
}

//////////////////////////////////////////////////////////////////////////////////////
// Display contract data submenu:

void dummy_setting_2_data_change(unsigned int enabled) {
    nvm_write((void *)&N_storage.dummy_setting_2, &enabled, 1);
    ui_idle();
}

const char* const dummy_setting_2_data_getter_values[] = {
  "No",
  "Yes",
  "Back"
};

const char* dummy_setting_2_data_getter(unsigned int idx) {
  if (idx < ARRAYLEN(dummy_setting_2_data_getter_values)) {
    return dummy_setting_2_data_getter_values[idx];
  }
  return NULL;
}

void dummy_setting_2_data_selector(unsigned int idx) {
  switch(idx) {
    case 0:
      dummy_setting_2_data_change(0);
      break;
    case 1:
      dummy_setting_2_data_change(1);
      break;
    default:
      ux_menulist_init(0, settings_submenu_getter, settings_submenu_selector);
  }
}

//////////////////////////////////////////////////////////////////////////////////////
// Settings menu:

const char* const settings_submenu_getter_values[] = {
  "Dummy setting 1",
  "Dummy setting 2",
  "Back",
};

const char* settings_submenu_getter(unsigned int idx) {
  if (idx < ARRAYLEN(settings_submenu_getter_values)) {
    return settings_submenu_getter_values[idx];
  }
  return NULL;
}

void settings_submenu_selector(unsigned int idx) {
  switch(idx) {
    case 0:
      ux_menulist_init_select(0, dummy_setting_1_data_getter, dummy_setting_1_data_selector, N_storage.dummy_setting_1);
      break;
    case 1:
      ux_menulist_init_select(0, dummy_setting_2_data_getter, dummy_setting_2_data_selector, N_storage.dummy_setting_2);
      break;
    default:
      ui_idle();
  }
}

//////////////////////////////////////////////////////////////////////
UX_STEP_NOCB(
    ux_idle_flow_1_step, 
    pnn, 
    {
      &C_boilerplate_logo,
      "Boilerplate",
      "is ready",
    });
UX_STEP_VALID(
    ux_idle_flow_2_step,
    pb,
    ux_menulist_init(0, settings_submenu_getter, settings_submenu_selector),
    {
      &C_icon_coggle,
      "Settings",
    });
UX_STEP_NOCB(
    ux_idle_flow_3_step, 
    bn, 
    {
      "Version",
      APPVERSION,
    });
UX_STEP_VALID(
    ux_idle_flow_4_step,
    pb,
    os_sched_exit(-1),
    {
      &C_icon_dashboard_x,
      "Quit",
    });
UX_FLOW(ux_idle_flow,
  &ux_idle_flow_1_step,
  &ux_idle_flow_2_step,
  &ux_idle_flow_3_step,
  &ux_idle_flow_4_step,
  FLOW_LOOP
);

void ui_idle(void) {
    // reserve a display stack slot if none yet
    if(G_ux.stack_count == 0) {
        ux_stack_push();
    }
    ux_flow_init(0, ux_idle_flow, NULL);
}
