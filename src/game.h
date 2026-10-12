#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>
#include <vector>
#include <random>
#include <stdlib.h>
#include <time.h>
#include "Player.h"
#include "Platform.h"
#include "Fireball.h"
#include "Turret.h"
#include "Heart.h"
#include "MenuBar.h"
#include "ScreenMenu.h"
using string = std::string;
using namespace sf;

class Background;

int MAX_FIREBALL = 4;
int MAX_HEART = 2;
int MAX_TURRET = 2;

void game(RenderWindow &window, string &window_state, View &view)
{
    // opening settings file
    json settings_file;
    std::ifstream file("saves/user_data_and_preference.json");
    file >> settings_file;
    file.close();

    // up, left, right, down
    std::vector<Keyboard::Key> keyboard_movement_keys;

    if (settings_file["settings_preferences"]["keyboard_preference"] == "wasd")
    {
        keyboard_movement_keys.push_back(Keyboard::W);
        keyboard_movement_keys.push_back(Keyboard::A);
        keyboard_movement_keys.push_back(Keyboard::D);
        keyboard_movement_keys.push_back(Keyboard::S);
    }
    else
    {
        keyboard_movement_keys.push_back(Keyboard::Up);
        keyboard_movement_keys.push_back(Keyboard::Left);
        keyboard_movement_keys.push_back(Keyboard::Right);
        keyboard_movement_keys.push_back(Keyboard::Down);
    }

    // transition
    CircleShape black_circle_transition;
    black_circle_transition.setFillColor(Color::Black);
    black_circle_transition.setRadius(523.f);
    black_circle_transition.setOrigin(Vector2f(black_circle_transition.getRadius(), black_circle_transition.getRadius()));
    float x_pos = window.getSize().x / 2.f, y_pos = window.getSize().y / 2.f;
    float vel_x = 0.f;
    black_circle_transition.setPosition(Vector2f(x_pos, y_pos));
    bool game_start = true, game_end = false;

    // background
    Texture background_texture;
    background_texture.loadFromFile("gfx/sprite and image/background.jpg");
    Background background(window, &background_texture);

    // warrior
    Texture warrior_idle;
    warrior_idle.loadFromFile("gfx/sprite and image/warrior-idle.png");
    Texture warrior_run;
    warrior_run.loadFromFile("gfx/sprite and image/warrior-run.png");
    Texture warrior_jump;
    warrior_jump.loadFromFile("gfx/sprite and image/warrior-jump.png");
    Texture warrior_attack;
    warrior_attack.loadFromFile("gfx/sprite and image/warrior-attack.png");
    Texture warrior_die;
    warrior_die.loadFromFile("gfx/sprite and image/warrior-die.png");
    Texture warrior_texture = warrior_idle;
    string warrior_state = "idle";
    int total_sprite = 2;
    float switch_time = 0.45f;
    Player warrior(&warrior_idle, warrior_state, total_sprite, switch_time, window);
    GroundCollision warrior_ground_collision(warrior.GetPlayerRect());
    Collision warrior_collision(warrior.GetPlayerRect());
    RectangleShape sword_attacking_hitbox;
    sword_attacking_hitbox.setSize(Vector2f(15 * 30 / 11.5, 5 * 30 / 11.5));
    sword_attacking_hitbox.setOrigin(sword_attacking_hitbox.getSize().x / 2.f, sword_attacking_hitbox.getSize().y / 2.f);
    sword_attacking_hitbox.setFillColor(Color(0, 0, 0, 0));
    Collision sword_attacking_hitbox_collision(sword_attacking_hitbox);
    RectangleShape warrior_attacking_hitbox;
    warrior_attacking_hitbox.setSize(Vector2f(22 * 30 / 11.5, 36 * 30 / 11.5));
    warrior_attacking_hitbox.setOrigin(warrior_attacking_hitbox.getSize().x / 2.f, warrior_attacking_hitbox.getSize().y / 2.f);
    warrior_attacking_hitbox.setFillColor(Color(0, 0, 0, 0));
    Collision warrior_attacking_hitbox_collision(warrior_attacking_hitbox);
    bool warrior_can_go_right = true;
    bool warrior_can_go_left = true;
    float attack_time = 0;
    float poison_damage_delay = 0;
    bool jumping_before = false;

    // Menu bar
    MenuBar menu_bar(window);
    // health bar
    HealthBar health_bar(window);
    // pause button
    PauseButton pause_button;

    // ground
    Spritesheet ground_sprite(string("gfx/metadata/ground.json"));
    std::vector<RectangleShape> ground;
    ground.push_back(ground_sprite.GetSprite());
    Texture ground_texture;
    ground_texture.loadFromFile("gfx/sprite and image/ground.png");
    ground[0].setTexture(&ground_texture);
    ground[0].setSize(Vector2f(ground_sprite.GetUvRect().width / 10, ground_sprite.GetUvRect().height / 10));
    ground[0].setPosition(Vector2f(0.f, window.getSize().y - ground[0].getSize().y));
    for (int i = 1; i < 3; i++)
    {
        ground.push_back(ground[0]);
        ground[i].setPosition(ground[i - 1].getPosition().x + ground[i - 1].getSize().x, ground[i - 1].getPosition().y);
    }

    // platforms
    Texture platform_spritesheet;
    platform_spritesheet.loadFromFile("gfx/sprite and image/platforms.png");
    std::vector<Platform> platforms;
    platforms.push_back(Platform(&platform_spritesheet, "long"));
    platforms[0].SetPosition(window.getSize().x * 1.f / 2.f, menu_bar.GetSize().y + 110.f + platforms[0].GetSize().y / 2.f);
    platforms.push_back(Platform(&platform_spritesheet, "short"));
    platforms[1].SetPosition(window.getSize().x * 1.f / 4.f, platforms[0].GetPosition().y + platforms[0].GetSize().y / 2.f + 80.f + platforms[1].GetSize().y / 2.f);
    platforms.push_back(Platform(&platform_spritesheet, "short"));
    platforms[2].SetPosition(window.getSize().x * 3.f / 4.f, platforms[1].GetPosition().y + 8.f);
    platforms.push_back(Platform(&platform_spritesheet, "medium"));
    platforms[3].SetPosition(window.getSize().x * 1.f / 2.f, platforms[1].GetPosition().y + platforms[1].GetSize().y / 2.f + 80.f + platforms[3].GetSize().y / 2.f);
    for (int j = 0; j < platforms.size(); j++)
    {
        platforms[j].SetOrigin(platforms[j].GetSize().x / 2.f, platforms[j].GetSize().y / 2.f);
    }

    // fireball
    Texture fireball_texture;
    fireball_texture.loadFromFile("gfx/sprite and image/fireball.png");
    std::vector<Fireball> fireballs;
    float fireball_spawn_timer = 0.f;
    float fireball_spawn_time = 3.f;
    float fireball_spawn_time_increase_time = 0.f;

    // turret
    Texture turret_texture;
    turret_texture.loadFromFile("gfx/sprite and image/turret.png");
    Texture turret_collapse_texture;
    turret_collapse_texture.loadFromFile("gfx/sprite and image/turret-collapsed.png");
    std::vector<Turret> turrets;
    float turret_appear_time = 8.f;
    float turret_appear_timer = 0.f;
    float turret_deploy_time_increase_time = 0.f;

    // poison
    std::vector<PoisonLiquid> poison_liquids;
    std::vector<PoisonPuddle> poison_puddles;
    std::vector<PoisonGasEffect> poison_gases;
    Texture poison_gas_texture;
    poison_gas_texture.loadFromFile("gfx/sprite and image/poison-gas-effect.png");

    // heart
    Texture heart_texture;
    heart_texture.loadFromFile("gfx/sprite and image/heart-symbol.png");
    std::vector<Heart> hearts;
    float heart_appear_timer = 0.f;

    view.setCenter(window.getSize().x / 2.f, window.getSize().y / 2.f);

    // screen shaking effect
    ScreenShake screen_shake(0.15f, 6.5f, view);
    // screen redden effect
    ScreenRedden screen_redden(window, menu_bar.GetRect());
    // screen darken effect
    ScreenDarken screen_darken(window);
    float screen_darken_fade_delay = 0.f;

    // pause menu
    PauseMenu pause_menu(window);
    // dead menu
    DeadMenu dead_menu(window);

    bool restarted = false;
    bool paused = false;
    bool switch_to_settings = false;
    float esc_pause_delay = 0.f;
    float sword_hit_delay = 0.f;

    // sound effects
    float sound_effect_volume = 100 * (int)settings_file["settings_preferences"]["sound_effect"] / 5;
    // === whoosh ===
    SoundBuffer whoosh_buffer;
    whoosh_buffer.loadFromFile("audio/sfx/whoosh.ogg");
    Sound whoosh_sound;
    whoosh_sound.setBuffer(whoosh_buffer);
    whoosh_sound.setVolume(sound_effect_volume);
    bool whoosh_sound_played = false;
    // === button click1 ===
    SoundBuffer button_click1_buffer;
    button_click1_buffer.loadFromFile("audio/sfx/button-click1.ogg");
    Sound button_click_sound;
    button_click_sound.setBuffer(button_click1_buffer);
    button_click_sound.setVolume(sound_effect_volume);
    bool button_click_sound_played = false;
    // === warrior running ===
    SoundBuffer warrior_running_buffer;
    warrior_running_buffer.loadFromFile("audio/sfx/warrior-running.ogg");
    Sound warrior_running_sound;
    warrior_running_sound.setBuffer(warrior_running_buffer);
    warrior_running_sound.setVolume(sound_effect_volume);
    warrior_running_sound.setLoop(true);
    bool warrior_running_sound_played = false;
    // === warrior jumping ===
    SoundBuffer warrior_jumping_buffer;
    warrior_jumping_buffer.loadFromFile("audio/sfx/warrior-jumping.ogg");
    Sound warrior_jumping_sound;
    warrior_jumping_sound.setBuffer(warrior_jumping_buffer);
    warrior_jumping_sound.setVolume(sound_effect_volume);
    bool warrior_jumping_sound_played = false;
    // === warrior land ===
    SoundBuffer warrior_land_buffer;
    warrior_land_buffer.loadFromFile("audio/sfx/warrior-land.ogg");
    Sound warrior_land_sound;
    warrior_land_sound.setBuffer(warrior_land_buffer);
    warrior_land_sound.setVolume(sound_effect_volume);
    bool warrior_land_sound_played = false;
    // === warrior attacking ===
    SoundBuffer warrior_attacking_buffer;
    warrior_attacking_buffer.loadFromFile("audio/sfx/warrior-attacking.ogg");
    Sound warrior_attacking_sound;
    warrior_attacking_sound.setBuffer(warrior_attacking_buffer);
    warrior_attacking_sound.setVolume(sound_effect_volume);
    bool warrior_attacking_sound_played = false;
    // === warrior damaged ===
    SoundBuffer warrior_damaged_buffer;
    warrior_damaged_buffer.loadFromFile("audio/sfx/warrior-damaged.ogg");
    Sound warrior_damaged_sound;
    warrior_damaged_sound.setBuffer(warrior_damaged_buffer);
    warrior_damaged_sound.setVolume(sound_effect_volume);
    bool warrior_damaged_sound_played = false;
    // === fireball ===
    SoundBuffer fireball_buffer;
    fireball_buffer.loadFromFile("audio/sfx/fireball.ogg");
    Sound fireball_sound;
    fireball_sound.setBuffer(fireball_buffer);
    fireball_sound.setVolume(0);
    fireball_sound.setLoop(true);
    bool fireball_sound_played = false;
    // === turret shoot ===
    SoundBuffer turret_shoot_buffer;
    turret_shoot_buffer.loadFromFile("audio/sfx/turret-shoot.ogg");
    Sound turret_shoot_sound;
    turret_shoot_sound.setBuffer(turret_shoot_buffer);
    turret_shoot_sound.setVolume(sound_effect_volume);
    bool turret_shoot_sound_played = false;
    // === turret damaged1 ===
    SoundBuffer turret_damaged1_buffer;
    turret_damaged1_buffer.loadFromFile("audio/sfx/turret-damaged1.ogg");
    Sound turret_damaged1_sound;
    turret_damaged1_sound.setBuffer(turret_damaged1_buffer);
    turret_damaged1_sound.setVolume(sound_effect_volume);
    // === turret damaged2 ===
    SoundBuffer turret_damaged2_buffer;
    turret_damaged2_buffer.loadFromFile("audio/sfx/turret-damaged2.ogg");
    Sound turret_damaged2_sound;
    turret_damaged2_sound.setBuffer(turret_damaged2_buffer);
    turret_damaged2_sound.setVolume(sound_effect_volume);
    bool turret_damaged_sound_played = false;
    // === turret collapsed ===
    SoundBuffer turret_collapsed_buffer;
    turret_collapsed_buffer.loadFromFile("audio/sfx/turret-collapsed.ogg");
    std::vector<Sound> turret_collapsed_sounds = {Sound(), Sound()};
    for (int i = 0; i < 2; i++)
    {
        turret_collapsed_sounds[i].setBuffer(turret_collapsed_buffer);
        turret_collapsed_sounds[i].setVolume(sound_effect_volume);
    }
    std::vector<bool> turret_collapsed_sound_played_bools = {false, false};
    // === poison puddles ===
    SoundBuffer poison_puddles_buffer;
    poison_puddles_buffer.loadFromFile("audio/sfx/poison-puddle.ogg");
    std::vector<Sound> poison_puddles_sounds = {Sound(), Sound()};
    for (int i = 0; i < 2; i++)
    {
        poison_puddles_sounds[i].setBuffer(poison_puddles_buffer);
        poison_puddles_sounds[i].setVolume(sound_effect_volume);
    }
    std::vector<bool> poison_puddles_sound_played_bools = {false, false};

    // music
    float music_volume = 100 * (int)settings_file["settings_preferences"]["music"] / 5;
    Music game_music;
    game_music.openFromFile("audio/music/Vandraren - Adrian von Ziegler.ogg");
    game_music.setLoop(true);
    game_music.setVolume(0);

    float total_time = 0.f, delta_time;
    Clock clock;

    game_music.play();

    // main loop
    while (window_state == "game")
    {
        Event window_event;
        while (window.pollEvent(window_event))
        {
            switch (window_event.type)
            {
            case Event::Closed:
                window_state = "quit";
                window.close();
            }
        }

        delta_time = clock.restart().asSeconds();
        if (delta_time > 1.0f / 20.0f)
        {
            delta_time = 1.0f / 20.0f;
        }
        if (!warrior.dead && !paused)
        {
            total_time += delta_time;
        }

        // black circle transition
        if (game_start)
        {
            if (x_pos - black_circle_transition.getRadius() < window.getSize().x)
                vel_x = Smooth(3000.f, vel_x, delta_time);
            else
            {
                vel_x = Smooth(0.f, vel_x, delta_time);
                if (vel_x == 0.f)
                {
                    x_pos = window.getSize().x + black_circle_transition.getRadius();
                    game_start = false;
                }
            }
            if (x_pos >= window.getSize().x - 30.f)
            {
                if (!whoosh_sound_played)
                {
                    whoosh_sound.play();
                    whoosh_sound_played = true;
                }
            }
            x_pos += vel_x * delta_time;
            black_circle_transition.setPosition(Vector2f(x_pos, y_pos));

            if (game_music.getVolume() < music_volume && x_pos >= window.getSize().x / 2.f)
                game_music.setVolume(game_music.getVolume() + 10 * delta_time);
        }

        if (!paused)
        {
            if (!warrior.dead)
            {
                if (warrior.can_jump)
                {
                    warrior_jumping_sound_played = false;
                    if (jumping_before)
                    {
                        if (!warrior_land_sound_played)
                        {
                            warrior_land_sound.play();
                            warrior_land_sound_played = true;
                        }
                    }
                    jumping_before = false;
                }

                if ((Keyboard::isKeyPressed(keyboard_movement_keys[0]) || Keyboard::isKeyPressed(keyboard_movement_keys[1]) || Keyboard::isKeyPressed(keyboard_movement_keys[2]) || Keyboard::isKeyPressed(keyboard_movement_keys[3]) || Keyboard::isKeyPressed(Keyboard::Space)) && app_in_focus(&window))
                {
                    if (Keyboard::isKeyPressed(keyboard_movement_keys[2]))
                    {
                        if (warrior_can_go_right)
                        {
                            warrior.Move(delta_time, "right");
                            warrior_texture = warrior_run;
                            warrior_state = "run";
                            total_sprite = 3;
                            switch_time = 0.15f;
                            if (!warrior_running_sound_played)
                            {
                                warrior_running_sound.play();
                                warrior_running_sound_played = true;
                            }
                        }
                        else
                            warrior.vel_x = 0.f;
                    }
                    if (Keyboard::isKeyPressed(keyboard_movement_keys[1]))
                    {
                        if (warrior_can_go_left)
                        {
                            warrior.Move(delta_time, "left");
                            warrior_texture = warrior_run;
                            warrior_state = "run";
                            total_sprite = 3;
                            switch_time = 0.15f;
                            if (!warrior_running_sound_played)
                            {
                                warrior_running_sound.play();
                                warrior_running_sound_played = true;
                            }
                        }
                        else
                            warrior.vel_x = 0.f;
                    }
                    if ((Keyboard::isKeyPressed(keyboard_movement_keys[0]) || Keyboard::isKeyPressed(Keyboard::Space)) && app_in_focus(&window))
                    {
                        warrior.Jump(delta_time);
                        if (!warrior.can_jump)
                        {
                            if (!warrior_jumping_sound_played)
                            {
                                warrior_jumping_sound.play();
                                warrior_jumping_sound_played = true;
                            }
                        }
                        jumping_before = true;
                    }
                    if (Keyboard::isKeyPressed(keyboard_movement_keys[3]) && app_in_focus(&window))
                        warrior.Fall(delta_time);
                }
                else
                {
                    warrior.vel_x = Smooth(0.f, warrior.vel_x, delta_time);
                    warrior_texture = warrior_idle;
                    warrior_state = "idle";
                    total_sprite = 2;
                    switch_time = 0.45f;
                    warrior_running_sound.stop();
                    warrior_running_sound_played = false;
                }

                if (!warrior.can_jump)
                {
                    warrior_texture = warrior_jump;
                    warrior_state = "jump";
                    total_sprite = 2;
                    warrior_running_sound.stop();
                    warrior_running_sound_played = false;
                    warrior_land_sound_played = false;
                }

                if (Mouse::isButtonPressed(Mouse::Left) && app_in_focus(&window))
                {
                    Vector2i mouse_pos = Mouse::getPosition(window);
                    if (mouse_pos.x > 0 && mouse_pos.x < window.getSize().x)
                    {
                        if (mouse_pos.y > menu_bar.GetSize().y && mouse_pos.y < window.getSize().y)
                        {
                            warrior.attacking = true;
                            if (!warrior_attacking_sound_played)
                            {
                                warrior_attacking_sound.play();
                                warrior_attacking_sound_played = true;
                            }
                        }
                    }
                }

                if (warrior.attacking)
                {
                    if (attack_time == 0)
                        warrior.SetSpriteCount(0);
                    attack_time += delta_time;
                    warrior_texture = warrior_attack;
                    warrior_state = "attack";
                    total_sprite = 2;
                    switch_time = 0.3f;
                }

                if (attack_time >= switch_time * total_sprite)
                {
                    warrior.attacking = false;
                    attack_time = 0;
                    warrior_attacking_sound_played = false;
                }
            }

            warrior.Gravity(delta_time);

            // if warrior is attacking set the hitbox for sword and player individually
            if (warrior.attacking && warrior.GetSpriteCount() == 1)
            {
                sword_attacking_hitbox.setPosition(Vector2f(warrior.GetPosition().x - warrior.GetSize().x / 2.f + sword_attacking_hitbox.getSize().x / 2.f, warrior.GetPosition().y - warrior.GetSize().y / 2.f + 18 * 30 / 11.5 + sword_attacking_hitbox.getSize().y / 2.f));
                warrior_attacking_hitbox.setPosition(Vector2f(warrior.GetPosition().x - warrior.GetSize().x / 2.f + 17 * 30 / 11.5 + warrior_attacking_hitbox.getSize().x / 2.f, warrior.GetPosition().y - warrior.GetSize().y / 2.f + warrior_attacking_hitbox.getSize().y / 2.f));

                if (warrior.faces_right)
                {
                    sword_attacking_hitbox.setPosition(Vector2f(sword_attacking_hitbox.getPosition().x + warrior.GetSize().x / 2.f + sword_attacking_hitbox.getSize().x / 2.f, sword_attacking_hitbox.getPosition().y));
                    warrior_attacking_hitbox.setPosition(Vector2f(warrior_attacking_hitbox.getPosition().x - warrior_attacking_hitbox.getSize().x / 2.f, warrior_attacking_hitbox.getPosition().y));
                }
            }
            // if not, hide hitboxes
            else
            {
                sword_attacking_hitbox.setPosition(Vector2f(-200, -200));
                warrior_attacking_hitbox.setPosition(Vector2f(-200, -200));
            }

            // keeps player from walking away from screen
            if (warrior.GetPosition().x >= window.getSize().x)
                warrior_can_go_right = false;
            else
                warrior_can_go_right = true;
            if (warrior.GetPosition().x <= 0)
                warrior_can_go_left = false;
            else
                warrior_can_go_left = true;

            // collision detection with ground
            string collision;
            for (int i = 0; i < 3; i++)
            {
                collision = warrior_ground_collision.CheckCollision(ground[i]);
                if (collision != "none")
                {
                    warrior.PlatformCollision(ground[i]);
                    break;
                }
            }
            // collision detection with platforms
            platforms[0].SetPosition(window.getSize().x * 1.f / 2.f - platforms[0].GetSize().x / 2.f, menu_bar.GetSize().y + 110.f);
            platforms[1].SetPosition(window.getSize().x * 1.f / 4.f - platforms[1].GetSize().x / 2.f, platforms[0].GetPosition().y + platforms[0].GetSize().y + 80.f);
            platforms[2].SetPosition(window.getSize().x * 3.f / 4.f - platforms[1].GetSize().x / 2.f, platforms[1].GetPosition().y + 8.f);
            platforms[3].SetPosition(window.getSize().x * 1.f / 2.f - platforms[3].GetSize().x / 2.f, platforms[1].GetPosition().y + platforms[1].GetSize().y + 80.f);
            for (int i = 0; i < 4; i++)
            {
                platforms[i].SetOrigin(0, 0);
            }
            for (int i = 0; i < 4; i++)
            {
                collision = warrior_ground_collision.CheckCollision(platforms[i].GetRect());
                if (collision != "none")
                {
                    if (warrior.GetPosition().y + warrior.GetSize().y / 2 < platforms[i].GetPosition().y)
                    {
                        if (warrior.vel_y > 0.f)
                        {
                            warrior.PlatformCollision(platforms[i].GetRect());
                            break;
                        }
                    }
                }
            }
            // warrior falling from platform
            if (!warrior.dead)
            {
                for (int i = 0; i < 4; i++)
                {
                    if (warrior.can_jump)
                    {
                        if (warrior.GetPosition().y + warrior.GetSize().y / 2 < platforms[i].GetPosition().y)
                        {
                            if (platforms[i].GetPosition().x >= warrior.GetPosition().x + warrior.GetSize().x / 2 || warrior.GetPosition().x - warrior.GetSize().x / 2 >= platforms[i].GetPosition().x + platforms[i].GetSize().x)
                            {
                                warrior.Fall(delta_time);
                                jumping_before = true;
                            }
                            else
                            {
                                break;
                            }
                        }
                    }
                }
            }

            // deploying fireballs
            if (!warrior.dead)
            {
                if (fireballs.size() < MAX_FIREBALL)
                {
                    // randomizing fireball deploy side
                    string direction;
                    int direction_chance = rand() % 2;
                    if (direction_chance == 0)
                        direction = "right";
                    else if (direction_chance == 1)
                        direction = "left";
                    Fireball fireball(&fireball_texture, direction, window);
                    // randomizing fireball deploy height from ground
                    float height = fireball.RandomizeHeight(platforms, ground);
                    fireball.SetPosition(fireball.GetPosition().x, height);
                    fireballs.push_back(fireball);
                }
            }
            // checks if any fireball is in the screen
            for (int i = 0; i < fireballs.size(); i++)
            {
                if (fireballs[i].GetPosition().x > 0 && fireballs[i].GetPosition().x < window.getSize().x)
                {
                    if (!fireball_sound_played)
                    {
                        fireball_sound.play();
                        fireball_sound_played = true;
                    }
                    break;
                }
                if (i == fireballs.size() - 1)
                {
                    fireball_sound.stop();
                    fireball_sound_played = false;
                }
            }
            // fade in and out sound of fireball
            if (fireball_sound.getVolume() < sound_effect_volume && fireball_sound_played)
                fireball_sound.setVolume(fireball_sound.getVolume() + 55 * delta_time);
            if (fireball_sound.getVolume() > 0 && !fireball_sound_played)
                fireball_sound.setVolume(fireball_sound.getVolume() - 55 * delta_time);
            // erasing fireballs if they are beyond the screen
            if (fireballs.size() == MAX_FIREBALL)
            {
                for (int i = 0; i < MAX_FIREBALL; i++)
                {
                    if (fireballs[i].can_deploy)
                    {
                        if (fireballs[i].GetPosition().x >= window.getSize().x + fireballs[i].GetSize().x + 50 || fireballs[i].GetPosition().x + fireballs[i].GetSize().x <= -50)
                        {
                            fireballs.erase(fireballs.begin() + i);
                        }
                    }
                }
            }
            // decreasing player health and erasing fireballs if colliding with player
            if (!warrior.dead)
            {
                if (fireballs.size() == MAX_FIREBALL)
                {
                    for (int i = 0; i < MAX_FIREBALL; i++)
                    {
                        if (!warrior.attacking)
                        {
                            if (warrior_collision.CheckCollision(fireballs[i].GetFireballRect()))
                            {
                                health_bar.animate = true;
                                health_bar.grow = false;
                                health_bar.health -= 60.f;
                                fireballs.erase(fireballs.begin() + i);
                                screen_shake.shaking = true;
                                if (!warrior_damaged_sound_played)
                                    warrior_damaged_sound.play();
                            }
                        }
                        else
                        {
                            if (warrior_attacking_hitbox_collision.CheckCollision(fireballs[i].GetFireballRect()))
                            {
                                health_bar.animate = true;
                                health_bar.grow = false;
                                health_bar.health -= 60.f;
                                fireballs.erase(fireballs.begin() + i);
                                screen_shake.shaking = true;
                                if (!warrior_damaged_sound_played)
                                    warrior_damaged_sound.play();
                            }
                        }
                    }
                }
            }
            // spawning fireballs every fireball_spawn_time seconds
            if (fireballs.size() == MAX_FIREBALL)
            {
                fireball_spawn_timer += delta_time;
                for (int i = 0; i < MAX_FIREBALL; i++)
                {
                    if (fireball_spawn_timer >= fireball_spawn_time && !fireballs[i].can_deploy)
                    {
                        fireballs[i].can_deploy = true;
                        fireball_spawn_timer = 0.f;
                    }
                }
            }
            // increasing fireball spawn time periodically
            fireball_spawn_time_increase_time += delta_time;
            if (fireball_spawn_time_increase_time >= 5.f)
            {
                fireball_spawn_time -= 0.175f;
                if (fireball_spawn_time <= 0)
                    fireball_spawn_time = 0;
                fireball_spawn_time_increase_time = 0;
            }

            // deploying turrets
            if (!warrior.dead)
            {
                if (turrets.size() < MAX_TURRET)
                {
                    Turret turret(&turret_texture, platforms[0]);
                    // randomizing turret deploy position
                    Vector2f pos = turret.RandomizePosition(platforms, ground, window);
                    while (turrets.size() == 1 && pos.x == turrets[0].GetPosition().x)
                    {
                        pos = turret.RandomizePosition(platforms, ground, window);
                    }
                    turret.SetPosition(pos.x, pos.y + platforms[0].GetSize().y + turret.GetSize().y / 2.f);
                    turrets.push_back(turret);
                }
                // spawning turrets every turret_appear_time seconds
                if (turrets.size() == MAX_TURRET)
                {
                    turret_appear_timer += delta_time;
                    for (int i = 0; i < MAX_TURRET; i++)
                    {
                        if (turret_appear_timer >= turret_appear_time && !turrets[i].can_spawn)
                        {
                            turrets[i].can_spawn = true;
                            turret_appear_timer = 0.f;
                        }
                    }
                }
                // decreasing turret hitpoints when player's sword colliding with turret
                if (turrets.size() == MAX_TURRET)
                {
                    for (int i = 0; i < MAX_TURRET; i++)
                    {
                        if (warrior.attacking)
                        {
                            if (sword_attacking_hitbox_collision.CheckCollision(turrets[i].GetTurretRect()))
                            {
                                if (turrets[i].animated)
                                {
                                    if (sword_hit_delay == 0)
                                    {
                                        turrets[i].hit += 1;
                                        if (!turret_damaged_sound_played && !turrets[i].collapsed)
                                        {
                                            int rand_index = rand() % 2;
                                            if (rand_index == 0)
                                                turret_damaged1_sound.play();
                                            else
                                                turret_damaged2_sound.play();
                                            turret_damaged_sound_played = true;
                                        }
                                    }
                                }
                                sword_hit_delay += delta_time;
                                break;
                            }
                            else
                                turret_damaged_sound_played = false;
                        }
                        else
                        {
                            sword_hit_delay = 0;
                            turret_damaged_sound_played = false;
                        }
                    }
                }
            }
            if (sword_hit_delay >= switch_time + 0.01f && warrior.attacking)
                sword_hit_delay = 0;
            if (turrets.size() == MAX_TURRET)
            {
                for (int i = 0; i < MAX_TURRET; i++)
                {
                    if (turrets[i].collapse_now && !turrets[i].collapsed && !turret_collapsed_sound_played_bools[i])
                    {
                        turret_collapsed_sounds[i].play();
                        turret_collapsed_sound_played_bools[i] = true;
                    }
                    if (turrets[i].collapsed)
                        turret_collapsed_sound_played_bools[i] = false;
                }
            }
            // removes turret ruins after some time
            if (turrets.size() == MAX_TURRET)
            {
                for (int i = 0; i < MAX_TURRET; i++)
                {
                    if (turrets[i].turret_ruins_disappear_time >= 2.f)
                    {
                        turrets.erase(turrets.begin() + i);
                        break;
                    }
                }
            }
            // increase turret_appear_time periodically
            turret_deploy_time_increase_time += delta_time;
            if (turret_deploy_time_increase_time >= 9.5f)
            {
                turret_appear_time -= 2.5f;
                turret_deploy_time_increase_time = 0;
            }

            // spawning poison liquids
            if (!warrior.dead)
            {
                if (poison_liquids.size() < MAX_TURRET)
                {
                    poison_liquids.push_back(PoisonLiquid());
                    poison_puddles.push_back(PoisonPuddle());
                    poison_gases.push_back(PoisonGasEffect(&poison_gas_texture));
                }
            }
            // hide poison liquid if colliding with ground
            if (poison_liquids.size() == MAX_TURRET)
            {
                for (int i = 0; i < MAX_TURRET; i++)
                {
                    if (poison_liquids[i].GetPosition().y >= ground[0].getPosition().y)
                    {
                        poison_puddles[i].SetPositionConstant(poison_liquids[i].GetPosition().x, poison_liquids[i].GetPosition().y);
                        poison_puddles[i].can_appear = true;
                        poison_gases[i].SetPosition(poison_puddles[i].GetPosition().x, poison_puddles[i].GetPosition().y - 29.f);
                        poison_gases[i].can_appear = true;
                        poison_liquids[i].SetPosition(0, 0);
                        poison_liquids[i].SetSize(0, 0);
                        poison_liquids[i].SetVelocity(0, 0);
                        poison_liquids[i].appear_delay = 0.f;
                        poison_liquids[i].shoot_delay = 0.f;
                        poison_liquids[i].can_appear = false;
                        poison_liquids[i].can_shoot = false;
                        poison_liquids[i].shot = false;
                        if (!poison_puddles_sound_played_bools[i])
                        {
                            poison_puddles_sounds[i].play();
                            poison_puddles_sound_played_bools[i] = true;
                        }
                    }
                }
            }
            // hide poison liquid if colliding with platform
            platforms[0].SetPosition(window.getSize().x * 1.f / 2.f, menu_bar.GetSize().y + 110.f + platforms[0].GetSize().y / 2.f);
            platforms[1].SetPosition(window.getSize().x * 1.f / 4.f, platforms[0].GetPosition().y + platforms[0].GetSize().y / 2.f + 80.f + platforms[1].GetSize().y / 2.f);
            platforms[2].SetPosition(window.getSize().x * 3.f / 4.f, platforms[1].GetPosition().y + 8.f);
            platforms[3].SetPosition(window.getSize().x * 1.f / 2.f, platforms[1].GetPosition().y + platforms[1].GetSize().y / 2.f + 80.f + platforms[3].GetSize().y / 2.f);
            for (int j = 0; j < platforms.size(); j++)
            {
                platforms[j].SetOrigin(platforms[j].GetSize().x / 2.f, platforms[j].GetSize().y / 2.f);
            }
            if (poison_liquids.size() == MAX_TURRET)
            {
                for (int i = 0; i < MAX_TURRET; i++)
                {
                    for (int j = 0; j < platforms.size(); j++)
                    {
                        if (poison_liquids[i].CheckCollision(platforms[j].GetRect()))
                        {
                            if (poison_liquids[i].GetVelocity().y > 0)
                            {
                                poison_puddles[i].SetPositionConstant(poison_liquids[i].GetPosition().x, platforms[j].GetPosition().y - platforms[j].GetSize().y / 2.f);
                                poison_puddles[i].can_appear = true;
                                poison_gases[i].SetPosition(poison_puddles[i].GetPosition().x, poison_puddles[i].GetPosition().y - 29.f);
                                poison_gases[i].can_appear = true;
                                poison_liquids[i].SetPosition(0, 0);
                                poison_liquids[i].SetSize(0, 0);
                                poison_liquids[i].SetVelocity(0, 0);
                                poison_liquids[i].appear_delay = 0.f;
                                poison_liquids[i].shoot_delay = 0.f;
                                poison_liquids[i].can_appear = false;
                                poison_liquids[i].can_shoot = false;
                                poison_liquids[i].shot = false;
                                if (!poison_puddles_sound_played_bools[i])
                                {
                                    poison_puddles_sounds[i].play();
                                    poison_puddles_sound_played_bools[i] = true;
                                }
                                break;
                            }
                        }
                    }
                }
            }
            // hide poison liquid if disappeared
            if (poison_liquids.size() == MAX_TURRET)
            {
                for (int i = 0; i < MAX_TURRET; i++)
                {
                    if (poison_liquids[i].GetSize().x <= 0 && poison_liquids[i].GetSize().y <= 0 && poison_liquids[i].can_appear)
                    {
                        poison_liquids[i].SetPosition(0, 0);
                        poison_liquids[i].SetSize(0, 0);
                        poison_liquids[i].SetVelocity(0, 0);
                        poison_liquids[i].appear_delay = 0.f;
                        poison_liquids[i].shoot_delay = 0.f;
                        poison_liquids[i].can_appear = false;
                        poison_liquids[i].can_shoot = false;
                        poison_liquids[i].shot = false;
                        poison_puddles_sound_played_bools[i] = false;
                    }
                }
            }
            // decreasing player health every 1 second if colliding with player
            if (poison_puddles.size() == MAX_TURRET)
            {
                for (int i = 0; i < MAX_TURRET; i++)
                {
                    if (!warrior.attacking)
                    {
                        if (warrior_collision.CheckCollision(poison_puddles[i].GetPdlRect()))
                        {
                            if (poison_damage_delay == 0 && !warrior.dead)
                            {
                                health_bar.animate = true;
                                health_bar.grow = false;
                                health_bar.health -= 30.f;
                                screen_shake.shaking = true;
                                if (!warrior_damaged_sound_played)
                                {
                                    warrior_damaged_sound.play();
                                    warrior_damaged_sound_played = true;
                                }
                            }
                            poison_damage_delay += delta_time;
                            if (poison_damage_delay >= 1.f)
                            {
                                poison_damage_delay = 0;
                                warrior_damaged_sound_played = false;
                            }
                        }
                    }
                    else
                    {
                        if (warrior_attacking_hitbox_collision.CheckCollision(poison_puddles[i].GetPdlRect()) && warrior.vel_y > 0)
                        {
                            if (poison_damage_delay == 0 && !warrior.dead)
                            {
                                health_bar.animate = true;
                                health_bar.grow = false;
                                health_bar.health -= 30.f;
                                screen_shake.shaking = true;
                                if (!warrior_damaged_sound_played)
                                {
                                    warrior_damaged_sound.play();
                                    warrior_damaged_sound_played = true;
                                }
                            }
                            poison_damage_delay += delta_time;
                            if (poison_damage_delay >= 1.f)
                            {
                                poison_damage_delay = 0;
                                warrior_damaged_sound_played = false;
                            }
                        }
                    }
                }
            }
            // disappearing poison puddle after some time
            if (poison_puddles.size() == MAX_TURRET)
            {
                for (int i = 0; i < MAX_TURRET; i++)
                {
                    if (poison_puddles[i].can_appear)
                    {
                        poison_puddles[i].disappear_timer += delta_time;
                        if (poison_puddles[i].disappear_timer >= 4.35f)
                        {
                            poison_puddles[i].SetPosition(-200, -100);
                            poison_puddles[i].can_appear = false;
                            poison_puddles[i].SetSpriteCount(0);
                            poison_puddles[i].pdl_animation_complete = false;
                            poison_gases[i].can_appear = false;

                            poison_puddles[i].disappear_timer = 0;
                        }
                    }
                }
            }

            // spawning hearts
            platforms[0].SetPosition(window.getSize().x * 1.f / 2.f - platforms[0].GetSize().x / 2.f, menu_bar.GetSize().y + 110.f);
            platforms[1].SetPosition(window.getSize().x * 1.f / 4.f - platforms[1].GetSize().x / 2.f, platforms[0].GetPosition().y + platforms[0].GetSize().y + 80.f);
            platforms[2].SetPosition(window.getSize().x * 3.f / 4.f - platforms[1].GetSize().x / 2.f, platforms[1].GetPosition().y + 8.f);
            platforms[3].SetPosition(window.getSize().x * 1.f / 2.f - platforms[3].GetSize().x / 2.f, platforms[1].GetPosition().y + platforms[1].GetSize().y + 80.f);
            for (int i = 0; i < 4; i++)
            {
                platforms[i].SetOrigin(0, 0);
            }
            if (!warrior.dead)
            {
                if (hearts.size() < MAX_HEART)
                {
                    Heart heart(&heart_texture);
                    // randomizing heart spawn position
                    Vector2f pos = heart.RandomizePosition(platforms, ground);
                    while (hearts.size() == 1 && pos.x == hearts[0].GetPosition().x)
                    {
                        pos = heart.RandomizePosition(platforms, ground);
                    }
                    heart.SetPosition(pos.x, pos.y);
                    hearts.push_back(heart);
                }
            }
            // increasing player health if colliding with heart
            if (!warrior.dead)
            {
                if (hearts.size() == MAX_HEART)
                {
                    for (int i = 0; i < MAX_HEART; i++)
                    {
                        if (hearts[i].can_spawn)
                        {
                            if (hearts[i].animated)
                            {
                                if (!warrior.attacking)
                                {
                                    if (warrior_collision.CheckCollision(hearts[i].GetHeartRect()))
                                    {
                                        health_bar.animate = true;
                                        health_bar.grow = true;
                                        health_bar.health += 40.f;
                                        hearts.erase(hearts.begin() + i);
                                    }
                                }
                                else
                                {
                                    if (warrior_attacking_hitbox_collision.CheckCollision(hearts[i].GetHeartRect()))
                                    {
                                        health_bar.animate = true;
                                        health_bar.grow = true;
                                        health_bar.health += 40.f;
                                        hearts.erase(hearts.begin() + i);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            // spawning hearts every 7 seconds
            if (hearts.size() == MAX_HEART)
            {
                heart_appear_timer += delta_time;
                for (int i = 0; i < MAX_HEART; i++)
                {
                    if (heart_appear_timer >= 7.f && !hearts[i].can_spawn)
                    {
                        hearts[i].can_spawn = true;
                        heart_appear_timer = 0.f;
                    }
                }
            }

            // screen shake and redden effect when player collides with fireball
            if (screen_shake.shaking)
            {
                screen_shake.Update(window, delta_time, view);
                screen_redden.alpha = 255.f;
                screen_redden.fade_now = true;
            }

            // player dies if his health is 0 or below
            if (health_bar.health <= 0)
            {
                warrior.dead = true;
                warrior.vel_x = 0.f;
                warrior_texture = warrior_die;
                warrior_state = "die";
                total_sprite = 4;
                switch_time = 0.4f;
            }
        }

        if (Mouse::isButtonPressed(Mouse::Left) && app_in_focus(&window))
        {
            // pause button
            if (!warrior.dead)
            {
                if (pause_button.CheckButtonClicked(Mouse::getPosition(window)))
                {
                    if (!paused)
                        paused = true;
                    if (!button_click_sound_played)
                    {
                        button_click_sound.play();
                        button_click_sound_played = true;
                    }
                }
            }

            // pause menu
            if (paused)
            {
                int button_clicked_index = pause_menu.CheckButtonClicked(Mouse::getPosition(window));
                if (button_clicked_index == 0)
                {
                    paused = false;
                    if (!button_click_sound_played)
                    {
                        button_click_sound.play();
                        button_click_sound_played = true;
                    }
                }
                else if (button_clicked_index == 1)
                {
                    restarted = true;
                    if (!button_click_sound_played)
                    {
                        button_click_sound.play();
                        button_click_sound_played = true;
                    }
                }
                else if (button_clicked_index == 2)
                {
                    switch_to_settings = true;
                    if (!button_click_sound_played)
                    {
                        button_click_sound.play();
                        button_click_sound_played = true;
                    }
                }
                else if (button_clicked_index == 3)
                {
                    game_end = true;
                    whoosh_sound_played = false;
                    if (!button_click_sound_played)
                    {
                        button_click_sound.play();
                        button_click_sound_played = true;
                    }
                }
            }

            // dead menu
            if (dead_menu.transitioned)
            {
                int button_clicked_index = dead_menu.CheckButtonClicked(Mouse::getPosition(window));
                if (button_clicked_index == 0)
                {
                    restarted = true;
                    if (!button_click_sound_played)
                    {
                        button_click_sound.play();
                        button_click_sound_played = true;
                    }
                }
                else if (button_clicked_index == 1)
                {
                    switch_to_settings = true;
                    if (!button_click_sound_played)
                    {
                        button_click_sound.play();
                        button_click_sound_played = true;
                    }
                }
                else if (button_clicked_index == 2)
                {
                    game_end = true;
                    whoosh_sound_played = false;
                    if (!button_click_sound_played)
                    {
                        button_click_sound.play();
                        button_click_sound_played = true;
                    }
                }
            }
        }
        else
            button_click_sound_played = false;
        if (Keyboard::isKeyPressed(Keyboard::Escape) && app_in_focus(&window))
        {
            if (esc_pause_delay == 0.f)
            {
                if (!paused)
                    paused = true;
                else
                    paused = false;
                button_click_sound.play();
            }
            esc_pause_delay += delta_time;
        }
        else
            esc_pause_delay = 0.f;

        if (esc_pause_delay >= 0.5f)
            esc_pause_delay = 0.f;

        if (paused)
        {
            warrior.vel_x = 0;
            warrior.vel_y = 0;
            for (int i = 0; i < fireballs.size(); i++)
            {
                fireballs[i].vel_x = 0.f;
            }
            for (int i = 0; i < poison_liquids.size(); i++)
            {
                if (!poison_liquids[i].paused)
                {
                    poison_liquids[i].saved_vel_x = poison_liquids[i].GetVelocity().x;
                    poison_liquids[i].saved_vel_y = poison_liquids[i].GetVelocity().y;
                    poison_liquids[i].paused = true;
                }
                poison_liquids[i].SetVelocity(0, 0);
            }
        }
        else
        {
            for (int i = 0; i < fireballs.size(); i++)
            {
                fireballs[i].vel_x = 350.f;
            }
            for (int i = 0; i < poison_liquids.size(); i++)
            {
                if (poison_liquids[i].paused)
                {
                    poison_liquids[i].SetVelocity(poison_liquids[i].saved_vel_x, poison_liquids[i].saved_vel_y);
                    poison_liquids[i].saved_vel_x = 0;
                    poison_liquids[i].saved_vel_y = 0;
                    poison_liquids[i].paused = false;
                }
            }
        }

        if (warrior.dead)
        {
            screen_darken_fade_delay += delta_time;
            if (screen_darken_fade_delay >= 2.f)
            {
                screen_darken.fade_now = true;
                screen_darken_fade_delay = 0;
            }
            warrior_running_sound.stop();
        }

        if (restarted)
        {
            // warrior
            warrior_texture = warrior_idle;
            warrior_state = "idle";
            int total_sprite = 2;
            float switch_time = 0.45f;
            warrior.dead = false;
            warrior.dead_animation_complete = false;
            warrior.x_pos = window.getSize().x / 2.f;
            bool warrior_can_go_right = true;
            bool warrior_can_go_left = true;
            warrior_running_sound.stop();
            warrior_running_sound_played = false;

            // health bar
            health_bar.health = 100.f;
            health_bar.animate = true;
            health_bar.grow = true;

            // fireballs
            fireballs.clear();
            fireball_spawn_timer = 0.f;
            fireball_spawn_time = 3.f;
            fireball_spawn_time_increase_time = 0.f;

            // turrets
            turrets.clear();
            poison_liquids.clear();
            poison_puddles.clear();
            poison_gases.clear();
            turret_appear_timer = 0.f;
            turret_appear_time = 8.f;
            turret_deploy_time_increase_time = 0.f;

            // heart
            hearts.clear();
            heart_appear_timer = 0.f;

            view.setCenter(window.getSize().x / 2.f, window.getSize().y / 2.f);

            screen_darken_fade_delay = 0.f;
            screen_darken.SetOpacity(0);
            screen_darken.alpha = 0.f;
            screen_darken.fade_now = false;
            screen_darken.faded = false;

            // dead menu
            dead_menu.Reset(window);

            total_time = 0.f;

            restarted = false;
            paused = false;
        }

        if (switch_to_settings)
        {
            settings(window, window_state, switch_to_settings, game_music);

            json settings_file;
            std::ifstream file("saves/user_data_and_preference.json");
            file >> settings_file;
            file.close();

            keyboard_movement_keys.clear();
            if (settings_file["settings_preferences"]["keyboard_preference"] == "wasd")
            {
                keyboard_movement_keys.push_back(Keyboard::W);
                keyboard_movement_keys.push_back(Keyboard::A);
                keyboard_movement_keys.push_back(Keyboard::D);
                keyboard_movement_keys.push_back(Keyboard::S);
            }
            else
            {
                keyboard_movement_keys.push_back(Keyboard::Up);
                keyboard_movement_keys.push_back(Keyboard::Left);
                keyboard_movement_keys.push_back(Keyboard::Right);
                keyboard_movement_keys.push_back(Keyboard::Down);
            }

            sound_effect_volume = 100 * (int)settings_file["settings_preferences"]["sound_effect"] / 5;
            music_volume = 100 * (int)settings_file["settings_preferences"]["music"] / 5;
            whoosh_sound.setVolume(sound_effect_volume);
            button_click_sound.setVolume(sound_effect_volume);
            warrior_running_sound.setVolume(sound_effect_volume);
            warrior_jumping_sound.setVolume(sound_effect_volume);
            warrior_land_sound.setVolume(sound_effect_volume);
            warrior_attacking_sound.setVolume(sound_effect_volume);
            warrior_damaged_sound.setVolume(sound_effect_volume);
            fireball_sound.setVolume(sound_effect_volume);
            turret_shoot_sound.setVolume(sound_effect_volume);
            turret_damaged1_sound.setVolume(sound_effect_volume);
            turret_damaged2_sound.setVolume(sound_effect_volume);
            for (int i = 0; i < 2; i++)
            {
                turret_collapsed_sounds[i].setVolume(sound_effect_volume);
                poison_puddles_sounds[i].setVolume(sound_effect_volume);
            }
            game_music.setVolume(music_volume);
        }

        if (game_end)
        {
            // black circle transition
            if (x_pos > window.getSize().x / 2.f)
            {
                vel_x = 1000.f;
                if (!whoosh_sound_played)
                {
                    whoosh_sound.play();
                    whoosh_sound_played = true;
                }
            }
            else
            {
                vel_x = 0.f;
                game_end = false;
                window_state = "title-screen";
            }
            x_pos -= vel_x * delta_time;
            black_circle_transition.setPosition(Vector2f(x_pos, y_pos));

            if (game_music.getVolume() > 0)
                game_music.setVolume(game_music.getVolume() - 50 * delta_time);
        }

        window.clear();
        background.Draw(window);
        if (turrets.size() == MAX_TURRET)
        {
            for (int i = 0; i < MAX_TURRET; i++)
            {
                if (turrets[i].can_spawn)
                {
                    turrets[i].Emerge(delta_time);
                    turrets[i].Update(&turret_texture, &turret_collapse_texture, platforms[0], delta_time);
                    turrets[i].Draw(window);
                }
            }
        }
        for (int i = 0; i < poison_liquids.size(); i++)
        {
            if (turrets[i].animated)
            {
                poison_liquids[i].Update(turrets[i], delta_time, warrior.GetPosition(), paused, turret_shoot_sound, turret_shoot_sound_played);
                poison_liquids[i].Draw(window);
            }
        }
        for (int i = 0; i < 3; i++)
        {
            window.draw(ground[i]);
        }
        for (int i = 0; i < 4; i++)
        {
            platforms[i].Draw(window);
        }
        for (int i = 0; i < poison_puddles.size(); i++)
        {
            if (poison_puddles[i].can_appear)
            {
                poison_puddles[i].Update(delta_time);
                poison_puddles[i].Draw(window);
            }
        }
        for (int i = 0; i < poison_gases.size(); i++)
        {
            if (poison_gases[i].can_appear)
            {
                poison_gases[i].Update(delta_time);
                poison_gases[i].Draw(window);
            }
        }
        warrior_ground_collision.Update(warrior.GetPlayerRect());
        warrior_collision.Update(warrior.GetPlayerRect());
        sword_attacking_hitbox_collision.Update(sword_attacking_hitbox);
        warrior_attacking_hitbox_collision.Update(warrior_attacking_hitbox);
        warrior.Update(&warrior_texture, warrior_state, total_sprite, switch_time, delta_time);
        warrior.Draw(window);
        window.draw(warrior_attacking_hitbox);
        window.draw(sword_attacking_hitbox);
        if (hearts.size() == MAX_HEART)
        {
            for (int i = 0; i < MAX_HEART; i++)
            {
                if (hearts[i].can_spawn)
                {
                    hearts[i].Animate(delta_time);
                    hearts[i].Draw(window);
                }
            }
        }
        if (fireballs.size() == MAX_FIREBALL)
        {
            for (int i = 0; i < MAX_FIREBALL; i++)
            {
                if (fireballs[i].can_deploy)
                {
                    fireballs[i].Move(delta_time);
                    fireballs[i].Update(&fireball_texture, delta_time);
                    fireballs[i].Draw(window);
                }
            }
        }
        if (screen_redden.fade_now)
        {
            screen_redden.Draw(window);
            screen_redden.FadeOut(delta_time);
        }
        menu_bar.Draw(window);
        pause_button.Draw(window);
        health_bar.Animate(delta_time);
        health_bar.Update();
        health_bar.Draw(window);

        if (screen_darken.fade_now)
        {
            screen_darken.FadeIn(delta_time);
        }
        screen_darken.Draw(window);
        if (!warrior.dead)
        {
            if (paused)
            {
                screen_darken.SetOpacity(155.f);
                pause_menu.Draw(window);
            }
            else
            {
                screen_darken.SetOpacity(0.f);
            }
        }
        if (screen_darken.faded)
        {
            screen_darken.fade_now = false;
            dead_menu.Animate(window, total_time, delta_time);
            dead_menu.Draw(window);
        }
        window.draw(black_circle_transition);

        window.setView(view);
        view.setCenter(screen_shake.original_view_pos.x, screen_shake.original_view_pos.y);
        window.display();
    }
}