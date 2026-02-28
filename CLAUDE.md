# SWEaW Hooks

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