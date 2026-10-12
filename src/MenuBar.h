#include <SFML/Graphics.hpp>
using namespace sf;

class MenuBar
{
public:
    MenuBar(RenderWindow &window)
    {
        menu_bar_texture.loadFromFile("gfx/sprite and image/menu-bar.png");
        menu_bar.setTexture(&menu_bar_texture);

        float menu_bar_width = 5280 * window.getSize().x / 5280;
        float menu_bar_height = 660 * window.getSize().x / 5280;
        menu_bar.setSize(Vector2f(menu_bar_width, menu_bar_height));
        menu_bar.setOrigin(0, 0);
        menu_bar.setPosition(Vector2f(0.f, 0.f));
    }

    RectangleShape GetRect()
    {
        return menu_bar;
    }

    Vector2f GetSize()
    {
        return menu_bar.getSize();
    }

    void Draw(RenderWindow &window)
    {
        window.draw(menu_bar);
    }

private:
    RectangleShape menu_bar;
    Texture menu_bar_texture;
};

class HealthBar
{
public:
    HealthBar(RenderWindow &window)
    {
        // outline
        outline_texture.loadFromFile("gfx/sprite and image/health-bar-outline.png");
        outline.setTexture(&outline_texture);
        float outline_width = 863 * 175 / 863;
        float outline_height = 119 * 175 / 863;
        outline.setSize(Vector2f(outline_width, outline_height));
        outline.setOrigin(outline.getSize().x, 0);
        outline.setPosition(Vector2f(window.getSize().x - 30.f, 30.f));

        // fill
        fill_texture.loadFromFile("gfx/sprite and image/health-bar-fill.png");
        fill.setTexture(&fill_texture);
        fill.setSize(outline.getSize());
        fill.setOrigin(fill.getSize().x, 0);
        fill.setPosition(outline.getPosition());

        // fill shade
        fill_shade.setFillColor(Color(0, 0, 0, 45));
        fill_shade.setSize(Vector2f(fill.getSize().x, 7.f));
        fill_shade.setOrigin(fill_shade.getSize().x, fill_shade.getSize().y);
        fill_shade.setPosition(Vector2f(fill.getPosition().x, fill.getPosition().y + fill.getSize().y));

        // health indicator
        health_indicator.setFillColor(Color(255, 0, 0));
        health_indicator_width = fill.getSize().x * health / 100.f;
        health_indicator_height = fill.getSize().y;
        health_indicator.setSize(Vector2f(health_indicator_width, health_indicator_height));
        health_indicator.setOrigin(0, 0);
        health_indicator.setPosition(Vector2f(fill.getPosition().x - fill.getSize().x, fill.getPosition().y));

        // health indicator shade
        health_indicator_shade.setFillColor(Color(0, 0, 0, 45));
        health_indicator_shade.setSize(Vector2f(health_indicator.getSize().x, 7.f));
        health_indicator_shade.setOrigin(0, health_indicator_shade.getSize().y);
        health_indicator_shade.setPosition(Vector2f(health_indicator.getPosition().x, health_indicator.getPosition().y + health_indicator.getSize().y));

        heart_texture.loadFromFile("gfx/sprite and image/heart-symbol.png");
        float heart_width = 330 * 45 / 330;
        float heart_height = 300 * 45 / 330;
        heart_symbol.setTexture(&heart_texture);
        heart_symbol.setSize(Vector2f(heart_width, heart_height));
        heart_symbol.setOrigin(heart_symbol.getSize().x, 0);
        heart_symbol.setPosition(Vector2f(health_indicator.getPosition().x + 16.f, health_indicator.getPosition().y - 7.f));
    }

    void Animate(float delta_time)
    {
        if (health <= 0.f)
        {
            health = 0.f;
        }
        if (health >= 100.f)
        {
            health = 100.f;
        }
        if (animate)
        {
            float growth_vel = 200.f;
            if (grow)
            {
                health_indicator_width += growth_vel * delta_time;
                if (health_indicator.getSize().x >= fill.getSize().x * health / 100.f)
                {
                    health_indicator_width = fill.getSize().x * health / 100.f;
                    animate = false;
                    grow = nan;
                }
            }
            else if (!grow)
            {
                health_indicator_width -= growth_vel * delta_time;
                if (health_indicator.getSize().x <= fill.getSize().x * health / 100.f)
                {
                    health_indicator_width = fill.getSize().x * health / 100.f;
                    animate = false;
                    grow = nan;
                }
            }
        }
    }

    void Update()
    {
        health_indicator.setSize(Vector2f(health_indicator_width, health_indicator_height));
        health_indicator_shade.setSize(Vector2f(health_indicator_width, 7.f));
    }

    void Draw(RenderWindow &window)
    {
        window.draw(fill);
        window.draw(fill_shade);
        window.draw(health_indicator);
        window.draw(health_indicator_shade);
        window.draw(outline);
        window.draw(heart_symbol);
    }

public:
    bool animate = false;
    bool grow;
    float health = 100.f;

private:
    RectangleShape outline;
    Texture outline_texture;
    RectangleShape fill;
    Texture fill_texture;
    RectangleShape fill_shade;
    RectangleShape health_indicator;
    float health_indicator_width;
    float health_indicator_height;
    RectangleShape health_indicator_shade;
    RectangleShape heart_symbol;
    Texture heart_texture;
};

class PauseButton
{
public:
    PauseButton()
    {
        button_texture.loadFromFile("gfx/sprite and image/pause-button.png");

        button.setTexture(&button_texture);
        button.setSize(Vector2f(25, 117 * 25 / 84));
        button.setPosition(Vector2f(35.f, 25.f));
    }

    bool CheckButtonClicked(Vector2i mouse_pos)
    {
        return button_clicked(button, mouse_pos);
    }

    void Draw(RenderWindow &window)
    {
        window.draw(button);
    }

private:
    RectangleShape button;
    Texture button_texture;
};