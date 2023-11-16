#ifndef __SERVER_H__
#define __SERVER_H__

#include "../common/utils.h"
#include "../common/constant.h"

/** Verify the parameters from the start of AS app, if present -p and -v
 * update port to use and turn on the verbose mode, if not present default
 * port is used and verbose mode remains off
 */
void filter_input(int argc, char **argv);

#endif // __SERVER_H__
