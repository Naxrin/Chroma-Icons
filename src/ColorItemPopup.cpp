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

        //wave trail
        auto wavetrl = CCSprite::createWithSpriteFrameName("GJ_colorBtn_001.png");
        wavetrl->setScale(0.6);
        auto waveBtn = CCMenuItemSpriteExtra::create(wavetrl, this, menu_selector(ColorItemPopup::onItemClicked));
        waveBtn->setPosition(CCPoint(80.f, 25.f));
        waveBtn->setTag(14*player + 9);
        catagory->addChild(waveBtn);

        auto waveFont = CCLabelBMFont::create("Wave Trail", "bigFont.fnt", 120.f, CCTextAlignment::kCCTextAlignmentLeft);
        waveFont->setAnchorPoint(CCPoint(0.f, 0.5));
        waveFont->setScale(0.5);
        waveFont->setPosition(CCPoint(100.f, 25.f));
        waveFont->setWidth(160.f);
        catagory->addChild(waveFont);

        //partical
        auto partical = CCSprite::createWithSpriteFrameName("GJ_colorBtn_001.png");
        partical->setScale(0.6);
        auto partiBtn = CCMenuItemSpriteExtra::create(partical, this, menu_selector(ColorItemPopup::onItemClicked));
        partiBtn->setPosition(CCPoint(250.f, 25.f));
        partiBtn->setTag(14*player + 10);
        catagory->addChild(partiBtn);

        auto partiFont = CCLabelBMFont::create("Regular Trail", "bigFont.fnt", 120.f, CCTextAlignment::kCCTextAlignmentLeft);
        partiFont->setAnchorPoint(CCPoint(0.f, 0.5));
        partiFont->setScale(0.5);
        partiFont->setPosition(CCPoint(270.f, 25.f));
        partiFont->setWidth(160.f);
        catagory->addChild(partiFont);

        catagory->updateLayout();
    }

    // same dual toggle
    //auto togglerMenu = CCMenu::create();
    //togglerMenu->ignoreAnchorPointForPosition(false);
    //togglerMenu->setPosition(CCPoint(winSize.width/2, winSize.height/2-20.f));
    //togglerMenu->setAnchorPoint(CCPoint(0.5, 0.5));
    //togglerMenu->setID("same-dual-toggler");

    //auto sameBtn = CCMenuItemToggler::createWithStandardSprites(this, menu_selector(ColorItemPopup::onSameDual), 0.7);
    //sameBtn->setPosition(CCPoint(10.f, 10.f));
    //sameBtn->toggle(Mod::get()->getSavedValue<bool>("same-dual"));
    //togglerMenu->addChild(sameBtn);
    //ml->getChildByID("second-catagory-menu")->setVisible(!Mod::get()->getSavedValue<bool>("same-dual"));

    //auto sameText = CCLabelBMFont::create("Same Dual Color", "bigFont.fnt", 120.f, CCTextAlignment::kCCTextAlignmentLeft);
    //sameText->setAnchorPoint(CCPoint(1.f, 0.5));    
    //sameText->setScale(0.5);
    //sameText->setPosition(CCPoint(sameText->getContentWidth()/2 + 30.f, 10.f));
    //togglerMenu->addChild(sameText);
    //togglerMenu->setContentSize(CCSize(sameText->getContentWidth()/2 +30.f, 20.f));
    //ml->addChild(togglerMenu); 
    //togglerMenu->updateLayout();

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
/*
void ColorItemPopup::onSameDual(CCObject *sender){
    auto menu = this->getChildByID("main-layer")->getChildByID("second-catagory-menu");
    bool status = static_cast<CCMenuItemToggler*>(sender)->isToggled();
    static_cast<CCMenu*>(menu)->setEnabled(status);
    static_cast<CCMenu*>(menu)->setOpacity((GLubyte)(status*50 + 50));
    Mod::get()->setSavedValue<bool>("same-dual", !static_cast<CCMenuItemToggler*>(sender)->isToggled());
}
*/
void ColorItemPopup::onMoreSettings(CCObject *sender){
    ColorOptionsPopup::create()->show();
}
/*
ColorData data;
*/
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