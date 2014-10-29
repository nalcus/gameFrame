#include "NPCEntity.h"
#include "Randomizer.h"
#include "Game.h"

NPCEntity::NPCEntity(sf::Texture* pTexture)
    : mRow(4)
    , mCol(4)
    , mFacing(SOUTH)
    , mFrame(0)
    , mFramesUntilNextFrame(0)
    , mFramesUntilAction(0)
    ,mZ(0)
{

    mFramesUntilAction = TheRandomizer::Instance()->getRandomInt()%600;
    mFramesUntilNextFrame= TheRandomizer::Instance()->getRandomInt()%600;
    mFacing=TheRandomizer::Instance()->getRandomInt()%4;
    mRow=TheRandomizer::Instance()->getRandomInt()%6;
    mCol=TheRandomizer::Instance()->getRandomInt()%4;
    mSprite.setPosition(TheRandomizer::Instance()->getRandomInt()%640,
                        TheRandomizer::Instance()->getRandomInt()%480);
    mSprite.setTexture(*pTexture);

    mSprite.setColor(sf::Color(
                         128+TheRandomizer::Instance()->getRandomInt()%127
                         ,128+TheRandomizer::Instance()->getRandomInt()%127
                         ,128+TheRandomizer::Instance()->getRandomInt()%127
                     ));

}


void NPCEntity::update(sf::Time deltaTime)
{
    //mFrameTime+=deltaTime;

    //mSprite.move(100.f*deltaTime.asSeconds(),0.f);

    //if framesuntil action is up do
    if (--mFramesUntilAction<1)
    {
        mFramesUntilAction = TheRandomizer::Instance()->getRandomInt()%600;
        mFacing = TheRandomizer::Instance()->getRandomInt()%4;
        if (mSprite.getPosition().y<-32.f)
        {
            mFacing=0;
        }
        else if (mSprite.getPosition().y>480.f)
        {
            mFacing=1;
        }
        if (mSprite.getPosition().x<-32.f)
        {
            mFacing=3;
        }
        else if (mSprite.getPosition().x>640.f)
        {
            mFacing=2;
        }
    }

    if (mFramesUntilAction>120)
    {
        switch (mFacing)
        {
        case 0:
            mSprite.move(0.f,50.f*deltaTime.asSeconds());
            break;
        case 1:
            mSprite.move(0.f,-50.f*deltaTime.asSeconds());
            break;
        case 2:
            mSprite.move(-50.f*deltaTime.asSeconds(),0.f);
            break;
        case 3:
            mSprite.move(50.f*deltaTime.asSeconds(),0.f);
            break;
        }
    }


    // switch frames every half second
    if (--mFramesUntilNextFrame<1)
    {
        mFramesUntilNextFrame=30+TheRandomizer::Instance()->getRandomInt()%5;
        mFrame++;
        if (mFrame>1)
        {
            mFrame=0;
        }
    }

    mZ = int(mSprite.getPosition().y)+32;

}

void NPCEntity::render()
{
    int x=0, y=0;

    if (mFacing==SOUTH)
    {
        if (mFrame==0)
        {
            x=mCol*32*4;
            y=mRow*32;
            sf::IntRect rect = sf::IntRect(x,y,32,32);
            mSprite.setTextureRect(rect);

        }
        else if (mFrame==1)
        {
            x=mCol*32*4;
            y=mRow*32;
            sf::IntRect rect = sf::IntRect(x+32,y,-32,32);
            mSprite.setTextureRect(rect);

        }

    }

    else if (mFacing==NORTH)
    {
        if (mFrame==0)
        {
            x=32+mCol*32*4;
            y=mRow*32;
            sf::IntRect rect = sf::IntRect(x,y,32,32);
            mSprite.setTextureRect(rect);

        }
        else if (mFrame==1)
        {
            x=32+mCol*32*4;
            y=mRow*32;
            sf::IntRect rect = sf::IntRect(x+32,y,-32,32);
            mSprite.setTextureRect(rect);
        }


    }

    else if (mFacing==WEST)
    {
        if (mFrame==0)
        {
            x=64+mCol*32*4;
            y=mRow*32;
            sf::IntRect rect = sf::IntRect(x,y,32,32);
            mSprite.setTextureRect(rect);
        }

        else if (mFrame==1)
        {
            x=96+mCol*32*4;
            y=mRow*32;
            sf::IntRect rect = sf::IntRect(x,y,32,32);
            mSprite.setTextureRect(rect);

        }

    }

    else if (mFacing==EAST)
    {
        if (mFrame==0)
        {
            x=64+mCol*32*4;
            y=mRow*32;
            sf::IntRect rect = sf::IntRect(x+32,y,-32,32);
            mSprite.setTextureRect(rect);

        }
        else if (mFrame==1)
        {
            x=96+mCol*32*4;
            y=mRow*32;
            sf::IntRect rect = sf::IntRect(x+32,y,-32,32);
            mSprite.setTextureRect(rect);

        }

    }

    TheGame::Instance()->getRenderTexture()->draw(mSprite);

}
