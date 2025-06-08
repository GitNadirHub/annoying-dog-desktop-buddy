#include <SFML/Graphics.hpp>
#include <Windows.h>

int main()
{
    sf::RenderWindow window(sf::VideoMode({ 400, 400 }), "Ralsei Pet Thingy");

    HWND hwnd = window.getNativeHandle();

    LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
    SetWindowLong(hwnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);

    //winapi magic lessgooooo


    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear();
        window.display();
    }
}