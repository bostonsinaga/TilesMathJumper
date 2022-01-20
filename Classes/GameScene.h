#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include "Tool.h"

class GameScene : public cocos2d::LayerGradient {
    
    public:

        // there's no 'id' in cpp, so we recommend returning the class instance pointer
        static cocos2d::Scene* createScene();

        // here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
        virtual bool init();
        
        // implement the "static create()" method manually
        CREATE_FUNC(GameScene);

    private:

        ////// METHODS //////

        //// main ////

        void touching();
        void delayTouch(float dt);
        void set_isTouchable(float dt);
        void onKeyReleasing();

        // blocky //

        void drawBlocky(bool isHide);
        void drawBlockyBarrier();
        void playerJumpSound();

        // tiles //

        cocos2d::Color4F changeExceptionColor(cocos2d::Color4F exceptionColor);
        void spawnTiles(float dt);
        void moveTile(cocos2d::Node *node, int posTag, bool isNormalTile);
        void tileBackToOrigin(cocos2d::Node *node, bool isNormalTile);
        void standOnTileDetection(float dt);
        void compareEvent(int tag, cocos2d::Node *theTile, cocos2d::Label *theTileLabel);
        void showEventSign(bool isTrue);
        int getTileScore(cocos2d::Label *label);

        // GUI //

        void generateDarkScreen();
        void updateScoreBar(int score_in, bool isInitialization = false, bool isColorError = false);
        void gameTitleAction();
        void gameTitleRotate(float dt);
        void inActionNewHighScoreNotice();
        void gameOverAction();

        // scenes //

        void startGame();
        void goToGameScene(float dt);
        cocos2d::RenderTexture *getRenTex();

        // tutorial needs //

        void checkingTutorialNeeds();
        void tut_tut1(float dt);
        void tut_tut2(float dt);

        ////// OBJECTS //////

        static const int
            tilesRowsCount = 5,
            colorMembersCount = 7,
            eventTilesCount = 7;

        cocos2d::Node *player;
        
        cocos2d::DrawNode
            *blocky,
            *blockyBarrier,
            *scoreBar,
            *highScoreHomeTitleBar,
            *darkDot,
            *darkScreen,
            *playButton;

        cocos2d::Sprite
            *tile[tilesRowsCount][3],
            *tileMultiColor[3],
            *eventSign;

        cocos2d::Label
            *tileLabel[tilesRowsCount][3],
            *scoreLabel,
            *highScoreHomeTitleLabel,
            *gameTitle,
            *eventLabel;

        cocos2d::Color4F currentColor, nextColor, colorGroup[colorMembersCount] = {

            Color4F::RED, Color4F::GREEN, Color4F::BLUE, Color4F::YELLOW,
            Color4F::ORANGE, Color4F::MAGENTA, Color4F(0.0, 1.0, 1.0, 1.0)
        };

        cocos2d::Vec2 onWayPos[3], scoreBarPos;
        cocos2d::Size waySize;

        Tool tool;

        ////// VARIABLES //////

        int 
            currentOnWayPos = 1,
            currentTilesRow = 0,
            currentBlockyColorsSort = 0,
            currentScore = 0,
            tileEventCounter = 3,
            eventTileTag = 0,
            comparisonCounter = 0,
            comparisonBuffer,
            currentEventTile = 0,
            answerCalc,
            blockyColorsSort[colorMembersCount] = {0, 4, 3, 1, 2, 6, 5};
        
        const int maxScore = 100;
        const float tileActionDT = 3.5, tileActionSpawnDT = 1.5;

        bool
            isTouchable = false,
            isJump = false,
            isGameStarted = false,
            isReturnable = false,
            isNewHighScoreNoticed = false; 

        cocos2d::Vec2 touchBegan, touchEnded;
};

#endif // __GAME_SCENE_H__