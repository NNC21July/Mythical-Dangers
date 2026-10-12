#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>
#include <vector>
#include "Spritesheet.h"
using string = std::string;
using namespace sf;

bool button_clicked(RectangleShape button, Vector2i mouse_pos);

void settings(RenderWindow &window, string &window_state, bool &switch_to_settings, Music &music)
{
    json settings_file;
    std::ifstream file("saves/user_data_and_preference.json");
    file >> settings_file;
    file.close();

    bool back_to_title = false;

    bool reset_high_score = false, reset_default_settings = false;

    // sound effects
    // === button click2 ===
    SoundBuffer button_click2_buffer;
    button_click2_buffer.loadFromFile("audio/sfx/button-click2.ogg");
    Sound button_click_sound;
    button_click_sound.setBuffer(button_click2_buffer);
    bool button_click_sound_played = false;
    // === notif ===
    SoundBuffer notif_buffer;
    notif_buffer.loadFromFile("audio/sfx/settings-notif.ogg");
    Sound notif_sound;
    notif_sound.setBuffer(notif_buffer);
    bool notif_sound_played = false;

    Texture settings_top_screen_texture;
    settings_top_screen_texture.loadFromFile("gfx/sprite and image/settings-top-screen.png");
    RectangleShape settings_top_screen;
    settings_top_screen.setTexture(&settings_top_screen_texture);
    settings_top_screen.setSize(Vector2f(window.getSize().x, 460 * window.getSize().x / 2400));
    settings_top_screen.setPosition(Vector2f(0, 0));
    RectangleShape settings_bottom_screen = settings_top_screen;
    settings_bottom_screen.setPosition(Vector2f(window.getSize().x, window.getSize().y));
    settings_bottom_screen.setScale(Vector2f(-1, -1));

    Font font;
    font.loadFromFile("gfx/font/VPPixel.otf");

    // title
    Text settings_title("SETTINGS", font, 50);
    settings_title.setFillColor(Color::Black);
    settings_title.setStyle(Text::Bold);
    settings_title.setPosition(round(window.getSize().x / 2.f - settings_title.getLocalBounds().width / 2.f), 55);

    // music and sound effect settings
    Text music_text("MUSIC", font, 25);
    music_text.setFillColor(Color::Black);
    music_text.setPosition(75, settings_title.getPosition().y + settings_title.getLocalBounds().height + 85);

    Text sound_effect_text("SOUND EFFECT", font, 25);
    sound_effect_text.setFillColor(Color::Black);
    sound_effect_text.setPosition(75, music_text.getPosition().y + music_text.getLocalBounds().height + 80);

    std::vector<Spritesheet> plus_button_spritesheets;
    std::vector<Spritesheet> minus_button_spritesheets;
    for (int i = 0; i < 2; i++)
    {
        plus_button_spritesheets.push_back(Spritesheet("gfx/metadata/volume-plus-minus-button.json", 0, 4, 0.075f));
        minus_button_spritesheets.push_back(Spritesheet("gfx/metadata/volume-plus-minus-button.json", 0, 4, 0.075f));
    }
    Texture plus_button_texture;
    plus_button_texture.loadFromFile("gfx/sprite and image/plus-button.png");
    Texture minus_button_texture;
    minus_button_texture.loadFromFile("gfx/sprite and image/minus-button.png");
    RectangleShape plus_button, minus_button;
    std::vector<RectangleShape> plus_buttons;
    std::vector<bool> plus_buttons_animate{false, false};
    std::vector<float> plus_buttons_timer{0.f, 0.f};
    std::vector<RectangleShape> minus_buttons;
    std::vector<bool> minus_buttons_animate{false, false};
    std::vector<float> minus_buttons_timer{0.f, 0.f};
    for (int i = 0; i < 2; i++)
    {
        minus_buttons.push_back(minus_button);
        minus_buttons[i].setTexture(&minus_button_texture);
        minus_buttons[i].setSize(Vector2f(52.5f, 49.5f));
        minus_buttons[i].setOrigin(Vector2f(0, 0));
        minus_buttons[i].setPosition(Vector2f(sound_effect_text.getPosition().x + sound_effect_text.getLocalBounds().width + 25, 0));
    }
    minus_buttons[0].move(Vector2f(0, music_text.getPosition().y + music_text.getLocalBounds().height / 2.f - minus_buttons[0].getSize().y / 2.f));
    minus_buttons[1].move(Vector2f(0, sound_effect_text.getPosition().y + sound_effect_text.getLocalBounds().height / 2.f - minus_buttons[0].getSize().y / 2.f));

    Texture volume_bar_placeholder_texture;
    volume_bar_placeholder_texture.loadFromFile("gfx/sprite and image/volume-bar-placeholder.png");
    std::vector<RectangleShape> volume_bar_placeholders;
    for (int i = 0; i < 2; i++)
    {
        RectangleShape volume_bar_placeholder;
        volume_bar_placeholder.setTexture(&volume_bar_placeholder_texture);
        volume_bar_placeholder.setSize(Vector2f(282.425f, 82.f));
        volume_bar_placeholder.setOrigin(Vector2f(0, volume_bar_placeholder.getSize().y / 2.f));
        volume_bar_placeholder.setPosition(Vector2f(minus_buttons[i].getPosition().x + minus_buttons[i].getSize().x + 20, minus_buttons[i].getPosition().y + minus_buttons[i].getSize().y / 2.f));
        volume_bar_placeholders.push_back(volume_bar_placeholder);
    }

    for (int i = 0; i < 2; i++)
    {
        plus_buttons.push_back(plus_button);
        plus_buttons[i].setTexture(&plus_button_texture);
        plus_buttons[i].setSize(Vector2f(52.5f, 49.5f));
        plus_buttons[i].setOrigin(Vector2f(0, 0));
        plus_buttons[i].setPosition(Vector2f(volume_bar_placeholders[i].getPosition().x + volume_bar_placeholders[i].getSize().x + 25, volume_bar_placeholders[i].getPosition().y - plus_buttons[i].getSize().y / 2.f));
    }

    Spritesheet volume_bar_spritesheet("gfx/metadata/volume-bar.json", 0, 2, 0);
    Texture volume_bar_texture;
    volume_bar_texture.loadFromFile("gfx/sprite and image/volume-bar.png");
    RectangleShape volume_bar_filled = volume_bar_spritesheet.GetSprite();
    volume_bar_filled.setTexture(&volume_bar_texture);
    volume_bar_filled.setSize(Vector2f(40.43f, 62.f));
    volume_bar_filled.setOrigin(Vector2f(0, volume_bar_filled.getSize().y / 2.f));
    volume_bar_spritesheet.sprite_count = 1;
    volume_bar_spritesheet.Update("gfx/metadata/volume-bar.json", 4, 0);
    RectangleShape volume_bar_empty = volume_bar_spritesheet.GetSprite();
    volume_bar_empty.setTexture(&volume_bar_texture);
    volume_bar_empty.setSize(Vector2f(40.43f, 62.f));
    volume_bar_empty.setOrigin(Vector2f(0, volume_bar_empty.getSize().y / 2.f));
    std::vector<std::vector<RectangleShape>> volume_bars;
    std::vector<int> volume_bars_filled_count;
    int music_volume = settings_file["settings_preferences"]["music"];
    int sound_effect_volume = settings_file["settings_preferences"]["sound_effect"];
    for (int i = 0; i < 2; i++)
    {
        volume_bars.push_back(std::vector<RectangleShape>());
        if (i == 0)
            volume_bars_filled_count.push_back(music_volume);
        else if (i == 1)
            volume_bars_filled_count.push_back(sound_effect_volume);
        for (int j = 0, x_start = volume_bar_placeholders[i].getPosition().x + 20; j < 5; j++)
        {
            RectangleShape volume_bar;
            if (j < volume_bars_filled_count[i])
                volume_bar = volume_bar_filled;
            else
                volume_bar = volume_bar_empty;
            volume_bar.setPosition(Vector2f(x_start, volume_bar_placeholders[i].getPosition().y));
            volume_bars[i].push_back(volume_bar);
            x_start += volume_bar.getSize().x + 10;
        }
    }

    // keyboard movement settings
    Text keyboard_preference_text("KEYBOARD MOVEMENT PREFERENCES", font, 25);
    keyboard_preference_text.setFillColor(Color::Black);
    keyboard_preference_text.setPosition(Vector2f(75, sound_effect_text.getPosition().y + sound_effect_text.getLocalBounds().height + 80));

    Texture wasd_key_texture;
    wasd_key_texture.loadFromFile("gfx/sprite and image/wasd-keys.png");
    RectangleShape wasd_key;
    wasd_key.setTexture(&wasd_key_texture);
    wasd_key.setSize(Vector2f(138.f, 87.f));
    wasd_key.setPosition(Vector2f(keyboard_preference_text.getPosition().x, keyboard_preference_text.getPosition().y + keyboard_preference_text.getLocalBounds().height + 30));

    Texture arrow_key_texture;
    arrow_key_texture.loadFromFile("gfx/sprite and image/arrow-keys.png");
    RectangleShape arrow_key;
    arrow_key.setTexture(&arrow_key_texture);
    arrow_key.setSize(wasd_key.getSize());
    arrow_key.setPosition(Vector2f(wasd_key.getPosition().x + wasd_key.getSize().x + 35, wasd_key.getPosition().y));

    Spritesheet radio_button_spritesheet("gfx/metadata/radio-button.json", 0, 2, 0);
    Texture radio_button_texture;
    radio_button_texture.loadFromFile("gfx/sprite and image/radio-button.png");
    RectangleShape radio_button_false = radio_button_spritesheet.GetSprite();
    radio_button_false.setTexture(&radio_button_texture);
    radio_button_false.setSize(Vector2f(35.f, 35.f));
    radio_button_spritesheet.sprite_count = 1;
    radio_button_spritesheet.Update("gfx/metadata/radio-button.json", 2, 0);
    RectangleShape radio_button_true = radio_button_spritesheet.GetSprite();
    radio_button_true.setTexture(&radio_button_texture);
    radio_button_true.setSize(radio_button_false.getSize());
    std::vector<RectangleShape> radio_buttons;
    string keyboard_preference = settings_file["settings_preferences"]["keyboard_preference"];
    if (keyboard_preference == "wasd")
    {
        radio_buttons.push_back(radio_button_true);
        radio_buttons.push_back(radio_button_false);
    }
    else
    {
        radio_buttons.push_back(radio_button_false);
        radio_buttons.push_back(radio_button_true);
    }
    for (RectangleShape &radio_button : radio_buttons)
        radio_button.setPosition(Vector2f(0, wasd_key.getPosition().y + wasd_key.getSize().y + 18));
    radio_buttons[0].move(Vector2f(wasd_key.getPosition().x + wasd_key.getSize().x / 2.f - radio_buttons[0].getSize().x / 2.f, 0));
    radio_buttons[1].move(Vector2f(arrow_key.getPosition().x + arrow_key.getSize().x / 2.f - radio_buttons[0].getSize().x / 2.f, 0));

    // reset saves json file settings
    Texture reset_button_texture;
    reset_button_texture.loadFromFile("gfx/sprite and image/settings-button.png");
    std::vector<RectangleShape> reset_buttons;
    std::vector<Text> reset_button_texts;
    for (int i = 0; i < 2; i++)
    {
        RectangleShape reset_button;
        reset_button.setTexture(&reset_button_texture);
        reset_button.setSize(Vector2f(219, 65));
        reset_button.setPosition(Vector2f(keyboard_preference_text.getPosition().x + keyboard_preference_text.getLocalBounds().width + 43, 0));
        reset_buttons.push_back(reset_button);

        Text reset_button_text;
        if (i == 0)
            reset_button_text.setString("RESET HIGH SCORE");
        else
            reset_button_text.setString("RESET TO DEFAULT");
        reset_button_text.setFont(font);
        reset_button_text.setCharacterSize(18);
        reset_button_text.setFillColor(Color::Black);
        reset_button_text.setPosition(Vector2f(round(reset_button.getPosition().x + reset_button.getSize().x / 2.f - reset_button_text.getLocalBounds().width / 2.f), 0));
        reset_button_texts.push_back(reset_button_text);
    }
    reset_buttons[0].move(0, keyboard_preference_text.getPosition().y + keyboard_preference_text.getLocalBounds().height);
    reset_buttons[1].move(0, reset_buttons[0].getPosition().y + reset_buttons[0].getSize().y + 20);
    for (int i = 0; i < 2; i++)
        reset_button_texts[i].move(0, round(reset_buttons[i].getPosition().y + reset_buttons[i].getSize().y / 2.f - reset_button_texts[i].getLocalBounds().height / 2.f - 5));
    reset_button_texts[1].move(0, -10);
    Text reset_default_settings_wrap_text;
    reset_default_settings_wrap_text = reset_button_texts[1];
    reset_default_settings_wrap_text.setString("SETTINGS");
    reset_default_settings_wrap_text.setPosition(Vector2f(reset_buttons[1].getPosition().x + reset_buttons[1].getSize().x / 2.f - reset_default_settings_wrap_text.getLocalBounds().width / 2.f, reset_button_texts[1].getPosition().y + reset_button_texts[1].getLocalBounds().height + 7));

    Text high_score_reset_notif("HIGH SCORE RESET", font, 20);
    high_score_reset_notif.setPosition(Vector2f(10, window.getSize().y + high_score_reset_notif.getCharacterSize() - 10));
    Text default_settings_reset_notif = high_score_reset_notif;
    default_settings_reset_notif.setString("SETTINGS RESET TO DEFAULT");
    Texture translucent_black_overlay_texture;
    translucent_black_overlay_texture.loadFromFile("gfx/sprite and image/translucent-black-overlay.png");
    RectangleShape translucent_black_overlay;
    translucent_black_overlay.setTexture(&translucent_black_overlay_texture);
    translucent_black_overlay.setSize(Vector2f(300, 50));
    translucent_black_overlay.setPosition(Vector2f(0, window.getSize().y));
    float notification_delay_timer = 0.f;
    bool move_notif_down = false;
    float mouse_click_delay = 0.f;

    // back button
    Texture back_button_texture;
    back_button_texture.loadFromFile("gfx/sprite and image/back-button.png");
    RectangleShape back_button;
    back_button.setTexture(&back_button_texture);
    back_button.setSize(Vector2f(36, 48));
    back_button.setPosition(Vector2f(85, 65));

    float delta_time;
    Clock clock;

    while (!back_to_title)
    {
        Event window_event;
        while (window.pollEvent(window_event))
        {
            if (window_event.type == Event::Closed)
            {
                settings_file["settings_preferences"]["music"] = volume_bars_filled_count[0];
                settings_file["settings_preferences"]["sound_effect"] = volume_bars_filled_count[1];
                settings_file["settings_preferences"]["keyboard_preference"] = keyboard_preference;
                std::ofstream modified_file("saves/user_data_and_preference.json");
                modified_file << settings_file;
                modified_file.close();

                window.close();
                window_state = "quit";
                return;
            }
        }
        delta_time = clock.restart().asSeconds();
        if (delta_time > 1.0f / 20.0f)
        {
            delta_time = 1.0f / 20.0f;
        }

        if (Mouse::isButtonPressed(Mouse::Left))
        {
            Vector2i mouse_pos = Mouse::getPosition(window);
            for (int i = 0; i < 2; i++)
            {
                if (button_clicked(plus_buttons[i], mouse_pos))
                {
                    if (volume_bars_filled_count[i] < 5 && !plus_buttons_animate[i])
                        volume_bars_filled_count[i] += 1;
                    plus_buttons_animate[i] = true;
                    if (!button_click_sound_played)
                    {
                        button_click_sound.play();
                        button_click_sound_played = true;
                    }
                }
                if (button_clicked(minus_buttons[i], mouse_pos))
                {
                    if (volume_bars_filled_count[i] > 0 && !minus_buttons_animate[i])
                        volume_bars_filled_count[i] -= 1;
                    minus_buttons_animate[i] = true;
                    if (!button_click_sound_played)
                    {
                        button_click_sound.play();
                        button_click_sound_played = true;
                    }
                }
                if (button_clicked(radio_buttons[i], mouse_pos))
                {
                    if (keyboard_preference == "arrow" && i == 0)
                    {
                        keyboard_preference = "wasd";
                        if (!button_click_sound_played)
                        {
                            button_click_sound.play();
                            button_click_sound_played = true;
                        }
                    }
                    if (keyboard_preference == "wasd" && i == 1)
                    {
                        keyboard_preference = "arrow";
                        if (!button_click_sound_played)
                        {
                            button_click_sound.play();
                            button_click_sound_played = true;
                        }
                    }
                }
                if (button_clicked(reset_buttons[i], mouse_pos))
                {
                    if (mouse_click_delay == 0.f)
                    {
                        high_score_reset_notif.setPosition(Vector2f(10, window.getSize().y + high_score_reset_notif.getCharacterSize() - 10));
                        default_settings_reset_notif.setPosition(high_score_reset_notif.getPosition());
                        translucent_black_overlay.setPosition(Vector2f(0, window.getSize().y));
                        if (i == 0)
                        {
                            reset_high_score = true;
                            reset_default_settings = false;
                            if (!button_click_sound_played)
                            {
                                button_click_sound.play();
                                button_click_sound_played = true;
                            }
                        }
                        else
                        {
                            reset_default_settings = true;
                            reset_high_score = false;
                            if (!button_click_sound_played)
                            {
                                button_click_sound.play();
                                button_click_sound_played = true;
                            }
                        }
                        mouse_click_delay += delta_time;
                        if (mouse_click_delay >= 0.2f)
                            mouse_click_delay = 0.f;
                    }
                }
                if (button_clicked(back_button, mouse_pos))
                {
                    back_to_title = true;
                    switch_to_settings = false;
                    settings_file["settings_preferences"]["music"] = volume_bars_filled_count[0];
                    settings_file["settings_preferences"]["sound_effect"] = volume_bars_filled_count[1];
                    settings_file["settings_preferences"]["keyboard_preference"] = keyboard_preference;
                    std::ofstream modified_file("saves/user_data_and_preference.json");
                    modified_file << settings_file;
                    modified_file.close();
                    if (!button_click_sound_played)
                    {
                        button_click_sound.play();
                        button_click_sound_played = true;
                    }
                }
                float music_volume = 100 * volume_bars_filled_count[0] / 5;
                float sound_effect_volume = 100 * volume_bars_filled_count[1] / 5;
                music.setVolume(music_volume);
                button_click_sound.setVolume(sound_effect_volume);
                notif_sound.setVolume(sound_effect_volume);
            }
        }
        else
        {
            mouse_click_delay = 0.f;
            button_click_sound_played = false;
        }

        if (reset_high_score)
        {
            settings_file["high_score"] = 0;
            if (translucent_black_overlay.getPosition().y > window.getSize().y - translucent_black_overlay.getSize().y && !move_notif_down)
            {
                high_score_reset_notif.move(0, -200 * delta_time);
                translucent_black_overlay.move(0, -200 * delta_time);
                if (!notif_sound_played)
                {
                    notif_sound.play();
                    notif_sound_played = true;
                }
            }
            else
            {
                notification_delay_timer += delta_time;
                if (notification_delay_timer >= 3.5f)
                {
                    move_notif_down = true;
                    notification_delay_timer = 0;
                }
                notif_sound_played = false;
            }
            if (move_notif_down)
            {
                if (translucent_black_overlay.getPosition().y < window.getSize().y + translucent_black_overlay.getSize().y)
                {
                    high_score_reset_notif.move(0, 200 * delta_time);
                    translucent_black_overlay.move(0, 200 * delta_time);
                }
                else
                {
                    reset_high_score = false;
                    move_notif_down = false;
                }
            }
        }
        if (reset_default_settings)
        {
            volume_bars_filled_count[0] = 3;
            volume_bars_filled_count[1] = 3;
            keyboard_preference = "wasd";
            if (translucent_black_overlay.getPosition().y > window.getSize().y - translucent_black_overlay.getSize().y && !move_notif_down)
            {
                default_settings_reset_notif.move(0, -200 * delta_time);
                translucent_black_overlay.move(0, -200 * delta_time);
                if (!notif_sound_played)
                {
                    notif_sound.play();
                    notif_sound_played = true;
                }
            }
            else
            {
                notification_delay_timer += delta_time;
                if (notification_delay_timer >= 3.5f)
                {
                    move_notif_down = true;
                    notification_delay_timer = 0;
                }
                notif_sound_played = false;
            }
            if (move_notif_down)
            {
                if (translucent_black_overlay.getPosition().y < window.getSize().y + translucent_black_overlay.getSize().y)
                {
                    default_settings_reset_notif.move(0, 200 * delta_time);
                    translucent_black_overlay.move(0, 200 * delta_time);
                }
                else
                {
                    reset_default_settings = false;
                    move_notif_down = false;
                }
            }
        }

        window.clear(Color(125, 96, 45));
        window.draw(settings_top_screen);
        window.draw(settings_bottom_screen);
        window.draw(settings_title);
        window.draw(music_text);
        window.draw(sound_effect_text);

        for (int i = 0; i < 2; i++)
        {
            minus_button_spritesheets[i].Update("gfx/metadata/volume-plus-minus-button.json", 4, 0.075f);
            if (minus_buttons_animate[i])
            {
                minus_button_spritesheets[i].Animate(delta_time);
                minus_buttons_timer[i] += delta_time;
            }
            minus_buttons[i] = minus_button_spritesheets[i].GetSprite();
            minus_buttons[i].setTexture(&minus_button_texture);
            minus_buttons[i].setSize(Vector2f(52.5f, 49.5f));
            minus_buttons[i].setOrigin(Vector2f(0, 0));
            minus_buttons[i].setPosition(Vector2f(sound_effect_text.getPosition().x + sound_effect_text.getLocalBounds().width + 25, 0));
            if (minus_buttons_timer[i] >= 0.075f * 4)
            {
                minus_button_spritesheets[i].sprite_count = 0;
                minus_buttons_animate[i] = false;
                minus_buttons_timer[i] = 0;
            }
        }
        minus_buttons[0].move(Vector2f(0, music_text.getPosition().y + music_text.getLocalBounds().height / 2.f - minus_buttons[0].getSize().y / 2.f));
        minus_buttons[1].move(Vector2f(0, sound_effect_text.getPosition().y + sound_effect_text.getLocalBounds().height / 2.f - minus_buttons[0].getSize().y / 2.f));

        for (int i = 0; i < 2; i++)
        {
            plus_button_spritesheets[i].Update("gfx/metadata/volume-plus-minus-button.json", 4, 0.075f);
            if (plus_buttons_animate[i])
            {
                plus_button_spritesheets[i].Animate(delta_time);
                plus_buttons_timer[i] += delta_time;
            }
            plus_buttons[i] = plus_button_spritesheets[i].GetSprite();
            plus_buttons[i].setTexture(&plus_button_texture);
            plus_buttons[i].setSize(Vector2f(52.5f, 49.5f));
            plus_buttons[i].setOrigin(Vector2f(0, 0));
            plus_buttons[i].setPosition(Vector2f(volume_bar_placeholders[i].getPosition().x + volume_bar_placeholders[i].getSize().x + 25, volume_bar_placeholders[i].getPosition().y - plus_buttons[i].getSize().y / 2.f));
            if (plus_buttons_timer[i] >= 0.075f * 4)
            {
                plus_button_spritesheets[i].sprite_count = 0;
                plus_buttons_animate[i] = false;
                plus_buttons_timer[i] = 0;
            }
        }

        for (int i = 0; i < 2; i++)
        {
            volume_bars[i].clear();
            for (int j = 0, x_start = volume_bar_placeholders[i].getPosition().x + 20; j < 5; j++)
            {
                RectangleShape volume_bar;
                if (j < volume_bars_filled_count[i])
                    volume_bar = volume_bar_filled;
                else
                    volume_bar = volume_bar_empty;
                volume_bar.setPosition(Vector2f(x_start, volume_bar_placeholders[i].getPosition().y));
                volume_bars[i].push_back(volume_bar);
                x_start += volume_bar.getSize().x + 10;
            }
        }

        radio_buttons.clear();
        if (keyboard_preference == "wasd")
        {
            radio_buttons.push_back(radio_button_true);
            radio_buttons.push_back(radio_button_false);
        }
        else
        {
            radio_buttons.push_back(radio_button_false);
            radio_buttons.push_back(radio_button_true);
        }
        for (RectangleShape &radio_button : radio_buttons)
            radio_button.setPosition(Vector2f(0, wasd_key.getPosition().y + wasd_key.getSize().y + 18));
        radio_buttons[0].move(Vector2f(wasd_key.getPosition().x + wasd_key.getSize().x / 2.f - radio_buttons[0].getSize().x / 2.f, 0));
        radio_buttons[1].move(Vector2f(arrow_key.getPosition().x + arrow_key.getSize().x / 2.f - radio_buttons[0].getSize().x / 2.f, 0));

        for (int i = 0; i < 2; i++)
        {
            window.draw(minus_buttons[i]);
            window.draw(volume_bar_placeholders[i]);
            window.draw(plus_buttons[i]);
            for (RectangleShape volume_bar : volume_bars[i])
                window.draw(volume_bar);
        }

        window.draw(keyboard_preference_text);
        window.draw(wasd_key);
        window.draw(arrow_key);
        for (RectangleShape radio_button : radio_buttons)
            window.draw(radio_button);

        for (int i = 0; i < 2; i++)
        {
            window.draw(reset_buttons[i]);
            window.draw(reset_button_texts[i]);
        }
        window.draw(reset_default_settings_wrap_text);
        window.draw(translucent_black_overlay);
        window.draw(high_score_reset_notif);
        window.draw(default_settings_reset_notif);

        window.draw(back_button);

        window.display();
    }
}
