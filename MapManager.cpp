#include "MapManager.h"
#include "Randomizer.h"

MapManager* MapManager::s_pInstance = 0;

#include <iostream>
#include "tinyxml2.h"
#include "Game.h"

bool checkDocError (tinyxml2::XMLDocument &rDoc)
{
    if (rDoc.ErrorID()!=0)
    {
        std::cout << "doc.ErrorID(): " << rDoc.ErrorID() << std::endl;
        return false;
    }
    return true;
}



MapManager::MapManager()
{
    mMapData.clear();

    tinyxml2::XMLDocument doc;
    doc.LoadFile( "assets/tileset2_map01.tmx" );

    if(!checkDocError(doc))
    {
        return ;
    }

    tinyxml2::XMLElement* mapElement = doc.FirstChildElement( "map" );


    mapElement->QueryIntAttribute( "width", &mMapWidth );
    mapElement->QueryIntAttribute( "height", &mMapHeight );


    tinyxml2::XMLElement* dataElement = doc.FirstChildElement()->FirstChildElement( "layer" )->FirstChildElement( "data" );
    if(!checkDocError(doc))
    {
        return;
    }

    tinyxml2::XMLElement* nextElement = dataElement->FirstChildElement();
    int nextElementData;

    while (nextElement!=0)
    {

        nextElement->QueryIntAttribute( "gid", &nextElementData);
        mMapData.push_back(nextElementData);
        nextElement = nextElement->NextSiblingElement();

    }

    mSprite.setScale(2.f,2.f);
}

void MapManager::setTileset(sf::Texture *pTileset)
{
    mSprite.setTexture(*pTileset);

}

void    MapManager::update(sf::Time deltaTime)
{


}

int  MapManager::getTileTypeAtScreenPosition(int x, int y)
{
    int rowoffset=0;
    int coloffset=0;

    int r=rowoffset+(y/32);
    int c=coloffset+(x/32);

    int index=c+(r*mMapWidth);

    int mapTile=-1;

    if (r>=0&&r<mMapWidth&&c>=0&&c<mMapWidth) // if r and c are within map bounds
    {

        index=c+(r*mMapWidth);
        if (index >=0 && index < mMapData.size()) // is index in range? out data
        {
            mapTile = mMapData.at(index)-1;
        }
    }
    return mapTile;
}

void     MapManager::render()
{
    int tileMapWidth=6;

    sf::IntRect rect = sf::IntRect(0,0,16,16);

    int rowoffset=0;
    int coloffset=0;

    int mapTile=0;
    int index=-1;

    for (int r=rowoffset; r<15+rowoffset; r++)
    {
        for (int c=coloffset; c<20+coloffset; c++)
        {

            if (r>=0&&r<mMapWidth&&c>=0&&c<mMapWidth) // if r and c are within map bounds
            {

                index=c+(r*mMapWidth);
                if (index >=0 && index < mMapData.size()) // is index in range? out data
                {

                    mapTile = mMapData.at(index)-1;
                    rect.top=16*int(mapTile/tileMapWidth);
                    rect.left=16*(mapTile%tileMapWidth);
                    mSprite.setTextureRect(rect);
                    mSprite.setPosition((c-coloffset)*32.f,(r-rowoffset)*32.f);
                    TheGame::Instance()->getRenderTexture()->draw(mSprite);
                }
                else
                {
                    // do nothing
                }

            }
            else
            {
                // do n0thing
            }
        }
    }


}
