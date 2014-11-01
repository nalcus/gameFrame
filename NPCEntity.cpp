#include "NPCEntity.h"
#include "Randomizer.h"
#include "Game.h"
#include "MapManager.h"
#include <iostream>

using namespace std;

NPCEntity::NPCEntity(sf::Texture* pTexture)
    : mFacing(LEFT)
    , mFrame(0)
    , mFramesUntilNextFrame(0)
    , mFramesUntilAction(0)
    ,mZ(0)
    ,mPosition(getRandomInt()%640,
               getRandomInt()%48)
    ,mVelocity(0.f,0.f)
{

    mFramesUntilAction = getRandomInt()%600;
    mFramesUntilNextFrame= getRandomInt()%600;
    mFacing=getRandomInt()%2;

    mSprite.setTexture(*pTexture);

    mSprite.setColor(sf::Color(
                         128+getRandomInt()%127
                         ,128+getRandomInt()%127
                         ,128+getRandomInt()%127
                     ));
    mSprite.setScale(2.f,2.f);
    mFalling=false;

}


void NPCEntity::update(sf::Time deltaTime)
{
    //mFrameTime+=deltaTime;

    //mSprite.move(100.f*deltaTime.asSeconds(),0.f);

    //if framesuntil action is up do

    int tileUnderfoot=TheMapManager::Instance()->getTileTypeAtScreenPosition(mPosition.x, mPosition.y);

    // get the tile under foot.

    int r=mPosition.y/32;
    int c=mPosition.x/32;
    int rOffset=int(mPosition.y)%32;
    int cOffset=int(mPosition.x)%32;

    //cout << "rOffset=" << rOffset << endl;
    //cout << "cOffset=" << cOffset << endl;
    //cout << "Tile="  << m << endl;

    if (--mFramesUntilAction<1)
    {
        mFramesUntilAction = getRandomInt()%600;
        mFacing = getRandomInt()%2;

    }

    // is the origin on a vertically bordering block?
    int vl=480*2; // default vertical limit is offscreen
    int hl=(mFacing==LEFT) ? 0:639; // either left or right of screen


    // set the border for right facing situations
    if (mFacing==RIGHT)
    {

        // don't walk off the edge of a tile 9 or tile 23. or through a 19.
        switch (tileUnderfoot)
        {
        case 9:
        case 23:
        case 19:
            hl=c*32+16;
            break;

        }
    }
    else if (mFacing==LEFT)
    {
        // look to see if we have tile 21 to the left. if so we can't move past it
        if (TheMapManager::Instance()->
                getTileTypeAtScreenPosition(
                    mPosition.x-32
                    , mPosition.y)==21)
        {
            hl=mPosition.x-cOffset;
        }

        // things the border halfway
        switch (tileUnderfoot)
        {
        case 7:
        case 11:
        case 18:
            hl=c*32+16;
            break;

        }
    }


    // these block types are vertically supporting
    switch (tileUnderfoot)
    {
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 11:
    case 18:
    case 19:
    case 22:
    case 23:
        vl=r*32+16;
    }



    // if your y position if lesser than your current vertical limit then you are off the ground are are falling.
    if (mPosition.y<vl)
    {
        mFalling=true;
    }
    else
    {
        mPosition.y=vl;
        mFalling=false;
    }

    if (mFramesUntilAction>120&&!mFalling)
    {
        switch (mFacing)
        {
        case LEFT:
            mVelocity.x=-50.f*deltaTime.asSeconds();
            mVelocity.y=0;
            break;
        case RIGHT:
            mVelocity.x=50.f*deltaTime.asSeconds();
            mVelocity.y=0;
            break;
        }
    }
    else
    {
        mVelocity.x=0;

    }

    if (mFalling)
    {
        mVelocity.y+=8.9*deltaTime.asSeconds();
    }


    // switch frames every half second
    if (--mFramesUntilNextFrame<1)
    {
        mFramesUntilNextFrame=30+getRandomInt()%5;
        mFrame++;
        if (mFrame>1)
        {
            mFrame=0;
        }
    }

    //locomote!!
    mPosition+=mVelocity;

    // correct horrizontal border crossing
    if ((mFacing==LEFT)? mPosition.x<hl:mPosition.x>hl)
    {
        mPosition.x=hl;
        mFacing= (mFacing==LEFT) ? RIGHT : LEFT;
    }

    // correct if you are just below the ground for some reason
    if (mPosition.y>vl)
    {
        mPosition.y=vl;
    }


    mZ = int(mPosition.y); // fixed z=y since we fixed our origin offsets :)




}

void NPCEntity::render()
{
    int x=0, y=0, w=0, h=0, oX=0, oY=0; // these we'll eventually read from xml
    int spriteOffsetX=0, spriteOffsetY=-28;

    if (mFacing==LEFT)
    {
        spriteOffsetX = -16;
        if (mFrame==0)
        {
            x=2;
            y=2;
            w=20;
            h=30; // these we'll eventually read from xml
            oX=3, oY=0;

        }
        else if (mFrame==1)
        {
            x=2;
            y=34;
            w=22;
            h=29;
            oX=0, oY=1;

        }

    }

    else if (mFacing==RIGHT)
    {
        spriteOffsetX=-4;
        if (mFrame==0)
        {
            x=2+20;
            y=2;
            w=-20;
            h=30;
            oX=-3+2, oY=0;

        }
        else if (mFrame==1)
        {
            x=2+22;
            y=34;
            w=-22;
            h=29;
            oX=0, oY=1;
        }


    }

    sf::IntRect rect = sf::IntRect(x,y,w,h);
    mSprite.setTextureRect(rect);

    mSprite.setPosition(spriteOffsetX*2+mPosition.x+ oX*2, spriteOffsetY*2+mPosition.y+oY*2);

    TheGame::Instance()->getRenderTexture()->draw(mSprite);
    TheGame::Instance()->drawMarker(mPosition.x, mPosition.y);

}
