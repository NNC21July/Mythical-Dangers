#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>
using string = std::string;
using namespace sf;

string int_to_str(int value)
{
    return std::to_string(value);
}

bool app_in_focus(sf::RenderWindow *app)
{
    if (app == NULL)
        return false;

    HWND handle = app->getSystemHandle();
    bool one = handle == GetFocus();
    bool two = handle == GetForegroundWindow();

    if (one != two) // strange 'half-focus': window is in front but you can't click anything - so we fix it
    {
        SetFocus(handle);
        SetForegroundWindow(handle);
    }

    return one && two;
}

void fade_in(RectangleShape &object, float velocity, float &alpha, float delta_time, bool &fade_now, bool &faded, Color color, float alpha_range)
{
    object.setFillColor(color);
    float fade_velocity = velocity;
    if (alpha < alpha_range)
    {
        alpha += fade_velocity * delta_time;
    }
    else
    {
        fade_now = false;
        faded = true;
    }
}

void fade_out(RectangleShape &object, float velocity, float &alpha, float delta_time, bool &fade_now, bool &faded, Color color, float alpha_range)
{
    object.setFillColor(color);
    float fade_velocity = velocity;
    if (alpha > alpha_range)
    {
        alpha -= fade_velocity * delta_time;
    }
    else
    {
        fade_now = false;
        faded = true;
        alpha = 255.f;
    }
}

bool button_clicked(RectangleShape button, Vector2i mouse_pos)
{
    Vector2f button_pos = button.getPosition();
    Vector2f button_size = button.getSize();

    if (button_pos.x < mouse_pos.x && mouse_pos.x < button_pos.x + button_size.x)
    {
        if (button_pos.y < mouse_pos.y && mouse_pos.y < button_pos.y + button_size.y)
        {
            return true;
        }
    }
    return false;
}

float Smooth(float goal, float current, float dt)
{
    dt *= 1000;
    float difference = goal - current;

    if (difference > dt)
    {
        return current + dt;
    }
    if (difference < -dt)
    {
        return current - dt;
    }
    return goal;
}

void link_redirect(const char *url)
{
    ShellExecute(0, 0, url, 0, 0, SW_SHOW);
}
