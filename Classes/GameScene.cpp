#include "GameScene.h"
#include "PauseScene.h"
#include "TutorialScene.h"

USING_NS_CC;

Scene* GameScene::createScene() {

    auto scene = Scene::create();
    auto layer = GameScene::create();
    scene->addChild(layer);
    return scene;
}

// on "init" you need to initialize your instance
bool GameScene::init() {

    //////////////////////////////
    // 1. super init first
    if (!LayerGradient::initWithColor(Color4B(0, 127, 255, 255), Color4B::WHITE, Vec2(0, -1))) {

        return false;
    }

    ////// INITIALIZATION //////

    //// WAY ////

    waySize = Size(tool.getBSz().width * 20, tool.getBSz().height * 20);
    Vec2 wayPts[4] = {

        -Vec2(waySize / 2),
        Vec2(-waySize.width / 8, waySize.height / 2),
        Vec2(waySize.width / 8, waySize.height / 2),
        Vec2(waySize.width / 2, -waySize.height / 2)
    };
    auto way = DrawNode::create();
    way->drawSolidPoly(wayPts, 4, Color4F(0.95, 0.95, 0.95, 1.0));
    way->setPosition(tool.ctrPos() - Vec2(0, waySize.height / 2));
    this->addChild(way);

    onWayPos[0] = tool.ctrPos() - Vec2(tool.getBSz().width * 3, waySize.height / 4); // left
    onWayPos[1] = tool.ctrPos() - Vec2(0, waySize.height / 4); // middle
    onWayPos[2] = tool.ctrPos() - Vec2(-tool.getBSz().width * 3, waySize.height / 4); // right

    //// PLAYER ////

    // colorize
    nextColor = colorGroup[currentBlockyColorsSort];

    // parent (player)
    player = Node::create();
    player->setPosition(onWayPos[1]);
    this->addChild(player, 1);

    // darkDot
    darkDot = DrawNode::create();
    darkDot->drawSolidCircle(
        
        Vec2(0, -tool.getBSz().height * 2),
        tool.getBSz().width * 2,
        0.0, 100, Color4F::BLACK
    );
    darkDot->setOpacity(0);
    darkDot->setScale(1.0, 0.75);
    player->addChild(darkDot);

    // blocky
    blocky = DrawNode::create();
    drawBlocky(false);
    player->addChild(blocky);

    // barrier
    blockyBarrier = DrawNode::create();
    drawBlockyBarrier();
    blockyBarrier->setScale(0.0, 1.0);
    player->addChild(blockyBarrier);

    // blocky action at main menu
    player->runAction(RepeatForever::create(Sequence::create(

        CallFunc::create([=]() {tool.playSFX("jump");}),
        JumpTo::create(0.5, onWayPos[currentOnWayPos], tool.getBSz().height, 1),
        nullptr
    )));

    //// TILE ////

    char str[32];
    for (int i = 0; i < tilesRowsCount; i++) {
        
        for (int j = 0; j < 3; j++) {

            std::sprintf(str, "GRAPHICS/tile_%d.png", j + 1);
            tile[i][j] = tool.mySprite(str);
            tile[i][j]->setPosition(tool.pos(0.0, 2.0));
            this->addChild(tile[i][j]);

            tileLabel[i][j] = tool.myLabel("");
            tileLabel[i][j]->setPosition(tile[i][j]->getContentSize() / 2);
            tileLabel[i][j]->enableOutline(Color4B::BLACK, 3);
            tileLabel[i][j]->setColor(Color3B(Color4F(0.95, 0.95, 0.95, 1.0)));
            tileLabel[i][j]->setScale(1.0, 0.75);
            tile[i][j]->addChild(tileLabel[i][j]);
        }
    }

    for (int i = 0; i < 3; i++) {
        
        std::sprintf(str, "GRAPHICS/tile_multi_color_%d.png", i + 1);
        tileMultiColor[i] = tool.mySprite(str);
        tileMultiColor[i]->setPosition(tool.pos(1.0, 2.0));
        this->addChild(tileMultiColor[i]);
    }

    eventLabel = tool.myLabel("");
    eventLabel->setPosition(tool.pos(0.5, 0.6));
    eventLabel->enableOutline(Color4B::BLACK, 2);
    this->addChild(eventLabel, 11);

    //// GUI ////

    generateDarkScreen();

    // play button
    Vec2 pbPoints[3] = {

        -Vec2(tool.getBSz().width * 1.5, tool.getBSz().height * 1.5),
        Vec2(-tool.getBSz().width * 1.5, tool.getBSz().height * 1.5),
        Vec2(tool.getBSz().width * 1.5, 0)
    };
    playButton = DrawNode::create();
    playButton->drawSolidPoly(pbPoints, 3, Color4F::WHITE);
    playButton->drawPoly(pbPoints, 3, true, Color4F::BLACK);
    playButton->setContentSize(tool.getBSz() * 3);
    playButton->setPosition(tool.ctrPos());
    this->addChild(playButton, 13);

    gameTitleAction();

    //// OTHERS ////

    // listeners
    delayTouch(0.5);
    touching();
    onKeyReleasing();
    
    return true;
}

// blocky //

void GameScene::drawBlocky(bool isHide) {

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

void GameScene::drawBlockyBarrier() {

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

void GameScene::playerJumpSound() {

    if (player->getPosition().x < onWayPos[1].x - tool.getBSz().width / 2) tool.playSFX("true 1");
    else if (player->getPosition().x > onWayPos[1].x + tool.getBSz().width / 2) tool.playSFX("true 3");
    else tool.playSFX("true 2");
}

// tiles //

cocos2d::Color4F GameScene::changeExceptionColor(cocos2d::Color4F exceptionColor) {

    int searchedTag;
    
    for (int i = 0; i < colorMembersCount; i++) {
        
        if (colorGroup[i] == exceptionColor) {
        
            searchedTag = i + 1;
            if (searchedTag >= colorMembersCount) searchedTag = 0;
        }
    }

    return colorGroup[searchedTag];
}

void GameScene::spawnTiles(float dt) {

    std::srand(std::time(0));
    int tileScore, nextColorHolder = std::rand() % 3, ratioRandomSelectTag = std::rand() % 2;

    tile[currentTilesRow][0]->setPosition(tool.ctrPos() - Vec2(waySize.width / 8 - tile[currentTilesRow][0]->getContentSize().width * 0.182, tool.getBSz().height));
    tile[currentTilesRow][1]->setPosition(tool.ctrPos() - Vec2(0, tool.getBSz().height));
    tile[currentTilesRow][2]->setPosition(tool.ctrPos() + Vec2(waySize.width / 8 - tile[currentTilesRow][2]->getContentSize().width * 0.182, -tool.getBSz().height));

    if (eventTileTag >= 2 && comparisonCounter == 1) {

        float ratio[3];

        for (int i = 0; i < 3; i++) {

            switch (eventTileTag) {

                case 2: { // <

                    ratio[0] = ratioRandomSelectTag == 0 ? 1.0f : 2.0f;
                    ratio[1] = ratioRandomSelectTag == 0 ? 2.0f : 1.0f;
                    ratio[2] = 0.5f;
                    break;
                }
                case 3: { // >

                    ratio[0] = ratioRandomSelectTag == 0 ? 1.0f : 0.5f;
                    ratio[1] = ratioRandomSelectTag == 0 ? 0.5f : 1.0f;
                    ratio[2] = 2.0f;
                    break;
                }
                default: { // = & + & -

                    ratio[0] = ratioRandomSelectTag == 0 ? 0.5f : 1.0f;
                    ratio[1] = ratioRandomSelectTag == 0 ? 1.0f : 0.5f;
                    ratio[2] = 2.0f;
                }
            }

            if (eventTileTag == 5) {
                
                if (currentOnWayPos == 1) tileScore = (answerCalc - float(comparisonBuffer)) * ratio[i];
                else {
                    
                    if      (std::abs(currentOnWayPos - i) == 0) tileScore = (answerCalc - float(comparisonBuffer)) * ratio[0];
                    else if (std::abs(currentOnWayPos - i) == 1) tileScore = (answerCalc - float(comparisonBuffer)) * ratio[1];
                    else if (std::abs(currentOnWayPos - i) == 2) tileScore = (answerCalc - float(comparisonBuffer)) * ratio[2];
                }
            }
            else if (eventTileTag == 6) {
                
                if (currentOnWayPos == 1) tileScore = (float(comparisonBuffer) - answerCalc) * ratio[i];
                else {
                    
                    if      (std::abs(currentOnWayPos - i) == 0) tileScore = (float(comparisonBuffer) - answerCalc) * ratio[0];
                    else if (std::abs(currentOnWayPos - i) == 1) tileScore = (float(comparisonBuffer) - answerCalc) * ratio[1];
                    else if (std::abs(currentOnWayPos - i) == 2) tileScore = (float(comparisonBuffer) - answerCalc) * ratio[2];
                }
            }
            else {

                if (currentOnWayPos == 1) tileScore = float(comparisonBuffer) * ratio[i];
                else {
                    
                    if      (std::abs(currentOnWayPos - i) == 0) tileScore = float(comparisonBuffer) * ratio[0];
                    else if (std::abs(currentOnWayPos - i) == 1) tileScore = float(comparisonBuffer) * ratio[1];
                    else if (std::abs(currentOnWayPos - i) == 2) tileScore = float(comparisonBuffer) * ratio[2];
                }

                if (tileScore > maxScore / 2) tileScore = maxScore / 2 + 1;
            }

            tileLabel[currentTilesRow][i]->setString(std::to_string(tileScore));
        }
        
        tile[currentTilesRow][0]->setColor(Color3B(nextColor));
        tile[currentTilesRow][1]->setColor(Color3B(nextColor));
        tile[currentTilesRow][2]->setColor(Color3B(nextColor));
    }
    else {

        for (int i = 0; i < 3; i++) {

            tileScore = std::abs(std::rand() % (maxScore / 4) + 1);
            if (tileScore >= maxScore / 2) tileScore -= std::rand() % tileScore;
            tileLabel[currentTilesRow][i]->setString(std::to_string(tileScore));
        }

        for (int i = 0; i < 3; i++) {
            
            if (nextColorHolder == i && std::abs(currentOnWayPos - i) >= 2) {
                
                nextColorHolder = i + 1;
                if (nextColorHolder >= 3) nextColorHolder = 0;
            }
        }

        Color4F useColor;

        useColor = colorGroup[std::rand() % colorMembersCount];
        tile[currentTilesRow][0]->setColor(Color3B(nextColorHolder == 0 ? nextColor : (useColor == nextColor ? changeExceptionColor(nextColor) : useColor)));

        useColor = colorGroup[std::rand() % colorMembersCount];
        tile[currentTilesRow][1]->setColor(Color3B(nextColorHolder == 1 ? nextColor : (useColor == nextColor ? changeExceptionColor(nextColor) : useColor)));

        useColor = colorGroup[std::rand() % colorMembersCount];
        tile[currentTilesRow][2]->setColor(Color3B(nextColorHolder == 2 ? nextColor : (useColor == nextColor ? changeExceptionColor(nextColor) : useColor)));
    }

    // TILE EVENTS //

    tileEventCounter--;

    if (tileEventCounter == 0) {

        tileEventCounter = 3;

        switch (currentEventTile) {

            case 1: {

                tile[currentTilesRow][nextColorHolder]->setColor(Color3B(Color4F(0.95, 0.95, 0.95, 1.0)));
                tileLabel[currentTilesRow][nextColorHolder]->setString("");
                tileMultiColor[nextColorHolder]->setPosition(tile[currentTilesRow][nextColorHolder]->getPosition());
                moveTile(tileMultiColor[nextColorHolder], nextColorHolder, false);
                break;
            }
            case 2: {

                tile[currentTilesRow][nextColorHolder]->setColor(Color3B::GRAY);
                tileLabel[currentTilesRow][nextColorHolder]->setString("<");
                break;
            }
            case 3: {

                tile[currentTilesRow][nextColorHolder]->setColor(Color3B::GRAY);
                tileLabel[currentTilesRow][nextColorHolder]->setString(">");
                break;
            }
            case 4: {

                tile[currentTilesRow][nextColorHolder]->setColor(Color3B::GRAY);
                tileLabel[currentTilesRow][nextColorHolder]->setString("=");
                break;
            }
            case 5: {

                tile[currentTilesRow][nextColorHolder]->setColor(Color3B::GRAY);
                tileLabel[currentTilesRow][nextColorHolder]->setString("+");
                break;
            }
            case 6: {

                tile[currentTilesRow][nextColorHolder]->setColor(Color3B::GRAY);
                tileLabel[currentTilesRow][nextColorHolder]->setString("-");
                break;
            }
        }

        tool.currentObjectCounter(&currentEventTile, eventTilesCount);
    }

    moveTile(tile[currentTilesRow][0], 0, true);
    moveTile(tile[currentTilesRow][1], 1, true);
    moveTile(tile[currentTilesRow][2], 2, true);

    tool.currentObjectCounter(&currentTilesRow, tilesRowsCount);
}

void GameScene::moveTile(cocos2d::Node *node, int posTag, bool isNormalTile) {

    Size tileSize = tile[0][posTag]->getContentSize();

    if (posTag == 0)
        node->runAction(Sequence::create(
            
            ScaleTo::create(0, 1),
            Spawn::create(

                MoveTo::create(tileActionDT, Point(tool.ctrPos() - Vec2(waySize.width / 2, waySize.height) + Vec2(tileSize * 3/2))),
                ScaleTo::create(tileActionDT, 3),
                nullptr
            ),
            MoveTo::create(0, tool.pos(isNormalTile == true ? 0.0 : 1.0, 2.0)),
            ScaleTo::create(0, 1),
            nullptr
        ));
    else if (posTag == 1)
        node->runAction(Sequence::create(
            
            ScaleTo::create(0, 1),
            Spawn::create(

                MoveTo::create(tileActionDT, Point(tool.ctrPos() - Vec2(0, waySize.height) + Vec2(0, tileSize.height * 3/2))),
                ScaleTo::create(tileActionDT, 3),
                nullptr
            ),
            MoveTo::create(0, tool.pos(isNormalTile == true ? 0.0 : 1.0, 2.0)),
            ScaleTo::create(0, 1),
            nullptr
        ));
    else if (posTag == 2)
        node->runAction(Sequence::create(
        
            ScaleTo::create(0, 1),
            Spawn::create(
    
                MoveTo::create(tileActionDT, Point(
                
                    tool.ctrPos() + Vec2(waySize.width / 2, -waySize.height) +
                    Vec2(-tileSize.width * 3/2, tileSize.height * 3/2)
                )),
                ScaleTo::create(tileActionDT, 3),
                nullptr
            ),
            MoveTo::create(0, tool.pos(isNormalTile == true ? 0.0 : 1.0, 2.0)),
            ScaleTo::create(0, 1),
            nullptr
        ));
}

void GameScene::tileBackToOrigin(cocos2d::Node *node, bool isNormalTile) {
        
    darkDot->setOpacity(127);
    darkDot->runAction(FadeOut::create(0.1));

    node->stopAllActions();
    node->setScale(1.0);
    node->setPosition(tool.pos(isNormalTile == true ? 0.0 : 1.0, 2.0));
}

void GameScene::standOnTileDetection(float dt) {

    if (isGameStarted) {
        
        bool isDetected = false;

        auto tileMultiColorFunc = [=]( int tag, int ct1, int ct2) {

            tileBackToOrigin(tile[ct1][ct2], true);
            tileBackToOrigin(tileMultiColor[tag], false);

            eventLabel->setString("Choose any color!");
            eventLabel->setOpacity(255);

            eventTileTag = 1;
        };

        for (int i = 0; i < tilesRowsCount; i++) {

            for (int j = 0; j < 3; j++) {
            
                if (isJump == false && tool.isAtPos(player, tile[i][j], true)) {

                    switch (eventTileTag) {

                        case 0: {

                            if (tool.isAtPos(tileMultiColor[0], tile[i][j], false)) {

                                tileMultiColorFunc(0, i, j);
                                playerJumpSound();
                            }
                            else if (tool.isAtPos(tileMultiColor[1], tile[i][j], false)) {

                                tileMultiColorFunc(1, i, j);
                                playerJumpSound();
                            }
                            else if (tool.isAtPos(tileMultiColor[2], tile[i][j], false)) {

                                tileMultiColorFunc(2, i, j);
                                playerJumpSound();
                            }
                            else if (tileLabel[i][j]->getString() == "<") {

                                tileBackToOrigin(tile[i][j], true);
                                eventTileTag = 2;
                                playerJumpSound();
                            }
                            else if (tileLabel[i][j]->getString() == ">") {

                                tileBackToOrigin(tile[i][j], true);
                                eventTileTag = 3;
                                playerJumpSound();
                            }
                            else if (tileLabel[i][j]->getString() == "=") {

                                tileBackToOrigin(tile[i][j], true);
                                eventTileTag = 4;
                                playerJumpSound();
                            }
                            else if (tileLabel[i][j]->getString() == "+") {

                                tileBackToOrigin(tile[i][j], true);
                                eventTileTag = 5;
                                playerJumpSound();
                            }
                            else if (tileLabel[i][j]->getString() == "-") {

                                tileBackToOrigin(tile[i][j], true);
                                eventTileTag = 6;
                                playerJumpSound();
                            }
                            else {

                                int tileScore = getTileScore(tileLabel[i][j]);

                                if (currentColor == tile[i][j]->getColor()) {

                                    tileBackToOrigin(tile[i][j], true);
                                    updateScoreBar(tileScore);
                                    playerJumpSound();
                                }
                                else updateScoreBar(-1, false, true);
                            }

                            break;
                        }
                        case 1: {

                            int tileScore = getTileScore(tileLabel[i][j]);

                            eventLabel->runAction(FadeOut::create(0.25));

                            tileBackToOrigin(tile[i][j], true);
                            updateScoreBar(tileScore);
                            playerJumpSound();

                            eventTileTag = 0;
                            break;
                        }
                        default: compareEvent(eventTileTag, tile[i][j], tileLabel[i][j]);
                    }

                    isDetected = true;
                    break;
                }
            }

            if (isDetected) break;
        }
    }
}

void GameScene::compareEvent(int tag, cocos2d::Node *theTile, cocos2d::Label *theTileLabel) {

    int tileScore = getTileScore(theTileLabel);

    if (currentColor == theTile->getColor()) {

        char str[32];
        comparisonCounter++;

        if (comparisonCounter == 1) {

            comparisonBuffer = tileScore;

            if (tag <= 4) {

                if (tag == 2) std::sprintf(str, "%d <", tileScore);
                else if (tag == 3) std::sprintf(str, "%d >", tileScore);
                else if (tag == 4) std::sprintf(str, "%d =", tileScore);
            }
            else {
                
                std::srand(time(0));

                if (tag == 5) {
                    
                    answerCalc = tileScore + (std::rand() % (maxScore / 2 - tileScore) + 1);
                    std::sprintf(str, "%d + ... = %d", tileScore, answerCalc);
                }
                else {
                    
                    answerCalc = tileScore - (std::rand() % tileScore + 1);
                    std::sprintf(str, "%d - ... = %d", tileScore, answerCalc);
                }
            }
            
            eventLabel->setString(str);
            eventLabel->setOpacity(255);

            tileBackToOrigin(theTile, true);
            updateScoreBar(tileScore);
            playerJumpSound();
        }
        else if (comparisonCounter == 2) {

            bool isTrue = false;

            if (tag <= 4) {

                if (tag == 2) {
                    
                    std::sprintf(str, "%d < %d", comparisonBuffer, tileScore);
                    if (comparisonBuffer < tileScore) isTrue = true;
                }
                else if (tag == 3) {
                    
                    std::sprintf(str, "%d > %d", comparisonBuffer, tileScore);
                    if (comparisonBuffer > tileScore) isTrue = true;
                }
                else if (tag == 4) {
                    
                    std::sprintf(str, "%d = %d", comparisonBuffer, tileScore);
                    if (comparisonBuffer == tileScore) isTrue = true;
                }
            }
            else {

                if (tag == 5) {
                    
                    std::sprintf(str, "%d + %d = %d", comparisonBuffer, tileScore, answerCalc);
                    if (comparisonBuffer + tileScore == answerCalc) isTrue = true;
                }
                else {
                    
                    std::sprintf(str, "%d - %d = %d", comparisonBuffer, tileScore, answerCalc);
                    if (comparisonBuffer - tileScore == answerCalc) isTrue = true;
                }
            }

            eventLabel->setString(str);

            if (isTrue) {

                tileBackToOrigin(theTile, true);
                updateScoreBar(tileScore);
                showEventSign(true);
                playerJumpSound();
                comparisonCounter = 0;
                eventTileTag = 0;
            }
            else {
                
                showEventSign(false);
                updateScoreBar(-1);
            }
        }
    }
    else updateScoreBar(-1, false, true);
}

void GameScene::showEventSign(bool isTrue) {

    if (isTrue) {

        eventSign = tool.mySprite("true.png");
        eventSign->runAction(Sequence::create(
            
            FadeOut::create(0.5),
            RemoveSelf::create(),
            nullptr
        ));
        eventLabel->runAction(FadeOut::create(0.5));
    }
    else eventSign = tool.mySprite("false.png");

    eventSign->setPosition(tool.pos(0.5, 0.6) + Vec2(0, tool.getBSz().height));
    this->addChild(eventSign, 11);
}

int GameScene::getTileScore(cocos2d::Label *label) {

    int i;
    for (i = 0; i <= maxScore; i++) if (label->getString() == std::to_string(i)) break;
    return i;
}

// GUI //

void GameScene::generateDarkScreen() {

    darkScreen = DrawNode::create();
    darkScreen->drawSolidRect(-Vec2(tool.getVSz() / 2), Vec2(tool.getVSz() / 2), Color4F(0.0, 0.0, 0.0, 0.5));
    darkScreen->setPosition(tool.ctrPos());
    this->addChild(darkScreen, 10);
}

void GameScene::updateScoreBar(int score_in, bool isInitialization, bool isColorError) {

    if (score_in >= 0) {

        auto func = [=]() {

            Size size = Size(tool.getBSz().width * (float(scoreLabel->getString().length()) / 2 + 0.5), tool.getBSz().height);
            scoreBar->drawSolidRect(-Vec2(size / 2), Vec2(size / 2), Color4F::WHITE);
            scoreBar->drawRect(-Vec2(size / 2), Vec2(size / 2), Color4F::BLACK);
            scoreBar->setPosition(tool.pos(1.0, 1.0) - size / 2 - tool.getBSz() / 2);
        };
    
        if (isInitialization) {

            scoreLabel->setString(std::to_string(score_in));
            func();
        }
        else {
            
            tool.changeObjectParent(scoreLabel, this, 0);

            scoreBar->runAction(RemoveSelf::create());
            scoreBar = DrawNode::create();
            this->addChild(scoreBar, 10);

            tool.changeObjectParent(scoreLabel, scoreBar, 0);

            currentScore += score_in;
            scoreLabel->setString(std::to_string(currentScore));

            if (currentScore > tool.getUDef()->getIntegerForKey("HIGH SCORE", 0))
                inActionNewHighScoreNotice();

            func();
        }
    }
    else {

        if (isColorError) {
            
            eventLabel->enableItalics();
            eventLabel->setPosition(tool.pos(0.5, 0.65));
            eventLabel->setString("Wrong color!");
            eventLabel->setColor(Color3B::RED);
            eventLabel->setOpacity(255);
        }

        eventTileTag = 0;
        scoreBar->setVisible(false);
        highScoreHomeTitleBar->setVisible(false);
        gameOverAction();
        tool.playSFX("false");
    }
}

void GameScene::inActionNewHighScoreNotice() {

    if (isNewHighScoreNoticed == false) {
        
        isNewHighScoreNoticed = true;

        scoreLabel->runAction(Sequence::create(
            
            CallFunc::create([=]() {scoreLabel->setColor(Color3B(255, 223, 31));}),
            DelayTime::create(0.25),
            CallFunc::create([=]() {scoreLabel->setColor(Color3B::GRAY);}),
            DelayTime::create(0.25),
            CallFunc::create([=]() {scoreLabel->setColor(Color3B(255, 223, 31));}),
            DelayTime::create(0.25),
            CallFunc::create([=]() {scoreLabel->setColor(Color3B::GRAY);}),
            DelayTime::create(0.25),
            CallFunc::create([=]() {scoreLabel->setColor(Color3B(255, 223, 31));}),
            DelayTime::create(0.25),
            CallFunc::create([=]() {scoreLabel->setColor(Color3B::GRAY);}),
            DelayTime::create(0.25),
            nullptr
        ));

        tool.changeObjectParent(highScoreHomeTitleLabel, this, 0);

        highScoreHomeTitleBar->runAction(RemoveSelf::create());
        highScoreHomeTitleBar = DrawNode::create();
        this->addChild(highScoreHomeTitleBar, 10);

        tool.changeObjectParent(highScoreHomeTitleLabel, highScoreHomeTitleBar, 0);

        highScoreHomeTitleLabel->setString("NEW HIGH SCORE:");

        Size size = Size(tool.getBSz().width * 4.25, tool.getBSz().height / 2);
        highScoreHomeTitleBar->drawSolidRect(-Vec2(size) / 2, Vec2(size) / 2, Color4F::WHITE);
        highScoreHomeTitleBar->drawRect(-Vec2(size) / 2, Vec2(size) / 2, Color4F::BLACK);
        highScoreHomeTitleBar->setPosition(

            scoreBarPos - Vec2(tool.getBSz().width *
            ((float(scoreLabel->getString().length()) / 2 + 0.5) / 2) + size.width / 2,
            -tool.getBSz().height / 4)
        );

        highScoreHomeTitleBar->runAction(Sequence::create(
            
            DelayTime::create(1.5),
            CallFunc::create([=]() {highScoreHomeTitleBar->setVisible(false);}),
            nullptr
        ));

        tool.playSFX("new high score");
    }
}

void GameScene::gameTitleAction() {

    gameTitle = tool.myLabel("TILES + MATH - JUMPER", tool.getBSz().height * 1.5);
    gameTitle->setPosition(tool.pos(0.5, 1.0) + Vec2(0, tool.getBSz().height * 1.5 * 3/2));
    gameTitle->enableOutline(Color4B::RED, 10);
    gameTitle->setColor(Color3B::ORANGE);
    gameTitle->setDimensions(tool.getBSz().width * 0.8 * 7.5, tool.getBSz().height * 4.8);
    gameTitle->setHorizontalAlignment(TextHAlignment::CENTER);
    gameTitle->setVerticalAlignment(TextVAlignment::CENTER);
    this->addChild(gameTitle, 10);

    gameTitle->runAction(MoveTo::create(0.5, tool.pos(0.5, 0.75)));
    this->scheduleOnce(static_cast<SEL_SCHEDULE>(&GameScene::gameTitleRotate), 0.5f);

    scoreBar = DrawNode::create();
    this->addChild(scoreBar, 10);

    scoreLabel = tool.myLabel("");
    scoreLabel->setColor(Color3B::GRAY);
    scoreLabel->enableOutline(Color4B::BLACK, 3);
    scoreBar->addChild(scoreLabel);

    updateScoreBar(tool.getUDef()->getIntegerForKey("HIGH SCORE", 0), true);

    Size size = Size(tool.getBSz().width * 3.25, tool.getBSz().height / 2);
    scoreBarPos = scoreBar->getPosition();

    highScoreHomeTitleBar = DrawNode::create();
    highScoreHomeTitleBar->drawSolidRect(-Vec2(size) / 2, Vec2(size) / 2, Color4F::WHITE);
    highScoreHomeTitleBar->drawRect(-Vec2(size) / 2, Vec2(size) / 2, Color4F::BLACK);
    highScoreHomeTitleBar->setPosition(
        scoreBarPos - Vec2(tool.getBSz().width *
        ((float(scoreLabel->getString().length()) / 2 + 0.5) / 2) + size.width / 2, -tool.getBSz().height / 4));
    this->addChild(highScoreHomeTitleBar, 10);

    highScoreHomeTitleLabel = tool.myLabel("HIGH SCORE:", tool.getBSz().height / 2);
    highScoreHomeTitleLabel->setColor(Color3B::GRAY);
    highScoreHomeTitleLabel->enableOutline(Color4B::BLACK, 3);
    highScoreHomeTitleBar->addChild(highScoreHomeTitleLabel);
}

void GameScene::gameTitleRotate(float dt) {

    gameTitle->runAction(RepeatForever::create(Sequence::create(

        RotateBy::create(0.25, 11.25),
        RotateBy::create(0.25, -11.25),
        RotateBy::create(0.25, -11.25),
        RotateBy::create(0.25, 11.25),
        nullptr
    )));
}

void GameScene::gameOverAction() {

    isTouchable = false;
    isGameStarted = false;

    player->runAction(ScaleTo::create(0.25, 0.0));
    generateDarkScreen();

    auto noticeLabel = tool.myLabel("GAME OVER!", tool.getBSz().height * 2);
    noticeLabel->setPosition(tool.ctrPos());
    noticeLabel->setColor(Color3B(255, 127, 127));
    noticeLabel->enableOutline(Color4B(191, 0, 0, 255), 4);
    this->addChild(noticeLabel, 12);
    noticeLabel->runAction(MoveTo::create(1.0, tool.pos(0.5, 0.8)));

    std::string strTitle[2];
    Color4B colTitle[2];
    int previousHighScore = tool.getUDef()->getIntegerForKey("HIGH SCORE", 0);

    if (currentScore > previousHighScore) {

        tool.setUserValue("HIGH SCORE", currentScore);
        
        strTitle[0] = "NEW HIGH SCORE:";
        strTitle[1] = "PREVIOUS HIGH SCORE:";

        colTitle[0] = Color4B::GREEN;
        colTitle[1] = Color4B::ORANGE;
    }
    else {

        strTitle[0] = "CURRENT SCORE:";
        strTitle[1] = "HIGH SCORE:";

        colTitle[0] = Color4B::ORANGE;
        colTitle[1] = Color4B::BLACK;
    }

    if (tool.getUDef()->getBoolForKey("IS TUTORIAL NEEDED 1", true) && currentScore > 50)
        tool.setUserValue("IS TUTORIAL NEEDED 1", false);

    auto currentScoreLabel_0 = tool.myLabel(strTitle[0]);
    currentScoreLabel_0->setPosition(tool.ctrPos());
    currentScoreLabel_0->enableOutline(colTitle[0], 3);
    this->addChild(currentScoreLabel_0, 10);

    auto currentScoreLabel_1 = tool.myLabel(std::to_string(currentScore));
    currentScoreLabel_1->setPosition(tool.ctrPos() - Vec2(0, tool.getBSz().height));
    currentScoreLabel_1->enableOutline(colTitle[0], 3);
    this->addChild(currentScoreLabel_1, 10);

    auto highScoreLabel_0 = tool.myLabel(strTitle[1]);
    highScoreLabel_0->setPosition(tool.ctrPos() - Vec2(0, tool.getBSz().height) * 3);
    highScoreLabel_0->enableOutline(colTitle[1], 3);
    this->addChild(highScoreLabel_0, 10);

    auto highScoreLabel_1 = tool.myLabel(std::to_string(previousHighScore));
    highScoreLabel_1->setPosition(tool.ctrPos() - Vec2(0, tool.getBSz().height) * 4);
    highScoreLabel_1->enableOutline(colTitle[1], 3);
    this->addChild(highScoreLabel_1, 10);

    isReturnable = true;
    this->scheduleOnce(static_cast<SEL_SCHEDULE>(&GameScene::goToGameScene), 3.0f);
}

// scenes //

void GameScene::startGame() {

    isGameStarted = true;
    player->stopAllActions();
    player->setPosition(onWayPos[1]);

    if (gameTitle != nullptr) {

        gameTitle->stopAllActions();
        gameTitle->runAction(RemoveSelf::create());
    }
    
    highScoreHomeTitleBar->setVisible(false);
    updateScoreBar(0);

    spawnTiles(0.0);
    this->schedule(static_cast<SEL_SCHEDULE>(&GameScene::spawnTiles), tileActionSpawnDT);
    this->schedule(static_cast<SEL_SCHEDULE>(&GameScene::standOnTileDetection), 0.0f);

    checkingTutorialNeeds();
}

void GameScene::goToGameScene(float dt) {

    auto scene = GameScene::createScene();
    Director::getInstance()->replaceScene(TransitionFade::create(0.25, scene));
}

cocos2d::RenderTexture *GameScene::getRenTex() {

    auto renTex = RenderTexture::create(tool.getVSz().width, tool.getVSz().height);
    renTex->setPosition(tool.ctrPos());

    renTex->begin();
    this->getParent()->visit();
    renTex->end();

    return renTex;
}

// tutorial needs //

void GameScene::checkingTutorialNeeds() {

    if (tool.getUDef()->getBoolForKey("IS TUTORIAL NEEDED 2", true)) {

        this->scheduleOnce(static_cast<SEL_SCHEDULE>(&GameScene::tut_tut1), 0.1f);
        this->scheduleOnce(static_cast<SEL_SCHEDULE>(&GameScene::tut_tut2), 0.6f);
    }
    else if (tool.getUDef()->getBoolForKey("IS TUTORIAL NEEDED 1", true)) {

        this->scheduleOnce(static_cast<SEL_SCHEDULE>(&GameScene::tut_tut1), 0.1f);
    }
}

void GameScene::tut_tut1(float dt) {

    auto scene = TutorialScene::createScene(getRenTex(), 1);
    Director::getInstance()->pushScene(scene);
    scene->runAction(RemoveSelf::create());
}

void GameScene::tut_tut2(float dt) {

    auto scene = TutorialScene::createScene(getRenTex(), 2);
    Director::getInstance()->pushScene(scene);
    scene->runAction(RemoveSelf::create());

    tool.setUserValue("IS TUTORIAL NEEDED 2", false);
}

//// EVENT LISTENERS ////

// touch event
void GameScene::touching() {

    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(true);
    touchListener->onTouchBegan = [=](Touch *touch, Event *event) {
            
        if (isTouchable) {
            
            touchBegan = touch->getLocation();

            if (isGameStarted == false && playButton != nullptr) {

                if (tool.isAtTouchPos(playButton, 'b', touchBegan, touchEnded)) {
                    
                    playButton->setScale(0.9);
                }
            }
        }

        return true;
    };
    touchListener->onTouchEnded = [=](Touch *touch, Event *event) {
            
        if (isTouchable) {

            touchEnded = touch->getLocation();

            if (isGameStarted) {
                
                int valSign = 0;
                if (touchEnded.x < touchBegan.x - tool.getBSz().width * 2) valSign = -1;
                else if (touchEnded.x > touchBegan.x + tool.getBSz().width * 2) valSign = 1;

                currentOnWayPos += valSign;
                if (currentOnWayPos < 0) currentOnWayPos = 0;
                else if (currentOnWayPos > 2) currentOnWayPos = 2;

                isJump = true;
                player->runAction(Sequence::create(
                    
                    JumpTo::create(0.25, onWayPos[currentOnWayPos], tool.getBSz().height, 1),
                    CallFunc::create([=]() {isJump = false;}),
                    nullptr
                ));

                blocky->runAction(Sequence::create(
                    
                    Spawn::create(
                    
                        ScaleTo::create(0.25, 0.0, 1.0),
                        MoveTo::create(0.25, Vec2(tool.getBSz().width, 0)),
                        nullptr
                    ),
                    MoveTo::create(0, Vec2(-tool.getBSz().width, 0)),
                    CallFunc::create([=]() {drawBlocky(true);}),
                    Spawn::create(
                    
                        ScaleTo::create(0.25, 1.0, 1.0),
                        MoveTo::create(0.25, Vec2::ZERO),
                        nullptr
                    ),
                    nullptr
                ));

                blockyBarrier->runAction(Sequence::create(
                    
                    MoveTo::create(0, Vec2(-tool.getBSz().width, 0)),
                    Spawn::create(
                    
                        ScaleTo::create(0.25, 1.0, 1.0),
                        MoveTo::create(0.25, Vec2::ZERO),
                        nullptr
                    ),
                    Spawn::create(
                        
                        ScaleTo::create(0.25, 0.0, 1.0),
                        MoveTo::create(0.25, Vec2(tool.getBSz().width, 0)),
                        nullptr
                    ),
                    CallFunc::create([=]() {drawBlockyBarrier();}),
                    nullptr
                ));

                delayTouch(0.5);
                tool.playSFX("jump");
            }
            else if (playButton == nullptr) {

                tool.playSFX("select on");
                goToGameScene(0.0);
            }
            else if (playButton != nullptr) {

                playButton->setScale(1.0);

                if (tool.isAtTouchPos(playButton, 'e', touchBegan, touchEnded)) {
                
                    darkScreen->runAction(RemoveSelf::create());
                    playButton->runAction(RemoveSelf::create());
                    playButton = nullptr;
                    startGame();
                    tool.playSFX("select on");
                }
            }
        }
        else if (isGameStarted == false && playButton == nullptr) {
            
            tool.playSFX("select on");
            goToGameScene(0.0);
        }
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
}

void GameScene::delayTouch(float dt) {

    isTouchable = false;
    this->scheduleOnce(static_cast<SEL_SCHEDULE>(&GameScene::set_isTouchable), dt);
}

void GameScene::set_isTouchable(float dt) {isTouchable = true;}

// back button ANDROID
void GameScene::onKeyReleasing() {

    auto backButtonListener = EventListenerKeyboard::create();
    backButtonListener->onKeyReleased = [=](cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event) {

        if (keyCode == EventKeyboard::KeyCode::KEY_BACK) {

            if (isTouchable) {

                tool.playSFX("select off");
                
                if (isGameStarted) {

                    auto scene = PauseScene::createScene(getRenTex());
                    Director::getInstance()->pushScene(scene);
                }
                else if (isReturnable == false) Director::getInstance()->end();
            }
            if (isReturnable) {
                
                tool.playSFX("select on");
                goToGameScene(0.0);
            }
        }
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(backButtonListener, this);
}