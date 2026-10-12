#include <SFML/Graphics.hpp>
using string = std::string;
using namespace sf;

void fade_in(RectangleShape &object, float velocity, float &alpha, float delta_time, bool &fade_now, bool &faded, Color color, float alpha_range);
void fade_out(RectangleShape &object, float velocity, float &alpha, float delta_time, bool &fade_now, bool &faded, Color color, float alpha_range);

void splash_screen(RenderWindow &window, string &window_state)
{
    // logo
    Texture logo_texture;
    logo_texture.loadFromFile("gfx/sprite and image/nnc-games-logo.png");
    RectangleShape logo;
    logo.setTexture(&logo_texture);
    logo.setSize(Vector2f(500, 90));
    logo.setOrigin(Vector2f(logo.getSize().x / 2.f, logo.getSize().y / 2.f));
    logo.setPosition(Vector2f(window.getSize().x / 2.f, window.getSize().y / 2.f));
    float logo_alpha = 0.f;
    bool logo_fade_in_now = true;
    bool logo_fade_out_now = false;
    bool logo_faded_in = false;
    bool logo_faded_out = false;
    float logo_appear_delay = 0.f;

    float total_time = 0.f, delta_time;
    Clock clock;

    while (window_state == "splash-screen")
    {
        Event window_event;
        while (window.pollEvent(window_event))
        {
            switch (window_event.type)
            {
            case Event::Closed:
                window_state = "quit";
                window.close();
            }
        }

        delta_time = clock.restart().asSeconds();
        if (delta_time > 1.0f / 20.0f)
        {
            delta_time = 1.0f / 20.0f;
        }

        if (logo_fade_in_now)
            fade_in(logo, 250.f, logo_alpha, delta_time, logo_fade_in_now, logo_faded_in, Color(255, 255, 255, logo_alpha), 255.f);
        if (logo_faded_in)
        {
            logo_appear_delay += delta_time;
            if (logo_appear_delay >= 2.5f)
            {
                logo_fade_out_now = true;
                logo_appear_delay = 0.f;
            }
        }
        if (logo_fade_out_now)
            fade_out(logo, 300.f, logo_alpha, delta_time, logo_fade_out_now, logo_faded_out, Color(255, 255, 255, logo_alpha), 0.f);

        if (logo_faded_out)
            window_state = "title-screen";

        window.clear();
        window.draw(logo);
        window.display();
    }
}