#ifndef LTASK_SERIALIZE_H
#define LTASK_SERIALIZE_H

#include <lua.h>

int seri_unpack(lua_State *L);
int seri_pack(lua_State *L);

#endif
