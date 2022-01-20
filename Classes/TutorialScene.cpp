#include "TutorialScene.h"
#include "GameScene.h"

USING_NS_CC;

Node *screenShot_ts;
int tutorialTag;

Scene* TutorialScene::createScene(cocos2d::RenderTexture *renTex, int tutorialTag_in) {

    tutorialTag = tutorialTag_in;
    screenShot_ts = renTex;

    auto scene = Scene::create();
    auto layer = TutorialScene::create();
    scene->addChild(layer);
    return scene;
}

// on "init" you need to initialize your instance
bool TutorialScene::init() {

    //////////////////////////////
    // 1. super init first
    if (!Layer::init()) {

        return false;
    }

    screenShot_ts->setPosition(tool.ctrPos());
    this->addChild(screenShot_ts);

    ////////////// stuff /////////////////

    // dark screen //

    darkScreen = DrawNode::create();
    darkScreen->setPosition(tool.ctrPos());
    darkScreen->drawSolidRect(

        Vec2(-tool.getVSz().width / 2, -tool.getVSz().height / 2),
        Vec2(tool.getVSz().width / 2, tool.getVSz().height / 2),
        Color4F(0, 0, 0, 100)
    );
    this->addChild(darkScreen);

    ///////////////// directing section /////////////////

    if (tutorialTag == 1) {

        arrow = tool.mySprite("arrow_tutorial.png");
        arrow->setPosition(tool.ctrPos() + Vec2(tool.getBSz().width, -tool.getBSz().height * 3.15));
        this->addChild(arrow);

        Size size = Size(tool.getBSz().width * 3.25, tool.getBSz().height * 2.5);
        label = tool.myLabel("", tool.getBSz().height / 2);
        label->setString("The COLOR of its mouth will be its NEXT COLOR!");
        label->setPosition(arrow->getPosition() + arrow->getContentSize() / 2 + Vec2(size.width / 2, 0));
        label->setDimensions(size.width, size.height);
        label->setHorizontalAlignment(TextHAlignment::CENTER);
        label->setVerticalAlignment(TextVAlignment::CENTER);

        generateVisualTut();
        blockyAction(0.0);
        this->schedule(static_cast<SEL_SCHEDULE>(&TutorialScene::blockyAction), 1.25f);
    }
    else if (tutorialTag == 2) {

        finger = tool.mySprite("finger_print.png");
        finger->setPosition(tool.pos(0.25, 0.25));
        this->addChild(finger);

        label = tool.myLabel("");
        label->setString("SWIPE SCREEN");
        label->setPosition(tool.pos(0.5, 0.8));

        finger->runAction(RepeatForever::create(
        
            Sequence::create(

                ScaleTo::create(0.25, 0.75),
                MoveTo::create(0.75, tool.pos(0.75, 0.25)),
                ScaleTo::create(0.25, 1.0),
                ScaleTo::create(0.25, 0.75),
                MoveTo::create(0.75, tool.pos(0.25, 0.25)),
                ScaleTo::create(0.25, 1.0),
                nullptr
            )
        ));
    }

    label->enableBold();
    label->setOpacity(100);
    this->addChild(label);

    label->runAction(RepeatForever::create(
        
        Sequence::create(
                
            FadeTo::create(0.5, 255),
            FadeTo::create(0.5, 100),
            nullptr
        )
    ));

    this->scheduleOnce(static_cast<SEL_SCHEDULE>(&TutorialScene::passNotice), tutorialTag == 1 ? 2.5f : 1.0f);
    delayTouch(tutorialTag == 1 ? 2.51 : 1.01);

    // event
    touching();
    onKeyReleasing();

    return true;
}

void TutorialScene::passNotice(float dt) {

    passLabel = tool.myLabel("OK!", tool.getBSz().height * 3/2);
    passLabel->setPosition(tutorialTag == 1 ? tool.pos(0.5, 0.0) + Vec2(0, tool.getBSz().height * 2) : tool.ctrPos());
    passLabel->setScale(0.875);
    passLabel->setOpacity(100);
    this->addChild(passLabel);

    passLabel->runAction(RepeatForever::create(
        
        Sequence::create(

            Spawn::create (
                
                ScaleTo::create(0.5, 1.0),
                FadeTo::create(0.5, 255),
                nullptr
            ),
            Spawn::create (
                
                ScaleTo::create(0.5, 0.875),
                FadeTo::create(0.5, 100),
                nullptr
            ),
            nullptr
        )
    ));
}

void TutorialScene::generateVisualTut() {

    visualLabel = tool.myLabel("CONSIDER!");
    visualLabel->setPosition(tool.ctrPos() + Vec2(0, tool.getBSz().height * 8));
    this->addChild(visualLabel);
    visualLabel->runAction(RepeatForever::create(Sequence::create(
        
        FadeTo::create(0.5, 255),
        FadeTo::create(0.5, 100),
        nullptr
    )));

    // blocky
    blockyPos = tool.ctrPos() + Vec2(0, tool.getBSz().height * 5);
    blocky = DrawNode::create();
    drawBlocky(false);
    blocky->setPosition(blockyPos);
    this->addChild(blocky);

    // barrier
    blockyBarrier = DrawNode::create();
    drawBlockyBarrier();
    blockyBarrier->setScale(0.0, 1.0);
    this->addChild(blockyBarrier);
}

void TutorialScene::removeVisualTut() {

    visualLabel->runAction(RemoveSelf::create());
    blocky->runAction(RemoveSelf::create());
    blockyBarrier->runAction(RemoveSelf::create());
}

// blocky //

void TutorialScene::drawBlocky(bool isHide) {

    currentColor = nextColor;
    nextColor = colorGroup[blockyColorsSort[currentBlockyColorsSort]];
    tool.currentObjectCounter(&currentBlockyColorsSort, colorMembersCount);

    blocky->setVisible(false);
    blocky->setScale(1.0);
    blocky->drawSolidRect(
        
        -Vec2(tool.getBSz().width, tool.getBSz().height * 2),
        Vec2(tool.getBSz().width, tool.getBSz().height * 2),
        currentColor
    );
    blocky->drawSolidRect(
        
        Vec2(-tool.getBSz().width * 0.75, tool.getBSz().height * 0.25),
        Vec2(tool.getBSz().width * 0.75, tool.getBSz().height * 1.75),
        Color4F(0.95, 0.95, 0.95, 1.0)
    );
    blocky->drawSolidRect(
        
        Vec2(-tool.getBSz().width * 0.5, tool.getBSz().height * 1.2),
        Vec2(-tool.getBSz().width * 0.2, tool.getBSz().height * 1.5),
        Color4F::BLACK
    );
    blocky->drawSolidRect(
        
        Vec2(tool.getBSz().width * 0.2, tool.getBSz().height * 1.2),
        Vec2(tool.getBSz().width * 0.5, tool.getBSz().height * 1.5),
        Color4F::BLACK
    );
    Vec2 mouthPts[3] = {
        
        Vec2(-tool.getBSz().width * 0.35, tool.getBSz().height * 1.0),
        Vec2(tool.getBSz().width * 0.35, tool.getBSz().height * 1.0),
        Vec2(tool.getBSz().width * 0.0, tool.getBSz().height * 0.7),
    };
    blocky->drawSolidPoly(mouthPts, 3, nextColor);
    if (isHide) blocky->setScale(0.0, 1.0);
    blocky->setVisible(true);
}

void TutorialScene::drawBlockyBarrier() {

    blockyBarrier->setVisible(false);
    blockyBarrier->setScale(1.0);
    blockyBarrier->drawSolidRect(
        
        -Vec2(tool.getBSz().width, tool.getBSz().height * 2),
        Vec2(tool.getBSz().width, tool.getBSz().height * 2),
        Color4F(currentColor.r / 2, currentColor.g / 2, currentColor.b / 2, 1.0)
    );
    blockyBarrier->setScale(0.0, 1.0);
    blockyBarrier->setVisible(true);
}

void TutorialScene::blockyAction(float dt) {

    blocky->runAction(Sequence::create(
        
        Spawn::create(
        
            ScaleTo::create(0.25, 0.0, 1.0),
            MoveTo::create(0.25, blockyPos + Vec2(tool.getBSz().width, 0)),
            nullptr
        ),
        MoveTo::create(0, blockyPos + Vec2(-tool.getBSz().width, 0)),
        CallFunc::create([=]() {drawBlocky(true);}),
        Spawn::create(
        
            ScaleTo::create(0.25, 1.0, 1.0),
            MoveTo::create(0.25, blockyPos + Vec2::ZERO),
            nullptr
        ),
        nullptr
    ));

    blockyBarrier->runAction(Sequence::create(
        
        MoveTo::create(0, blockyPos + Vec2(-tool.getBSz().width, 0)),
        Spawn::create(
        
            ScaleTo::create(0.25, 1.0, 1.0),
            MoveTo::create(0.25, blockyPos + Vec2::ZERO),
            nullptr
        ),
        Spawn::create(
            
            ScaleTo::create(0.25, 0.0, 1.0),
            MoveTo::create(0.25, blockyPos + Vec2(tool.getBSz().width, 0)),
            nullptr
        ),
        CallFunc::create([=]() {drawBlockyBarrier();}),
        nullptr
    ));
}

// general //

void TutorialScene::touching() {

    touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(true);
    touchListener->onTouchBegan = [=] (Touch *touch, Event *event) {

        return true;
    };
    touchListener->onTouchEnded = [=] (Touch *touch, Event *event) {

        if (isTouchable) {
            
            isTouchable = false;

            _eventDispatcher->removeEventListener(touchListener);
            _eventDispatcher->removeEventListener(backButtonListener);
            popGameScene();
        }
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
}

void TutorialScene::popGameScene() {

    if (tutorialTag == 1) {
        
        arrow->stopAllActions();
        arrow->runAction(RemoveSelf::create());
        this->unschedule(static_cast<SEL_SCHEDULE>(&TutorialScene::blockyAction));
        removeVisualTut();
    }
    else if (tutorialTag == 2) {
        
        finger->stopAllActions();
        finger->runAction(RemoveSelf::create());
    }

    label->stopAllActions();
    passLabel->stopAllActions();
        
    label->runAction(RemoveSelf::create());
    passLabel->runAction(RemoveSelf::create());
    darkScreen->runAction(RemoveSelf::create());
    screenShot_ts->runAction(RemoveSelf::create());

    Director::getInstance()->popScene();
}

// touch and contact pacifier //

void TutorialScene::delayTouch(float dt) {

    isTouchable = false;
    this->scheduleOnce(static_cast<SEL_SCHEDULE>(&TutorialScene::set_isTouchable), dt);
}

void TutorialScene::set_isTouchable(float dt) {isTouchable = true;}

// back button ANDROID
void TutorialScene::onKeyReleasing() {

    backButtonListener = EventListenerKeyboard::create();
    backButtonListener->onKeyReleased = [=](cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event) {

        if (keyCode == EventKeyboard::KeyCode::KEY_BACK) {

            if (isTouchable) {
                
                _eventDispatcher->removeEventListener(touchListener);
                _eventDispatcher->removeEventListener(backButtonListener);
                popGameScene();
            }
        }
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(backButtonListener, this);
}