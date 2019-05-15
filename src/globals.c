#include "globals.h"
#include "os.h"

#ifdef TARGET_NANOX
#include "ux.h"
ux_state_t G_ux;
bolos_ux_params_t G_ux_params;
#else // TARGET_NANOX
ux_state_t ux;
#endif // TARGET_NANOX

// display stepped screens
unsigned int ux_step;
unsigned int ux_step_count;


const internalStorage_t N_storage_real;
