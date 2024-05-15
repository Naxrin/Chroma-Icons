#include "IamHead.hpp"

using namespace geode::prelude;

const int gametype[9] = {1, 4, 5, 6, 7, 8, 9, 13, 14};
bool refresh = true; // for main.cpp

bool ColorItemPopup::setup() {
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    this->setTitle("Select the Color You Wanna Modify");
    auto gm = GameManager::sharedState();

    //layer
    auto ml = static_cast<CCNode*>(this->getChildren()->objectAtIndex(0));
    ml->setID("main-layer");

    //player mode
    int garageitem[9] = {
        gm->getPlayerFrame(), gm->getPlayerShip(), gm->getPlayerBall(),
        gm->getPlayerBird(), gm->getPlayerDart(), gm->getPlayerRobot(),
        gm->getPlayerSpider(), gm->getPlayerSwing(), gm->getPlayerJetpack(),
    };

    for (int player = 0; player < 2; player ++){

        // catagory
        auto catagory = CCMenu::create();
        catagory->ignoreAnchorPointForPosition(false);
        catagory->setPosition(CCPoint(winSize.width/2, winSize.height/2 + 30.f - 90.f*player));
        catagory->setContentSize(CCSize(400.f, 80.f));
        catagory->setID(player == 1? "second-catagory-menu" : "main-catagory-menu");
        ml->addChild(catagory);

        // font
        auto font = CCLabelBMFont::create(player == 1? "Player 2" : "Player 1", "goldFont.fnt", 200.f, CCTextAlignment::kCCTextAlignmentCenter);
        font->setPosition(CCPoint(200.f, 90.f));
        font->setScale(0.6);
        font->setID(player == 1? "second-player-label" : "main-player-label");
        catagory->addChild(font);

        //player icon
        for (int status = 0; status < 9; status ++){
            auto icon = GJItemIcon::createBrowserItem(UnlockType(gametype[status]), garageitem[status]);
            icon->setScale(0.8);
            auto btn = CCMenuItemSpriteExtra::create(icon, this, menu_selector(ColorItemPopup::onItemClicked));
            btn->setPosition(CCPoint(40.f + 40.f*status, 55.f));
            //btn->setID("color");
            btn->setTag(14*player + status);
            catagory->addChild(btn);
        }

        // regular trail
        auto regtrailImg = CCSprite::createWithSpriteFrameName("GJ_colorBtn_001.png");
        regtrailImg->setScale(0.6);
        auto regtrailBtn = CCMenuItemSpriteExtra::create(regtrailImg, this, menu_selector(ColorItemPopup::onItemClicked));
        regtrailBtn->setPosition(CCPoint(50.f, 25.f));
        regtrailBtn->setTag(14*player + 10);
        catagory->addChild(regtrailBtn);

        auto regtrailFont = CCLabelBMFont::create("Regular Trail", "bigFont.fnt", 120.f, CCTextAlignment::kCCTextAlignmentLeft);
        regtrailFont->setAnchorPoint(CCPoint(0.f, 0.5));
        regtrailFont->setScale(0.5);
        regtrailFont->setPosition(CCPoint(70.f, 25.f));
        regtrailFont->setWidth(160.f);
        catagory->addChild(regtrailFont);

        // wave trail
        auto wavetrailImg = CCSprite::createWithSpriteFrameName("GJ_colorBtn_001.png");
        wavetrailImg->setScale(0.6);
        auto wavetrailBtn = CCMenuItemSpriteExtra::create(wavetrailImg, this, menu_selector(ColorItemPopup::onItemClicked));
        wavetrailBtn->setPosition(CCPoint(250.f, 25.f));
        wavetrailBtn->setTag(14*player + 9);
        catagory->addChild(wavetrailBtn);

        auto waveFont = CCLabelBMFont::create("Wave Trail", "bigFont.fnt", 120.f, CCTextAlignment::kCCTextAlignmentLeft);
        waveFont->setAnchorPoint(CCPoint(0.f, 0.5));
        waveFont->setScale(0.5);
        waveFont->setPosition(CCPoint(270.f, 25.f));
        waveFont->setWidth(160.f);
        catagory->addChild(waveFont);

        catagory->updateLayout();
    }

    auto settingsMenu = CCMenu::create();

    auto settingsImg = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
    settingsImg->setScale(0.5);
    auto settingsBtn = CCMenuItemSpriteExtra::create(settingsImg, this, menu_selector(ColorItemPopup::onMoreSettings));
    settingsBtn->setPosition(CCPoint(180.f, 100.f));
    settingsMenu->addChild(settingsBtn);

    auto confirmImg = ButtonSprite::create("Apply", "goldFont.fnt", "GJ_button_01.png", 0.8);
    confirmImg->setScale(0.8);
    auto confirmBtn = CCMenuItemSpriteExtra::create(confirmImg, this, menu_selector(ColorItemPopup::onClose));
    confirmBtn->setPosition(CCPoint(0.f, -105.f));
    settingsMenu->addChild(confirmBtn);

    ml->addChild(settingsMenu);
    settingsMenu->updateLayout();

    return true;
}

void ColorItemPopup::onItemClicked(CCObject *sender){
    int tag = sender->getTag();
    ColorSetupPopup::create(tag)->show();
}

void ColorItemPopup::onMoreSettings(CCObject *sender){
    ColorOptionsPopup::create()->show();
}

void ColorItemPopup::onClose(CCObject *sender){
    refresh = true;
    Popup::onClose(sender);
}

ColorItemPopup * ColorItemPopup::create(){
    auto popup = new ColorItemPopup();

    if (popup && popup->init(420.f, 260.f)){
        popup->autorelease();
        return popup;
    };

    log::warn("The ColorSelectPopup failed to initialize!");
    CC_SAFE_DELETE(popup);
    return nullptr;
}