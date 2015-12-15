#include "stdafx.h"
#include "psiacmd.h"

#define _PSIA_CMD
#define SERVICENODE(a, b) const static string psia_service::a = b;
#include "psiacmd.h"
#undef  _PSIA_CMD
#undef  SERVICENODE

#define _PSIA_CMD_RESRC
#define RESOURCENOE(a, b) const static string psia_service::a = b;
#include "psiacmd.h"
#undef  _PSIA_CMD_RESRC
#undef  RESOURCENODE