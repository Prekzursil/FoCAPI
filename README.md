# FoCAPI

A powrprof.dll hook which adds custom Lua functions to Forces of Corruption.

## Usage

Drop powrprof.dll and MinHooks.x64.dll into the `/corruption` folder of your Empire at War installation.
Works with both the debug kit `StarWarsI.exe` and client version `StarWarsG.exe` of the Steam version.
Not supported on disc / GoG versions (sorry).

## Global Functions

- Global `WriteToFile(file, text)` - Appends a line of text specified file in the game directory.
  - Parameter `file` string - The file name to append.
  - Parameter `text` string - The text line to append.
- Global `Get_All_Players()` - Returns a 1-based Lua table of all active players, iterable with `ipairs()`. Each entry is the same PlayerWrapper type returned by `Find_Player()`.
- Global `Add_Tutorial_Text(key, text, duration, r, g, b, a)` - Adds a line to the tutorial text pane.
  - Parameter `key`: string - The key used to identify this line, later can be used to update or remove the text.
  - Parameter `text`: string - The text to display.
  - Parameter `duration` number? - The duration to display the text. Default: -1 (infinite)
  - Parameter `r` number? - The red color component. Default: 1
  - Parameter `g` number? - The green color component. Default: 1
  - Parameter `b` number? - The blue color component. Default: 1
  - Parameter `a` number? - The alpha component. Default: 1
- Global `Remove_Tutorial_Text(key)` - Removes the line associated with the key.
  - Parameter `key` string - The key used to identify the line when it was added.
  
## GameObjectTypeWrapper

- `Get_Display_Name()` - Returns the localized text string display name of the object type.