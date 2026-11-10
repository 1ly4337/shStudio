# shStudio 10

A Windows editor for Shaiya client and server data: SData tables, 3D models, worlds, maps, archives and SQL data.

Features:

- **Dark mode**: Views → Dark mode. The setting is saved in `HKCU\Software\shStudio 10\DarkMode`; when it is unset,
  the Windows app theme decides. Takes effect after a restart (the app offers one).
- **BinarySData files**: the `DB*Data.SData` / `DB*Text_<LANG>.SData` files of newer clients open, edit and save,
  including the encrypted data tables. `DB*Text` language files open in a table editor.

## Build

Requirements: Windows, Visual Studio 2022 or newer with **Desktop development with C++** (includes CMake 3.20+).

```bat
build.cmd                 :: Release  -> "out\build\Release\shStudio 10.exe"
build.cmd RelWithDebInfo  :: with PDB
```

Visual Studio: after configuring, open `out\build\shStudio10.slnx` (startup project: shStudio).

Or by hand (the target must be **Win32**):

```bat
cmake -S . -B out\build -A Win32
cmake --build out\build --config Release
```

Outputs:

| Target | What it is |
|---|---|
| `shStudio 10.exe` | the application |

## Layout

| Path | Contents |
|---|---|
| `src/app/` | WinMain, main frame, application, settings/options, workspace/client/server tree, dark mode |
| `src/core/` | streams, strings, containers, exceptions, serialization, math, files, registry, threads |
| `src/core/image/` | image loading (DDS/TGA/BMP), GDI helpers and caches |
| `src/data/` | SData tables: items, item sets, cash shop, monsters, NPC/player skills, quests, NPC data, grades, DB*Text tables |
| `src/formats/model/` | 3D model formats: .3dc/.3de/.3do/.smod/.mlt/.mlx/.itm/.mon, animations, effects |
| `src/formats/world/` | world formats: .wld/.svmap/.dg/.vani/.wtr/.zon, height maps, spots, portals, spawns, NPCs |
| `src/render3d/` | OpenGL views and mouse controllers |
| `src/archive/` | data.sah/data.saf archive, content providers, file/folder trees |
| `src/sql/` | SQL Server connection, tables, schema, SQL views and dialogs |
| `src/ui/` | controls, views, lists, editors and dialogs |
| `src/crypto/` | data-file encryption |
| `res/` | resources (menus, dialogs, icons, bitmaps, version) as a compiled `.res` |

## License

MIT — see [LICENSE](LICENSE). Copyright (c) 2026 ilya. Copies and forks must keep the copyright notice.
