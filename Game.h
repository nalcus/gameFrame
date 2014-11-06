#ifndef __Game__
#define __Game__

#include <SFML/Graphics.hpp>
#include "tinyxml2.h"

#include "Entity.h"

bool checkDocError (tinyxml2::XMLDocument &rDoc);

class Game
{
public:

    static Game * Instance()
    {
        if (s_pInstance==0)
        {
            s_pInstance=new Game();
            return s_pInstance;
        }
        return s_pInstance;
    }
    void run();
    void processEvents();
    void update(sf::Time deltaTime);
    void render();
    void updateStatistics(sf::Time elapsedTime);

    sf::RenderWindow * getWindow()
    {
        return &mWindow;
    }
    sf::RenderTexture * getRenderTexture()
    {
        return &mRenderTexture;
    }

    void applyShader(sf::RenderStates);

    void drawMarker(int x, int y);

    int getDisplayWidth() {return DisplayWidth;}
    int getDisplayHeight() {return DisplayHeight;}
    int getFullScreen() {return Fullscreen;}

     sf::Vector2f mCameraOffset;

private:
    Game();
    static const float		PlayerSpeed;
    static const sf::Time	TimePerFrame;
    static const int DisplayWidth;
    static const int DisplayHeight;
    static const bool Fullscreen;
    static const bool VSync;

    sf::RenderWindow mWindow;
    sf::RenderTexture mRenderTexture;
    sf::Sprite mRenderSprite;
    sf::Sprite mMarkerSprite;

    sf::Texture mNPCTexture;
    sf::Texture mPlayerTexture;
    sf::Texture mTileset;
    sf::Texture mMarker;




    static Game* s_pInstance;

    sf::Font				mFont;
    sf::Text				mStatisticsText;
    sf::Time				mStatisticsUpdateTime;

    std::size_t				mStatisticsNumFrames;
};

typedef Game TheGame;

#endif // __Game__
