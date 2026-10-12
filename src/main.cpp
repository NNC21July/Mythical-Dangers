#include <SFML/Graphics.hpp>
#include "splash_screen.h"
#include "title_screen.h"
#include "game.h"
using string = std::string;
using namespace sf;

int main()
{
    RenderWindow window(VideoMode(800.f, 650.f), "Mythical Dangers", Style::Close);
    window.clear(Color::Black);

    Image icon;
    icon.loadFromFile("./icon.png");

    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    View view(Vector2f(0.f, 0.f), Vector2f(window.getSize().x, window.getSize().y));

    string window_state = "splash-screen";

    while (window.isOpen())
    {
        if (window_state == "splash-screen")
        {
            splash_screen(window, window_state);
        }
        else if (window_state == "title-screen")
        {
            title_screen(window, window_state);
        }
        else if (window_state == "game")
        {
            game(window, window_state, view);
        }
    }

    return 0;
}