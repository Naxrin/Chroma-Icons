#pragma once
//#ifndef PERSON_H_
//#define PERSON_H_
//#endif

#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/TextInput.hpp>
using namespace geode::prelude;

matjson::Array Color2Array(ccColor3B color);

ccColor3B Array2Color(matjson::Array array);

struct ColorData{
    int mode;
    int type;
    ccColor3B color;
    ccColor3B gradient1;
    ccColor3B gradient2;
};

ColorData from_json(matjson::Value const& value);
matjson::Value to_json(ColorData const& value);

class ColorItemPopup : public Popup<>{
protected:
    bool setup() override;
    void onItemClicked(CCObject*  sender);
    //void onSameDual(CCObject*  sender);
    void onMoreSettings(CCObject *sender);
    void onClose(CCObject *sender);
public:
    static ColorItemPopup* create();
};

class ColorOptionsPopup : public Popup<>{
protected:
    bool setup() override;
    void addTextToggler(CCNode* parent, const char* text, const char* desc, CCPoint p, int tag, cocos2d::SEL_MenuHandler callback, bool yes, bool title);
    void onActivateThisMod(CCObject *sender);
    void onRyder(CCObject *sender);
    void onCommon(CCObject *sender);
    void onEditor(CCObject *sender);
    void onSpeedTextChanged(const std::string & input);
    void onDragSlider(CCObject *sender);
public:
    static ColorOptionsPopup* create();
};

class ColorSetupPopup : public Popup<>{
protected:
    int tag;                  // the identifier of the button clicked in ColorItemPopup.
    int channel;                    // the channel tag of current status
    int gamemode;                   // the item to edit its color, cube, ship, ... wave trail, particles.    
    std::string player;             // player id: P1 or P2

    bool init(int tag);
    bool setup() override;
    void initialSettings();
    void addTextToggler(CCNode* parent, const char* text, CCPoint p, cocos2d::SEL_MenuHandler callback, int tag);
    void setSpeedMenu(float value);
    void onSwitchChannel(CCObject* sender);
    void onSwitchMode(CCObject* sender);
    void onPickColor(CCObject* sender);
    void onSwitchChromaType(CCObject* sender);   
    void onApplyCommon(CCObject* sender);
    void onClose(CCObject *sender);
public:
    struct ColorData data;                 // the color setup struct.
    static ColorSetupPopup * create(int);
};
/*
class MyColorPickPopup : public ColorPickPopup{
protected:
    bool init(ccColor3B color, ColorSetupPopup* summoner);
    ColorSetupPopup* summoner;
    void colorValueChanged(ccColor3B color);
};

public:
    static MyColorPickPopup* create(ccColor3B color, ColorSetupPopup* summoner);
};*/
