#include "menu.h"
#include "os.h"

volatile uint8_t dummy_setting_1;
volatile uint8_t dummy_setting_2;

#if defined(TARGET_NANOS)

#ifdef HAVE_U2F

static const ux_menu_entry_t menu_main[];
static const ux_menu_entry_t menu_settings[];
static const ux_menu_entry_t menu_dummy_setting_1[];
static const ux_menu_entry_t menu_dummy_setting_2[];

// change the setting
static void menu_dummy_setting_1_change(unsigned int enabled) {
    dummy_setting_1 = enabled;
    nvm_write(&N_storage.dummy_setting_1, (void*)&dummy_setting_1, sizeof(uint8_t));
    // go back to the menu entry
    UX_MENU_DISPLAY(0, menu_settings, NULL);
}

static void menu_dummy_setting_2_change(unsigned int enabled) {
    dummy_setting_2 = enabled;
    nvm_write(&N_storage.dummy_setting_2, (void*)&dummy_setting_2, sizeof(uint8_t));
    // go back to the menu entry
    UX_MENU_DISPLAY(0, menu_settings, NULL);
}

// show the currently activated entry
static void menu_dummy_setting_1_init(unsigned int ignored) {
    UNUSED(ignored);
    UX_MENU_DISPLAY(N_storage.dummy_setting_1?1:0, menu_dummy_setting_1, NULL);
}

static void menu_dummy_setting_2_init(unsigned int ignored) {
    UNUSED(ignored);
    UX_MENU_DISPLAY(N_storage.dummy_setting_2?1:0, menu_dummy_setting_2, NULL);
}

static const ux_menu_entry_t menu_dummy_setting_1[] = {
    {NULL, menu_dummy_setting_1_change, 0, NULL, "No", NULL, 0, 0},
    {NULL, menu_dummy_setting_1_change, 1, NULL, "Yes", NULL, 0, 0},
    UX_MENU_END
};

static const ux_menu_entry_t menu_dummy_setting_2[] = {
    {NULL, menu_dummy_setting_2_change, 0, NULL, "No", NULL, 0, 0},
    {NULL, menu_dummy_setting_2_change, 1, NULL, "Yes", NULL, 0, 0},
    UX_MENU_END
};

static const ux_menu_entry_t menu_settings[] = {
    {NULL, menu_dummy_setting_1_init, 0, NULL, "Dummy setting 1", NULL, 0, 0},
    {NULL, menu_dummy_setting_2_init, 0, NULL, "Dummy setting 2", NULL, 0, 0},
    {menu_main, NULL, 1, &C_icon_back, "Back", NULL, 61, 40},
    UX_MENU_END
};
#endif // HAVE_U2F

static const ux_menu_entry_t menu_about[] = {
    {NULL, NULL, 0, NULL, "Version", APPVERSION , 0, 0},
    {menu_main, NULL, 2, &C_icon_back, "Back", NULL, 61, 40},
    UX_MENU_END
};

static const ux_menu_entry_t menu_main[] = {
    //{NULL, NULL, 0, &NAME3(C_nanos_badge_, CHAINID, ), "Use wallet to", "view accounts", 33, 12},
    {NULL, NULL, 0, NULL, "Boilerplate", "ready", 0, 0},
    {menu_settings, NULL, 0, NULL, "Settings", NULL, 0, 0},
    {menu_about, NULL, 0, NULL, "About", NULL, 0, 0},
    {NULL, os_sched_exit, 0, &C_icon_dashboard, "Quit app", NULL, 50, 29},
    UX_MENU_END
};

#elif defined(TARGET_NANOX)

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


#endif

void ui_idle(void) {
#if defined(TARGET_NANOS)
    UX_MENU_DISPLAY(0, menu_main, NULL);
#elif defined(TARGET_NANOX)
    // reserve a display stack slot if none yet
    if(G_ux.stack_count == 0) {
        ux_stack_push();
    }
    ux_flow_init(0, ux_idle_flow, NULL);
#endif // #if TARGET_ID
}
