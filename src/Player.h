#include <SFML/Graphics.hpp>
#include <math.h>
using string = std::string;
using namespace sf;

float Smooth(float goal, float current, float dt);

class Player
{
public:
    Player(Texture *spritesheet, string state, int total_sprite, float switch_time, RenderWindow &window)
    {
        warrior = Spritesheet(string("gfx/metadata/warrior-" + state + ".json"), 0, total_sprite, switch_time);

        warrior_rect = warrior.GetSprite();
        warrior_rect.setTexture(spritesheet);
        warrior_rect.setSize(Vector2f(warrior.GetUvRect().width / 11.5, warrior.GetUvRect().height / 11.5));
        warrior_rect.setOrigin(warrior_rect.getSize().x / 2, warrior_rect.getSize().y / 2);
        x_pos = window.getSize().x / 2;
        y_pos = 125.f;
        warrior_rect.setPosition(Vector2f(x_pos, y_pos));
    }

    RectangleShape GetPlayerRect()
    {
        return warrior_rect;
    }

    Vector2f GetSize()
    {
        return warrior_rect.getSize();
    }

    Vector2f GetPosition()
    {
        return warrior_rect.getPosition();
    }

    int GetSpriteCount()
    {
        return warrior.sprite_count;
    }

    void SetOrigin(float x, float y)
    {
        warrior_rect.setOrigin(Vector2f(x, y));
    }

    void SetPosition(float x_pos, float y_pos)
    {
        warrior_rect.setPosition(Vector2f(x_pos, y_pos));
    }

    void SetPositionRelative(float x, float y)
    {
        warrior_rect.move(x, y);
    }

    void SetSpriteCount(int sprite_count)
    {
        warrior.sprite_count = sprite_count;
    }

    void Move(float delta_time, string direction)
    {
        x_pos = warrior_rect.getPosition().x;
        y_pos = warrior_rect.getPosition().y;
        vel_x = Smooth(185.f, vel_x, delta_time);
        if (direction == "right")
        {
            faces_right = true;
        }
        else if (direction == "left")
        {
            faces_right = false;
        }
    }

    void Jump(float delta_time)
    {
        if (can_jump)
        {
            y_pos -= 25.f;
            float jump_height = 125.f;

            // kinematic equation -> initial_velocity = sqrt(final_velocity^2 - 2 * gravity * height)
            vel_y = -sqrt(pow(0, 2) + 2 * gravity * jump_height);
        }

        can_jump = false;
    }

    void Fall(float delta_time)
    {
        if (can_jump)
        {
            float jump_height = 0.f;

            vel_y = -sqrt(pow(0, 2) + 2 * gravity * jump_height);
        }
        can_jump = false;
    }

    void Gravity(float delta_time)
    {
        if (!can_jump)
        {
            vel_y += gravity * delta_time;
            y_pos += vel_y * delta_time;
        }
    }

    void PlatformCollision(RectangleShape platform)
    {
        y_pos = platform.getPosition().y - warrior_rect.getSize().y / 2 - 0.01f;
        can_jump = true;
    }

    void Update(Texture *spritesheet, string state, int total_sprite, float switch_time, float delta_time)
    {
        if (state == "idle" && warrior.sprite_count >= 2)
        {
            warrior.sprite_count = 0;
        }
        if (state == "jump")
        {
            warrior.sprite_count = 0;
        }
        if (state == "attack" && warrior.sprite_count >= 2)
        {
            warrior.sprite_count = 0;
        }

        if (!dead)
        {
            if (!can_jump)
            {
                if (vel_y > 0.f)
                {
                    warrior.sprite_count = 1;
                }
            }
        }

        warrior.Update(string("gfx/metadata/warrior-" + state + ".json"), total_sprite, switch_time);
        if (can_jump)
        {
            warrior.Animate(delta_time);
        }

        warrior_rect = warrior.GetSprite();
        warrior_rect.setTexture(spritesheet);
        warrior_rect.setSize(Vector2f(warrior.GetUvRect().width / 11.5, warrior.GetUvRect().height / 11.5));
        warrior_rect.setOrigin(warrior_rect.getSize().x / 2, warrior_rect.getSize().y / 2);
        if (faces_right)
        {
            warrior_rect.setScale(Vector2f(-1, 1));
            x_pos += vel_x * delta_time;
        }
        else
        {
            warrior_rect.setScale(Vector2f(1, 1));
            x_pos -= vel_x * delta_time;
        }
        warrior_rect.setPosition(Vector2f(x_pos, y_pos));

        if (dead)
        {
            if (warrior.sprite_count == 3)
            {
                dead_animation_complete = true;
            }
        }

        if (dead_animation_complete)
        {
            warrior.sprite_count = 3;
            SetPosition(GetPosition().x, GetPosition().y + 7.f);
        }
    }

    void Draw(RenderWindow &window)
    {
        window.draw(warrior_rect);
    }

public:
    float x_pos, y_pos;
    float vel_x = 0.f, vel_y = 0.f;
    bool can_jump = false;
    bool dead = false;
    bool dead_animation_complete = false;
    bool attacking = false;
    bool faces_right = false;

private:
    Spritesheet warrior = Spritesheet(string("gfx/metadata/warrior-idle.json"), 0, 2, 0.45f);
    RectangleShape warrior_rect;
    float gravity = 981.f;
};
