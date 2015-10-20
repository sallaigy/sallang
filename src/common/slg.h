/**
 * slg.h: Common macros and functions
 */
#ifndef _SALLANG_SLG_H
#define _SALLANG_SLG_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define SLG_TRY(A, E) if (!(A)) goto E;

#endif