#include "MapManager.h"
#include "Randomizer.h"

MapManager* MapManager::s_pInstance = 0;

#include <iostream>
#include "tinyxml2.h"
#include "Game.h"

using namespace std;

bool checkDocError (tinyxml2::XMLDocument &rDoc);



MapManager::MapManager()
{
    mBgData.clear(); // clear our background data vector
    mClipData.clear(); // clear our clip data vector

    tinyxml2::XMLDocument doc;
    doc.LoadFile( "assets/tileset2_map02.tmx" ); // open the map

    if(!checkDocError(doc))
    {
        return ;
    }

    tinyxml2::XMLElement* mapElement = doc.FirstChildElement( "map" );
    if (mapElement==0)
    {
        return;
    }

    mapElement->QueryIntAttribute( "width", &mMapWidth );
    mapElement->QueryIntAttribute( "height", &mMapHeight );

    // we need the clip tile first gid

    // first look for clip tileset  elemen t
    tinyxml2::XMLElement* nextTilesetElement = doc.FirstChildElement()->FirstChildElement( "tileset" );
    if (nextTilesetElement==0)
    {
        return;
    }

    // look for layer name "bg_layer_0". this is the art layer.
    string tilesetName;
    while (nextTilesetElement!=0)
    {
        // get layer's name attribute
        tilesetName = nextTilesetElement->Attribute("name");
        // make sure it matches our string if not..
        if (tilesetName!="collision_tileset")
        {
            // check the next sibling layer
            nextTilesetElement = nextTilesetElement->NextSiblingElement("tileset");
        }
        else
        {
            // if we did find it exit the while loop with a break while we are pointing at the matching layer.
            break;
        }
    }

    if (nextTilesetElement==0)
    {
        return;
    }
    // set clip first gid to what we got from xml...
    nextTilesetElement->QueryIntAttribute( "firstgid", &mClipFirstGID);






    // first look for bg layer
    tinyxml2::XMLElement* nextLayerElement = doc.FirstChildElement()->FirstChildElement( "layer" );
    if (nextLayerElement==0)
    {
        return;
    }

    // look for layer name "bg_layer_0". this is the art layer.
    string layerName;
    while (nextLayerElement!=0)
    {
        // get layer's name attribute
        layerName = nextLayerElement->Attribute("name");
        // make sure it matches our string if not..
        if (layerName!="bg_layer_0")
        {
            // check the next sibling layer
            nextLayerElement = nextLayerElement->NextSiblingElement("layer");
        }
        else
        {
            // if we did find it exit the while loop with a break while we are pointing at the matching layer.
            break;
        }
    }
    // if we didn't find the layer return.
    if (nextLayerElement==0)
    {
        return;
    }


    // jump into the data section
    tinyxml2::XMLElement* dataElement = nextLayerElement->FirstChildElement( "data" );

    // if we didn't find it return
    if (dataElement==0)
    {
        return;
    }

    // cycle through the data and fill our vector
    tinyxml2::XMLElement* nextElement = dataElement->FirstChildElement();
    int nextElementData;
    while (nextElement!=0)
    {

        nextElement->QueryIntAttribute( "gid", &nextElementData);
        mBgData.push_back(nextElementData);
        nextElement = nextElement->NextSiblingElement();

    }

    // first look for bg layer
    nextLayerElement = doc.FirstChildElement()->FirstChildElement( "layer" );
    if (nextLayerElement==0)
    {
        return;
    }

    // look for layer name "clip_layer_0". this is the art layer.
    while (nextLayerElement!=0)
    {
        // get layer's name attribute
        layerName = nextLayerElement->Attribute("name");
        // make sure it matches our string if not..
        if (layerName!="clip_layer_0")
        {
            // check the next sibling layer
            nextLayerElement = nextLayerElement->NextSiblingElement("layer");
        }
        else
        {
            // if we did find it exit the while loop with a break while we are pointing at the matching layer.
            break;
        }
    }
    // if we didn't find the layer return.
    if (nextLayerElement==0)
    {
        return;
    }

    // jump into the data section
    dataElement = nextLayerElement->FirstChildElement( "data" );

    // if we didn't find it return
    if (dataElement==0)
    {
        return;
    }

    // cycle through the data and fill our vector
    nextElement = dataElement->FirstChildElement();
    while (nextElement!=0)
    {

        nextElement->QueryIntAttribute( "gid", &nextElementData);
        mClipData.push_back(nextElementData);
        nextElement = nextElement->NextSiblingElement();

    }


    // set scale for big pixels!
    mSprite.setScale(2.f,2.f);
}

void MapManager::setTileset(sf::Texture *pTileset)
{
    mSprite.setTexture(*pTileset);

}

void    MapManager::update(sf::Time deltaTime)
{


}

int  MapManager::getClipAtScreenPosition(int x, int y)
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
        if (index >=0 && index < mClipData.size()) // is index in range? out data
        {
            mapTile = mClipData.at(index)-mClipFirstGID;
        }
    }

    return mapTile;
}

void     MapManager::render()
{
    int tileMapWidth=6;

    sf::IntRect rect = sf::IntRect(0,0,16,16);

    int xoffset=TheGame::Instance()->mCameraOffset.x;
    int yoffset=TheGame::Instance()->mCameraOffset.y;

    int rowoffset=yoffset/32;
    int coloffset=xoffset/32;

    int xfine=xoffset%32;
    int yfine=yoffset%32;

    int mapTile=0;
    int index=-1;

    int totalRows=2+int(TheGame::Instance()->getDisplayHeight())/32;
    int totalCols=1+int(TheGame::Instance()->getDisplayWidth())/32;

    for (int r=rowoffset; r<totalRows+rowoffset; r++)
    {
        for (int c=coloffset; c<totalCols+coloffset; c++)
        {

            if (r>=0&&r<mMapWidth&&c>=0&&c<mMapWidth) // if r and c are within map bounds
            {

                index=c+(r*mMapWidth);
                if (index >=0 && index < mBgData.size()) // is index in range? out data
                {

                    mapTile = mBgData.at(index)-(mBgFirstGID+1);
                    rect.top=16*int(mapTile/tileMapWidth);
                    rect.left=16*(mapTile%tileMapWidth);
                    mSprite.setTextureRect(rect);
                    mSprite.setPosition(-xfine+
                                        (c-coloffset)*32
                                        ,-yfine+(r-rowoffset)*32);


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
