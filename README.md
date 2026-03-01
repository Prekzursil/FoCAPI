# FoCAPI

A powrprof.dll hook which adds custom Lua functions to Forces of Corruption.

## Usage

Drop powrprof.dll and MinHooks.x64.dll into the `/corruption/` folder of your Empire at War installation.
Currently only works with the debug kit `StarWarsI.exe`; `StarWarsG.exe` support is coming soon.

## Functions

- Global `WriteToFile(str a, str b)` - Appends a line of text `b` to the file name `a` in the game directory.
- Global `Get_All_Players()` - Returns a 1-based Lua table of all active players, iterable with `ipairs()`. Each entry is the same PlayerWrapper type returned by `Find_Player()`.
