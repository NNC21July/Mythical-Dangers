# Mythical Dangers

**A single-screen, 2D arcade survival game built with C++ and SFML.**

Developed as a personal project in **2022**, Mythical Dangers challenges players to survive for as long as possible in a fixed arena. Dodge fireballs entering from either side of the screen, avoid poison launched by automated turrets, and destroy turrets with your sword before they become overwhelming.

![How to play Mythical Dangers](gfx/sprite%20and%20image/how-to-play.png)

## Screenshots

![Mythical Dangers - Main Menu](docs/media/main-menu.png)

![Mythical Dangers - Gameplay Screenshot](docs/media/gameplay.png)

## Features

- **Arcade survival gameplay:** Survive an increasingly difficult combination of incoming fireballs and poison-projectile attacks.
- **Dynamic turret aiming:** Turrets launch poison orbs along parabolic trajectories calculated from the player's position at firing time.
- **Custom character mechanics:** Player movement, jumping, gravity, platform collisions, sword attacks, and sprite-sheet animations.
- **Progressive difficulty:** Fireball spawn intervals shorten over time, while turret appearances add pressure.
- **Persistent progress and settings:** High scores, keyboard preferences, and audio levels are stored in JSON.

## Gameplay Demo

![Mythical Dangers - Gameplay Demo](docs/media/gameplay.gif)

## Technical Highlights

### Real-time gameplay and collisions

Implemented the main game loop and input handling in C++ with SFML, along with custom player movement, jumping, gravity, platform interactions, projectile updates, and collision checks.

### Projectile trajectory calculation

In [`src/Turret.h`](src/Turret.h), turret-fired poison orbs use the player's position at firing time to calculate a parabolic launch trajectory. This is one part of the game's wider hazard and combat systems.

### Animation and persistence

Used sprite-sheet frame metadata in JSON to drive animations, and [`saves/user_data_and_preference.json`](saves/user_data_and_preference.json) to store the high score and user settings.

## Controls

| Action | Input |
| --- | --- |
| Move left / right | **A / D** or **Left / Right Arrow** (selectable in Settings) |
| Jump | **W**, **Up Arrow**, or **Space** |
| Move down | **S** or **Down Arrow** |
| Sword attack | **Left mouse button** |
| Pause / resume | **Esc** |

You can switch between **WASD** and **arrow-key** movement in the game's settings.

## Technologies

- **Language:** C++
- **Graphics, audio, and input:** SFML 2
- **Data storage and sprite metadata:** JSON using nlohmann/json
- **Original development period:** April–June 2022

## Build and Run (Windows, MSYS2 UCRT64)

Mythical Dangers builds with **C++17 and SFML 2.6.2**. You do **not** need to install SFML or switch to an older compiler. The build automatically downloads SFML 2.6.2 **source code** and compiles it with your installed C++ compiler. You need an internet connection for the first build.

### One-time prerequisites

1. Install [MSYS2](https://www.msys2.org/) on Windows.
2. Open the **MSYS2 UCRT64** terminal (not the plain MSYS2/MSYS terminal).
3. Update MSYS2 if needed with `pacman -Syu` (restart the terminal if prompted).
4. Install the development tools:

```bash
pacman -S --needed git mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-make mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-ninja
```

### Download and compile

1. On the repository page, click **Code → Download ZIP** and extract it.
2. In your **MSYS2 UCRT64** terminal, change into the extracted folder. For example:

```bash
cd /c/Users/asus/Downloads/Mythical-Dangers-main
```

3. Compile the game:

```bash
mingw32-make
```

This configures the build, downloads and builds **SFML 2.6.2** using the active UCRT64 compiler, and creates `MythicalDangers.exe` directly in the repository root, alongside its required SFML, OpenAL, and MinGW runtime DLLs. The first build can take several minutes.

### Run

After a successful build, simply **double-click `MythicalDangers.exe`** in the extracted project folder, or launch it from the repository root in the UCRT64 terminal:

```bash
./MythicalDangers.exe
```

The build places the executable and matching DLLs beside `gfx/`, `audio/`, `saves/`, and `icon.png`, so the game's relative asset paths work when launched from Explorer. **Keep these folders in place.** The `build/` folder is only for intermediate compilation files.

The generated `.exe` and `.dll` files are ignored by Git and will not appear in a fresh **Code → Download ZIP**; each person runs `mingw32-make` once to create them.

### Troubleshooting

- **`cmake: command not found` or `ninja: command not found`:** Install the UCRT64 packages above and reopen the UCRT64 terminal.
- **SFML download fails:** Check your internet connection and verify that `git` is installed. Then retry `mingw32-make`.
- **`skipping incompatible libsfml-*.a`:** You're still using the old linker command or Makefile. Use the updated Makefile/CMakeLists from this repository, which ignore the old bundled `lib/` archives.
- **Missing asset errors:** Double-click `MythicalDangers.exe` in the project root. Running it from an unrelated shell working directory can still break relative paths.
- **Missing runtime DLL errors:** Ensure the build completed and that the SFML, OpenAL and MinGW runtime DLLs were copied beside `MythicalDangers.exe`. If necessary, run it from the UCRT64 terminal to diagnose missing dependencies.

To rebuild from scratch, run `mingw32-make clean` followed by `mingw32-make`.

The [Windows source-build check](.github/workflows/windows-build.yml) uses the same process to verify compilation on GitHub Actions. It checks build success; actual gameplay/audio must still be tested manually on Windows.

## Project Structure

- `src/` — C++ gameplay, physics, collision, menus, and settings code
- `gfx/` — images, sprite sheets, animation metadata, and fonts
- `audio/` — music and sound effects
- `saves/` — high scores and user preferences
- `include/nlohmann/` — JSON headers used by the game
- `include/SFML/` and `lib/` — old SFML 2.5.1 development files, **not used by the current CMake build**
- `CMakeLists.txt` — downloads and builds compatible SFML 2.6.2 source
- `Makefile` — convenient wrapper for CMake and Ninja

## About

Created by **Nathanael Nicolas Chang** as a personal C++ game-development project in 2022. The project demonstrates gameplay programming and physics implementation using SFML rather than a full game engine.

**Third-party assets:** SFML, nlohmann/json, and any externally sourced music, sound effects, fonts, or artwork retain their respective licenses. Verify redistribution permissions and provide the appropriate credits before publishing their files in a public repository or release.
