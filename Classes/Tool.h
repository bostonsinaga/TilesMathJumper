#ifndef _TOOL_H_
#define _TOOL_H_

#include "cocos2d.h"
#include "AudioEngine.h"

USING_NS_CC;

class Tool {

    public:

        Sprite *mySprite(std::string filePath) {

            auto sprite = Sprite::create(filePath);
            sprite->setContentSize(sprite->getContentSize() * getUDef()->getFloatForKey("SCALE RATE"));
            return sprite;
        }

        Label *myLabel(std::string str, float height = 0.0) {

            if (height == 0.0) height = getBSz().height;
            auto label = Label::createWithSystemFont(str, "", height);
            return label;
        }

        void mySpriteFrames(
            
            Sprite **sprite,
            int count,
            const char *format,
            int z_order,
            Node *parent
        ) {

            char name[100];

            for (int i = 0; i < count; i++) {
                
                std::sprintf(name, format, i + 1);
                sprite[i] = mySprite(name);
                sprite[i]->setOpacity(0);

                if (i == 0) {

                    sprite[i]->setPosition(pos(0.5, 2.0));
                    parent->addChild(sprite[i], z_order);
                }
                else {
                    
                    sprite[i]->setPosition(sprite[0]->getContentSize() / 2);
                    sprite[0]->addChild(sprite[i]);
                }
            }
        }

        void mySpriteAnimation(
            
            Sprite **sprite,
            int count,
            float dt,
            Vec2 pos,
            bool isColorize = false,
            Color3B color = Color3B::WHITE
        ) {

            for (int i = 0; i < count; i++) {

                if (isColorize) sprite[i]->setColor(color);
                if (i == 0) sprite[i]->setPosition(pos);
                sprite[i]->runAction(Sequence::create(

                    DelayTime::create(i * dt), 
                    FadeIn::create(0),
                    DelayTime::create(dt),
                    FadeOut::create(0),
                    nullptr
                ));
            }
        }

        void setUserValue(

            const char *dataName,
            int value,
            int *valueName = nullptr,
            int defaultValue = 0
        ) {

            uDef->setIntegerForKey(dataName, value);
            uDef->flush();
            if (valueName != nullptr) *valueName = uDef->getIntegerForKey(dataName, defaultValue);
        }

        void currentObjectCounter(int *currentTag, int total) {

            int tag = *currentTag;
            tag++;
            *currentTag = tag;
            if (*currentTag >= total) *currentTag = 0;
        }

        void changeObjectParent(Node *child, Node *parent, int z_order) {

            child->retain();
            child->removeFromParent();
            parent->addChild(child, z_order);
            child->release();
        }

        Size getBSz() {return Size(120, 120) * uDef->getFloatForKey("SCALE RATE");} // brick size
        Size getVSz() {return vSize;}

        bool isAtTouchPos(Node *node, char touchCondition, Vec2 began, Vec2 ended) {

            /*
                'b' for Began
                'e' for Began with Ended
            */
            if (touchCondition == 'b') {

                if (began.x >= node->getPosition().x - node->getContentSize().width / 2 && began.x <= node->getPosition().x + node->getContentSize().width / 2
                    && began.y >= node->getPosition().y - node->getContentSize().height / 2 && began.y <= node->getPosition().y + node->getContentSize().height / 2) {

                    return true;
                }
                else return false;
            }
            else if (touchCondition == 'e') {

                if (began.x >= node->getPosition().x - node->getContentSize().width / 2 && began.x <= node->getPosition().x + node->getContentSize().width / 2
                    && began.y >= node->getPosition().y - node->getContentSize().height / 2 && began.y <= node->getPosition().y + node->getContentSize().height / 2
                    && ended.x >= node->getPosition().x - node->getContentSize().width / 2 && ended.x <= node->getPosition().x + node->getContentSize().width / 2
                    && ended.y >= node->getPosition().y - node->getContentSize().height / 2 && ended.y <= node->getPosition().y + node->getContentSize().height / 2) {

                    return true;
                }
                else return false;
            }
            else return false;
        }

        bool isAtPos(cocos2d::Node *subject, cocos2d::Node *teritory, bool isPlayer) {

            if (isPlayer) {

                if (subject->getPosition().x <= teritory->getPosition().x + teritory->getContentSize().width / 2 * teritory->getScaleX()
                    && subject->getPosition().y - getBSz().height * 2 <= teritory->getPosition().y + teritory->getContentSize().height / 2 * teritory->getScaleY()
                    && subject->getPosition().x >= teritory->getPosition().x - teritory->getContentSize().width / 2 * teritory->getScaleX()
                    && subject->getPosition().y - getBSz().height * 2 >= teritory->getPosition().y - teritory->getContentSize().height / 2 * teritory->getScaleY())
                return true;
                else return false;
            }
            else {
                
                if (subject->getPosition().x <= teritory->getPosition().x + teritory->getContentSize().width / 2 * teritory->getScaleX()
                    && subject->getPosition().y <= teritory->getPosition().y + teritory->getContentSize().height / 2 * teritory->getScaleY()
                    && subject->getPosition().x >= teritory->getPosition().x - teritory->getContentSize().width / 2 * teritory->getScaleX()
                    && subject->getPosition().y >= teritory->getPosition().y - teritory->getContentSize().height / 2 * teritory->getScaleY())
                return true;
                else return false;
            }
        }

        Vec2 ctrPos() {

            return Point(vSize.width / 2 + vOrigin.x, vSize.height / 2 + vOrigin.y);
        }

        Vec2 pos(float x, float y) {

            return Point(vSize.width * x + vOrigin.x, vSize.height * y + vOrigin.y);
        }

        UserDefault *getUDef() {return uDef;}

        // AUDIO ENGINE //

        void playSFX(std::string designation) {
                
            if      (designation == "false")          AudioEngine::play2d("SOUNDS/false.mp3",          false, 1.0);
            else if (designation == "jump")           AudioEngine::play2d("SOUNDS/jump.mp3",           false, 1.0);
            else if (designation == "new high score") AudioEngine::play2d("SOUNDS/new_high_score.mp3", false, 1.0);
            else if (designation == "select off")     AudioEngine::play2d("SOUNDS/select_off.mp3",     false, 1.0);
            else if (designation == "select on")      AudioEngine::play2d("SOUNDS/select_on.mp3",      false, 1.0);
            else if (designation == "true 1")         AudioEngine::play2d("SOUNDS/true_1.mp3",         false, 1.0);
            else if (designation == "true 2")         AudioEngine::play2d("SOUNDS/true_2.mp3",         false, 1.0);
            else if (designation == "true 3")         AudioEngine::play2d("SOUNDS/true_3.mp3",         false, 1.0);
        }

    private:

        Size vSize = Director::getInstance()->getVisibleSize();
        Vec2 vOrigin = Director::getInstance()->getVisibleOrigin();
        UserDefault *uDef = UserDefault::getInstance();
};

#endif // _TOOL_H_