#include "IamHead.hpp"

using namespace geode::prelude;

// slightly different with the on for ColorSetupPopup
void ColorOptionsPopup::addTextToggler(CCNode* parent, const char* text, const char* desc, CCPoint p, int tag, cocos2d::SEL_MenuHandler callback, bool yes, bool down=false){
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
    label->setContentSize(CCSize(380.f, 20.f));
    label->setWidth(380.f);
    label->setAnchorPoint(CCPoint(0.f, 0.5));
    label->setPosition(CCPoint(25.f, 27.f));
    menu->addChild(label);

    auto hint = CCLabelBMFont::create(desc, "chatFont.fnt", 120.f, CCTextAlignment::kCCTextAlignmentLeft);
    hint->setScale(0.7);
    hint->setWidth(380.f);
    hint->setAnchorPoint(CCPoint(0.f, 0.5));
    hint->setPosition(CCPoint(0.f, 7.f-5*down));
    hint->setColor({255, 255, 0});
    menu->addChild(hint);

    menu->updateLayout();
}

bool ColorOptionsPopup::setup(){
    // general
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    this->setTitle("More Options");

    // layer
    auto ml = static_cast<CCNode*>(this->getChildren()->objectAtIndex(0));
    ml->setID("main-layer");

    //auto scroll = ScrollLayer::create(CCRect{20.f, 20.f, 380.f, 240.f});
    //ml->addChild(scroll);

    addTextToggler(ml,
        "Switch",
        "Ultimate Switch of this mod.\nYes you aren't required to disable me and reboot GD!",
        CCPoint(winSize.width/2-190.f, winSize.height/2 + 60.f),
        1,
        menu_selector(ColorOptionsPopup::onActivateThisMod),
        Mod::get()->getSavedValue<bool>("activate", true),
        true
    );

    addTextToggler(ml,
        "Same Dual Color",
        "Duplicate Player 1's color setup to Player 2.",
        CCPoint(winSize.width/2-190.f, winSize.height/2 + 10.f),
        2,
        menu_selector(ColorOptionsPopup::onCommon),
        Mod::get()->getSavedValue<bool>("common")
    );
    addTextToggler(ml,
        "Seperate Riders",
        "The cube rider of ship/ufo/jetpack will follow the cube's own color setup.",
        CCPoint(winSize.width/2-190.f, winSize.height/2 - 30.f),
        3,
        menu_selector(ColorOptionsPopup::onRyder),
        Mod::get()->getSavedValue<bool>("rider")
    );
    /*
    addTextToggler(ml,
        "Editor Test",
        "Apply to Editor Test Run. Coming Soon~~~",
        CCPoint(winSize.width/2-190.f, winSize.height/2 - 70.f),
        4,
        menu_selector(ColorOptionsPopup::onEditor),
        Mod::get()->getSavedValue<bool>("editor", true)
    );
    */
    auto speedMenu = CCMenu::create();
    //speedMenu->ignoreAnchorPointForPosition(false);
    speedMenu->setPosition(CCPoint(winSize.width/2 -190.f, winSize.height/2-130.f));
    speedMenu->setContentSize(CCSize(380.f, 50.f));
    speedMenu->setID("speed-menu");

    auto speedLabel = CCLabelBMFont::create("Speed", "bigFont.fnt", 120.f, CCTextAlignment::kCCTextAlignmentLeft);
    speedLabel->setPosition(CCPoint(40.f, 30.f));
    speedLabel->setScale(0.5);
    speedMenu->addChild(speedLabel);

    auto inputer = TextInput::create(90.f, "1.0");
    inputer->setPosition(CCPoint(85.f, 30.f));
    inputer->setScale(0.8);
    inputer->setFilter("1234567890.");
    inputer->setCallback([this] (auto input) { this->onSpeedTextChanged(input); });
    inputer->setID("text-input");
    speedMenu->addChild(inputer);

    auto slider = Slider::create(this, menu_selector(ColorOptionsPopup::onDragSlider), 0.6);
    slider->setPosition(CCPoint(70.f, 10.f));
    //slider->setAnchorPoint(CCPoint(0.f, 0.f));
    slider->setID("slider");
    speedMenu->addChild(slider);

    auto value = Mod::get()->getSavedValue<float>("speed", 1.0);
    slider->setValue(value/5 < 1 ? value/5 : 1);
    inputer->setString(cocos2d::CCString::createWithFormat("%.2f", static_cast<float>(value * 5))->getCString());
    speedMenu->updateLayout();
    ml->addChild(speedMenu);

    return true;
}

void ColorOptionsPopup::onActivateThisMod(CCObject *sender){
    Mod::get()->setSavedValue("activate", !static_cast<CCMenuItemToggler*>(sender)->isToggled());
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

void ColorOptionsPopup::onSpeedTextChanged(const std::string& text){
    if (text != ""){
        auto slider = static_cast<Slider*>(this->getChildByID("main-layer")->getChildByID("speed-menu")->getChildByID("slider"));
        slider->setValue(stof(text)/5 < 1 ? stof(text)/5 : 1);

        Mod::get()->setSavedValue("speed", stof(text));
    }  
}

void ColorOptionsPopup::onDragSlider(CCObject *sender){
    auto speedMenu = this->getChildByID("main-layer")->getChildByID("speed-menu");
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