#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <nlohmann/json.hpp>
#include <cmath>
#include <iostream>
#include <windows.h>
#include "settings.h"
#include "ScreenEffects.h"
using string = std::string;
using namespace sf;

bool button_clicked(RectangleShape button, Vector2i mouse_pos);
float Smooth(float goal, float current, float dt);

class Background
{
public:
    Background(RenderWindow &window, Texture *background_texture)
    {
        if (window.getSize().x / window.getSize().y == 16.f / 9.f)
        {
            bg_width = window.getSize().x;
            bg_height = window.getSize().y;
        }
        else
        {
            bg_width = window.getSize().y / 9.f * 16.f;
            bg_height = window.getSize().y;
            if (bg_width < window.getSize().x)
            {
                bg_width = window.getSize().x;
                bg_height = window.getSize().x / 16.f * 9.f;
            }
        }
        background.setSize(Vector2f(bg_width, bg_height));
        background.setOrigin(Vector2f(bg_width / 2, bg_height / 2));
        background.setPosition(Vector2f(window.getSize().x / 2, window.getSize().y / 2));
        background.setTexture(background_texture);
    }

    void Draw(RenderWindow &window)
    {
        window.draw(background);
    }

private:
    RectangleShape background;
    float bg_width, bg_height;
};

void title_screen(RenderWindow &window, string &window_state)
{
    // transition
    CircleShape black_circle_transition;
    black_circle_transition.setFillColor(Color::Black);
    black_circle_transition.setRadius(523.f);
    black_circle_transition.setOrigin(Vector2f(black_circle_transition.getRadius(), black_circle_transition.getRadius()));
    float x_pos = window.getSize().x / 2.f, y_pos = window.getSize().y / 2.f;
    float vel_x = 0.f;
    black_circle_transition.setPosition(Vector2f(x_pos, y_pos));

    // background
    Texture background_texture;
    background_texture.loadFromFile("gfx/sprite and image/background.jpg");
    Background background(window, &background_texture);

    // game title
    RectangleShape game_title;
    Texture game_title_texture;
    game_title_texture.loadFromFile("gfx/sprite and image/game-title.png");
    game_title.setTexture(&game_title_texture);
    game_title.setSize(Vector2f(400, 132));
    game_title.setOrigin(Vector2f(game_title.getSize().x / 2.f, 0));
    float game_title_y_pos = 69.f;
    game_title.setPosition(Vector2f(window.getSize().x / 2.f, game_title_y_pos));

    // buttons
    Texture button_texture;
    button_texture.loadFromFile("gfx/sprite and image/button.png");
    Font button_font;
    button_font.loadFromFile("gfx/font/VPPixel.otf");
    std::vector<RectangleShape> buttons;
    std::vector<Text> button_texts;
    for (int i = 0; i < 3; i++)
    {
        button_texts.push_back(Text());
        button_texts[i].setFont(button_font);
        button_texts[i].setFillColor(Color::Black);
        button_texts[i].setCharacterSize(30);
        button_texts[i].setStyle(Text::Bold);
    }
    button_texts[0].setString("PLAY");
    button_texts[1].setString("SETTINGS");
    button_texts[2].setString("HOW TO PLAY");
    for (int i = 0, y_start = 250; i < 3; i++)
    {
        buttons.push_back(RectangleShape());
        buttons[i].setTexture(&button_texture);
        buttons[i].setSize(Vector2f(350, 63));
        buttons[i].setPosition(Vector2f(window.getSize().x / 2.f - buttons[i].getSize().x / 2.f, y_start));

        button_texts[i].setOrigin(Vector2f(button_texts[i].getLocalBounds().width / 2.f, button_texts[i].getLocalBounds().height / 2.f));
        button_texts[i].setPosition(Vector2f(window.getSize().x / 2.f, y_start + buttons[i].getSize().y / 2.f - 8));

        y_start += buttons[i].getSize().y + 24;
    }
    bool can_click_button = false;

    // How to play
    Texture how_to_play_texture;
    how_to_play_texture.loadFromFile("gfx/sprite and image/how-to-play.png");
    RectangleShape how_to_play;
    how_to_play.setTexture(&how_to_play_texture);
    how_to_play.setSize(Vector2f(700, 560));
    how_to_play.setOrigin(Vector2f(how_to_play.getSize().x / 2.f, how_to_play.getSize().y / 2.f));
    how_to_play.setPosition(Vector2f(window.getSize().x / 2.f, window.getSize().y / 2.f));
    bool display_how_to_play = false;
    RectangleShape how_to_play_exit;
    how_to_play_exit.setFillColor(Color(0, 0, 0, 0));
    how_to_play_exit.setSize(Vector2f(140, 66));
    how_to_play_exit.setPosition(Vector2f(how_to_play.getPosition().x - how_to_play.getSize().x / 2.f + 512, how_to_play.getPosition().y - how_to_play.getSize().y / 2.f + 451));
    ScreenDarken dark_overlay(window);
    dark_overlay.SetOpacity(155.f);

    // follow instagram
    Texture instagram_icon_texture;
    instagram_icon_texture.loadFromFile("gfx/sprite and image/instagram-icon.png");
    RectangleShape instagram_icon;
    instagram_icon.setTexture(&instagram_icon_texture);
    instagram_icon.setSize(Vector2f(35, 35));
    instagram_icon.setOrigin(Vector2f(0, instagram_icon.getSize().y));
    instagram_icon.setPosition(Vector2f(10, window.getSize().y - 10));
    Text follow_instagram_text("Follow my Instagram!", button_font, 20);
    follow_instagram_text.setFillColor(Color::White);
    follow_instagram_text.setPosition(Vector2f(instagram_icon.getPosition().x + instagram_icon.getSize().x + 12, instagram_icon.getPosition().y - 29));
    RectangleShape follow_instagram_click_area;
    follow_instagram_click_area.setFillColor(Color(0, 0, 0, 0));
    follow_instagram_click_area.setSize(Vector2f(instagram_icon.getSize().x + 10 + follow_instagram_text.getLocalBounds().width, instagram_icon.getSize().y));
    follow_instagram_click_area.setPosition(Vector2f(instagram_icon.getPosition().x, instagram_icon.getPosition().y - instagram_icon.getSize().y));

    // opening json file to access volume of sound effects and music
    json settings_file;
    std::ifstream file("saves/user_data_and_preference.json");
    file >> settings_file;
    file.close();

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

    // music
    float music_volume = 100 * (int)settings_file["settings_preferences"]["music"] / 5;
    Music title_screen_music;
    title_screen_music.openFromFile("audio/music/Darkwood Forest - Derek Fiechter.ogg");
    title_screen_music.setLoop(true);
    title_screen_music.setVolume(0);

    bool switch_to_game = false;
    bool switch_to_settings = false;

    float total_time = 0.f, delta_time;
    Clock clock;

    title_screen_music.play();

    while (window_state == "title-screen")
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

        // black circle transition
        if (!switch_to_game)
        {
            if (x_pos + black_circle_transition.getRadius() > 0.f)
            {
                vel_x = Smooth(3000.f, vel_x, delta_time);
            }
            else
            {
                vel_x = 0.f;
                can_click_button = true;
                if (vel_x == 0.f)
                    x_pos = -black_circle_transition.getRadius();
            }
            if (x_pos <= 30.f)
            {
                if (!whoosh_sound_played)
                {
                    whoosh_sound.play();
                    whoosh_sound_played = true;
                }
            }
            x_pos -= vel_x * delta_time;
            black_circle_transition.setPosition(Vector2f(x_pos, y_pos));

            if (title_screen_music.getVolume() < music_volume)
                title_screen_music.setVolume(title_screen_music.getVolume() + 10 * delta_time);
        }

        if (Mouse::isButtonPressed(Mouse::Left) && can_click_button && app_in_focus(&window))
        {
            Vector2i mouse_pos = Mouse::getPosition(window);
            for (int i = 0; i < 3; i++)
            {
                if (button_clicked(buttons[i], mouse_pos) && !display_how_to_play)
                {
                    if (i == 0)
                    {
                        switch_to_game = true;
                        whoosh_sound_played = false;
                        if (!button_click_sound_played)
                        {
                            button_click_sound.play();
                            button_click_sound_played = true;
                        }
                        break;
                    }
                    else if (i == 1)
                    {
                        switch_to_settings = true;
                        if (!button_click_sound_played)
                        {
                            button_click_sound.play();
                            button_click_sound_played = true;
                        }
                        break;
                    }
                    else if (i == 2)
                    {
                        display_how_to_play = true;
                        if (!button_click_sound_played)
                        {
                            button_click_sound.play();
                            button_click_sound_played = true;
                        }
                        break;
                    }
                }
            }
            if (button_clicked(how_to_play_exit, mouse_pos) && display_how_to_play)
            {
                display_how_to_play = false;
                if (!button_click_sound_played)
                {
                    button_click_sound.play();
                    button_click_sound_played = true;
                }
            }
            if (button_clicked(follow_instagram_click_area, mouse_pos) && !display_how_to_play)
            {
                link_redirect("https://instagram.com/nnc21nathanael");
                if (!button_click_sound_played)
                {
                    button_click_sound.play();
                    button_click_sound_played = true;
                }
            }
        }
        else
            button_click_sound_played = false;

        // SINE WAVE for game title smooth movement
        game_title.setPosition(Vector2f(window.getSize().x / 2.f, 10 * sin(total_time * 2) + game_title_y_pos));

        // black circle transition
        if (switch_to_game)
        {
            if (x_pos < window.getSize().x / 2.f)
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
                window_state = "game";
            }
            x_pos += vel_x * delta_time;
            black_circle_transition.setPosition(Vector2f(x_pos, y_pos));

            if (title_screen_music.getVolume() > 0)
                title_screen_music.setVolume(title_screen_music.getVolume() - 50 * delta_time);
        }

        window.clear();
        background.Draw(window);
        window.draw(game_title);
        for (int i = 0; i < 3; i++)
        {
            window.draw(buttons[i]);
            window.draw(button_texts[i]);
        }
        window.draw(instagram_icon);
        window.draw(follow_instagram_text);
        window.draw(follow_instagram_click_area);
        if (display_how_to_play)
        {
            dark_overlay.Draw(window);
            window.draw(how_to_play);
            window.draw(how_to_play_exit);
        }
        window.draw(black_circle_transition);

        if (switch_to_settings)
        {
            settings(window, window_state, switch_to_settings, title_screen_music);
            json settings_file;
            std::ifstream file("saves/user_data_and_preference.json");
            file >> settings_file;
            file.close();

            whoosh_sound.setVolume(float(100 * (int)settings_file["settings_preferences"]["sound_effect"] / 5));
            button_click_sound.setVolume(float(100 * (int)settings_file["settings_preferences"]["sound_effect"] / 5));
            title_screen_music.setVolume(float(100 * (int)settings_file["settings_preferences"]["music"] / 5));
        }
        window.display();

        total_time += delta_time;
    }
}