//
// Created by cpasjuste on 22/11/16.
//

#include <algorithm>

#ifdef __SWITCH__
extern "C" {
#include <switch/services/hid.h>
}
#endif

#include "c2dui.h"

using namespace c2d;
using namespace c2dui;

UIMain::UIMain(const Vector2f &size) : C2DRenderer(size) {

}

UIMain::~UIMain() {
    // ui elements (C2DObject)
    // are deleted by the renderer
}

void UIMain::init(UIRomList *uiRomList, UIMenu *uiMenu,
                  UIEmu *uiEmu, UIStateMenu *uiState) {

    uiHighlight = new UIHighlight();
    skin->loadRectangleShape(uiHighlight, {"SKIN_CONFIG", "HIGHLIGHT"});
    float alpha = uiHighlight->getAlpha();
    if (alpha > 0) {
        uiHighlight->add(new TweenAlpha((float) uiHighlight->getAlpha() * 0.5f,
                                        uiHighlight->getAlpha(), 0.5f, TweenLoop::PingPong));
    }
    uiHighlight->setOrigin(Origin::Center);
    uiHighlight->setLayer(1);
    uiHighlight->setVisibility(Visibility::Hidden);
    add(uiHighlight);

    this->uiRomList = uiRomList;
    this->uiRomList->updateRomList();
    add(this->uiRomList);

    // build menus from options
    this->uiMenu = uiMenu;
    add(this->uiMenu);

    this->uiEmu = uiEmu;
    add(this->uiEmu);

    this->uiState = uiState;
    add(this->uiState);

    // scaling factor mainly used for borders,
    // based on switch resolution..
    scaling = std::min(getSize().x / 1280, 1.0f);
    //printf("scaling: %f\n", scaling);

    uiMessageBox = new MessageBox(
            FloatRect(
                    getSize().x / 2,
                    getSize().y / 2,
                    getSize().x / 2,
                    getSize().y / 2),
            getInput(), skin->font, getFontSize());
    skin->loadRectangleShape(uiMessageBox, {"SKIN_CONFIG", "MESSAGEBOX"});
    uiMessageBox->setOrigin(Origin::Center);
    add(uiMessageBox);

    uiProgressBox = new UIProgressBox(this);
    add(uiProgressBox);

    updateInputMapping(false);
    getInput()->setRepeatDelay(INPUT_DELAY);

}

void UIMain::setConfig(Config *cfg) {
    config = cfg;
}

void UIMain::setSkin(Skin *s) {
    skin = s;
}

bool UIMain::onInput(c2d::Input::Player *players) {
    return Renderer::onInput(players);
}

void UIMain::onDraw(c2d::Transform &transform, bool draw) {

    if (uiEmu && !uiEmu->isVisible()) {
        unsigned int keys = getInput()->getKeys(0);
        if (keys != Input::Key::Delay) {
            if (keys && timer.getElapsedTime().asSeconds() > 5) {
                getInput()->setRepeatDelay(INPUT_DELAY / 12);
            } else if (keys && timer.getElapsedTime().asSeconds() > 3) {
                getInput()->setRepeatDelay(INPUT_DELAY / 8);
            } else if (keys && timer.getElapsedTime().asSeconds() > 1) {
                getInput()->setRepeatDelay(INPUT_DELAY / 4);
            } else if (!keys) {
                getInput()->setRepeatDelay(INPUT_DELAY);
                timer.restart();
            }
        }
    }

    Renderer::onDraw(transform, draw);
}

float UIMain::getScaling() {
    return scaling;
}

Skin *UIMain::getSkin() {
    return skin;
}

Config *UIMain::getConfig() {
    return config;
}

UIHighlight *UIMain::getUiHighlight() {
    return uiHighlight;
}

UIRomList *UIMain::getUiRomList() {
    return uiRomList;
}

UIEmu *UIMain::getUiEmu() {
    return uiEmu;
}

UIMenu *UIMain::getUiMenu() {
    return uiMenu;
}

UIStateMenu *UIMain::getUiStateMenu() {
    return uiState;
}

UIProgressBox *UIMain::getUiProgressBox() {
    return uiProgressBox;
}

MessageBox *UIMain::getUiMessageBox() {
    return uiMessageBox;
}

Font *UIMain::getFont() {
    return skin->font;
}

int UIMain::getFontSize() {
    return C2D_DEFAULT_CHAR_SIZE;
}

void UIMain::updateInputMapping(bool isRomConfig) {

    if (isRomConfig) {
        getInput()->setKeyboardMapping(config->getRomPlayerInputKeys(0));
        int dz = config->get(Option::Index::JOY_DEADZONE, true)->getValueInt();
        for (int i = 0; i < PLAYER_MAX; i++) {
            getInput()->setJoystickMapping(i, config->getRomPlayerInputButtons(i), dz);
            getInput()->players[i].lx.id = config->get(Option::Index::JOY_AXIS_LX, true)->getValueInt();
            getInput()->players[i].ly.id = config->get(Option::Index::JOY_AXIS_LY, true)->getValueInt();
            getInput()->players[i].rx.id = config->get(Option::Index::JOY_AXIS_RX, true)->getValueInt();
            getInput()->players[i].ry.id = config->get(Option::Index::JOY_AXIS_RY, true)->getValueInt();
        }
    } else {
        getInput()->setKeyboardMapping(config->getGuiPlayerInputKeys(0));
        int dz = config->get(Option::Index::JOY_DEADZONE)->getValueInt();
        for (int i = 0; i < PLAYER_MAX; i++) {
            getInput()->setJoystickMapping(i, config->getGuiPlayerInputButtons(i), dz);
            getInput()->players[i].lx.id = config->get(Option::Index::JOY_AXIS_LX)->getValueInt();
            getInput()->players[i].ly.id = config->get(Option::Index::JOY_AXIS_LY)->getValueInt();
            getInput()->players[i].rx.id = config->get(Option::Index::JOY_AXIS_RX)->getValueInt();
            getInput()->players[i].ry.id = config->get(Option::Index::JOY_AXIS_RY)->getValueInt();
        }
    }

#ifdef __SWITCH__
    int single_joy = config->get(Option::Index::JOY_SINGLEJOYCON)->getValueBool();
    if (!hidGetHandheldMode()) {
        if (single_joy > 0) {
            for (int id = 0; id < 8; id++) {
                hidSetNpadJoyAssignmentModeSingleByDefault((HidControllerID) id);
            }
            hidSetNpadJoyHoldType(HidJoyHoldType_Horizontal);
            hidScanInput();
        } else {
            // find all left/right single JoyCon pairs and join them together
            for (int id = 0; id < 8; id++) {
                hidSetNpadJoyAssignmentModeDual((HidControllerID) id);
            }
            int lastRightId = 8;
            for (int id0 = 0; id0 < 8; id0++) {
                if (hidGetControllerType((HidControllerID) id0) & TYPE_JOYCON_LEFT) {
                    for (int id1 = lastRightId - 1; id1 >= 0; id1--) {
                        if (hidGetControllerType((HidControllerID) id1) & TYPE_JOYCON_RIGHT) {
                            lastRightId = id1;
                            // prevent missing player numbers
                            if (id0 < id1) {
                                hidMergeSingleJoyAsDualJoy((HidControllerID) id0, (HidControllerID) id1);
                            } else if (id0 > id1) {
                                hidMergeSingleJoyAsDualJoy((HidControllerID) id1, (HidControllerID) id0);
                            }
                            break;
                        }
                    }
                }
            }
        }
    }
#endif
}
