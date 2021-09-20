/*
 * Rosalie's Mupen GUI - https://github.com/Rosalie241/RMG
 *  Copyright (C) 2020 Rosalie Wanders <rosalie@mailbox.org>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3.
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#include "ControllerWidget.hpp"
#include "SDL_gamecontroller.h"

#include <QPixmap>
#include <QResizeEvent>
#include <QPainter>
#include <SDL.h>
#include <qpixmap.h>

using namespace UserInterface::Widget;

struct tmp
{
    int a;
    int b;
};

static QSize tmpSize;

ControllerWidget::ControllerWidget(QWidget* parent) : QWidget(parent)
{
	this->setupUi(this);

    int w = this->size().width();
    int h = this->size().height();

    tmpSize = this->size();

    this->controllerPixmap = QIcon(":Resource/Controller_NoAnalogStick.svg").pixmap(QSize(w, h));

    this->deadZoneSlider->setValue(25);
    this->analogStickRangeSlider->setValue(100);
    this->controllerPluggedCheckBox->setChecked(false);

    this->drawControllerImage();
}

ControllerWidget::~ControllerWidget()
{

}
#define AXIS_PEAK 32768
#include <iostream>
void ControllerWidget::drawControllerImage()
{
    QPixmap finalControllerPixmap = QPixmap(this->controllerPixmap);
    QSize controllerPixmapSize = this->controllerPixmap.size();
    QPainter painter(&finalControllerPixmap);
    // paint all overlayImages on top of baseImage
    for (auto& buttonImageUri : this->controllerImages)
    {
        QPixmap buttonImage = QIcon(buttonImageUri).pixmap(controllerPixmapSize);
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        painter.drawPixmap(0, 0, buttonImage);
    }
    // render stick w/ offset
    QPixmap stickImage = QIcon(":Resource/Controller_AnalogStick.svg").pixmap(controllerPixmapSize);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    int offsetx = 0, offsety = 0;
    int width = controllerPixmapSize.width();
    int height = controllerPixmapSize.height();
    // normal x=3093.517 y=3509.456
    // bottom center x=3093.517 y=3915.018 (x%=0 y%=10.925)
    // top center x=3093.517 y=3103.893 (x%=0 y%=12.265)
    // right center x=3499.075 y=3509.456 (x%=12.3034 y%=0)
    // left center x=2687.959 y=3509.456 (x%=12.3034, y%=0)

    const int maxOffsety = (int)((double)(height * 0.12265f) / 2);
    const int maxOffsetx = maxOffsety;
    //const int maxOffsetx = (int)((double)(height * 0.123034f) / 2);

    for (auto& axis : currentAxisList)
    {
        int percentage = (double)((float)axis.state / AXIS_PEAK * 100);

        if (axis.axis == SDL_CONTROLLER_AXIS_LEFTX)
        {
            offsetx = ((float)maxOffsetx / 100 * percentage);
        } else if (axis.axis == SDL_CONTROLLER_AXIS_LEFTY)
        {
            offsety = ((float)maxOffsety / 100 * percentage);
        }
    }


    //offsety = (int)((double)(height * 0.123034f) / 2);
    //offsetx = -(int)((double)(width * 0.123034f) / 2);

    std::cout << "width: " << width << ", height: " << height << std::endl;
    std::cout << "offsetx: " << offsetx << ", offsety: " << offsety << std::endl;
    std::cout << offsety << std::endl;
    //offsetx = 

    painter.drawPixmap(offsetx, offsety, stickImage);

    painter.end();

    this->imageLabel->setPixmap(finalControllerPixmap);
}

#include <iostream>

void ControllerWidget::ClearInputDevices()
{
    this->inputDeviceNameList.clear();
    this->inputDeviceComboBox->clear();
}

void ControllerWidget::AddInputDevice(QString deviceName, int deviceNum)
{
    QString name = deviceName;

    if (deviceNum != -1)
    {
        name += " (";
        name += QString::number(deviceNum);
        name += ")";
    }

    this->inputDeviceNameList.append(deviceName);
    this->inputDeviceComboBox->addItem(name, deviceNum);
}

void ControllerWidget::GetCurrentInputDevice(QString& deviceName, int& deviceNum)
{
    int currentIndex = this->inputDeviceComboBox->currentIndex();
    deviceName = this->inputDeviceNameList.at(currentIndex);
    deviceNum = this->inputDeviceComboBox->itemData(currentIndex).toInt();
}

void ControllerWidget::on_deadZoneSlider_valueChanged(int value)
{
    QString title = tr("Deadzone: ");
    title += QString::number(value);
    title += "%";

    this->deadZoneGroupBox->setTitle(title);
}

void ControllerWidget::on_analogStickRangeSlider_valueChanged(int value)
{
    QString text = QString::number(value);
    text += "%";
    this->analogStickRangeLabel->setText(text);
}

void ControllerWidget::on_controllerPluggedCheckBox_toggled(bool value)
{
    QWidget* buttonList[] = {
        // dpad
        this->dpadUpButton,
        this->dpadDownButton,
        this->dpadLeftButton,
        this->dpadRightButton,
        // analog stick
        this->analogStickUpButton,
        this->analogStickDownButton,
        this->analogStickLeftButton,
        this->analogStickRightButton,
        this->analogStickRangeSlider,
        // cbuttons
        this->cbuttonUpButton,
        this->cbuttonDownButton,
        this->cbuttonLeftButton,
        this->cbuttonRightButton,
        // triggers
        this->leftTriggerButton,
        this->rightTriggerButton,
        this->zTriggerButton,
        // buttons
        this->aButton,
        this->bButton,
        this->startButton,
        // misc UI elements
        this->deadZoneSlider,
        this->setupButton,
        this->pushButton_16,
        this->inputDeviceComboBox
    };

    for (auto& widget : buttonList)
    {
        widget->setEnabled(value);
    }
}


void ControllerWidget::on_setupButton_clicked()
{
}

QPixmap createImageWithOverlay(QPixmap& baseImage, QList<QPixmap> overlayImages);

QList<QString> tmpList;

struct
{
    SDL_GameControllerButton button;
    QString icon;
} keybindings2[] = 
{
    { SDL_CONTROLLER_BUTTON_A, ":Resource/Controller_Pressed_A.svg" },
    { SDL_CONTROLLER_BUTTON_B, ":Resource/Controller_Pressed_B.svg" },
    { SDL_CONTROLLER_BUTTON_START, ":Resource/Controller_Pressed_Start.svg" },
  //  { Qt::Key_4, ":Resource/Controller_Pressed_AnalogStick.svg" },
    { SDL_CONTROLLER_BUTTON_DPAD_UP, ":Resource/Controller_Pressed_DpadUp.svg" },
    { SDL_CONTROLLER_BUTTON_DPAD_DOWN, ":Resource/Controller_Pressed_DpadDown.svg" },
    { SDL_CONTROLLER_BUTTON_DPAD_LEFT, ":Resource/Controller_Pressed_DpadLeft.svg" },
    { SDL_CONTROLLER_BUTTON_DPAD_RIGHT, ":Resource/Controller_Pressed_DpadRight.svg" },
  //  { Qt::Key_I, ":Resource/Controller_Pressed_CButtonUp.svg" },
  //  { Qt::Key_K, ":Resource/Controller_Pressed_CButtonDown.svg" },
  //  { Qt::Key_J, ":Resource/Controller_Pressed_CButtonLeft.svg" },
  //  { Qt::Key_L, ":Resource/Controller_Pressed_CButtonRight.svg" },
    { SDL_CONTROLLER_BUTTON_LEFTSHOULDER, ":Resource/Controller_Pressed_LeftTrigger.svg" },
    { SDL_CONTROLLER_BUTTON_RIGHTSHOULDER, ":Resource/Controller_Pressed_RightTrigger.svg" },
};

void ControllerWidget::SetButtonState(SDL_GameControllerButton button, int state)
{
    bool needsDraw = false;

    for (auto& keybinding : keybindings2)
    {
        if (keybinding.button == button)
        {
            if (state && !controllerImages.contains(keybinding.icon))
            {
                controllerImages.append(keybinding.icon);
                needsDraw = true;
            } else if (!state && controllerImages.contains(keybinding.icon))
            {
                controllerImages.removeOne(keybinding.icon);
                needsDraw = true;
            }
        }
    }

    if (needsDraw) {
        this->drawControllerImage();
    }
}

void ControllerWidget::SetAxisState(SDL_GameControllerAxis axis, int16_t state)
{
    axis_t axisType = {axis, state};

    currentAxisList.append(axisType);

    this->drawControllerImage();
}

void ControllerWidget::SetMaxAxis(int16_t max)
{
    this->maxAxis = max;
}

void ControllerWidget::ClearAxisState()
{
    currentAxisList.clear();
}

bool ControllerWidget::IsPluggedIn()
{
    return this->controllerPluggedCheckBox->isChecked();
}
