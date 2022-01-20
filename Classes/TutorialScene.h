#ifndef __TUTORIAL_SCENE_H__
#define __TUTORIAL_SCENE_H__

#include "cocos2d.h"
#include "Tool.h"

class TutorialScene : public cocos2d::Layer {
    
    public:

        // there's no 'id' in cpp, so we recommend returning the class instance pointer
        static cocos2d::Scene* createScene(cocos2d::RenderTexture *renTex, int tutorialTag_in);

        // here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
        virtual bool init();
        
        // implement the "static create()" method manually
        CREATE_FUNC(TutorialScene);

    private:

        // METHODS //

        void touching();
        void delayTouch(float dt);
        void set_isTouchable(float dt);
        void onKeyReleasing();

        void passNotice(float dt);
        void generateVisualTut();
        void removeVisualTut();
        void drawBlocky(bool isHide);
        void drawBlockyBarrier();
        void blockyAction(float dt);
        void popGameScene();

        // OBJECTS //

        static const int colorMembersCount = 7;

        cocos2d::EventListenerTouchOneByOne *touchListener;
        cocos2d::EventListenerKeyboard *backButtonListener;

        cocos2d::DrawNode *darkScreen;
        cocos2d::Sprite *arrow, *finger;
        cocos2d::Label *label, *passLabel;

        // visual tutorial
        cocos2d::DrawNode *blocky, *blockyBarrier;
        cocos2d::Label *visualLabel;
        cocos2d::Color4F currentColor, nextColor = Color4F::RED, colorGroup[colorMembersCount] = {

            Color4F::RED, Color4F::GREEN, Color4F::BLUE, Color4F::YELLOW,
            Color4F::ORANGE, Color4F::MAGENTA, Color4F(0.0, 1.0, 1.0, 1.0)
        };
        cocos2d::Vec2 blockyPos;

        Tool tool;

        // VARIABLES //

        int currentBlockyColorsSort = 0, blockyColorsSort[colorMembersCount] = {0, 4, 3, 1, 2, 6, 5};
        bool isTouchable = false;
};

#endif // __TUTORIAL_SCENE_H__