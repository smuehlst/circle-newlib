/* Stub version of _exit.  */

#include <limits.h>
#include "config.h"
#include "circle_macros.h"
#include <_ansi.h>
#include <_syslist.h>

// Workaround for use of ASSERT_STATIC from Circle's assert.h,
// which is not available in the newlib headers.
#define ASSERT_STATIC(expr) static_assert (expr, #expr)
#include <circle/startup.h>

extern "C"
void
_exit(int rc)
{
  set_qemu_exit_status(rc);
  halt();
}
