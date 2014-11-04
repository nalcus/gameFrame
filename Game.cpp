#include "Game.h"
#include "Entity.h"
#include "NPCEntity.h"
#include "EntityManager.h"
#include "StringHelp.h"
#include "MapManager.h"
#include "PlayerEntity.h"

#include <SFML/Graphics.hpp>
#include <iostream>
#include "Randomizer.h"

const float Game::PlayerSpeed = 100.f;
const sf::Time Game::TimePerFrame = sf::seconds(1.f/60.f);

const int Game::DisplayWidth = 800;
const int Game::DisplayHeight = 600;
const bool Game::Fullscreen = false;

Game* Game::s_pInstance = 0;

Game::Game()
    : mWindow(sf::VideoMode(DisplayWidth, DisplayHeight), "gameFrame", (Fullscreen)?sf::Style::Fullscreen:sf::Style::Close)
    , mNPCTexture()
    , mPlayerTexture()
    , mTileset()
    , mMarker ()
    , mMarkerSprite()
    , mFont()
    , mStatisticsText()
    , mStatisticsUpdateTime()
    , mStatisticsNumFrames(0)
{

    reseedRandomizer();

    mRenderTexture.create(DisplayWidth, DisplayHeight);
    mRenderSprite.setTexture(mRenderTexture.getTexture());

    if (!mNPCTexture.loadFromFile("assets/bum1_spritesheets.png"))
    {
        std::cout << "didn't load file bum1" << std::endl;
    }

    if (!mPlayerTexture.loadFromFile("assets/hoodie_spritesheet.png"))
    {
        std::cout << "didn't load file hoodie" << std::endl;
    }

    if (!mTileset.loadFromFile("assets/tileset2.png"))
    {
        std::cout << "didn't load file tileset2" << std::endl;
    }

    if (!mMarker.loadFromFile("assets/marker.png"))
    {
        std::cout << "didn't load file marker" << std::endl;
    }

    mMarkerSprite.setTexture(mMarker);


    TheMapManager::Instance()->setTileset(&mTileset);

    mFont.loadFromFile("assets/04B_25__.TTF");
    mStatisticsText.setFont(mFont);
    mStatisticsText.setColor(sf::Color::White);
    mStatisticsText.setPosition(5.f, 5.f);
    mStatisticsText.setCharacterSize(20);

    // add NPCs

    int numberOfNPCs = 10;

    for (int i=0; i<numberOfNPCs; i++)
    {
        NPCEntity *tempent=new NPCEntity(&mNPCTexture);
        TheEntityManager::Instance()->pushEntity(tempent);
    }

    // add Player

    PlayerEntity *tempent=new PlayerEntity(&mPlayerTexture);
    TheEntityManager::Instance()->pushEntity(tempent);

}

void Game::processEvents()
{
    sf::Event event;
    while (mWindow.pollEvent(event))
    {
        if (event.type== sf::Event::Closed)
        {

            mWindow.close();
        }


        if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.code == sf::Keyboard::Escape)
            {
                mWindow.close();
            }


        }
    }
}

void Game::run()
{
    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;
    while (mWindow.isOpen())
    {
        // clock.restart() rests the clock and returns the time since last restart
        sf::Time elapsedTime = clock.restart();
        timeSinceLastUpdate += elapsedTime;
        while (timeSinceLastUpdate > TimePerFrame)
        {
            timeSinceLastUpdate -= TimePerFrame;
            processEvents();
            update(TimePerFrame);
        }
        updateStatistics(elapsedTime);
        render();

    }

}

void Game::updateStatistics(sf::Time elapsedTime)
{
    mStatisticsUpdateTime += elapsedTime;
    mStatisticsNumFrames += 1;

    if (mStatisticsUpdateTime >= sf::seconds(1.0f))
    {
        mStatisticsText.setString(
            "Frames / Second = " + toString(mStatisticsNumFrames) + "\n" +
            "Time / Update = " + toString(mStatisticsUpdateTime.asMicroseconds() / mStatisticsNumFrames) + "us");

        mStatisticsUpdateTime -= sf::seconds(1.0f);
        mStatisticsNumFrames = 0;
    }
}

void Game::update(sf::Time deltaTime)
{
    TheEntityManager::Instance()->update(deltaTime);
}

void Game::render()
{
    mRenderTexture.clear(sf::Color::White);
    TheMapManager::Instance()->render();
    TheEntityManager::Instance()->render();
    mRenderTexture.draw(mStatisticsText);
    mRenderTexture.display();
    mRenderSprite.setScale(1.f,1.f);
    mWindow.draw(mRenderSprite);
    mWindow.display();


}

void Game::drawMarker(int x, int y)
{
    mMarkerSprite.setPosition(x-4,y-4);
    mRenderTexture.draw(mMarkerSprite);
}

void Game::applyShader(sf::RenderStates states)

{

    mRenderTexture.draw(mRenderSprite, states); // We apply our shader on the current render

}

