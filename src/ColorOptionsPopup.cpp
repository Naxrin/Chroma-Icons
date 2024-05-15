#include "IamHead.hpp"

using namespace geode::prelude;

// slightly different with the on for ColorSetupPopup
void ColorOptionsPopup::addTextToggler(CCNode* parent, const char* text, const char* desc, CCPoint p, int tag, cocos2d::SEL_MenuHandler callback, bool yes, int down = 0){
    auto menu = CCMenu::create();
    menu->setPosition(p);
    menu->setContentSize(CCSize(360.f, 40.f));
    menu->setTag(tag);
    parent->addChild(menu);

    auto toggler = CCMenuItemToggler::createWithStandardSprites(this, callback, 0.6);
    toggler->setPosition(CCPoint(10.f, 27.f));
    toggler->toggle(yes);
    toggler->setID("toggler");
    menu->addChild(toggler);

    auto label = CCLabelBMFont::create(text, "bigFont.fnt", 120.f, CCTextAlignment::kCCTextAlignmentLeft);
    label->setScale(0.5);
    label->setContentSize(CCSize(330.f, 20.f));
    label->setWidth(340.f);
    label->setAnchorPoint(CCPoint(0.f, 0.5));
    label->setPosition(CCPoint(25.f, 27.f));
    label->setID("label");
    menu->addChild(label);

    auto hint = CCLabelBMFont::create(desc, "chatFont.fnt", 120.f, CCTextAlignment::kCCTextAlignmentLeft);
    hint->setScale(0.7);
    hint->setWidth(360.f);
    hint->setAnchorPoint(CCPoint(0.f, 0.5));
    hint->setPosition(CCPoint(0.f, 7.f-5*down));
    hint->setColor({255, 255, 0});
    hint->setID("hint");
    menu->addChild(hint);

    menu->updateLayout();
}
/*
void ColorOptionsPopup::addPhaseTogglers(CCNode* parent, const char* text, CCPoint p, int tag, const char *item){
    auto menu = CCMenu::create();
    menu->setPosition(p);
    menu->setContentSize(CCSize(370.f, 30.f));
    menu->setTag(tag);
    parent->addChild(menu);


    auto slider = Slider::create(this, menu_selector(ColorOptionsPopup::onDragSlider), 0.6);
    slider->set
}*/

bool ColorOptionsPopup::setup(){
    // general
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    this->setTitle("More Options");

    // layer
    auto ml = static_cast<CCNode*>(this->getChildren()->objectAtIndex(0));
    ml->setID("main-layer");

    auto background = CCScale9Sprite::create("square02_001.png", CCRect(0.f, 0.f, 80.f, 80.f));
    background->setContentSize(CCSize(390.f, 210.f));
    background->setPosition(CCPoint(winSize.width/2, winSize.height/2 - 10.f));
    background->setOpacity((GLubyte)64);
    background->setID("background");
    ml->addChild(background);

    auto scroll = ScrollLayer::create(CCRect(0.f, 0.f, 380.f, 450.f), true);
	scroll->setPosition(CCPoint(winSize.width/2-190.f, winSize.height/2-110.f));
	scroll->setContentSize(CCSize(380.f, 200.f));
	scroll->setTouchEnabled(true);
	scroll->setID("scroller");
    ml->addChild(scroll);

    auto generalTitle = CCLabelBMFont::create("General Options", "goldFont.fnt", 120.f, CCTextAlignment::kCCTextAlignmentLeft);
    generalTitle->setPosition(CCPoint(10.f, 390.f));
    generalTitle->setAnchorPoint(CCPoint(0.f, 0.5));
    generalTitle->setWidth(360.f);
    generalTitle->setScale(0.7);
    scroll->m_contentLayer->addChild(generalTitle);

    addTextToggler(scroll->m_contentLayer,
        "Switch",
        "Ultimate Switch of this mod.\nYou aren't required to reboot GD to toggle this mod!",
        CCPoint(10.f, 410.f),
        1,
        menu_selector(ColorOptionsPopup::onActivateThisMod),
        Mod::get()->getSavedValue<bool>("activate", true),
        true
    );

    addTextToggler(scroll->m_contentLayer,
        "Same Dual Color Mode",
        "Duplicate Player 1's color setup to Player 2.\nNot fully equal to general Same Dual Color things,\nbut you can also uncheck Seperate Dual Mode Phase if u hope that.",
        CCPoint(10.f, 340.f),
        2,
        menu_selector(ColorOptionsPopup::onCommon),
        Mod::get()->getSavedValue<bool>("same-dual"),
        2
    );

    addTextToggler(scroll->m_contentLayer,
        "Seperate Riders",
        "The cube rider of ship/ufo/jetpack will follow the cube's own color setup.",
        CCPoint(10.f, 280.f),
        3,
        menu_selector(ColorOptionsPopup::onRyder),
        Mod::get()->getSavedValue<bool>("rider")
    );

    addTextToggler(scroll->m_contentLayer,
        "Editor Test",
        "Apply to Editor Playtest. Will also add a button there if activated",
        CCPoint(10.f, 240.f),
        4,
        menu_selector(ColorOptionsPopup::onEditor),
        Mod::get()->getSavedValue<bool>("editor", true)
    );

    auto phaseTitle = CCLabelBMFont::create("Phase Options", "goldFont.fnt", 120.f, CCTextAlignment::kCCTextAlignmentLeft);
    phaseTitle->setPosition(CCPoint(10.f, 160.f));
    phaseTitle->setAnchorPoint(CCPoint(0.f, 0.5));
    phaseTitle->setWidth(360.f);
    phaseTitle->setScale(0.7);
    scroll->m_contentLayer->addChild(phaseTitle);

    addTextToggler(scroll->m_contentLayer,
        "Seperate Dual Mode Phase",
        "Let cycle phase of Player 2 keeps 180 degrees away from phase of Player 1\nOtherwise the two players use the same phase",
        CCPoint(10.f, 110.f),
        5,
        menu_selector(ColorOptionsPopup::onSepDual),
        Mod::get()->getSavedValue<bool>("sep-dual"),
        true
    );

    addTextToggler(scroll->m_contentLayer,
        "Seperate Secondary Color Phase",
        "Let cycle phase of Secondary Color keeps 60 degrees lead.\nOtherwise Secondary Color aligns with Main Color",
        CCPoint(10.f, 60.f),
        6,
        menu_selector(ColorOptionsPopup::onSepSecond),
        Mod::get()->getSavedValue<bool>("sep-second", true),
        true
    );

    addTextToggler(scroll->m_contentLayer,
        "Seperate Glow Color Phase",
        "Let cycle phase of Glow Color keeps 60 degrees lag.\nOtherwise Secondary Color aligns with Main Color",
        CCPoint(10.f, 10.f),
        7,
        menu_selector(ColorOptionsPopup::onSepGlow),
        Mod::get()->getSavedValue<bool>("sep-glow", true),
        true
    );

    auto label = static_cast<CCLabelBMFont*>(scroll->m_contentLayer->getChildByTag(1)->getChildByID("label"));
    label->setColor(Mod::get()->getSavedValue<bool>("activate", true) ? ccColor3B(0, 255, 0) : ccColor3B(255, 0, 0));
    label->setString(Mod::get()->getSavedValue<bool>("activate", true) ? "Switch : ON" : "Switch : OFF");

    auto speedMenu = CCMenu::create();
    //speedMenu->ignoreAnchorPointForPosition(false);
    speedMenu->setPosition(CCPoint(10.f, 175.f));
    speedMenu->setContentSize(CCSize(360.f, 60.f));
    speedMenu->setID("speed-menu");

    auto speedLabel = CCLabelBMFont::create("Varient Speed", "bigFont.fnt", 120.f, CCTextAlignment::kCCTextAlignmentLeft);
    speedLabel->setPosition(CCPoint(0.f, 50.f));
    speedLabel->setAnchorPoint(CCPoint(0.f, 0.5));
    speedLabel->setScale(0.5);
    speedMenu->addChild(speedLabel);

    auto speedHint = CCLabelBMFont::create("Set the cycle speed for chromatic and gradient colors.", "chatFont.fnt", 120.f, CCTextAlignment::kCCTextAlignmentLeft);
    speedHint->setPosition(CCPoint(0.f, 35.f));
    speedHint->setAnchorPoint(CCPoint(0.f, 0.5));
    speedHint->setScale(0.7);
    speedHint->setWidth(360.f);
    speedHint->setColor({255, 255, 0});
    speedMenu->addChild(speedHint);

    auto inputer = TextInput::create(90.f, "1.0");
    inputer->setPosition(CCPoint(45.f, 10.f));
    inputer->setScale(0.8);
    inputer->setFilter("1234567890.");
    inputer->setCallback([this] (auto input) { this->onSpeedTextChanged(input); });
    inputer->setID("text-input");
    speedMenu->addChild(inputer);

    auto slider = Slider::create(this, menu_selector(ColorOptionsPopup::onDragSlider), 0.6);
    slider->setPosition(CCPoint(150.f, 10.f));
    //slider->setAnchorPoint(CCPoint(0.f, 0.f));
    slider->setID("slider");
    speedMenu->addChild(slider);

    auto value = Mod::get()->getSavedValue<float>("speed", 1.0);
    slider->setValue(value/5 < 1 ? value/5 : 1);
    inputer->setString(cocos2d::CCString::createWithFormat("%.2f", static_cast<float>(value))->getCString());
    speedMenu->updateLayout();
    scroll->m_contentLayer->addChild(speedMenu);

    scroll->moveToTop();

    return true;
}

void ColorOptionsPopup::onActivateThisMod(CCObject *sender){
    bool activated = !static_cast<CCMenuItemToggler*>(sender)->isToggled();
    Mod::get()->setSavedValue("activate", activated);
    auto label = static_cast<CCLabelBMFont*>(static_cast<CCMenuItemToggler*>(sender)->getParent()->getChildByID("label"));
    label->setColor(activated ? ccColor3B(0, 255, 0) : ccColor3B(255, 0, 0));
    label->setString(activated ? "Switch : ON" : "Switch : OFF");
}

void ColorOptionsPopup::onCommon(CCObject *sender){
    Mod::get()->setSavedValue("same-dual", !static_cast<CCMenuItemToggler*>(sender)->isToggled());
}

void ColorOptionsPopup::onRyder(CCObject *sender){
    Mod::get()->setSavedValue("rider", !static_cast<CCMenuItemToggler*>(sender)->isToggled());
}


void ColorOptionsPopup::onEditor(CCObject *sender){
    Mod::get()->setSavedValue("editor", !static_cast<CCMenuItemToggler*>(sender)->isToggled());
}

void ColorOptionsPopup::onSepDual(CCObject *sender){
    Mod::get()->setSavedValue("sep-dual", !static_cast<CCMenuItemToggler*>(sender)->isToggled());
}

void ColorOptionsPopup::onSepSecond(CCObject *sender){
    Mod::get()->setSavedValue("sep-second", !static_cast<CCMenuItemToggler*>(sender)->isToggled());
}

void ColorOptionsPopup::onSepGlow(CCObject *sender){
    Mod::get()->setSavedValue("sep-glow", !static_cast<CCMenuItemToggler*>(sender)->isToggled());
}

void ColorOptionsPopup::onSpeedTextChanged(const std::string& text){
    if (text != ""){
        auto slider = static_cast<Slider*>(static_cast<ScrollLayer*>(this->getChildByID("main-layer")->getChildByID("scroller"))->m_contentLayer->getChildByID("speed-menu")->getChildByID("slider"));
        slider->setValue(stof(text)/5 < 1 ? stof(text)/5 : 1);

        Mod::get()->setSavedValue("speed", stof(text));
    }  
}

void ColorOptionsPopup::onDragSlider(CCObject *sender){
    auto speedMenu = static_cast<ScrollLayer*>(this->getChildByID("main-layer")->getChildByID("scroller"))->m_contentLayer->getChildByID("speed-menu");
    auto value = static_cast<Slider*>(speedMenu->getChildByID("slider"))->getValue();
    static_cast<TextInput*>(speedMenu->getChildByID("text-input"))->setString(cocos2d::CCString::createWithFormat("%.2f", static_cast<float>(value * 5))->getCString());
    
    Mod::get()->setSavedValue("speed", value * 5);
}

ColorOptionsPopup * ColorOptionsPopup::create(){
    auto popup = new ColorOptionsPopup();

    if (popup && popup->init(420.f, 260.f)){
        popup->autorelease();
        return popup;
    };

    log::warn("The ColorSelectPopup failed to initialize!");
    CC_SAFE_DELETE(popup);
    return nullptr;
}