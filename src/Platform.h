#include <SFML/Graphics.hpp>
using string = std::string;
using namespace sf;

class Platform
{
public:
    Platform(Texture *spritesheet, string platform_type)
    {
        int platform_sprite_count;
        if (platform_type == "long")
        {
            platform_sprite_count = 0;
        }
        else if (platform_type == "medium")
        {
            platform_sprite_count = 1;
        }
        else if (platform_type == "short")
        {
            platform_sprite_count = 2;
        }
        Spritesheet platform = Spritesheet(string("gfx/metadata/platforms.json"), platform_sprite_count, 3, 0.f);
        platform_rect = platform.GetSprite();

        platform_rect.setTexture(spritesheet);
        platform_rect.setSize(Vector2f(platform.GetUvRect().width / 11.5, platform.GetUvRect().height / 11.5));
        platform_rect.setOrigin(Vector2f(0, 0));
    }

    RectangleShape GetRect()
    {
        return platform_rect;
    }

    Vector2f GetSize()
    {
        return platform_rect.getSize();
    }

    Vector2f GetPosition()
    {
        return platform_rect.getPosition();
    }

    void SetOrigin(float x, float y) {
        platform_rect.setOrigin(x, y);
    }

    void SetPosition(float x_pos, float y_pos)
    {
        platform_rect.setPosition(Vector2f(x_pos, y_pos));
    }

    void SetPositionRelative(float x_move, float y_move)
    {
        platform_rect.move(Vector2f(x_move, y_move));
    }

    void Draw(RenderWindow &window)
    {
        window.draw(platform_rect);
    }

private:
    Spritesheet platform = Spritesheet(string("gfx/metadata/platforms.json"));
    RectangleShape platform_rect;
};