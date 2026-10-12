#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>
using namespace sf;

void fade_out(RectangleShape &object, float velocity, float &alpha, float delta_time, bool &fade_now, bool &faded, Color color, float alpha_range);
void fade_in(RectangleShape &object, float velocity, float &alpha, float delta_time, bool &fade_now, bool &faded, Color color, float alpha_range);

class ScreenShake
{
public:
    ScreenShake(float duration, float power, View &view)
    {
        original_view_pos = view.getCenter();
        shake_time_remaining = duration;
        shake_time_remaining_reset = duration;
        shake_power = power;
        shake_power_reset = power;
        shake_fade_time = power / duration;
    }

    void Reset()
    {
        shake_time_remaining = shake_time_remaining_reset;
        shake_power = shake_power_reset;
    }

    void Update(RenderWindow &window, float delta_time, View &view)
    {
        if (shake_time_remaining > 0)
        {
            shake_time_remaining -= delta_time;

            float x_amount = rand() % 3 - 1 * shake_power;
            float y_amount = rand() % 3 - 1 * shake_power;

            view.move(x_amount, y_amount);

            if (shake_power > 0)
            {
                shake_power -= shake_fade_time * delta_time;
            }
        }
        else
        {
            shaking = false;
            Reset();
        }
    }

public:
    bool shaking = false;
    Vector2f original_view_pos;

private:
    float shake_time_remaining, shake_power, shake_fade_time;
    float shake_time_remaining_reset, shake_power_reset;
};

class ScreenRedden
{
public:
    ScreenRedden(RenderWindow &window, RectangleShape menu_bar)
    {
        red_effect_texture.loadFromFile("gfx/sprite and image/hurt-screen-effect.png");

        red_effect.setTexture(&red_effect_texture);
        red_effect.setPosition(Vector2f(0, menu_bar.getPosition().y + menu_bar.getPosition().y));
        red_effect.setSize(Vector2f(window.getSize().x, window.getSize().y));
    }

    void FadeOut(float delta_time)
    {
        fade_out(red_effect, 100.f, alpha, delta_time, fade_now, faded, Color(255, 255, 255, alpha), 0.f);
    }

    void Draw(RenderWindow &window)
    {
        window.draw(red_effect);
    }

public:
    float alpha = 255.f;
    bool fade_now = false;
    bool faded = false;

private:
    RectangleShape red_effect;
    Texture red_effect_texture;
};

class ScreenDarken
{
public:
    ScreenDarken(RenderWindow &window)
    {
        dark_effect.setFillColor(Color(0, 0, 0, alpha));
        dark_effect.setPosition(Vector2f(0, 0));
        dark_effect.setSize(Vector2f(window.getSize().x, window.getSize().y));
    }

    void FadeIn(float delta_time)
    {
        fade_in(dark_effect, 300.f, alpha, delta_time, fade_now, faded, Color(0, 0, 0, alpha), 155.f);
    }

    void SetOpacity(float alpha)
    {
        dark_effect.setFillColor(Color(0, 0, 0, alpha));
    }

    void Draw(RenderWindow &window)
    {
        window.draw(dark_effect);
    }

public:
    bool fade_now = false;
    bool faded = false;
    float alpha = 0.f;

private:
    RectangleShape dark_effect;
};