#ifndef _PSIA_CMD_H_
#define _PSIA_CMD_H_

#pragma once
#pragma region PSIA_SERVICE_NODE
class psia_service
{
public:
#define _PSIA_CMD
#define SERVICENODE(a, b) const static string a;
#include "psia_cmd_defs.dat"
#undef  _PSIA_CMD
#undef  SERVICENODE

};

class psia_resource
{
public:
#define _PSIA_CMD_RESRC
#define RESOURCENODE(a, b) const static string a;
#include "psia_cmd_defs.dat"
#undef  _PSIA_CMD_RESRC
#undef  RESOURCENODE
};
#pragma endregion PSIA_SERVICE_NODE
#endif