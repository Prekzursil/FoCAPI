# FoCAPI

## Project Overview

This project is a hook API for Star Wars: Empire at War: Forces of Corruption to expose custom Lua functions available for scripting.

## Client and Debug Kits

- StarWarsG.exe -- the binary shipped with the game for all clients. PDB unavailable.
- StarWarsI.exe -- the debug kit binary which comes with debug functionality but is too slow to run normally. PDB available.

## DLL Hook

- Uses MinHook to create the DLL hook into the game via powerprof.dll
- Using IDA Free to find addresses of function calls.

## Empire at War Lua Scripting Engine

The source code for the Lua scripting engine in Empire at War was leaked at: https://github.com/PetroglyphGames/GlyphX-Reference. Use this as a guide for what Lua functions and variables are already available, and how the engine hooks Lua scripting into the rest of the game. No other engine source code is available.

## Adding a New Lua Function

1. Create `functions/<module>.cpp` — implement `L_*` function(s) and `Register_<Module>(lua_State* L)`
2. Add `void Register_<Module>(lua_State* L);` to `functions/functions.h`
3. Add `Register_<Module>(L);` to `functions/functions.cpp`
4. Add new RVAs (if needed) to `rvas.h` in the relevant section, for each supported executable
5. Add `functions\<module>.cpp` to `FoCAPI.vcxproj` and `FoCAPI.vcxproj.filters`
