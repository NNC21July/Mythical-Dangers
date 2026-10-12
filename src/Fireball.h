#include <SFML/Graphics.hpp>
#include <vector>
using string = std::string;
using namespace sf;

class Spritesheet;

class Fireball
{
public:
    Fireball(Texture *spritesheet, string direction, RenderWindow &window)
    {
        fireball_rect = fireball.GetSprite();
        fireball_rect.setTexture(spritesheet);
        fireball_rect.setSize(Vector2f(fireball.GetUvRect().width / 13, fireball.GetUvRect().height / 13));
        fireball_rect.setOrigin(0.f, 0.f);
        if (direction == "right")
        {
            faces_right = true;
            x_pos = 0.f - fireball_rect.getSize().x;
            y_pos = 75.f;
        }
        else if (direction == "left")
        {
            faces_right = false;
            x_pos = window.getSize().x + fireball_rect.getSize().x;
            y_pos = 75.f;
        }
        fireball_rect.setPosition(Vector2f(x_pos, y_pos));
    }

    RectangleShape GetFireballRect()
    {
        return fireball_rect;
    }

    Vector2f GetSize()
    {
        return fireball_rect.getSize();
    }

    Vector2f GetPosition()
    {
        return fireball_rect.getPosition();
    }

    void SetPosition(float x_pos, float y_pos)
    {
        fireball_rect.setPosition(Vector2f(x_pos, y_pos));
    }

    float RandomizeHeight(std::vector<Platform> platforms, std::vector<RectangleShape> ground)
    {
        float height;
        int height_chance = rand() % 4;
        if (height_chance == 0)
        {
            height = platforms[0].GetPosition().y - 38.f;
        }
        else if (height_chance == 1)
        {
            height = platforms[1].GetPosition().y - 38.f;
        }
        else if (height_chance == 2)
        {
            height = platforms[3].GetPosition().y - 38.f;
        }
        else if (height_chance == 3)
        {
            height = ground[0].getPosition().y - 38.f;
        }

        return height;
    }

    void Move(float delta_time)
    {
        x_pos = fireball_rect.getPosition().x;
        y_pos = fireball_rect.getPosition().y;

        if (faces_right)
        {
            x_pos += vel_x * delta_time;
        }
        else
        {
            x_pos -= vel_x * delta_time;
        }
    }

    void Update(Texture *spritesheet, float delta_time)
    {
        fireball.Update(string("gfx/metadata/fireball.json"), 4, 0.135f);
        fireball.Animate(delta_time);

        fireball_rect = fireball.GetSprite();
        fireball_rect.setTexture(spritesheet);
        fireball_rect.setSize(Vector2f(fireball.GetUvRect().width / 13, fireball.GetUvRect().height / 13));
        fireball_rect.setOrigin(fireball_rect.getSize().x / 2.f, fireball_rect.getSize().y / 2.f);
        fireball_rect.setPosition(Vector2f(x_pos, y_pos));

        if (faces_right)
        {
            fireball_rect.setScale(Vector2f(-1, 1));
        }
        else
        {
            fireball_rect.setScale(Vector2f(1, 1));
        }
    }

    void Draw(RenderWindow &window)
    {
        window.draw(fireball_rect);
    }

public:
    bool can_deploy = false;
    float vel_x = 350.f, vel_y = 0.f;

private:
    Spritesheet fireball = Spritesheet(string("gfx/metadata/fireball.json"), 0, 4, 0.135f);
    RectangleShape fireball_rect;

    bool faces_right;

    float x_pos, y_pos;
};