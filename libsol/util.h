#pragma once

#define ARRAY_LEN(a) (sizeof(a) / sizeof((a)[0]))
#define BAIL_IF(x) {int err = x; if (err) return err;}
