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
	doc.LoadFile( "assets/map1.tmx" );

    if(!checkDocError(doc)) {return ;}

    tinyxml2::XMLElement* mapElement = doc.FirstChildElement( "map" );


    mapElement->QueryIntAttribute( "width", &mMapWidth );
    mapElement->QueryIntAttribute( "height", &mMapHeight );


    tinyxml2::XMLElement* dataElement = doc.FirstChildElement()->FirstChildElement( "layer" )->FirstChildElement( "data" );
      if(!checkDocError(doc)) {return;}

    tinyxml2::XMLElement* nextElement = dataElement->FirstChildElement();
     int nextElementData;

     while (nextElement!=0)
     {

        nextElement->QueryIntAttribute( "gid", &nextElementData);
        mMapData.push_back(nextElementData);
        nextElement = nextElement->NextSiblingElement();

     }
}

void MapManager::setTileset(sf::Texture *pTileset)
{
    mSprite.setTexture(*pTileset);

}

void    MapManager::update(sf::Time deltaTime)
    {


    }

void     MapManager::render()
    {
        int tileMapWidth=16;

        sf::IntRect rect = sf::IntRect(0,0,32,32);

        int rowoffset=1;
        int coloffset=1;

        for (int r=rowoffset;r<15+rowoffset;r++)
        {
            for (int c=coloffset;c<20+coloffset;c++)
            {

                    if (r>0&&r<mMapWidth&&c>0&&c<mMapWidth)
                    {


                        int mapTile = mMapData.at(c+(r*mMapWidth))-1;
                        rect.top=32*int(mapTile/tileMapWidth);
                        rect.left=32*(mapTile%tileMapWidth);
                        mSprite.setTextureRect(rect);
                        mSprite.setPosition((c-coloffset)*32.f,(r-rowoffset)*32.f);
                        TheGame::Instance()->getRenderTexture()->draw(mSprite);
                    }
            }
        }


    }
