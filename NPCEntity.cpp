#include "NPCEntity.h"
#include "Randomizer.h"
#include "Game.h"
#include "MapManager.h"
#include <iostream>
#include "frameData.h"

using namespace std;

NPCEntity::NPCEntity(sf::Texture* pTexture)
    : mFacing(LEFT)
    , mFrame(0)
    , mFramesUntilNextAnimationFrame(0)
    , mFramesUntilAction(0)

{
    mType = "ENEMY";
        // enemmy has no parent entity
    mParentEntity=0;
        // we're alive!!
    mActive=true;
    mAge=0;

    mSize=sf::Vector2f(32.f,48.f);


    mPosition.x=80+getRandomInt()%2800;
    mPosition.y=-1*getRandomInt()%100;

    mVelocity.x=0.f;
    mVelocity.y=0.f;

    mFramesUntilAction = getRandomInt()%600;
    mFramesUntilNextAnimationFrame= getRandomInt()%60;

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

    //readFramesDataFromFile( "assets/bum1_spritesheet.xml");
    mFramesData.clear();

    tinyxml2::XMLDocument doc;
    doc.LoadFile( "assets/bum1_spritesheet.xml" ); // open the map

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


void NPCEntity::update(sf::Time deltaTime)
{

    // get the tile under foot.
    int tileUnderfoot=TheMapManager::Instance()->getClipAtScreenPosition(mPosition.x, mPosition.y);




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
    int vl=256+TheMapManager::Instance()->getMapHeight()*32; // default vertical limit is offscreen
    int hl=(mFacing==LEFT) ? 0:-1+TheMapManager::Instance()->getMapWidth()*32; // either left or right of screen


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
    }


    // switch frames every half second
    if (--mFramesUntilNextAnimationFrame<1)
    {
        mFramesUntilNextAnimationFrame=30+getRandomInt()%5;
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
            x=mFramesData.at(0).x;
            y=mFramesData.at(0).y;
            w=mFramesData.at(0).w;
            h=mFramesData.at(0).h;
            oX=mFramesData.at(0).oX;
            oY=mFramesData.at(0).oY;

        }
        else if (mFrame==1)
        {
            x=mFramesData.at(1).x;
            y=mFramesData.at(1).y;
            w=mFramesData.at(1).w;
            h=mFramesData.at(1).h;
            oX=mFramesData.at(1).oX;
            oY=mFramesData.at(1).oY;

        }

    }

    else if (mFacing==RIGHT)
    {
        spriteOffsetX=-4;
        if (mFrame==0)
        {
            x=mFramesData.at(0).x+mFramesData.at(0).w;
            y=mFramesData.at(0).y;
            w=0-mFramesData.at(0).w;
            h=mFramesData.at(0).h;
            oX=-mFramesData.at(0).oX+mFramesData.at(0).mX;
            oY=mFramesData.at(0).oY;



        }
        else if (mFrame==1)
        {
            x=mFramesData.at(1).x+mFramesData.at(1).w;
            y=mFramesData.at(1).y;
            w=0-mFramesData.at(1).w;
            h=mFramesData.at(1).h;
            oX=-mFramesData.at(1).oX+mFramesData.at(1).mX;
            oY=mFramesData.at(1).oY;
        }


    }

    sf::IntRect rect = sf::IntRect(x,y,w,h);
    mSprite.setTextureRect(rect);

    mSprite.setPosition(-TheGame::Instance()->mCameraOffset.x+spriteOffsetX*2+mPosition.x+oX*2,
                        -TheGame::Instance()->mCameraOffset.y+spriteOffsetY*2+mPosition.y+oY*2);

    TheGame::Instance()->getRenderTexture()->draw(mSprite);



}

void NPCEntity::receiveCollision(Entity* SourceEntity)
{
    std::string Pr="PROJECTILE";

    if (*SourceEntity->getType()==Pr)
    {
       mActive=false;
    }
}
