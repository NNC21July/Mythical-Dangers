#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include "CustomFunctions.h"
using string = std::string;
using json = nlohmann::json;
using namespace sf;

class Spritesheet
{
public:
    Spritesheet(string metadata, int sprite_count = 0, int total_sprite = 1, float switch_time = 0.f)
    {
        this->total_sprite = total_sprite;
        this->switch_time = switch_time;
        this->sprite_count = sprite_count;
        total_time = 0.f;

        // opening json metadata
        std::ifstream i(metadata);
        i >> json_file;
        i.close();

        json frame = json_file["frames"]["New Piskel" + int_to_str(sprite_count) + ".png"]["frame"];
        int x = frame["x"];
        int y = frame["y"];
        int w = frame["w"];
        int h = frame["h"];

        uv_rect.left = x;
        uv_rect.top = y;
        uv_rect.width = w;
        uv_rect.height = h;

        sprite.setTextureRect(uv_rect);
    }

    void Animate(float delta_time)
    {
        total_time += delta_time;
        if (total_time >= switch_time)
        {
            total_time -= switch_time;
            sprite_count += 1;
            if (sprite_count >= total_sprite)
            {
                sprite_count = 0;
            }
        }
    }

    void Update(string metadata, int total_sprite, float switch_time)
    {
        this->total_sprite = total_sprite;
        this->switch_time = switch_time;

        std::ifstream i(metadata);
        i >> json_file;
        i.close();

        json frame = json_file["frames"]["New Piskel" + int_to_str(sprite_count) + ".png"]["frame"];
        int x = frame["x"];
        int y = frame["y"];
        int w = frame["w"];
        int h = frame["h"];

        uv_rect.left = x;
        uv_rect.top = y;
        uv_rect.width = w;
        uv_rect.height = h;

        sprite.setTextureRect(uv_rect);
    }

    RectangleShape GetSprite()
    {
        return sprite;
    }

    IntRect GetUvRect()
    {
        return uv_rect;
    }

public:
    int sprite_count;

private:
    RectangleShape sprite;
    json json_file;
    IntRect uv_rect;

    int total_sprite;
    float switch_time, total_time;
};