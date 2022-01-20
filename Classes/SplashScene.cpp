#include "SplashScene.h"
#include "GameScene.h"
#include "AudioEngine.h"

USING_NS_CC;

Scene* SplashScene::createScene() {

    auto scene = Scene::create();
    auto layer = SplashScene::create();
    scene->addChild(layer);
    return scene;
}

// on "init" you need to initialize your instance
bool SplashScene::init() {

    //////////////////////////////
    // 1. super init first
    if (!LayerColor::initWithColor(Color4B::WHITE)) {

        return false;
    }

    AudioEngine::preload("SOUNDS/false.mp3");
    AudioEngine::preload("SOUNDS/jump.mp3");
    AudioEngine::preload("SOUNDS/new_high_score.mp3");
    AudioEngine::preload("SOUNDS/select_off.mp3");
    AudioEngine::preload("SOUNDS/select_on.mp3");
    AudioEngine::preload("SOUNDS/true_1.mp3");
    AudioEngine::preload("SOUNDS/true_2.mp3");
    AudioEngine::preload("SOUNDS/true_3.mp3");

    // view //

    auto splashScreen = tool.mySprite("developer_logo.png");
    splashScreen->setPosition(tool.ctrPos());
    this->addChild(splashScreen);

    this->scheduleOnce(static_cast<SEL_SCHEDULE>(&SplashScene::goToGameScene), 2.0f);

    return true;
}

void SplashScene::goToGameScene(float dt) {

    auto scene = GameScene::createScene();
    Director::getInstance()->replaceScene(TransitionCrossFade::create(0.25, scene));
}