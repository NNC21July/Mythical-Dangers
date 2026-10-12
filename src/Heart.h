#include <SFML/Graphics.hpp>
#include <vector>
using namespace sf;

class Platform;

class Heart
{
public:
    Heart(Texture *heart_texture)
    {
        heart.setTexture(heart_texture);
        heart.setSize(Vector2f(0, 0));
        heart.setOrigin(heart.getSize().x / 2.f, heart.getSize().y / 2.f);
    }

    RectangleShape GetHeartRect()
    {
        return heart;
    }

    Vector2f GetPosition()
    {
        return heart.getPosition();
    }

    void SetPosition(float x_pos, float y_pos)
    {
        heart.setPosition(Vector2f(x_pos, y_pos));
    }

    Vector2f RandomizePosition(std::vector<Platform> platforms, std::vector<RectangleShape> ground)
    {
        float x_pos, y_pos;
        int pos_chance = rand() % 5;
        if (pos_chance == 0)
        {
            x_pos = platforms[0].GetPosition().x + platforms[0].GetSize().x / 2.f;
            y_pos = platforms[0].GetPosition().y - 20.f;
        }
        else if (pos_chance == 1)
        {
            x_pos = platforms[1].GetPosition().x + platforms[1].GetSize().x / 2.f;
            y_pos = platforms[1].GetPosition().y - 20.f;
        }
        else if (pos_chance == 2)
        {
            x_pos = platforms[2].GetPosition().x + platforms[2].GetSize().x / 2.f;
            y_pos = platforms[2].GetPosition().y - 20.f;
        }
        else if (pos_chance == 3)
        {
            x_pos = platforms[3].GetPosition().x + platforms[3].GetSize().x / 2.f;
            y_pos = platforms[3].GetPosition().y - 20.f;
        }
        else if (pos_chance == 4)
        {
            x_pos = ground[0].getPosition().x + ground[0].getSize().x / 2.f;
            y_pos = ground[0].getPosition().y - 20.f;
        }
        else if (pos_chance == 5)
        {
            x_pos = ground[1].getPosition().x + ground[1].getSize().x / 2.f;
            y_pos = ground[0].getPosition().y - 20.f;
        }
        else if (pos_chance == 6)
        {
            x_pos = ground[2].getPosition().x + ground[2].getSize().x / 2.f;
            y_pos = ground[0].getPosition().y - 20.f;
        }

        return Vector2f(x_pos, y_pos);
    }

    void Animate(float delta_time)
    {
        float width = heart.getSize().x;
        float height = heart.getSize().y;
        float width_growth_vel = 200.f;
        float height_growth_vel = 30 * width_growth_vel / 33;
        if (width < 33 && height < 30)
        {
            width += width_growth_vel * delta_time;
            height += height_growth_vel * delta_time;
        }
        else
        {
            animated = true;
        }
        heart.setSize(Vector2f(width, height));
        heart.setOrigin(heart.getSize().x / 2.f, heart.getSize().y / 2.f);
    }

    void Draw(RenderWindow &window)
    {
        window.draw(heart);
    }

public:
    bool can_spawn = false;
    bool animated = false;

private:
    RectangleShape heart;
};