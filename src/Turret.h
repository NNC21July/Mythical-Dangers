#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <random>
#include "Collision.h"
using string = std::string;
using namespace sf;

class Turret
{
public:
    Turret(Texture *turret_texture, Platform platform)
    {
        turret.sprite_count = hit;
        turret_rect = turret.GetSprite();
        turret_rect.setTexture(turret_texture);
        turret_rect.setSize(Vector2f(57, 93));
        turret_rect.setOrigin(turret_rect.getSize().x / 2.f, turret_rect.getSize().y / 2.f);

        turret_img_crop.left = turret.GetUvRect().left;
        turret_img_crop.top = turret.GetUvRect().top;
        turret_img_crop.width = turret_rect.getSize().x * 10;
        turret_img_crop.height = (turret_rect.getSize().y - (turret_rect.getSize().y - platform.GetSize().y)) * 10;

        turret_rect.setTextureRect(turret_img_crop);
        turret_rect.setSize(Vector2f(turret_img_crop.width / 10.f, turret_img_crop.height / 10.f));
    }

    RectangleShape GetTurretRect()
    {
        return turret_rect;
    }

    Vector2f GetSize()
    {
        return turret_rect.getSize();
    }

    Vector2f GetPosition()
    {
        return turret_rect.getPosition();
    }

    void SetPosition(float x_pos, float y_pos)
    {
        turret_rect.setPosition(Vector2f(x_pos, y_pos));
    }

    Vector2f RandomizePosition(std::vector<Platform> platforms, std::vector<RectangleShape> ground, RenderWindow &window)
    {
        int pos_chance = rand() % 5;
        if (pos_chance == 0)
        {
            x_pos = platforms[1].GetPosition().x + platforms[1].GetSize().x / 2.f;
            y_pos = platforms[1].GetPosition().y - 93 / 2.f;
        }
        else if (pos_chance == 1)
        {
            x_pos = platforms[2].GetPosition().x + platforms[2].GetSize().x / 2.f;
            y_pos = platforms[2].GetPosition().y - 93 / 2.f;
        }
        else if (pos_chance == 2)
        {
            x_pos = platforms[3].GetPosition().x + platforms[3].GetSize().x / 2.f;
            y_pos = platforms[3].GetPosition().y - 93 / 2.f;
        }
        else if (pos_chance == 3)
        {
            x_pos = ground[0].getPosition().x + ground[0].getSize().x / 2.f;
            y_pos = ground[0].getPosition().y - 93 / 2.f;
        }
        else if (pos_chance == 4)
        {
            x_pos = window.getSize().x - ground[2].getSize().x / 2.f;
            y_pos = ground[0].getPosition().y - 93 / 2.f;
        }

        return Vector2f(x_pos, y_pos);
    }

    void Emerge(float delta_time)
    {
        float emerge_velocity = 100.f;
        float current_y_pos = GetPosition().y;
        if (current_y_pos >= y_pos)
        {
            current_y_pos -= emerge_velocity * delta_time;
        }
        else
        {
            animated = true;
        }
        SetPosition(x_pos, current_y_pos);

        if (GetSize().y < 93)
        {
            turret_img_crop.height += emerge_velocity * delta_time * 10;
            turret_rect.setTextureRect(turret_img_crop);
            turret_rect.setSize(Vector2f(turret_img_crop.width / 10.f, turret_img_crop.height / 10.f));
        }
    }

    void Collapse(Texture *texture, float delta_time)
    {
        if (!collapsed)
        {
            turret.Update("gfx/metadata/turret-collapsed.json", 6, 0.097f);
            turret.Animate(delta_time);
            turret_rect = turret.GetSprite();
            turret_rect.setTexture(texture);
            turret_rect.setSize(Vector2f(75, 93));
            turret_rect.setOrigin(turret_rect.getSize().x / 2.f, turret_rect.getSize().y / 2.f);
            turret_rect.setPosition(x_pos, y_pos);
        }
    }

    void Update(Texture *turret_texture, Texture *turret_collapse_texture, Platform platform, float delta_time)
    {
        if (animated && this->hit < 4 && !collapse_now)
        {
            turret.Update("gfx/metadata/turret.json", 4, 0.f);
            turret.sprite_count = hit;
            turret_rect = turret.GetSprite();
            turret_rect.setTexture(turret_texture);
            turret_rect.setSize(Vector2f(57, 93));
            turret_rect.setOrigin(turret_rect.getSize().x / 2.f, turret_rect.getSize().y / 2.f);
            turret_rect.setPosition(x_pos, y_pos);

            turret_img_crop.left = turret.GetUvRect().left;
            turret_img_crop.top = turret.GetUvRect().top;

            turret_rect.setTextureRect(turret_img_crop);
        }
        if (this->hit > 3)
        {
            collapse_now = true;
            turret.sprite_count = 0;
            hit = 0;
        }
        if (collapse_now)
        {
            Collapse(turret_collapse_texture, delta_time);
        }
        if (collapse_now && !collapsed && turret.sprite_count == 5)
        {
            collapsed = true;
        }
        if (collapsed)
        {
            turret.sprite_count = 5;
            turret_ruins_disappear_time += delta_time;
        }
    }

    void Draw(RenderWindow &window)
    {
        window.draw(turret_rect);
    }

public:
    bool can_spawn = false;
    bool animated = false;
    float x_pos = 0, y_pos = 0;
    int hit = 0;
    bool collapse_now = false;
    bool collapsed = false;
    float turret_ruins_disappear_time = 0.f;

private:
    Spritesheet turret = Spritesheet(string("gfx/metadata/turret.json"), 0, 4, 0.f);
    RectangleShape turret_rect;
    IntRect turret_img_crop;
};

class PoisonLiquid
{
public:
    PoisonLiquid()
    {
        poison_texture.loadFromFile("gfx/sprite and image/poison-liquid.png");

        poison_rect = poison.GetSprite();
        poison_rect.setTexture(&poison_texture);
        poison_rect.setSize(Vector2f(width, height));
        poison_rect.setOrigin(poison_rect.getSize().x / 2.f, poison_rect.getSize().y / 2.f);
        SetPosition(0, 0);
    }

    Vector2f GetSize()
    {
        return poison_rect.getSize();
    }

    Vector2f GetPosition()
    {
        return poison_rect.getPosition();
    }

    Vector2f GetVelocity()
    {
        return Vector2f(vel_x, vel_y);
    }

    void SetSize(float x, float y)
    {
        width = x;
        height = y;
    }

    void SetOrigin(float x, float y)
    {
        poison_rect.setOrigin(x, y);
    }

    void SetPosition(float x_pos, float y_pos)
    {
        poison_rect.setPosition(Vector2f(x_pos, y_pos));
    }

    void SetVelocity(float x, float y)
    {
        vel_x = x;
        vel_y = y;
    }

    void Appear(float delta_time)
    {
        float growth_vel = 175.f;
        if (width < 25 && height < 25)
        {
            width += growth_vel * delta_time;
            height += growth_vel * delta_time;
        }
        poison_rect.setSize(Vector2f(width, height));
        poison_rect.setOrigin(poison_rect.getSize().x / 2.f, poison_rect.getSize().y / 2.f);
    }

    void Disappear(float delta_time)
    {
        float shrink_vel = 210.f;
        if (width > 0 && height > 0)
        {
            width -= shrink_vel * delta_time;
            height -= shrink_vel * delta_time;
        }
        poison_rect.setSize(Vector2f(width, height));
        poison_rect.setOrigin(poison_rect.getSize().x / 2.f, poison_rect.getSize().y / 2.f);
    }

    void Shoot(Vector2f player_pos, float delta_time)
    {
        float h;
        float gravity = -981.f;

        if (!shot)
        {
            displacement_x = player_pos.x - poison_rect.getPosition().x;
            displacement_y = poison_rect.getPosition().y - player_pos.y;
            if (displacement_y < 0)
                displacement_y = player_pos.y - poison_rect.getPosition().y;
            h = displacement_y + 25.f;
            // kinematic equation in both axis
            vel_x = displacement_x / (sqrt(-2 * h / gravity) + sqrt(2 * (displacement_y - h) / gravity));
            if (displacement_y <= 50)
                vel_x /= 1.8;
            vel_y = -abs(sqrt(-2 * gravity * h));
            // particle effect
            float particle_vel_x_substract;
            if (vel_x > 0)
            {
                particle_vel_x_substract = -275;
            }
            else
            {
                particle_vel_x_substract = 275;
            }
            for (int i = 0; i < rand() % 2 + 2; i++)
            {
                particles.push_back({{GetPosition().x, GetPosition().y}, {float(rand() % 550 - vel_x + particle_vel_x_substract) / 2.f, 200}, {float(rand() % 2 + 4)}});
                particle_shapes.push_back(CircleShape());
            }
            shot = true;
        }
        vel_y -= gravity * delta_time;

        // particle effect
        for (int i = 0; i < particles.size(); i++)
        {
            particle_shapes[i].setPosition(Vector2f(particles[i][0][0], particles[i][0][1]));
            particle_shapes[i].setRadius(particles[i][2][0]);
            particle_shapes[i].setFillColor(Color(91, 41, 230));
            particles[i][0][0] += particles[i][1][0] * delta_time;
            particles[i][0][1] += particles[i][1][1] * delta_time;
            particles[i][2][0] -= 10 * delta_time;
            if (particles[i][2][0] <= 0)
            {
                particles.erase(particles.begin() + i);
                particle_shapes.erase(particle_shapes.begin() + i);
            }
        }
    }

    bool CheckCollision(RectangleShape object)
    {
        return poison_collision.CheckCollision(object);
    }

    string CheckPlatformCollision(RectangleShape platform)
    {
        return poison_ground_collision.CheckCollision(platform);
    }

    void Update(Turret turret, float delta_time, Vector2f player_pos, bool &game_paused, Sound &turret_shoot_sound, bool &turret_shoot_sound_played)
    {
        poison.Update(string("gfx/metadata/poison-liquid.json"), 4, 0.2f);
        poison.Animate(delta_time);

        poison_rect = poison.GetSprite();
        poison_rect.setTexture(&poison_texture);
        poison_rect.setOrigin(poison_rect.getSize().x / 2.f, poison_rect.getSize().y / 2.f);
        if (!shot)
        {
            x_pos = turret.GetPosition().x;
            y_pos = turret.GetPosition().y;
        }

        SetPosition(x_pos, y_pos);

        poison_ground_collision.Update(poison_rect);
        poison_collision.Update(poison_rect);

        // appear delay
        if (!game_paused)
        {
            if (turret.animated)
            {
                appear_delay += delta_time;
            }
            if (appear_delay >= 2.f)
            {
                can_appear = true;
                appear_delay = 0.f;
            }
        }
        if (can_appear)
        {
            Appear(delta_time);
            appear_delay = 0.f;
        }
        if (turret.collapse_now)
        {
            Disappear(delta_time);
        }

        if (!game_paused)
        {
            // shoot delay
            if (width >= 25 && height >= 25)
            {
                shoot_delay += delta_time;
            }
            if (shoot_delay >= 2.5f)
            {
                can_shoot = true;
                turret_shoot_sound_played = false;
                shoot_delay = 0.f;
            }
            if (can_shoot)
            {
                Shoot(player_pos, delta_time);
                if (!turret_shoot_sound_played)
                {
                    turret_shoot_sound.play();
                    turret_shoot_sound_played = true;
                }
            }
            if (shot)
            {
                x_pos += vel_x * delta_time;
                y_pos += vel_y * delta_time;
            }
        }
    }

    void Draw(RenderWindow &window)
    {
        window.draw(poison_rect);
        for (int i = 0; i < particle_shapes.size(); i++)
        {
            window.draw(particle_shapes[i]);
        }
    }

public:
    float appear_delay = 0.f;
    bool can_appear = false;
    float shoot_delay = 0.f;
    bool can_shoot = false;
    bool shot = false;
    float saved_vel_x, saved_vel_y;
    float paused = false;

private:
    Spritesheet poison = Spritesheet(string("gfx/metadata/poison-liquid.json"), 0, 4, 0.2f);
    RectangleShape poison_rect;
    Texture poison_texture;
    GroundCollision poison_ground_collision = GroundCollision(poison_rect);
    Collision poison_collision = Collision(poison_rect);
    float width = 0, height = 0;
    float displacement_x, displacement_y;
    float vel_x = 0.f, vel_y = 0.f;
    float x_pos, y_pos;
    // "particles" vector -> "particle" vector -> "location", "velocity", "radius" vectors
    std::vector<std::vector<std::vector<float>>> particles;
    std::vector<CircleShape> particle_shapes;
};

class PoisonPuddle
{
public:
    PoisonPuddle()
    {
        poison_pdl_texture.loadFromFile("gfx/sprite and image/poison-puddle.png");

        poison_pdl_rect = poison_pdl.GetSprite();
        poison_pdl_rect.setTexture(&poison_pdl_texture);
        poison_pdl_rect.setSize(Vector2f(poison_pdl.GetUvRect().width * 7 / 10, poison_pdl.GetUvRect().height * 7 / 10));
        poison_pdl_rect.setOrigin(poison_pdl_rect.getSize().x / 2.f, poison_pdl_rect.getSize().y / 2.f);
        poison_pdl_rect.setPosition(Vector2f(x_pos, y_pos));
    }

    RectangleShape GetPdlRect()
    {
        return poison_pdl_rect;
    }

    Vector2f GetPosition()
    {
        return Vector2f(x_pos, y_pos);
    }

    void SetPosition(float x_pos, float y_pos)
    {
        poison_pdl_rect.setPosition(Vector2f(x_pos, y_pos));
    }

    void SetPositionConstant(float x_pos, float y_pos)
    {
        this->x_pos = x_pos;
        this->y_pos = y_pos;
    }

    void SetSpriteCount(int sprite_count)
    {
        poison_pdl.sprite_count = sprite_count;
    }

    void Update(float delta_time)
    {
        poison_pdl.Update(string("gfx/metadata/poison-puddle.json"), 3, 0.35f);
        poison_pdl.Animate(delta_time);
        if (poison_pdl.sprite_count == 2)
        {
            pdl_animation_complete = true;
        }
        if (pdl_animation_complete)
        {
            poison_pdl.sprite_count = 2;
        }

        poison_pdl_rect = poison_pdl.GetSprite();
        poison_pdl_rect.setTexture(&poison_pdl_texture);
        poison_pdl_rect.setSize(Vector2f(poison_pdl.GetUvRect().width * 7 / 10, poison_pdl.GetUvRect().height * 7 / 10));
        poison_pdl_rect.setOrigin(poison_pdl_rect.getSize().x / 2.f, poison_pdl_rect.getSize().y / 2.f);
        poison_pdl_rect.setPosition(Vector2f(x_pos, y_pos));
    }

    void Draw(RenderWindow &window)
    {
        window.draw(poison_pdl_rect);
    }

public:
    bool can_appear = false;
    bool pdl_animation_complete = false;
    float disappear_timer = 0;

private:
    Spritesheet poison_pdl = Spritesheet(string("gfx/metadata/poison-puddle.json"), 0, 3, 0.35f);
    RectangleShape poison_pdl_rect;
    Texture poison_pdl_texture;
    float x_pos = -10.f, y_pos = -10.f;
};

class PoisonGasEffect
{
public:
    PoisonGasEffect(Texture *poison_gas_texture)
    {
        this->poison_gas_texture = *poison_gas_texture;

        poison_gas_rect = poison_gas.GetSprite();
        poison_gas_rect.setTexture(poison_gas_texture);
        poison_gas_rect.setSize(Vector2f(poison_gas.GetUvRect().width * 7 / 10, poison_gas.GetUvRect().height * 7 / 10));
        poison_gas_rect.setOrigin(poison_gas_rect.getSize().x / 2.f, poison_gas_rect.getSize().y / 2.f);
        poison_gas_rect.setPosition(Vector2f(x_pos, y_pos));
    }

    Vector2f GetPosition()
    {
        return poison_gas_rect.getPosition();
    }

    void SetPosition(float x_pos, float y_pos)
    {
        this->x_pos = x_pos;
        this->y_pos = y_pos;
    }

    void Update(float delta_time)
    {
        poison_gas.Update(string("gfx/metadata/poison-gas-effect.json"), 5, 0.35f);
        poison_gas.Animate(delta_time);

        poison_gas_rect = poison_gas.GetSprite();
        poison_gas_rect.setTexture(&poison_gas_texture);
        poison_gas_rect.setSize(Vector2f(poison_gas.GetUvRect().width * 7 / 10, poison_gas.GetUvRect().height * 7 / 10));
        poison_gas_rect.setOrigin(poison_gas_rect.getSize().x / 2.f, poison_gas_rect.getSize().y / 2.f);
        poison_gas_rect.setPosition(Vector2f(x_pos, y_pos));
    }

    void Draw(RenderWindow &window)
    {
        window.draw(poison_gas_rect);
    }

public:
    bool can_appear = false;

private:
    Spritesheet poison_gas = Spritesheet(string("gfx/metadata/poison-gas-effect.json"), 0, 5, 0.35f);
    RectangleShape poison_gas_rect;
    Texture poison_gas_texture;
    float x_pos = 0, y_pos = 0;
};
