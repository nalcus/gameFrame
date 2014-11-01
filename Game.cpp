#include "Game.h"
#include "Entity.h"
#include "NPCEntity.h"
#include "EntityManager.h"
#include "StringHelp.h"
#include "MapManager.h"

#include <SFML/Graphics.hpp>
#include <iostream>
#include "Randomizer.h"

const float Game::PlayerSpeed = 100.f;
const sf::Time Game::TimePerFrame = sf::seconds(1.f/60.f);

Game* Game::s_pInstance = 0;

Game::Game()
: mWindow(sf::VideoMode(640, 480), "gameFrame")
, mTexture()
, mTileset()
, mMarker ()
, mMarkerSprite()
, mFont()
, mStatisticsText()
, mStatisticsUpdateTime()
, mStatisticsNumFrames(0)
{

    reseedRandomizer();

    mRenderTexture.create(640, 480);
    mRenderSprite.setTexture(mRenderTexture.getTexture());

    if (!mTexture.loadFromFile("assets/bum1_spritesheets.png"))
    {
        std::cout << "didn't load file bum1" << std::endl;
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
	mStatisticsText.setCharacterSize(16);

    int numberOfNPCs = 10;

    for (int i=0; i<numberOfNPCs;i++)
    {
        NPCEntity *tempent=new NPCEntity(&mTexture);
        TheEntityManager::Instance()->pushEntity(tempent);
    }

}

void Game::processEvents()
{
      sf::Event event;
        while (mWindow.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                mWindow.close();
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

