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

### Parabolic projectile physics

Implemented in `src/Turret.h` (`PoisonLiquid::Shoot`), poison-orb aiming uses the horizontal and vertical displacement between the turret and the player's position to calculate an initial launch velocity under gravity. The orb then follows its trajectory independently—it does **not** continuously home in on the player.

### Real-time gameplay and collisions

Implemented the main game loop and input handling in C++ with SFML, along with custom player movement, jumping, gravity, platform interactions, projectile updates, and collision checks.

### Animation and persistence

Used sprite-sheet frame metadata in JSON to drive animations, and `saves/user_data_and_preference.json` to store the high score and user settings.

## Controls

| Action | Input |
|---|---|
| Move left / right | A / D or Left / Right Arrow |
| Jump | W, Up Arrow, or Space |
| Move down | S or Down Arrow |
| Sword attack | Left mouse button |
| Pause / resume | Esc |

You can switch between **WASD** and **arrow-key** movement in the game's settings.

## Technologies

- **Language:** C++
- **Graphics, audio, and input:** SFML 2
- **Data storage and sprite metadata:** JSON using nlohmann/json
- **Original development period:** April–June 2022

## Build and Run (Windows)

The original project includes a `Makefile`, bundled SFML headers in `include/`, and linker libraries in `lib/`. It was built using a **32-bit Windows MinGW toolchain** and **SFML 2**. A compatible compiler and runtime DLLs are required; this historical build has not been tested across modern toolchains.

1. Install a compatible **32-bit MinGW g++** toolchain and `mingw32-make` (or GNU Make).
2. Open a terminal in the repository root (the folder containing `Makefile`).
3. Run `mingw32-make` (or `make`, depending on your installation).
4. Make sure the required SFML and MinGW runtime DLLs are available next to `main.exe` or through your system path.
5. From the **repository root**, run `./main.exe` (or `.\main.exe` in PowerShell).

**Important:** The game loads files using relative paths. Keep the `gfx/`, `audio/`, and `saves/` folders in place and launch it from the repository root. The `saves/user_data_and_preference.json` file must exist.

## Project Structure

- `src/` — C++ gameplay, physics, collision, menus, and settings code
- `gfx/` — images, sprite sheets, animation metadata, and fonts
- `audio/` — music and sound effects
- `saves/` — high scores and user preferences
- `include/` and `lib/` — bundled headers and libraries used by the original build
- `Makefile` — compilation and linking instructions

## About

Created by **Nathanael Nicolas Chang** as a personal C++ game-development project in 2022. The project demonstrates gameplay programming and physics implementation using SFML rather than a full game engine.

**Third-party assets:** SFML, nlohmann/json, and any externally sourced music, sound effects, fonts, or artwork retain their respective licenses. Verify redistribution permissions and provide the appropriate credits before publishing their files in a public repository or release.