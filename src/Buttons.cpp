#include <Geode/modify/GJGarageLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>

#include <Geode/loader/Mod.hpp>

#include "IamHead.hpp"

using namespace geode::prelude;

class $modify(IconLayer, GJGarageLayer){
	bool init(){
		if (!GJGarageLayer::init()) return false;

        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto menu = this->getChildByID("shards-menu");
        menu->setContentSize(CCSize(40.f, 260.f));
        menu->setPositionY(winSize.height/2-25.f);

        if (menu){
            auto img = CCSprite::create("rgbicon.png"_spr);
            img->setScale(0.6);
            img->setRotation(10.f);

            auto btn = CircleButtonSprite::create(img, CircleBaseColor::Gray, CircleBaseSize::Small);
            auto button = CCMenuItemSpriteExtra::create(btn, this, menu_selector(IconLayer::onColorButton));//menu_selector(IconLayer::onColorButton)
            button->setID("icon-chroma-button");
            
            menu->addChild(button);
            menu->updateLayout();
        }
        else
            log::warn("The button wants to lay in Garage Menu but he failed to find the shards menu");
		return true;
	}

	void onColorButton(CCObject *sender){
		ColorItemPopup::create()->show();
	}
};

class $modify(PoseLayer, PauseLayer){
	static void onModify(auto& self) {
        self.setHookPriority("PauseLayer::create", -100);}

	static PauseLayer* create(bool p){
		auto pl = PauseLayer::create(p);

        auto winSize = CCDirector::sharedDirector()->getWinSize();
        //button
        auto img = CCSprite::create("rgbicon.png"_spr);
        img->setScale(0.6);
        //img->setRotation(10.f);

        auto button = CCMenuItemSpriteExtra::create(img, pl, menu_selector(PoseLayer::onColorButton));//menu_selector(IconLayer::onColorButton)
        button->setID("icon-chroma-button");

        auto chromaMenu = CCMenu::create();
        chromaMenu->setPosition(CCPoint(0.f, 0.f));
        chromaMenu->addChild(button);
        chromaMenu->updateLayout();

        auto bp = pl->getChildByID("better-pause-node");
        if (bp){
            button->setPosition(CCPoint(winSize.width-30.f, winSize.height-30.f));
            bp->addChild(chromaMenu);
        }
        else
        {
            button->setPosition(CCPoint(winSize.width-30.f, 30.f));
            pl->addChild(chromaMenu);
        }
            
		return pl;
	}

	void onColorButton(CCObject *){
		ColorItemPopup::create()->show();
	}
};
