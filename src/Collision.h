#include <SFML/Graphics.hpp>
using string = std::string;
using namespace sf;

class Collision
{
public:
    Collision(RectangleShape object) { this->object = object; }

    bool CheckCollision(RectangleShape other)
    {
        Vector2f object_pos = object.getPosition();
        Vector2f object_half_size = Vector2f(object.getSize().x / 2.f, object.getSize().y / 2.f);
        Vector2f other_pos = other.getPosition();
        Vector2f other_half_size = Vector2f(other.getSize().x / 2.f, other.getSize().y / 2.f);

        float delta_x = other_pos.x - object_pos.x;
        float delta_y = other_pos.y - object_pos.y;

        float intersect_x = abs(delta_x) - (other_half_size.x + object_half_size.x);
        float intersect_y = abs(delta_y) - (other_half_size.y + object_half_size.y);

        if (intersect_x < 0.f && intersect_y < 0.f)
        {
            return true;
        }

        return false;
    }

    void Update(RectangleShape object) { this->object = object; }

private:
    RectangleShape object;
};

class GroundCollision
{
public:
    GroundCollision(RectangleShape object)
    {
        this->object = object;
    }

    string CheckCollision(RectangleShape other)
    {
        object.setOrigin(0, 0);
        other.setOrigin(0, 0);

        Vector2f object_pos = object.getPosition();
        Vector2f object_size = object.getSize();
        Vector2f other_pos = other.getPosition();
        Vector2f other_size = other.getSize();

        if ((other_pos.x <= object_pos.x && object_pos.x <= other_pos.x + other_size.x) && (object_pos.y + object_size.y >= other_pos.y + other_size.y))
        {
            return "bottom";
        }
        return "none";
    }

    void Update(RectangleShape object)
    {
        this->object = object;
    }

private:
    RectangleShape object;
};
