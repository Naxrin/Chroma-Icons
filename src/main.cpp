#include <Geode/modify/PlayLayer.hpp>
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
    float fC = fV * fS; // Chroma
    float fHPrime = fmod(fH / 60.0, 6);
    float fX = fC * (1 - fabs(fmod(fHPrime, 2) - 1));
    float fM = fV - fC;

    if (0 <= fHPrime && fHPrime < 1) {
        fR = fC; fG = fX; fB = 0;
    } else if (1 <= fHPrime && fHPrime < 2) {
        fR = fX; fG = fC; fB = 0;
    } else if (2 <= fHPrime && fHPrime < 3) {
        fR = 0;  fG = fC; fB = fX;
    } else if (3 <= fHPrime && fHPrime < 4) {
        fR = 0;  fG = fX; fB = fC;
    } else if (4 <= fHPrime && fHPrime < 5) {
        fR = fX; fG = 0;  fB = fC;
    } else if (5 <= fHPrime && fHPrime < 6) {
        fR = fC; fG = 0;  fB = fX;
    } else {
        fR = 0;  fG = 0;  fB = 0;
    }

    fR += fM;
    fG += fM;
    fB += fM;
}

// thanks to shadowforce78
//float phase = 0;

cocos2d::_ccColor3B getRainbow(float &phase, float offset, float saturation) {
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
    float p = fmod(t, 180);

    cocos2d::_ccColor3B out;
    float y;
    if (smooth)
        // smooth
        y = (t>270)&&(t<90) ? (pow(((t-180)/90 - 1), 2) - 1) : (1 - pow((t/90 - 1), 2));  // wtf
    else{
        y = fabs(t/90-2)-1;
    }
    out.r = (c1.r*(y-1) + c2.r*(1-y))/2;
    out.g = (c1.g*(y-1) + c2.g*(1-y))/2;
    out.b = (c1.b*(y-1) + c2.b*(1-y))/2;
    return out;
}

float phase;
ColorData chroma[22];
// a recorder of their gamemode last frame
int pmode[2] = {-1, -1};
// a recorder of their common configs last frame
int pcommon[2] = {-1, -1};

class $modify(PlayLayer) {
    bool init(GJGameLevel *level, bool useRep, bool empty){
        if (!PlayLayer::init(level, useRep, empty))
            return false;
        phase = 0;
        return true;
    }

    void postUpdate(float p0) {
	    PlayLayer::postUpdate(p0);     
        // main switch
        if (Mod::get()->getSavedValue<bool>("activate", true)){
            // get current phase
            float speed = Mod::get()->getSavedValue<float>("speed", 1.f);
            phase = fmod(phase + speed, 360.f);

            //get settings and chroma!
            if (m_player1)
                chromaPlayer(false, phase);
            if (m_player2)
                chromaPlayer(true, phase);
        }else if (refresh){
            refresh = false;
            // return to default
            if (m_player1){
                m_player1->setColor(gm->colorForIdx(gm->getPlayerColor()));
                m_player1->setSecondColor(gm->colorForIdx(gm->getPlayerColor2()));
                m_player1->m_iconGlow->setColor(gm->colorForIdx(gm->getPlayerGlowColor()));
                m_player1->m_waveTrail->setColor(gm->colorForIdx(GameManager::get()->getGameVariable("6969") ? gm->getPlayerColor2() : gm->getPlayerColor()));
                m_player1->m_regularTrail->setColor(gm->colorForIdx(gm->getPlayerColor()));
            }
            if (m_player2){
                m_player2->setColor(gm->colorForIdx(gm->getPlayerColor2()));
                m_player2->setSecondColor(gm->colorForIdx(gm->getPlayerColor()));
                m_player2->m_iconGlow->setColor(gm->colorForIdx(gm->getPlayerGlowColor()));
                m_player2->m_waveTrail->setColor(gm->colorForIdx(GameManager::get()->getGameVariable("6969") ? gm->getPlayerColor() : gm->getPlayerColor2()));
                m_player2->m_regularTrail->setColor(gm->colorForIdx(gm->getPlayerColor2()));
            }
        }

    }

    void chromaPlayer(bool p2, float &phase){
        PlayerObject *p = p2 ? m_player2 : m_player1;
        std::string player = (p2 && !Mod::get()->getSavedValue<bool>("same-dual")) ? "P2" : "P1";

        // Step 1: Update plan to get settings
        log::debug("mode={}", pmode[p2]);
        pcommon[0] = Mod::get()->getSavedValue<int>("P1-common", -1);
        pcommon[1] = Mod::get()->getSavedValue<int>("P2-common", -1);
        
        // not common, detect gamemode change
        pmode[p2] = detectGamemode(p);

        // Step 2: refresh settings
        // refresh icons config
        int index = (pcommon[p2] == -1) ? pmode[p2] : pcommon[p2];
        for (int chnl = 0; chnl < 3; chnl ++){
            chroma[chnl + 11*p2] = from_json(
                Mod::get()->getSavedValue<matjson::Value>(player + "-" + gamemodetag[index] + "-" + channels[chnl],
                defaultColorSetup
            ));
            // rider
            chroma[chnl + 3 + 11*p2] = from_json(
                Mod::get()->getSavedValue<matjson::Value>(player + "-cube-" + channels[chnl],
                defaultColorSetup
            ));
        }
        // refresh wave trail config
        //if (pmode[p2] == 4)
        chroma[6 + 11*p2] = from_json(
            Mod::get()->getSavedValue<matjson::Value>(player + "-trail",
            defaultColorSetup
        ));
        // refresh tail config
        chroma[7 + 11*p2] = from_json(
            Mod::get()->getSavedValue<matjson::Value>(player + "-tail",
            defaultColorSetup
        )); 
        

        // Step 3: Apply the current effects and draw the icon
        bool swt;
        bool rider = Mod::get()->getSavedValue<bool>("rider", false);

        // player
        chromaPlayer(p, *(chroma + 11*p2), phase, p2, gm->colorForIdx(p2 ? gm->getPlayerColor2() : gm->getPlayerColor()));
        chromaPlayerSecondary(p, *(chroma + 1 + 11*p2), phase, p2, gm->colorForIdx(p2 ? gm->getPlayerColor() : gm->getPlayerColor2()));   
        chromaProcess(p->m_iconGlow,*(chroma + 2 + 11*p2), phase, p2, gm->colorForIdx(gm->getPlayerGlowColor()));
        // player with vehicle  
        if (rider && (pmode[p2] == 1 || pmode[p2] == 3 || pmode[p2] == 8)){
            chromaProcess(p->m_vehicleGlow, *(chroma + 2 + 11*p2), phase, p2, gm->colorForIdx(gm->getPlayerGlowColor()));
            chromaProcess(p->m_iconSprite, *(chroma + 3 + 11*p2), phase, p2, gm->colorForIdx(p2 ? gm->getPlayerColor2() : gm->getPlayerColor()));
            chromaProcess(p->m_iconSpriteSecondary, *(chroma + 4 + 11*p2), phase, p2, gm->colorForIdx(p2 ? gm->getPlayerColor() : gm->getPlayerColor2()));
            chromaProcess(p->m_iconGlow, *(chroma + 5 + 11*p2), phase, p2, gm->colorForIdx(gm->getPlayerGlowColor()));
        }
        // wave trail
        swt = GameManager::get()->getGameVariable("6969");
        chromaProcess(p->m_waveTrail, *(chroma + 6 + 11*p2), phase, p2, gm->colorForIdx((p2 && !swt) || (!p2 && swt) ? gm->getPlayerColor2() : gm->getPlayerColor()));
        chromaProcess(p->m_regularTrail, *(chroma + 5 + 11*p2), phase, p2, gm->colorForIdx((p2 && !swt) || (!p2 && swt) ? gm->getPlayerColor2() : gm->getPlayerColor()));
        /*
        switch (pmode[p2]){
        // ship and ufo
        case 2:
            // vehicle
            chromaProcess(p->m_vehicleSprite, chroma[11*p2], phase, p2, gm->colorForIdx(p2 ? gm->getPlayerColor2() : gm->getPlayerColor()));
            chromaProcess(p->m_vehicleSpriteSecondary, chroma[1 + 11*p2], phase, p2, gm->colorForIdx(p2 ? gm->getPlayerColor() : gm->getPlayerColor2()));
            chromaProcess(p->m_vehicleGlow, chroma[2 + 11*p2], phase, p2, gm->colorForIdx(gm->getPlayerGlowColor()));
            // rider
            chromaProcess(p->m_iconSprite, chroma[3*rider + 11*p2], phase, p2, gm->colorForIdx(p2 ? gm->getPlayerColor2() : gm->getPlayerColor()));
            chromaProcess(p->m_iconSpriteSecondary, chroma[1 + 3*rider + 11*p2], phase, p2, gm->colorForIdx(p2 ? gm->getPlayerColor() : gm->getPlayerColor2()));
            chromaProcess(p->m_iconGlow, chroma[2 + 3*rider + 11*p2], phase, p2, gm->colorForIdx(gm->getPlayerGlowColor()));
            break;
        case 3:
            // vehicle
            chromaProcess(p->m_vehicleSprite, chroma[11*p2], phase, p2, gm->colorForIdx(p2 ? gm->getPlayerColor2() : gm->getPlayerColor()));
            chromaProcess(p->m_vehicleSpriteSecondary, chroma[1 + 11*p2], phase, p2, gm->colorForIdx(p2 ? gm->getPlayerColor() : gm->getPlayerColor2()));
            chromaProcess(p->m_vehicleGlow, chroma[2 + 11*p2], phase, p2, gm->colorForIdx(gm->getPlayerGlowColor()));
            // rider
            chromaProcess(p->m_iconSprite, chroma[3*rider + 11*p2], phase, p2, gm->colorForIdx(p2 ? gm->getPlayerColor2() : gm->getPlayerColor()));
            chromaProcess(p->m_iconSpriteSecondary, chroma[1 + 3*rider + 11*p2], phase, p2, gm->colorForIdx(p2 ? gm->getPlayerColor() : gm->getPlayerColor2()));
            chromaProcess(p->m_iconGlow, chroma[2 + 3*rider + 11*p2], phase, p2, gm->colorForIdx(gm->getPlayerGlowColor()));
            break;
        // robot
        case 5:
            chromaProcess(p->m_robotSprite, chroma[11*p2], phase, p2, gm->colorForIdx(p2 ? gm->getPlayerColor2() : gm->getPlayerColor()));
            //chromaProcess(p->m_iconSprite, chroma[3 + 11*p2], phase, p2, gm->colorForIdx(p2 ? gm->getPlayerColor2() : gm->getPlayerColor()));
            chromaProcess(p->m_iconSpriteSecondary, chroma[1 + 11*p2], phase, p2, gm->colorForIdx(p2 ? gm->getPlayerColor() : gm->getPlayerColor2()));
            chromaProcess(p->m_iconGlow, chroma[2 + 11*p2], phase, p2, gm->colorForIdx(gm->getPlayerGlowColor()));
            break;
        // spider
        case 6:
            chromaProcess(p->m_spiderSprite, chroma[11*p2], phase, p2, gm->colorForIdx(p2 ? gm->getPlayerColor2() : gm->getPlayerColor()));
            //chromaProcess(p->m_iconSprite, chroma[3 + 11*p2], phase, p2, gm->colorForIdx(p2 ? gm->getPlayerColor2() : gm->getPlayerColor()));
            chromaProcess(p->m_iconSpriteSecondary, chroma[1 + 11*p2], phase, p2, gm->colorForIdx(p2 ? gm->getPlayerColor() : gm->getPlayerColor2()));
            chromaProcess(p->m_iconGlow, chroma[2 + 11*p2], phase, p2, gm->colorForIdx(gm->getPlayerGlowColor()));
            break;
        // wave
        case 4:
            swt = GameManager::get()->getGameVariable("6969");
            chromaProcess(p->m_waveTrail, chroma[6 + 11*p2], phase, p2, gm->colorForIdx((p2 && !swt) || (!p2 && swt) ? gm->getPlayerColor2() : gm->getPlayerColor()));
            chromaProcess(p->m_iconSprite, chroma[11*p2], phase, p2, gm->colorForIdx(p2 ? gm->getPlayerColor2() : gm->getPlayerColor()));
            chromaProcess(p->m_iconSpriteSecondary, chroma[1 + 11*p2], phase, p2, gm->colorForIdx(p2 ? gm->getPlayerColor() : gm->getPlayerColor2()));
            chromaProcess(p->m_iconGlow, chroma[2 + 11*p2], phase, p2, gm->colorForIdx(gm->getPlayerGlowColor()));
            break;
        // cube and ball and dart and swing
        default:
            chromaProcess(p->m_iconSprite, chroma[11*p2], phase, p2, gm->colorForIdx(p2 ? gm->getPlayerColor2() : gm->getPlayerColor()));
            chromaProcess(p->m_iconSpriteSecondary, chroma[1 + 11*p2], phase, p2, gm->colorForIdx(p2 ? gm->getPlayerColor() : gm->getPlayerColor2()));
            chromaProcess(p->m_iconGlow, chroma[2 + 11*p2], phase, p2, gm->colorForIdx(gm->getPlayerGlowColor()));
        }*/
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
    void chromaPlayer(PlayerObject *sprite, ColorData chroma, float &phase, bool &p2, ccColor3B defaultVal){
        switch(chroma.mode){
        case 1:
            sprite->setColor(chroma.color);
            break;   
        case 2:
            sprite->setColor(getRainbow(phase, p2 * 180, chroma.type ? 50 : 100));
            break;
        case 3:
            sprite->setColor(getGradient(phase, p2 * 180, false, chroma.gradient1, chroma.gradient2));
            break;
        default:
            sprite->setColor(defaultVal);
        }
    }

    void chromaPlayerSecondary(PlayerObject *sprite, ColorData chroma, float &phase, bool &p2, ccColor3B defaultVal){
        switch(chroma.mode){
        case 1:
            sprite->setSecondColor(chroma.color);
            break;
        case 2:
            sprite->setSecondColor(getRainbow(phase, p2 * 180, chroma.type ? 50 : 100));
            break;
        case 3:
            sprite->setSecondColor(getGradient(phase, p2 * 180, false, chroma.gradient1, chroma.gradient2));
            break;
        default:
            sprite->setSecondColor(defaultVal);
        }
    }

    void chromaProcess(auto *sprite, ColorData chroma, float &phase, bool &p2, ccColor3B defaultVal){
        switch(chroma.mode){
        case 1:
            if (refresh)
                sprite->setColor(chroma.color);
                break;   
        case 2:
            sprite->setColor(getRainbow(phase, p2 * 180, chroma.type ? 50 : 100));
            break;
        case 3:
            sprite->setColor(getGradient(phase, p2 * 180, false, chroma.gradient1, chroma.gradient2));
            break;
        default:
            sprite->setColor(defaultVal);
        }
    }
};


