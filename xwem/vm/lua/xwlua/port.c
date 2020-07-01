/**
 * @file
 * @brief Port Lua for XuanWuOS
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * + http://www.lua.org/license.html
 */

#include <xwos/standard.h>
#include <xwos/lib/string.h>
#include <xwos/mm/mempool/allocator.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "xwlua/port.h"

static const luaL_Reg xwlua_loadedlibs[] = {
        {LUA_GNAME, luaopen_base},
        {LUA_LOADLIBNAME, luaopen_package},
        {LUA_COLIBNAME, luaopen_coroutine},
        {LUA_TABLIBNAME, luaopen_table},
        {LUA_IOLIBNAME, luaopen_io},
        //{LUA_OSLIBNAME, luaopen_os},
        {LUA_STRLIBNAME, luaopen_string},
        {LUA_MATHLIBNAME, luaopen_math},
        {LUA_UTF8LIBNAME, luaopen_utf8},
        {LUA_DBLIBNAME, luaopen_debug},
        {NULL, NULL}
};


void xwlua_openlibs(lua_State * L)
{
        const luaL_Reg *lib;

        for (lib = xwlua_loadedlibs; lib->func; lib++) {
                luaL_requiref(L, lib->name, lib->func, 1);
                lua_pop(L, 1);
        }
}

static
int xwlua_panic(lua_State * L)
{
        lua_writestringerror("PANIC: unprotected error in call to Lua API (%s)\n",
                             lua_tostring(L, -1));
        return 0; /* return to Lua to abort */
}

/*
** Emit a warning. '*warnstate' means:
** 0 - warning system is off;
** 1 - ready to start a new message;
** 2 - previous message is to be continued.
*/
static
void xwlua_warnf(void * ud, const char * message, int tocont)
{
        int * warnstate = (int *)ud;
        if ((*warnstate != 2) && (!tocont) && (*message == '@')) {
                /* control message? */
                if (strcmp(message, "@off") == 0) {
                        *warnstate = 0;
                } else if (strcmp(message, "@on") == 0) {
                        *warnstate = 1;
                }
                return;
        } else if (*warnstate == 0) {
                /* warnings off? */
                return;
        }
        if (*warnstate == 1) {
                /* previous message was the last? */
                lua_writestringerror("%s", "Lua warning: "); /* start a new warning */
        }
        lua_writestringerror("%s", message); /* write message */
        if (tocont) {/* not the last part? */
                *warnstate = 2;  /* to be continued */
        } else {  /* last part */
                lua_writestringerror("%s", "\n"); /* finish message with end-of-line */
                *warnstate = 1; /* ready to start a new message */
        }
}

void * xwlua_alloc(void * ud, void * ptr, size_t osize, size_t nsize)
{
        void * mem;

        XWOS_UNUSED(ud);

        if (NULL == ptr) {
                xwmm_mempool_malloc(xwlua_mempool, nsize, &mem);
        } else if (0 == nsize) {
                xwmm_mempool_free(xwlua_mempool, ptr);
                mem = NULL;
        } else if (osize >= nsize) {
                mem = ptr;
        } else {
                mem = ptr;
                xwmm_mempool_realloc(xwlua_mempool, nsize, &mem);
        }
        return mem;
}

lua_State * xwlua_newstate(lua_Alloc f)
{
        lua_State * L;

        if (f) {
                L = lua_newstate(f, NULL);
                if (L) {
                        int * warnstate;
                        lua_atpanic(L, xwlua_panic);
                        warnstate = (int *)lua_newuserdatauv(L, sizeof(int), 0);
                        luaL_ref(L, LUA_REGISTRYINDEX);
                        *warnstate = 0;
                        lua_setwarnf(L, xwlua_warnf, warnstate);
                }
        } else {
                L = NULL;
        }
        return L;
}
