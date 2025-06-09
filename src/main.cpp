#include <SFML/Graphics.hpp>
#include <Windows.h>

sf::Texture t_sleep("sprites/dog_sleep.png");
sf::Sprite dog(t_sleep);

void animateSprite(int frameCount, int frameWidth, int frameHeight, float animationSpeed, sf::Time elapsedTime)
{ //makes it gifable
    int currentFrame = (int) (elapsedTime.asSeconds() / animationSpeed) % frameCount;
    dog.setTextureRect(sf::IntRect({ currentFrame * frameWidth, 0 }, { frameWidth, frameHeight }));
}

sf::Clock clocky;

bool dragging = false;
sf::Vector2i dragOffset;

int main()
{
    sf::RenderWindow window(sf::VideoMode({ 128, 128 }), "Annoying Dog Pet Thingy", sf::Style::None);

    HWND hwnd = window.getNativeHandle();
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); //makes it on top of anything
    LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
    SetWindowLong(hwnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hwnd, RGB(0, 1, 0), 0, LWA_COLORKEY);

    //winapi can do magic lessgooooo


    while (window.isOpen())
    {
        sf::Time elapsed = clocky.getElapsedTime();

        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
            {
                sf::Vector2i mousePos = sf::Mouse::getPosition();
                if (mousePos.x > window.getPosition().x && mousePos.x < window.getPosition().x + window.getSize().x
                    && mousePos.y>window.getPosition().y && mousePos.y < window.getPosition().y + window.getSize().y)
                {
                    window.setPosition({ mousePos.x - 64, mousePos.y - 16 });
                }

            }
        }

        animateSprite(2, 560, 260, 1, elapsed);

        float desiredWidth = 128.f;
        float scaleX = desiredWidth / 560;
        dog.setScale({ scaleX, scaleX });


        window.clear(sf::Color(0, 1, 0));
        window.draw(dog);
        window.display();
    }
}