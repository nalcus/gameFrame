
#include "PlayerEntity.h"
#include "Randomizer.h"
#include "Game.h"
#include "MapManager.h"
#include <iostream>
#include "frameData.h"

using namespace std;

PlayerEntity::PlayerEntity(sf::Texture* pTexture)
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
    mFramesUntilNextFrame= getRandomInt()%60;
    mFacing=getRandomInt()%2;

    mSprite.setTexture(*pTexture);

    mSprite.setColor(sf::Color(
                         128+getRandomInt()%127
                         ,128+getRandomInt()%127
                         ,128+getRandomInt()%127
                     ));
    mSprite.setScale(2.f,2.f);
    mFalling=false;


    // read different frame values from file.


    mFramesData.clear();

    tinyxml2::XMLDocument doc;
    doc.LoadFile( "assets/hoodie_spritesheet.xml" ); // open the map

    if(!checkDocError(doc))
    {
        return ;
    }

    tinyxml2::XMLElement* mapElement = doc.FirstChildElement( "TextureAtlas" );
    if (mapElement==0)
    {
        std::cout << "textureatlas not found!" << std::endl;
        return;
    }

    tinyxml2::XMLElement* nextElement = mapElement->FirstChildElement("sprite");
    if (mapElement==0)
    {
        std::cout << "sprite not found!" << std::endl;
        return;
    }

    std::string n;
    int x, y, w, h, oX, oY, oW, oH, mX;
    float pX, pY;
    while (nextElement!=0)
    {
        n = nextElement->Attribute("n");
        nextElement->QueryIntAttribute( "x", &x);
        nextElement->QueryIntAttribute( "y", &y);
        nextElement->QueryIntAttribute( "w", &w);
        nextElement->QueryIntAttribute( "h", &h);
        nextElement->QueryFloatAttribute("pX", &pX);
        nextElement->QueryFloatAttribute("pY", &pY);
        nextElement->QueryIntAttribute( "oX", &oX);
        nextElement->QueryIntAttribute( "oY", &oY);
        nextElement->QueryIntAttribute( "oW", &oW);
        nextElement->QueryIntAttribute( "oH", &oH);
        nextElement->QueryIntAttribute( "mX", &mX);
        mFramesData.push_back(frameData(n,x,y,w,h,pX,pY,oX,oY,oW,oH,mX));
        nextElement = nextElement->NextSiblingElement();
    }
}


void PlayerEntity::update(sf::Time deltaTime)
{
    //mFrameTime+=deltaTime;

    //mSprite.move(100.f*deltaTime.asSeconds(),0.f);

    //if framesuntil action is up do

    int tileUnderfoot=TheMapManager::Instance()->getClipAtScreenPosition(mPosition.x, mPosition.y);

    // get the tile under foot.

    int r=mPosition.y/32;
    int c=mPosition.x/32;
    int rOffset=int(mPosition.y)%32;
    int cOffset=int(mPosition.x)%32;

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

        // don't walk off the edge of a tile
        switch (tileUnderfoot)
        {
        case 3:
        case 5:
        case 6:
            hl=c*32+16;
            break;

        }
    }
    else if (mFacing==LEFT)
    {
        // look to see if we have solid tile to the left
        if (TheMapManager::Instance()->
                getClipAtScreenPosition(
                    mPosition.x-32
                    , mPosition.y)==1)
        {
            hl=mPosition.x-cOffset;
        }

        // things the border halfway
        switch (tileUnderfoot)
        {
        case 4:
        case 7:
        case 8:
            hl=c*32+16;
            break;

        }
    }


    // these block types are vertically supporting
    switch (tileUnderfoot)
    {
    case 2:
    case 3:
    case 4:
    case 5:
    case 8:

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
        mFrame=3;
    }
    else if (!mFalling&&mFrame==3)
    {
        mFrame=12;
    }



    // switch running frames every quarter second
    if (--mFramesUntilNextFrame<1&&!mFalling)
    {
        // running frames
        mFramesUntilNextFrame=15+getRandomInt()%5;
        mFrame++;
        if (mFrame>2)
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

void PlayerEntity::render()
{
    int x=0, y=0, w=0, h=0, oX=0, oY=0; // these we'll eventually read from xml
    int spriteOffsetX=0, spriteOffsetY=-28;

    if (mFacing==RIGHT)
    {
        spriteOffsetX = -24;

        x=mFramesData.at(mFrame).x;
        y=mFramesData.at(mFrame).y;
        w=mFramesData.at(mFrame).w;
        h=mFramesData.at(mFrame).h;
        oX=mFramesData.at(mFrame).oX;
        oY=mFramesData.at(mFrame).oY;
    }

    else if (mFacing==LEFT)
    {
        spriteOffsetX=10;

        x=mFramesData.at(mFrame).x+mFramesData.at(mFrame).w;
        y=mFramesData.at(mFrame).y;
        w=0-mFramesData.at(mFrame).w;
        h=mFramesData.at(mFrame).h;
        oX=-mFramesData.at(mFrame).oX+mFramesData.at(mFrame).mX;
        oY=mFramesData.at(mFrame).oY;
    }

    sf::IntRect rect = sf::IntRect(x,y,w,h);
    mSprite.setTextureRect(rect);

    mSprite.setPosition(spriteOffsetX*2+mPosition.x+ oX*2, spriteOffsetY*2+mPosition.y+oY*2);

    TheGame::Instance()->getRenderTexture()->draw(mSprite);
    TheGame::Instance()->drawMarker(mPosition.x, mPosition.y);

}
