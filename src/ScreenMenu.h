#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>
#include <math.h>
using json = nlohmann::json;
using namespace sf;

class DeadMenu
{
public:
    DeadMenu(RenderWindow &window)
    {
        dead_menu_texture.loadFromFile("gfx/sprite and image/dead-menu.png");

        dead_menu.setTexture(&dead_menu_texture);
        dead_menu.setSize(Vector2f(376, 520));
        dead_menu.setOrigin(dead_menu.getSize().x / 2, dead_menu.getSize().y / 2);

        font.loadFromFile("gfx/font/VPPixel.otf");
        time.setFont(font);
        time.setCharacterSize(27);
        time.setFillColor(Color::Black);
        time.setStyle(Text::Bold);

        x_pos = window.getSize().x / 2.f;
        y_pos = window.getSize().y + dead_menu.getSize().y / 2.f;
        vel_x = 0.f;
        vel_y = 0.f;
        animation_distance = window.getSize().y - dead_menu.getPosition().y;
        dead_menu.setPosition(Vector2f(x_pos, y_pos));

        for (int i = 0, y_start = 272; i < 3; i++)
        {
            RectangleShape button;
            float button_x = x_pos + 32 - dead_menu.getSize().x / 2.f;
            float button_y = y_pos + y_start - dead_menu.getSize().y / 2.f;
            button.setPosition(Vector2f(button_x, button_y));
            button.setSize(Vector2f(320, 56));
            button.setFillColor(Color(0, 0, 0, 0));
            buttons.push_back(button);

            y_start += buttons[i].getSize().y + 24;
        }

        high_score_badge_texture.loadFromFile("gfx/sprite and image/high-score-badge.png");

        high_score_badge.setTexture(&high_score_badge_texture);
        high_score_badge.setSize(Vector2f(0, 0));
        high_score_badge.setOrigin(Vector2f(high_score_badge.getSize().x / 2.f, high_score_badge.getSize().y / 2.f));
        high_score_badge.setPosition(Vector2f(545, 310));

        for (int i = 0; i < 15; i++)
        {
            high_score_particles.push_back({{high_score_badge.getPosition().x, high_score_badge.getPosition().y}, {float(rand() % 550 - 275), -200}, {float(rand() % 2 + 10)}});
            high_score_particle_shapes.push_back(CircleShape());
        }
    }

    void Reset(RenderWindow &window)
    {
        x_pos = window.getSize().x / 2.f;
        y_pos = window.getSize().y + dead_menu.getSize().y / 2.f;
        high_score_badge.setSize(Vector2f(0, 0));
        for (int i = 0; i < 15; i++)
        {
            high_score_particles.push_back({{high_score_badge.getPosition().x, high_score_badge.getPosition().y}, {float(rand() % 550 - 275), -200}, {float(rand() % 2 + 10)}});
            high_score_particle_shapes.push_back(CircleShape());
        }
        animation_called = false;
        transitioned = false;
        score_time[0] = 0;
        score_time[1] = 0;
        score_is_highest = false;
    }

    int CheckButtonClicked(Vector2i mouse_pos)
    {
        for (int i = 0; i < 3; i++)
        {
            if (button_clicked(buttons[i], mouse_pos))
                return i;
        }
        return 4;
    }

    void Animate(RenderWindow &window, float score_time, float delta_time)
    {
        // accessing previous high score in json file
        json high_score_file;
        std::ifstream i("saves/user_data_and_preference.json");
        i >> high_score_file;
        i.close();
        int high_score = high_score_file["high_score"];

        if ((int)round(score_time) > high_score)
            score_is_highest = true;

        // formatting time
        if (!animation_called)
            this->score_time[1] = score_time;
        while (this->score_time[1] >= 60)
        {
            this->score_time[1] -= 60;
            this->score_time[0] += 1;
        }
        string seconds_formatted;
        if (this->score_time[1] < 10)
            seconds_formatted = "0" + std::to_string((int)this->score_time[1]);
        else
            seconds_formatted = std::to_string((int)this->score_time[1]);

        time.setString(std::to_string((int)this->score_time[0]) + ":" + seconds_formatted);
        time.setOrigin(time.getLocalBounds().width / 2.f, time.getLocalBounds().height / 2.f);

        if (dead_menu.getPosition().y > window.getSize().y / 2.f + 300.f)
            vel_y = Smooth(7000.f, vel_y, delta_time);
        else
            vel_y = Smooth(0.f, vel_y, delta_time);

        if (vel_y == 0 && animation_called)
            transitioned = true;
        y_pos -= vel_y * delta_time;
        for (int i = 0, y_start = 272; i < 3; i++)
        {
            float button_x = x_pos + 32 - dead_menu.getSize().x / 2.f;
            float button_y = y_pos + y_start - dead_menu.getSize().y / 2.f;
            buttons[i].setPosition(Vector2f(button_x, button_y));

            y_start += buttons[i].getSize().y + 24;
        }
        time.setPosition(Vector2f(round(x_pos), round(y_pos - 30.f)));
        dead_menu.setPosition(Vector2f(x_pos, y_pos));

        animation_called = true;

        if (score_is_highest)
        {
            high_score_file["high_score"] = (int)round(score_time);

            std::ofstream modified_file("saves/user_data_and_preference.json");
            modified_file << high_score_file;
            modified_file.close();
        }

        if (transitioned && score_is_highest)
        {
            if (high_score_badge.getSize().x < 35)
            {
                high_score_badge_vel_x = Smooth(300.f, high_score_badge_vel_x, delta_time);
                high_score_badge_vel_y = Smooth(300.f, high_score_badge_vel_y, delta_time);
            }
            else
            {
                high_score_badge_vel_x = Smooth(0.f, high_score_badge_vel_x, delta_time);
                high_score_badge_vel_y = Smooth(0.f, high_score_badge_vel_y, delta_time);
            }
        }
        high_score_badge.setSize(Vector2f(high_score_badge.getSize().x + high_score_badge_vel_x * delta_time, high_score_badge.getSize().y + high_score_badge_vel_y * delta_time));
        high_score_badge.setOrigin(Vector2f(high_score_badge.getSize().x / 2.f, high_score_badge.getSize().y / 2.f));

        if (high_score_badge.getSize().x > 35 && high_score_badge_vel_x < 100.f)
        {
            for (int i = 0; i < high_score_particles.size(); i++)
            {
                high_score_particle_shapes[i].setPosition(Vector2f(high_score_particles[i][0][0], high_score_particles[i][0][1]));
                high_score_particle_shapes[i].setRadius(high_score_particles[i][2][0]);
                high_score_particle_shapes[i].setFillColor(Color::White);
                high_score_particles[i][0][0] += high_score_particles[i][1][0] * delta_time;
                high_score_particles[i][0][1] += high_score_particles[i][1][1] * delta_time;
                high_score_particles[i][2][0] -= 28 * delta_time;
                if (high_score_particles[i][2][0] <= 0)
                {
                    high_score_particles.erase(high_score_particles.begin() + i);
                    high_score_particle_shapes.erase(high_score_particle_shapes.begin() + i);
                }
            }
        }
    }

    void Draw(RenderWindow &window)
    {
        window.draw(dead_menu);
        window.draw(time);
        for (RectangleShape button : buttons)
            window.draw(button);
        for (CircleShape &particle_shape : high_score_particle_shapes)
            window.draw(particle_shape);
        window.draw(high_score_badge);
    }

public:
    // minutes and seconds
    std::vector<int> score_time = {0, 0};
    bool animation_called = false;
    bool transitioned = false;

private:
    RectangleShape dead_menu;
    Texture dead_menu_texture;
    Text time;
    Font font;
    float vel_x, vel_y;
    float x_pos, y_pos;
    float animation_distance, animation_time, animation_deceleration;
    std::vector<RectangleShape> buttons;
    bool score_is_highest;
    RectangleShape high_score_badge;
    Texture high_score_badge_texture;
    float high_score_badge_vel_x = 0.f, high_score_badge_vel_y = 0.f;
    // "high_score_particles" vector -> "particle" vector -> "location", "velocity", "radius" vectors
    std::vector<std::vector<std::vector<float>>> high_score_particles;
    std::vector<CircleShape> high_score_particle_shapes;
};

class PauseMenu
{
public:
    PauseMenu(RenderWindow &window)
    {
        pause_menu_texture.loadFromFile("gfx/sprite and image/pause-menu.png");

        pause_menu.setTexture(&pause_menu_texture);
        pause_menu.setSize(Vector2f(376, 536));
        pause_menu.setOrigin(pause_menu.getSize().x / 2.f, pause_menu.getSize().y / 2.f);
        pause_menu.setPosition(Vector2f(window.getSize().x / 2.f, window.getSize().y / 2.f));

        for (int i = 0, y_start = 200; i < 4; i++)
        {
            RectangleShape button;
            float button_x = pause_menu.getPosition().x + 32 - pause_menu.getSize().x / 2.f;
            float button_y = pause_menu.getPosition().y + y_start - pause_menu.getSize().y / 2.f;
            button.setPosition(Vector2f(button_x, button_y));
            button.setSize(Vector2f(320, 56));
            button.setFillColor(Color(0, 0, 0, 0));
            buttons.push_back(button);

            y_start += buttons[i].getSize().y + 24;
        }
    }

    int CheckButtonClicked(Vector2i mouse_pos)
    {
        for (int i = 0; i < 4; i++)
        {
            if (button_clicked(buttons[i], mouse_pos))
            {
                return i;
            }
        }
        return 4;
    }

    void Draw(RenderWindow &window)
    {
        window.draw(pause_menu);
        for (int i = 0; i < 4; i++)
        {
            window.draw(buttons[i]);
        }
    }

private:
    RectangleShape pause_menu;
    Texture pause_menu_texture;
    std::vector<RectangleShape> buttons;
};