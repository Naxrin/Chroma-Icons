#include "IamHead.hpp"
//#include <>
using namespace geode::prelude;

//player names:: P1/P2
std::string players[2] = {"P1", "P2"};
// color channel for icons
std::string channels[3] = {"Main", "Secondary", "Glow"};
// gamemodes in string explanation
std::string gamemodetag[14] = {
    "cube", "ship", "ball", "ufo", "wave", "robot", "spider", "swing", "jetpack",
    "trail", "tail", "dashfire", "teleport", "particles"};

//clipboard
ccColor3B clipColor = ccColor3B(0, 0, 0);
bool canPaste = false;

matjson::Array Color2Array(ccColor3B color){
    matjson::Array array;

    array.push_back(color.r);
    array.push_back(color.g);
    array.push_back(color.b);
    return array;
}

ccColor3B Array2Color(matjson::Array array){
    char r = array[0].as_int();
    char g = array[1].as_int();
    char b = array[2].as_int();
    auto color = ccColor3B({(GLubyte)r, (GLubyte)g, (GLubyte)b});
    return color;
}

ColorData from_json(matjson::Value const& value) {
    return ColorData {
        .mode = value["mode"].as_int(),
        .type = value["type"].as_int(),
        .color = Array2Color(value["color"].as_array()),

        .gradient1 = Array2Color(value["gradient"].as_array()[0].as_array()),
        .gradient2 = Array2Color(value["gradient"].as_array()[1].as_array()),
    };
}

matjson::Value to_json(ColorData const& value) {
    auto obj = matjson::Value();

    obj["mode"] = value.mode;
    obj["type"] = value.type;
    obj["color"] = Color2Array(value.color);

    matjson::Array gradients;
    gradients.push_back(Color2Array(value.gradient1));
    gradients.push_back(Color2Array(value.gradient2));
    obj["gradient"] = gradients;

    return obj;
}

matjson::Value defaultColorSetup = to_json(ColorData{
    .mode = 0,
    .type = 0,
    .color = ccColor3B({255, 255, 255}),
    .gradient1 = ccColor3B({255, 255, 255}),
    .gradient2 = ccColor3B({255, 255, 255})
});
        
void ColorSetupPopup::addTextToggler(CCNode* parent, const char* text, CCPoint p, cocos2d::SEL_MenuHandler callback, int tag){
    auto menu = CCMenu::create();

    auto toggler = CCMenuItemToggler::createWithStandardSprites(this, callback, 0.5);
    toggler->setPosition(CCPoint(10.f, 10.f));
    toggler->setID("toggler");
    menu->addChild(toggler);

    auto label = CCLabelBMFont::create(text, "bigFont.fnt", 120.f, CCTextAlignment::kCCTextAlignmentLeft);
    label->setScale(0.4);
    label->setAnchorPoint(CCPoint(0.f, 0.5));
    label->setPosition(CCPoint(25.f, 10.f));
    menu->addChild(label);

    float width = label->getContentWidth();
    menu->setTag(tag);
    menu->setPosition(p);
    menu->setContentSize(CCSize(0.4*width + 25.f, 20.f));
    parent->addChild(menu);

    menu->updateLayout();
}

void ColorSetupPopup::addColorSet(CCNode* parent, const char* text, CCPoint p, int tag){
    auto menu = CCMenu::create();

    auto label = CCLabelBMFont::create(text, "bigFont.fnt", 200.f, CCTextAlignment::kCCTextAlignmentLeft);
    label->setAnchorPoint(CCPoint(0.f, 0.5));
    label->setScale(0.4);
    label->setPosition(CCPoint(0.f, 35.f));
    label->setID("label");
    menu->addChild(label);

    auto colorImg = CCSprite::createWithSpriteFrameName("GJ_colorBtn_001.png");
    colorImg->setScale(0.6);
    colorImg->setID("sprite");
    auto colorBtn = CCMenuItemSpriteExtra::create(colorImg, this, menu_selector(ColorSetupPopup::onPickColor));
    colorBtn->setPosition(CCPoint(10.f, 10.f));
    colorBtn->setID("button");
    menu->addChild(colorBtn);

    auto copyImg = CCSprite::createWithSpriteFrameName("GJ_copyBtn_001.png");
    copyImg->setScale(0.5);
    auto copyBtn = CCMenuItemSpriteExtra::create(copyImg, this, menu_selector(ColorSetupPopup::onCopyColor));
    copyBtn->setPosition(CCPoint(40.f, 10.f));
    copyBtn->setID("copy");
    menu->addChild(copyBtn);

    auto pasteImg = CCSprite::createWithSpriteFrameName("GJ_pasteBtn_001.png");
    pasteImg->setScale(0.5);
    auto pasteBtn = CCMenuItemSpriteExtra::create(pasteImg, this, menu_selector(ColorSetupPopup::onPasteColor));
    pasteBtn->setPosition(CCPoint(70.f, 10.f));
    pasteBtn->setID("paste");
    menu->addChild(pasteBtn);

    menu->setTag(tag);
    menu->setPosition(p);
    menu->setContentSize(CCSize(60.f, 50.f));
    parent->addChild(menu);
    menu->updateLayout();
}

bool ColorSetupPopup::setup() {
    // general
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    this->setTitle("Pick A Sexy Color Mode!");

    // common used status
    player = tag < 14 ? "P1" : "P2";
    gamemode = tag % 14;
    channel = 0;

    // layer
    auto ml = static_cast<CCNode*>(this->getChildren()->objectAtIndex(0));
    ml->setID("main-layer");
    
    // channel select if modifying icons
    auto chnlMenu = CCMenu::create();
    chnlMenu->setPosition(CCPoint(winSize.width/2 - 190.f, winSize.height/2 - 110.f));
    chnlMenu->setContentSize(CCSize(120.f, 80.f));
    chnlMenu->setID("color-channel-menu");
    ml->addChild(chnlMenu);

    auto chnlBG = CCScale9Sprite::create("square02_001.png", CCRect(0.f, 0.f, 80.f, 80.f));
    chnlBG->setContentSize(CCSize(130.f, 90.f));
    chnlBG->setPosition(CCPoint(-5.f, -5.f));
    chnlBG->setAnchorPoint(CCPoint(0.f, 0.f));
    chnlBG->setOpacity((GLubyte)64);
    chnlBG->setID("background");
    chnlMenu->addChild(chnlBG);

    auto chnlTitle = CCLabelBMFont::create("Color Channel", "goldFont.fnt", 120.f, CCTextAlignment::kCCTextAlignmentLeft);
    chnlTitle->setAnchorPoint(CCPoint(0.f, 0.5f));
    chnlTitle->setPosition(CCPoint(5.f, 70.f));
    chnlTitle->setScale(0.6);
    chnlMenu->addChild(chnlTitle);

    const char* texts[3] = {"Main", "Secondary", "Glow"};
    
    for (int chnl = 0; chnl < 3; chnl ++){
        addTextToggler(
            chnlMenu,
            texts[chnl],
            CCPoint(0.f, 40.f-20.f*chnl),
            menu_selector(ColorSetupPopup::onSwitchChannel),
            chnl
        );
    }
    chnlMenu->setVisible(gamemode < 9); // only icons  
    chnlMenu->updateLayout();

    // mode menu
    auto modeMenu = CCMenu::create();
    modeMenu->setPosition(CCPoint(winSize.width/2 - 190.f, winSize.height/2 - 10.f));
    modeMenu->setContentSize(CCSize(120.f, 100.f));
    modeMenu->setID("color-mode-menu");
    ml->addChild(modeMenu);

    auto modeBG = CCScale9Sprite::create("square02_001.png", CCRect(0.f, 0.f, 80.f, 80.f));
    modeBG->setContentSize(CCSize(130.f, 110.f));
    modeBG->setPosition(CCPoint(-5.f, -5.f));
    modeBG->setAnchorPoint(CCPoint(0.f, 0.f));
    modeBG->setOpacity((GLubyte)64);
    modeBG->setID("background");
    modeMenu->addChild(modeBG);

    auto modeTitle = CCLabelBMFont::create("Color Mode", "goldFont.fnt", 120.f, CCTextAlignment::kCCTextAlignmentLeft);
    modeTitle->setAnchorPoint(CCPoint(0.f, 0.5f));
    modeTitle->setPosition(CCPoint(5.f, 90.f));
    modeTitle->setScale(0.6);
    modeMenu->addChild(modeTitle);

    char const* modes[4] = {"Default", "Static", "Chromatic", "Gradient"};
    for (int mode = 0; mode < 4; mode ++){
        addTextToggler(
            modeMenu,
            modes[mode],
            CCPoint(0.f, 60.f-20.f*mode),
            menu_selector(ColorSetupPopup::onSwitchMode),
            mode
        );
    }

    // WORKPLACE
    auto setupMenu = CCMenu::create();
    setupMenu->setID("setup-menu");
    setupMenu->setPosition(CCPoint(winSize.width/2 - 50.f, winSize.height/2 - 80.f));
    setupMenu->setContentSize(CCSize(240.f, 170.f));
    ml->addChild(setupMenu);

    auto setupBG = CCScale9Sprite::create("square02_001.png", CCRect(0.f, 0.f, 80.f, 80.f));
    setupBG->setContentSize(CCSize(250.f, 180.f));
    setupBG->setPosition(CCPoint(-5.f, -5.f));
    setupBG->setAnchorPoint(CCPoint(0.f, 0.f));
    setupBG->setOpacity((GLubyte)64);
    setupBG->setID("background");
    setupMenu->addChild(setupBG);

    auto dynamicTitle = CCLabelBMFont::create("Color Mode", "goldFont.fnt", 120.f, CCTextAlignment::kCCTextAlignmentLeft);
    dynamicTitle->setAnchorPoint(CCPoint(0.f, 0.5));
    dynamicTitle->setScale(0.6);
    dynamicTitle->setPosition(CCPoint(5.f, 160.f));
    dynamicTitle->setID("setup-title");
    setupMenu->addChild(dynamicTitle);

    // default
    auto defaultPart = CCMenu::create();
    defaultPart->setID("Default");
    defaultPart->setPosition(CCPoint(0.f, 0.f));
    defaultPart->setContentSize(CCSize(240.f, 170.f));
    setupMenu->addChild(defaultPart);

    auto defaultFont = CCLabelBMFont::create("No any effect\nIsn't it cool enough?", "bigFont.fnt", 200.f, CCTextAlignment::kCCTextAlignmentLeft);
    defaultFont->setAnchorPoint(CCPoint(0.f, 0.5));
    defaultFont->setScale(0.4);
    defaultFont->setPosition(CCPoint(5.f, 130.f));
    defaultFont->setWidth(230.f);
    defaultPart->addChild(defaultFont); 

    // static
    auto staticPart = CCMenu::create();
    staticPart->setID("Static");
    staticPart->setPosition(CCPoint(0.f, 0.f));
    staticPart->setContentSize(CCSize(240.f, 170.f));
    setupMenu->addChild(staticPart);

    addColorSet(
        staticPart,
        "Pick your fav color!",
        CCPoint(10.f, 100.f),
        0
    );

    // chromatic
    auto chromaPart = CCMenu::create();
    chromaPart->setID("Chromatic");
    chromaPart->setPosition(CCPoint(0.f, 0.f));
    chromaPart->setContentSize(CCSize(240.f, 170.f));
    setupMenu->addChild(chromaPart);

    auto chromaticFont = CCLabelBMFont::create("CHROMATIC!", "bigFont.fnt", 120.f, CCTextAlignment::kCCTextAlignmentLeft);
    chromaticFont->setAnchorPoint(CCPoint(0.f, 0.5));
    chromaticFont->setScale(0.4);
    chromaticFont->setPosition(CCPoint(5.f, 130.f));
    chromaticFont->setWidth(230.f),
    chromaPart->addChild(chromaticFont);

    auto chromaticType = CCLabelBMFont::create("Select Type", "goldFont.fnt", 120.f, CCTextAlignment::kCCTextAlignmentLeft);
    chromaticType->setAnchorPoint(CCPoint(0.f, 0.5));
    chromaticType->setScale(0.5);
    chromaticType->setPosition(CCPoint(5.f, 100.f));
    chromaticType->setWidth(230.f),
    chromaPart->addChild(chromaticType);

    addTextToggler(
        chromaPart,
        "RGB",
        CCPoint(0.f, 70.f),
        menu_selector(ColorSetupPopup::onSwitchChromaType),
        0
    );
    addTextToggler(
        chromaPart,
        "Pastel",
        CCPoint(0.f, 50.f),
        menu_selector(ColorSetupPopup::onSwitchChromaType),
        1
    );

    // gradient
    auto gradientPart = CCMenu::create();
    gradientPart->setID("Gradient");
    gradientPart->setPosition(CCPoint(0.f, 0.f));
    gradientPart->setContentSize(CCSize(240.f, 170.f));
    setupMenu->addChild(gradientPart);

    auto gradientFont = CCLabelBMFont::create("Pick your fav colors to cycle!", "bigFont.fnt", 120.f, CCTextAlignment::kCCTextAlignmentLeft);
    gradientFont->setAnchorPoint(CCPoint(0.f, 0.5));
    gradientFont->setScale(0.4);
    gradientFont->setPosition(CCPoint(5.f, 130.f));
    gradientFont->setWidth(230.f);
    //gradientFont->setTag(3);
    gradientPart->addChild(gradientFont);

    addColorSet(
        gradientPart,
        "Gradient Color 1",
        CCPoint(10.f, 70.f),
        1
    );

    addColorSet(
        gradientPart,
        "Gradient Color 2",
        CCPoint(10.f, 20.f),
        2
    );
    
    // additional cube menu
    auto commonMenu = CCMenu::create();
    commonMenu->setPosition(CCPoint(winSize.width/2 - 50.f, winSize.height/2 -115.f));
    commonMenu->setContentSize(CCSize(250.f, 20.f));
    commonMenu->setID("additional-menu");
    ml->addChild(commonMenu);

    addTextToggler(
        commonMenu,
        "Apply to Common",
        CCPoint(10.f, 0.f),
        menu_selector(ColorSetupPopup::onApplyCommon),
        10
    );
    
    auto confirmImg = ButtonSprite::create("Apply", "goldFont.fnt", "GJ_button_01.png", 0.8);
    confirmImg->setScale(0.8);
    auto confirmBtn = CCMenuItemSpriteExtra::create(confirmImg, this, menu_selector(ColorSetupPopup::onClose));
    confirmBtn->setPosition(CCPoint(215.f, 10.f));
    commonMenu->addChild(confirmBtn);

    commonMenu->getChildByTag(10)->setVisible(gamemode < 9); // only gamemode
    static_cast<CCMenuItemToggler*>(commonMenu->getChildByTag(10)->getChildByID("toggler"))->toggle(Mod::get()->getSavedValue<int64_t>(player + "-common") == gamemode);

    // tell the user what does Apply to Common means
    auto infoImg = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    infoImg->setScale(0.5);
    auto infoBtn = CCMenuItemSpriteExtra::create(infoImg, this, menu_selector(ColorSetupPopup::onDesc));
    infoBtn->setPosition(CCPoint(-5.f, 20.f));
    commonMenu->getChildByTag(10)->addChild(infoBtn);
    commonMenu->updateLayout();

    
    // get settings and initialize
    std::string settingKey = player + "-" + gamemodetag[gamemode] + (gamemode < 9 ? "-" + channels[channel] : "");
    data = from_json(Mod::get()->getSavedValue<matjson::Value>(settingKey, defaultColorSetup));

    // initialize
    static_cast<CCMenuItemToggler*>(chnlMenu->getChildByTag(channel)->getChildByID("toggler"))->toggle(true);
    // chroma mode
    static_cast<CCMenuItemToggler*>(modeMenu->getChildByTag(data.mode)->getChildByID("toggler"))->toggle(true);

    initialSettings();

    return true;
}

void ColorSetupPopup::initialSettings(){
    // main layer
    auto ml = this->getChildByID("main-layer");
    
    // switch to workplace to current chroma mode
    CCObject* obj;
    char const* modes[4] = {"Default", "Static", "Chromatic", "Gradient"};

    CCARRAY_FOREACH(ml->getChildByID("setup-menu")->getChildren(), obj){
        CCNode* node = static_cast<CCNode*>(obj);
        node->setVisible(node->getID() == modes[data.mode] || node->getID() == "setup-title" || node->getID() == "background");
    }
    static_cast<CCLabelBMFont*>(ml->getChildByID("setup-menu")->getChildByID("setup-title"))->setString(modes[data.mode]);

    // chroma type
    static_cast<CCMenuItemToggler*>(ml->getChildByID("setup-menu")->getChildByID("Chromatic")->getChildByTag(data.type)->getChildByID("toggler"))->toggle(true);
    static_cast<CCMenuItemToggler*>(ml->getChildByID("setup-menu")->getChildByID("Chromatic")->getChildByTag(!data.type)->getChildByID("toggler"))->toggle(false);
    // color thing
    static_cast<CCSprite*>(ml->getChildByID("setup-menu")->getChildByID("Static")->getChildByTag(0)->getChildByID("button")->getChildByID("sprite"))->setColor(data.color);
    static_cast<CCSprite*>(ml->getChildByID("setup-menu")->getChildByID("Gradient")->getChildByTag(1)->getChildByID("button")->getChildByID("sprite"))->setColor(data.gradient1);
    static_cast<CCSprite*>(ml->getChildByID("setup-menu")->getChildByID("Gradient")->getChildByTag(2)->getChildByID("button")->getChildByID("sprite"))->setColor(data.gradient2);
    
    static_cast<CCMenu*>(this->getChildByID("main-layer")->getChildByID("color-mode-menu")->getChildByTag(data.mode))->updateLayout();
    updateLayout();
}

void ColorSetupPopup::onSwitchChannel(CCObject *sender){
    auto setupMenu = this->getChildByID("main-layer")->getChildByID("setup-menu");
    data.color = static_cast<CCSprite*>(setupMenu->getChildByID("Static")->getChildByTag(0)->getChildByID("button")->getChildByID("sprite"))->getColor();
    data.gradient1 = static_cast<CCSprite*>(setupMenu->getChildByID("Gradient")->getChildByTag(1)->getChildByID("button")->getChildByID("sprite"))->getColor();
    data.gradient2 = static_cast<CCSprite*>(setupMenu->getChildByID("Gradient")->getChildByTag(2)->getChildByID("button")->getChildByID("sprite"))->getColor();
    // write settings
    std::string settingKey = player + "-" + gamemodetag[gamemode] + "-" + channels[channel];
    Mod::get()->setSavedValue<matjson::Value>(settingKey, to_json(data));

    // toggle off the old channel toggler
    static_cast<CCMenuItemToggler*>(this->getChildByID("main-layer")->getChildByID("color-channel-menu")->getChildByTag(channel)->getChildByID("toggler"))->toggle(false);
    // toggle off the old chroma mode menu
    static_cast<CCMenuItemToggler*>(this->getChildByID("main-layer")->getChildByID("color-mode-menu")->getChildByTag(data.mode)->getChildByID("toggler"))->toggle(false);

    // new channel tag
    channel = static_cast<CCNode*>(sender)->getParent()->getTag();  
    // fetch new settings
    settingKey = player + "-" + gamemodetag[gamemode] + "-" + channels[channel];
    data = from_json(Mod::get()->getSavedValue<matjson::Value>(settingKey, defaultColorSetup));

    // pretend the new mode button is clicked
    // toggle off the old chroma mode menu
    static_cast<CCMenuItemToggler*>(this->getChildByID("main-layer")->getChildByID("color-mode-menu")->getChildByTag(data.mode)->getChildByID("toggler"))->toggle(true);
    static_cast<CCMenu*>(this->getChildByID("main-layer")->getChildByID("color-mode-menu")->getChildByTag(data.mode))->updateLayout();
    // refresh
    initialSettings();
}

void ColorSetupPopup::onSwitchMode(CCObject *sender){
    // main layer
    auto ml = this->getChildByID("main-layer");

    // toggle off the former
    static_cast<CCMenuItemToggler*>(ml->getChildByID("color-mode-menu")->getChildByTag(data.mode)->getChildByID("toggler"))->toggle(false);
    // new
    data.mode = static_cast<CCNode*>(sender)->getParent()->getTag();

    // switch to workplace to current chroma mode
    CCObject* obj;
    char const* modes[4] = {"Default", "Static", "Chromatic", "Gradient"};

    CCARRAY_FOREACH(ml->getChildByID("setup-menu")->getChildren(), obj){
        CCNode* node = static_cast<CCNode*>(obj);
        node->setVisible(node->getID() == modes[data.mode] || node->getID() == "setup-title" || node->getID() == "background");
    }
    static_cast<CCLabelBMFont*>(ml->getChildByID("setup-menu")->getChildByID("setup-title"))->setString(modes[data.mode]);
}

void ColorSetupPopup::onSwitchChromaType(CCObject *sender){
    // toggle on the new
    auto menu = this->getChildByID("main-layer")->getChildByID("setup-menu")->getChildByID("Chromatic");
    static_cast<CCMenuItemToggler*>(menu->getChildByTag(data.type)->getChildByID("toggler"))->toggle(false);

    // edit setup
    data.type = static_cast<CCNode*>(sender)->getParent()->getTag();
}

void ColorSetupPopup::onPickColor(CCObject *sender){
    // HOW TO SET COLOR?
    auto color = static_cast<CCSprite*>(static_cast<CCMenuItemSpriteExtra*>(sender)->getChildren()->objectAtIndex(0))->getColor();
    auto popup = ColorPickPopup::create(color);
    popup->setColorTarget(static_cast<CCSprite*>(static_cast<CCNode*>(sender)->getChildren()->objectAtIndex(0)));
    popup->show();
}

void ColorSetupPopup::onApplyCommon(CCObject *sender){
    // setup
    bool check = static_cast<CCMenuItemToggler*>(sender)->isToggled();
    Mod::get()->setSavedValue(player + "-common", check ? -1 : gamemode);
}

void ColorSetupPopup::onDesc(CCObject *sender){
    FLAlertLayer::create(
        "Apply to Common",
        "Apply this color mode to All Gamemodes of this player.\nWill not merge options of other gamemodes,\nonly affect in-game visual effects.",
        "OK")->show();
}

void ColorSetupPopup::onCopyColor(CCObject *sender){
    auto source = static_cast<CCNode*>(sender)->getParent()->getChildByID("button")->getChildByID("sprite");
    clipColor = static_cast<CCSprite*>(source)->getColor();
    canPaste = true;
}

void ColorSetupPopup::onPasteColor(CCObject *sender){
    if (canPaste){
        auto target = static_cast<CCNode*>(sender)->getParent()->getChildByID("button")->getChildByID("sprite");
        static_cast<CCSprite*>(target)->setColor(clipColor);
    }
    else{
        FLAlertLayer::create(
            "Cannot Paste",
            "You did not ever copied a color and the clipboard is now empty!",
            "Alright")->show();
    }

}

bool ColorSetupPopup::init(int tag){
    this->tag = tag;
    if (!Popup::init(420.f, 260.f))
        return false;
    return true;
    
}

ColorSetupPopup * ColorSetupPopup::create(int tag){
    auto popup = new ColorSetupPopup();
    if (popup && popup->init(tag)){
        popup->autorelease();
        return popup;
    };

    log::warn("The ColorSelectPopup failed to initialize!");
    CC_SAFE_DELETE(popup);
    return nullptr;
}

void ColorSetupPopup::onClose(CCObject *sender){
    auto setupMenu = this->getChildByID("main-layer")->getChildByID("setup-menu");
    data.color = static_cast<CCSprite*>(setupMenu->getChildByID("Static")->getChildByTag(0)->getChildByID("button")->getChildByID("sprite"))->getColor();
    data.gradient1 = static_cast<CCSprite*>(setupMenu->getChildByID("Gradient")->getChildByTag(1)->getChildByID("button")->getChildByID("sprite"))->getColor();
    data.gradient2 = static_cast<CCSprite*>(setupMenu->getChildByID("Gradient")->getChildByTag(2)->getChildByID("button")->getChildByID("sprite"))->getColor();
    
    // write settings
    std::string settingKey = player + "-" + gamemodetag[gamemode] + (gamemode < 9 ? "-" + channels[channel] : "");
    Mod::get()->setSavedValue<matjson::Value>(settingKey, to_json(data));

    Popup::onClose(sender);
}