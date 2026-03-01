#include "functions.h"

// To add a new Lua function module:
//   1. Create functions/<module>.cpp with L_* implementation(s) and Register_<Module>()
//   2. Declare void Register_<Module>(lua_State* L); in functions.h
//   3. Call Register_<Module>(L); below
//   4. Add new RVAs (if needed) to rvas.h in the relevant section
//   5. Add functions\<module>.cpp to FoCAPI.vcxproj and .vcxproj.filters

void Register_All(lua_State* L) {
    Register_FileIO(L);
    Register_Players(L);
}
