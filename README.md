# FoCAPI

A powrprof.dll hook which adds new Lua global and member functions to Forces of Corruption.

## Usage

Drop **powrprof.dll** into the `/corruption` folder of your Star Wars: Empire at War Gold Pack installation.
Works with both the debug kit `StarWarsI.exe` and client version `StarWarsG.exe` of the Steam version of Forces of Corruption.
Currently does not support the Empire at War base game, though theoretically it could.
Not supported on disc / GoG versions (sorry).

## Global Functions

### `WriteToFile(file, text)`

Appends a line of `text` to the `file` in the game directory.

**Parameters**:

| Parameter | Type | Description |
| --- | --- | --- |
| file | `string` | The file name. |
| text | `string` | The text to append. |

**Returns**: `nil`

**Example**:

```lua
WriteToFile("example.txt", "Hello, world!")
```

### `Get_All_Players()`

Returns a 1-based Lua table of all active player objects, iterable with `ipairs()`.

**Parameters**: *none*

**Returns**: `table<integer, PlayerObject>`

**Example**:

```lua
local players = Get_All_Players()

for _, player in ipairs(players) do
  local playerId = player.Get_ID()

  if playerId >= 0 then
    -- Add non-neutral players to our PlayerTable global
    PlayerTable[playerId] = player
  end
end
```

### `Add_Tutorial_Text(key, text, duration, r, g, b, a)`

Adds or updates a line of text to the tutorial text pane with optional duration and coloring.

**Parameters**:

| Parameter | Type | Description |
| --- | --- | --- |
| key | `string` | The key used to identify this line. Can be later used to update text, duration, and color, or to remove the line. |
| text | `string` | The text to display. |
| duration | `number?` | *Optional, default: `-1`*. The duration to display the text. To display text indefinitely until removal, set to `-1`. |
| r | `number?` | *Optional, default: `1.0`*. The red color component, `0.0` to `1.0`. |
| g | `number?` | *Optional, default: `1.0`*. The green color component, `0.0` to `1.0`. |
| b | `number?` | *Optional, default: `1.0`*. The blue color component, `0.0` to `1.0`. |
| a | `number?` | *Optional, default: `1.0`*. The alpha component, `0.0` to `1.0`. |

**Returns**: `nil`

**Example**:

```lua
local players = Get_All_Players()

for _, player in ipairs(players) do
  local name = player.Get_Name()
  local faction = player.Get_Faction_Name()
  local credits = player.Get_Credits()
  -- For example, the key of the first Empire player might be PC:1
  local key = string.format("PC:%d", player.Get_ID())
  -- Will print "EMPIRE Player [Name]: $ 8000"
  local text = string.format("%s Player %s: $ %d", faction, name, credits)

  Add_Tutorial_Text(key, text, -1, 1, 1, 1, 1)
end
```

### `Remove_Tutorial_Text(key)`

Removes the tutorial text line associated with the `key` if it exists.

**Parameters**:

| Parameter | Type | Description |
| --- | --- | --- |
| key | `string` | The key of the line to remove. |

**Returns**: `nil`

**Example**:

```lua
Add_Tutorial_Text("TEXT1", "Hello, world!")
Add_Tutorial_Text("TEXT2", "This is a test.")
Remove_Tutorial_Text("TEXT1")
-- "This is a test." still shows
```

## GameObjectType Member Functions

### `.Get_Display_Name()`

Returns the localized text string display name of the object.

**Parameters**: *none*

**Returns**: `string` The localized display name text.

**Example**:

```lua
local object = Find_First_Object("TIE_FIGHTER")

if TestValid(object) then
  local name = object.Get_Display_Name()
  -- Will print "Imperial TIE Fighter" (or whatever its text name in MasterTextFile.dat is)
  local text = string.format("Imperial %s", name)

  Add_Tutorial_Text("TEXT1", text)
end
```

## Process

The process of reverse engineering `StarWarsI.exe` using the `StarWarsI.pdb` that Petroglyph provided is pretty straightforward. It's relatively easy to search for function names in a disassembler such as Ghidra or IDA Free, find the relative virtual address (RVA) of that function, and then add it as a pointer in the DLL that can be called.

The tricky part is finding RVAs in `StarWarsG.exe`. We do not have the symbols for that binary with the PDB file like we do with `StarWarsI.exe`. For now, I've been using the version control function of Ghidra to compare `StarWarsI.exe` with `StarWarsG.exe` which uses algorithms to map functions between the two binaries that it thinks are a match. So far, it has been mostly accurate, but it has not found matches for everything that I've wanted to add. There are some instances where I had to go digging for references to find out candidate addresses for a match, and there was a lot of trial and error in some cases until things finally worked.

There are also instances where functions in `StarWarsI.exe` do not exist in `StarWarsG.exe`. This most commonly seems to be the case with object constructors and field accessors. When Petroglyph compiled `StarWarsG.exe`, they likely used compiler optimization settings which "inlines" construction of objects and accessors of instance fields to reduce the number of JMP instructions. This works well for performance, but it gets in the way of reverse engineering the game engine. For these cases, we usually have to re-create our own constructors and hard reference the exact offset of fields. These offset addresses also are not identical across `StarWarsI.exe` and `StarWarsG.exe` which adds to the complexity.