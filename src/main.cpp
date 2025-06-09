#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <Windows.h>
#include <tlhelp32.h>
#include <string>
#include <iostream>
#include "menu.hpp"

sf::Texture t_sleep("sprites/dog_sleep.png");
sf::Texture t_drag("sprites/dog_drag.png");
sf::Texture t_epic("sprites/dog_epic.png");
sf::Texture t_idle("sprites/dog_idle.png");
sf::Texture t_spin("sprites/dog_spin.png");
sf::Sprite dog(t_sleep);

sf::SoundBuffer healBuffer("audios/heal.wav");
sf::Sound healSound(healBuffer);

sf::SoundBuffer startupBuffer("audios/startup.wav");
sf::Sound startupSound(startupBuffer);

sf::SoundBuffer megalovaniaBuffer("audios/megalovania-whatsapp-edition.wav");
sf::Sound megalovaniaSound(megalovaniaBuffer);

sf::SoundBuffer gasterBuffer("audios/gaster.wav");
sf::Sound gasterSound(gasterBuffer);

bool isProcessRunning(const std::wstring& processName)
{
    bool found = false;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE)
        return false;

    PROCESSENTRY32W pe;
    pe.dwSize = sizeof(PROCESSENTRY32W);

    if (Process32FirstW(snapshot, &pe))
    {
        do
        {
            if (processName == pe.szExeFile)
            {
                found = true;
                break;
            }
        } while (Process32NextW(snapshot, &pe));
    }

    CloseHandle(snapshot);
    return found;
} // this was painful, thanks random guy on the internet <3

void animateSprite(int frameCount, int frameWidth, int frameHeight, float animationSpeed, sf::Time elapsedTime)
{ //makes it gifable
    int currentFrame = (int) (elapsedTime.asSeconds() / animationSpeed) % frameCount;
    dog.setTextureRect(sf::IntRect({ currentFrame * frameWidth, 0 }, { frameWidth, frameHeight }));
}

sf::Clock clocky, inactivityClock;

bool dragging = false;
sf::Vector2i dragOffset;

enum dogStateEnum {Dragged, dogSleep, EPIC, Idle, dogSpin};

dogStateEnum dogState = dogSleep;

bool isClosing = 0;
float fade = 255;

sf::Clock deltaClock;

int main()
{
    sf::RenderWindow window(sf::VideoMode({ 256, 384 }), "Annoying Dog Pet Thingy", sf::Style::None);
    startupSound.play();
    sf::Font font("sprites/deltarune.ttf");


    HWND hwnd = window.getNativeHandle();
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); //makes it on top of anything
    LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
    SetWindowLong(hwnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hwnd, RGB(0, 1, 0), 0, LWA_COLORKEY);

    //winapi can do magic lessgooooo
    
    //HMENU hPopupMenu = CreatePopupMenu();
    //AppendMenuW(hPopupMenu, MF_STRING, ID_MENU_CLOSE, L"Close");

    bool lastMouseStatePressed = 0;
    bool lastEpicState = 0;
    sf::Vector2i dragOffset;
    bool rightClickPrev = false;

    Menu menu;
    menu.items = 
    {
        { "Close", Close },
        { "S P I N", Spin},
        {"Feed", Feed}
    };

    bool rightPressedLastFrame = false;
    bool leftReleasedLastFrame = true;
    bool menuOpen = 0;


    while (window.isOpen())
    {
        sf::Time elapsed = clocky.getElapsedTime();
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            // Open menu on new right-click
            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right) && !rightPressedLastFrame) {
                menu.open(mousePos);
                menuOpen = 1;
            }
            rightPressedLastFrame = sf::Mouse::isButtonPressed(sf::Mouse::Button::Right);

            if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && !leftReleasedLastFrame && menu.isOpen) 
            {
                MenuAction action = menu.handleClick(mousePos);
                menu.close();
                inactivityClock.restart();
                switch (action)
                {
                case Close:
                    if (!isClosing)
                        isClosing = 1, gasterSound.play();
                    break; //compiler, pls shush, i added it
                case Spin:
                    dogState = dogSpin;
                    megalovaniaSound.play();
                    break;
                case Feed:
                    healSound.play();
                    break;
                }
            }
            leftReleasedLastFrame = !sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
            

            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && !menu.isOpen)
            {
                sf::Vector2i mousePos = sf::Mouse::getPosition();
                if (mousePos.x > window.getPosition().x && mousePos.x < window.getPosition().x + window.getSize().x
                    && mousePos.y>window.getPosition().y && mousePos.y < window.getPosition().y + window.getSize().y || dogState==Dragged)
                {
                    dogState = Dragged;
                    window.setPosition({ mousePos.x - 64, mousePos.y - 16 });
                    dog.setPosition({ 26, 20 });
                    lastMouseStatePressed = 1;
                    inactivityClock.restart();
                }
            }
            else if (lastMouseStatePressed)
            {
                dogState = Idle;
                dog.setPosition({ 0, 64 });
                lastMouseStatePressed = 0;
            }
        }

        float scaleY, desiredHeight, desiredWidth, scaleX;

        switch (dogState)
        {
        case Dragged:
            dog.setTexture(t_drag, true); //second arg must be true to reset the IntRect thingy
            desiredHeight = 128.f;
            scaleY = desiredHeight / t_drag.getSize().y;
            dog.setScale({ scaleY, scaleY });
            break;
        case dogSleep:
            dog.setTexture(t_sleep);
            animateSprite(2, 560, 260, 1, elapsed);
            desiredWidth = 128.f;
            scaleX = desiredWidth / 560;
            dog.setScale({ scaleX, scaleX });
            break;
        case EPIC:
            dog.setTexture(t_epic, true);
            animateSprite(28, 121, 128, 0.1, elapsed);
            dog.setScale({ 1, 1 });
            break;
        case Idle:
            dog.setTexture(t_idle, true);
            desiredWidth = 128.f;
            scaleX = desiredWidth / 1111; //about three halves of 741, the texture size
            dog.setScale({ scaleX, scaleX });
            break;
        case dogSpin:
            dog.setTexture(t_spin, true);
            animateSprite(4, 220, 220, 0.1, elapsed);
            desiredWidth = 128.f;
            scaleX = desiredWidth / 220;
            dog.setScale({ scaleX, scaleX });
        }

        if (inactivityClock.getElapsedTime().asSeconds() > 20)
            dogState = dogSleep;

        if (isProcessRunning(L"Undertale.exe") || isProcessRunning(L"DELTARUNE.exe"))
        {
            dogState = EPIC;
            lastEpicState = 1;
        }
        else if (lastEpicState)
        {
            lastEpicState = 0;
            dogState = dogSleep;
        }
        
        float deltaTime = deltaClock.restart().asSeconds(); //wowie??

        if (isClosing)
        {
            fade -= 100 * deltaTime;
            if (fade>=0)
                dog.setColor(sf::Color(255, 255, 255, fade));
            else
                window.close();
        }

        window.clear(sf::Color(0, 1, 0));
        window.draw(dog);
        if (menu.isOpen)
            menu.draw(window, font);
        window.display();
        SetCursor(LoadCursor(NULL, IDC_HAND)); //might be bad to call this every frame, but uhhhhhhhhhhhhhhhhhhhhh can't be THAT bad... right?
    }
}