#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/CCMotionStreak.hpp>

#include "IamHead.hpp"

using namespace geode::prelude;

// from ColorSetupPopup.cpp
extern std::string players[2];
// from ColorSetupPopup.cpp
extern std::string channels[3];
// from ColorSetupPopup.cpp
extern std::string gamemodetag[14];
// from ColorSetupPopup.cpp
extern std::string yesImNoob;
// from ColorSetupPopup.cpp
extern matjson::Value defaultColorSetup;

extern bool refresh;

auto gm = GameManager::sharedState();

// thanks to shadowforce78
void HSVtoRGB(float &fR, float &fG, float &fB, float &fH, float &fS, float &fV) {
  float c = fV * fS;
  float x = static_cast<float>(static_cast<double>(c) * ( 1 - std::abs(std::fmod(fH / 60.0f, 2) - 1)));
  float m = fV - c;

  fR = ( fH < 60.0f )
       ? c
       : ( fH < 120.0f )
         ? x
         : ( fH < 180.0f )
           ? 0
           : ( fH < 240.0f )
             ? 0
             : ( fH < 300.0f )
               ? x
               : c;
  fG = ( fH < 60.0f )
       ? x
       : ( fH < 120.0f )
         ? c
         : ( fH < 180.0f )
           ? c
           : ( fH < 240.0f )
             ? x
             : ( fH < 300.0f )
               ? 0
               : 0;
  fB = ( fH < 60.0f )
       ? 0
       : ( fH < 120.0f )
         ? 0
         : ( fH < 180.0f )
           ? x
           : ( fH < 240.0f )
             ? c
             : ( fH < 300.0f )
               ? c
               : x;

    fR += m;
    fG += m;
    fB += m;
}

void RGBtoHSV(float &fR, float &fG, float &fB, float &fH, float &fS, float &fV){
    float delta;

    if (fR >= fG && fR >= fB){
        fV = fR;
        delta = fG >= fB ? fR-fB : fR-fG;
        fH = 60*fmod((fG-fB)/(delta), 6);
    }
    else if (fG >= fB && fG >= fR){
        fV = fG;
        delta = fB >= fR ? fG-fR : fG-fB;
        fH = 60*((fB-fR)/delta + 2);
    }
    else if (fB >= fR && fB >= fG){
        fV = fB;
        delta = fR >= fG ? fB-fG : fB-fR;
        fH = 60*((fR-fG)/delta + 4);
    }

    fS = fV == 0 ? 0 : delta / fV;
}

cocos2d::_ccColor3B getRainbow(float &phase, float offset, float saturation){
    float R, G, B;

    float hue = fmod(phase + offset, 360);
    float sat = saturation / 100.0;
    float vc = 1;
    HSVtoRGB(R, G, B, hue, sat, vc);

    cocos2d::_ccColor3B out;

    out.r = R * 255;
    out.g = G * 255;
    out.b = B * 255;
    return out;
}

cocos2d::_ccColor3B getGradient(float &phase, float offset, bool smooth, ccColor3B c1, ccColor3B c2){
    float t = fmod(phase + offset, 360);

    //float r1, g1, b1, r2, g2, b2, r, g, b;
    //float h1, s1, v1, h2, s2, v2, h, s, v;

    //r1 = c1.r / 255; g1 = c1.g / 255; b1 = c1.b / 255;
    //r2 = c2.r / 255; g2 = c2.g / 255; b1 = c2.b / 255;

    //RGBtoHSV(r1, g1, b1, h1, s1, v1);
    //RGBtoHSV(r2, g2, b2, h2, s2, v2);


    /*
    float y;
    smooth = false;
    if (smooth)
        // smooth
        y = (t>270)&&(t<90) ? (pow(((t-180)/90 - 1), 2) - 1) : (1 - pow((t/90 - 1), 2));  // wtf
    else{
        y = fabs(t/90-2)-1;
    }
    */
    float y = fabs(t-180)/180;

    //h = h1*(1-y) + h2*y;
    //s = s1*(1-y) + s2*y;
    //v = v1*(1-y) + v2*y;
    //RGBtoHSV(r1, g1, b1, h, s, v);
    //HSVtoRGB(r, g, b, h, s, v);

    //log::debug("rgb  {}  {}  {}", r, g, b);
    //log::debug("hsv  {}  {}  {}", h, s, v);

    cocos2d::_ccColor3B out;

    //out.r = r * 255;
    //out.g = g * 255;
    //out.b = b * 255;

    out.r = c1.r*(1-y) + c2.r*y;
    out.g = c1.g*(1-y) + c2.g*y;
    out.b = c1.b*(1-y) + c2.b*y;
    return out;
}

float phase;
ColorData chroma[22];
// a recorder of their gamemode last frame
int pmode = -1;
// a recorder of their common configs last frame
int pcommon = -1;

/*
class $modify(PlayLayer) {

    bool init(GJGameLevel *level, bool useRep, bool empty){
        if (!PlayLayer::init(level, useRep, empty))
            return false;
        phase = 0;
        return true;
    }

    void postUpdate(float p0) {
	    PlayLayer::postUpdate(p0);
        //plat mode
        bool plat = m_level->isPlatformer();        
        if (Mod::get()->getSavedValue<bool>("activate", true) || refresh){
            // get current phase
            float speed = Mod::get()->getSavedValue<float>("speed", 1.f);
            phase = fmod(phase + speed, 360.f);

            //get settings and chroma!
            if (m_player1)
                chromaPlayer(false, phase, m_player1, m_player2, plat);
            if (m_player2)
                chromaPlayer(true, phase, m_player1, m_player2, plat);
        }
        if (refresh)
            refresh = false;
    }
};
*/
class $modify(MyGJBaseGameLayer, GJBaseGameLayer){
    bool init(){
        if (!GJBaseGameLayer::init())
            return false;
        phase = 0;
        return true;
    }
    void update(float delta) {

        //plat mode
        bool plat = m_level->isPlatformer();
        bool activate = Mod::get()->getSavedValue<bool>("activate", true)
            && (PlayLayer::get() || LevelEditorLayer::get() && Mod::get()->getSavedValue<bool>("editor", true));
        if (activate || refresh){
            // get current phase
            float speed = Mod::get()->getSavedValue<float>("speed", 1.f);
            phase = fmod(phase + 240 * delta * speed, 360.f);

            //get settings and chroma!
            if (m_player1)
                chromaPlayer(false, phase);
            if (m_player2)
                chromaPlayer(true, phase);
        }
        if (refresh)
            refresh = false;

	    GJBaseGameLayer::update(delta);            
    }


    void chromaPlayer(bool p2, float &phase){
        PlayerObject *p = p2 ? m_player2 : m_player1;
        std::string player = (p2 && !Mod::get()->getSavedValue<bool>("same-dual")) ? "P2" : "P1";
        bool yes = Mod::get()->getSavedValue<bool>("activate", true);

        // Step 1: Update plan to get settings
        //log::debug("mode={}", pmode[p2]);
        pcommon = Mod::get()->getSavedValue<int>(player + "-common", -1);
        
        // not common, detect gamemode change
        pmode = detectGamemode(p);

        // Step 2: refresh settings
        // refresh icons config
        int index = (pcommon == -1) ? pmode : pcommon;
        for (int chnl = 0; chnl < 3; chnl ++){
            chroma[chnl + 11*p2] = yes ? from_json(
                Mod::get()->getSavedValue<matjson::Value>(player + "-" + gamemodetag[index] + "-" + channels[chnl],
                defaultColorSetup
            )) : from_json(defaultColorSetup);
            // rider
            chroma[chnl + 3 + 11*p2] = yes ? from_json(
                Mod::get()->getSavedValue<matjson::Value>(player + "-cube-" + channels[chnl],
                defaultColorSetup
            )) : from_json(defaultColorSetup);
        }
        // refresh wave trail config
        //if (pmode[p2] == 4)
        chroma[6 + 11*p2] = yes ? from_json(
            Mod::get()->getSavedValue<matjson::Value>(player + "-trail",
            defaultColorSetup
        )) : from_json(defaultColorSetup);
        // refresh tail config
        chroma[7 + 11*p2] = yes ? from_json(
            Mod::get()->getSavedValue<matjson::Value>(player + "-tail",
            defaultColorSetup
        )) : from_json(defaultColorSetup); 
        
        // debug
        if (refresh){
            refresh = false;
        }
        // Step 3: Apply the current effects and draw the icon
        bool swt;
        bool rider = Mod::get()->getSavedValue<bool>("rider", false);

        // player
        chromaPlayerColor(p, *(chroma + 11*p2), phase, p2, gm->colorForIdx(p2 ? gm->getPlayerColor2() : gm->getPlayerColor()));
        chromaPlayerSecondColor(p, *(chroma + 1 + 11*p2), phase, p2, gm->colorForIdx(p2 ? gm->getPlayerColor() : gm->getPlayerColor2()));

        // glow
        CCObject *obj;
        CCSprite* glow_sprites;
        // why isn't there even a quick function to edit robot/spider glow frame?
        switch (pmode){
        case 5:
            glow_sprites = static_cast<CCSprite*>(static_cast<CCSprite*>(p->m_robotSprite->getChildren()->objectAtIndex(0))->getChildren()->objectAtIndex(0));
            CCARRAY_FOREACH(glow_sprites->getChildren(), obj){
                auto target = static_cast<CCSprite*>(obj);
                chromaProcess(target, *(chroma + 2 + 11*p2), phase, p2, gm->colorForIdx(gm->getPlayerGlowColor()));
            }
            break;
        case 6:
            glow_sprites = static_cast<CCSprite*>(static_cast<CCSprite*>(p->m_spiderSprite->getChildren()->objectAtIndex(0))->getChildren()->objectAtIndex(0));
            CCARRAY_FOREACH(glow_sprites->getChildren(), obj){
                auto target = static_cast<CCSprite*>(obj);
                chromaProcess(target, *(chroma + 2 + 11*p2), phase, p2, gm->colorForIdx(gm->getPlayerGlowColor()));
            }
            break;
        default:
            chromaProcess(p->m_iconGlow, *(chroma + 2 + 11*p2), phase, p2, gm->colorForIdx(gm->getPlayerGlowColor()));
            chromaProcess(p->m_vehicleGlow, *(chroma + 2 + 11*p2), phase, p2, gm->colorForIdx(gm->getPlayerGlowColor()));
        }

        // player with vehicle  
        if (rider && (pmode == 1 || pmode == 3 || pmode == 8)){
            chromaProcess(p->m_iconSprite, *(chroma + 3 + 11*p2), phase, p2, gm->colorForIdx(p2 ? gm->getPlayerColor2() : gm->getPlayerColor()));
            chromaProcess(p->m_iconSpriteSecondary, *(chroma + 4 + 11*p2), phase, p2, gm->colorForIdx(p2 ? gm->getPlayerColor() : gm->getPlayerColor2()));
            chromaProcess(p->m_iconGlow, *(chroma + 5 + 11*p2), phase, p2, gm->colorForIdx(gm->getPlayerGlowColor()));
        }
        // wave trail
        swt = GameManager::get()->getGameVariable("6969");
        chromaProcess(p->m_waveTrail, *(chroma + 6 + 11*p2), phase, p2, gm->colorForIdx((p2 && !swt) || (!p2 && swt) ? gm->getPlayerColor2() : gm->getPlayerColor()));
        chromaProcess(p->m_regularTrail, *(chroma + 5 + 11*p2), phase, p2, gm->colorForIdx((p2 && !swt) || (!p2 && swt) ? gm->getPlayerColor2() : gm->getPlayerColor()));

    }

    // yes detect current gamemode
    int detectGamemode(PlayerObject *p){
        if (p->m_isShip) return m_level->isPlatformer() ? 8 : 1;
        else if (p->m_isBall) return 2;
        else if (p->m_isBird) return 3;
        else if (p->m_isDart) return 4;
        else if (p->m_isRobot) return 5;
        else if (p->m_isSpider) return 6;
        else if (p->m_isSwing) return 7;
        else return 0;
    }

    // chroma option
    void chromaPlayerColor(PlayerObject *sprite, ColorData chroma, float &phase, bool &p2, ccColor3B defaultVal){
        float offset = p2 * 180 * Mod::get()->getSavedValue<bool>("sep-dual");
        switch(chroma.mode){
        case 1:
            sprite->setColor(chroma.color);
            break;   
        case 2:
            sprite->setColor(getRainbow(phase, offset, chroma.type ? 50 : 100));
            break;
        case 3:
            sprite->setColor(getGradient(phase, offset, false, chroma.gradient1, chroma.gradient2));
            break;
        default:
            sprite->setColor(defaultVal);
        }
    }

    void chromaPlayerSecondColor(PlayerObject *sprite, ColorData chroma, float &phase, bool &p2, ccColor3B defaultVal){
        float offset = p2 * 180 * Mod::get()->getSavedValue<bool>("sep-dual") + 60 * Mod::get()->getSavedValue<bool>("sep-second", true);
        switch(chroma.mode){
        case 1:
            sprite->setSecondColor(chroma.color);
            break;   
        case 2:
            sprite->setSecondColor(getRainbow(phase, offset, chroma.type ? 50 : 100));
            break;
        case 3:
            sprite->setSecondColor(getGradient(phase, offset, false, chroma.gradient1, chroma.gradient2));
            break;
        default:
            sprite->setSecondColor(defaultVal);
        }
    }

    void chromaProcess(auto *sprite, ColorData chroma, float &phase, bool &p2, ccColor3B defaultVal){
        float offset = p2 * 180 * Mod::get()->getSavedValue<bool>("sep-dual") - 60 * Mod::get()->getSavedValue<bool>("sep-glow", true);
        switch(chroma.mode){
        case 1:
            sprite->setColor(chroma.color);
            break;
        case 2:
            sprite->setColor(getRainbow(phase, offset, chroma.type ? 50 : 100));
            break;
        case 3:
            sprite->setColor(getGradient(phase, offset, false, chroma.gradient1, chroma.gradient2));
            break;
        default:
            sprite->setColor(defaultVal);
        }
    }
};


