#include "stdafx.h"
#include "psiacmd.h"

#define _PSIA_CMD
#define SERVICENODE(a, b) const string psia_service::a = b;
#include "psia_cmd_defs.dat"
#undef  _PSIA_CMD
#undef  SERVICENODE

#define _PSIA_CMD_RESRC
#define RESOURCENODE(a, b) const string psia_resource::a = b;
#include "psia_cmd_defs.dat"
#undef  _PSIA_CMD_RESRC
#undef  RESOURCENODE