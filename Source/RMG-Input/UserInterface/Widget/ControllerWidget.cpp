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

#include <QPixmap>
#include <QResizeEvent>
#include <QPainter>
#include <SDL.h>

#define SDL_AXIS_PEAK 32768

using namespace UserInterface::Widget;

ControllerWidget::ControllerWidget(QWidget* parent) : QWidget(parent)
{
    this->setupUi(this);

    this->controllerPixmap = QIcon(":Resource/Controller_NoAnalogStick.svg").pixmap(this->size());
    this->deadZoneSlider->setValue(15);
    this->analogStickRangeSlider->setValue(100);
    this->controllerPluggedCheckBox->setChecked(false);

    this->needsControllerImageDraw = true;
    this->DrawControllerImage();
}

ControllerWidget::~ControllerWidget()
{

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

void ControllerWidget::RemoveInputDevice(QString deviceName, int deviceNum)
{
    inputDeviceNameList.removeOne(deviceName);

    for (int i = 0; i < this->inputDeviceComboBox->count(); i++)
    {
        int tmpNum = this->inputDeviceComboBox->itemData(i).toInt();
        QString tmpName = this->inputDeviceComboBox->itemText(i);

        if (tmpName.contains(deviceName) && deviceNum == tmpNum)
        {
            this->inputDeviceComboBox->removeItem(i);
            break;
        }
    }
}

void ControllerWidget::DrawControllerImage()
{
    if (!this->needsControllerImageDraw)
    {
        return;
    }
    else
    { // make sure we don't keep on drawing
        this->needsControllerImageDraw = false;
    }

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

    // render stick with offset specified by controller axis
    QPixmap stickImage = QIcon(":Resource/Controller_AnalogStick.svg").pixmap(controllerPixmapSize);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    int offsetx = 0, offsety = 0;
    static const int width = controllerPixmapSize.width();
    static const int height = controllerPixmapSize.height();
    // considering we're using a vector image,
    // we'll move the analog stick by a percentage
    // of the total width/height from the image
    static const int maxOffsety = ((double)(height * 0.12265f) / 2);
    static const int maxOffsetx = maxOffsety;
    // calculate deadzone value based on user input
    const int deadZoneValue = (SDL_AXIS_PEAK / 100 * deadZoneSlider->value());
    // calculate offset percentages for the image
    const int xOffsetPercentage = (double)((double)xAxisState / SDL_AXIS_PEAK * 100);
    const int yOffsetPercentage = (double)((double)yAxisState / SDL_AXIS_PEAK * 100);

    // make sure we respect the deadzone
#if 0
    if ((abs(xState) > deadZoneValue))
    {
        offsetx = ((float)maxOffsetx / 100 * xOffsetPercentage);
    }
    if ((abs(yState) > deadZoneValue))
    {
        offsety = ((float)maxOffsety / 100 * yOffsetPercentage);
    }
#else
    if (sqrt(pow(xAxisState, 2) + pow(yAxisState, 2)) > deadZoneValue)
    {
        offsetx = ((float)maxOffsetx / 100 * xOffsetPercentage);
        offsety = ((float)maxOffsety / 100 * yOffsetPercentage);
    }
#endif

    painter.drawPixmap(offsetx, offsety, stickImage);
    painter.end();

    this->imageLabel->setPixmap(finalControllerPixmap);
}

void ControllerWidget::ClearControllerImage()
{
    this->controllerImages.clear();
    this->xAxisState = this->yAxisState = 0;
    this->needsControllerImageDraw = true;
    this->DrawControllerImage();
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

#include <iostream>
void ControllerWidget::on_inputDeviceComboBox_currentIndexChanged(int value)
{
    QString deviceName = this->inputDeviceNameList[value];
    int deviceNum = this->inputDeviceComboBox->itemData(value).toInt();

    this->ClearControllerImage();

    emit this->CurrentInputDeviceChanged(deviceName, deviceNum);
}

void ControllerWidget::on_controllerPluggedCheckBox_toggled(bool value)
{
    QWidget* widgetList[] = {
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

    for (auto& widget : widgetList)
    {
        widget->setEnabled(value);
    }

    this->ClearControllerImage();
}


void ControllerWidget::on_setupButton_clicked()
{
}

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
    for (auto& keybinding : keybindings2)
    {
        if (keybinding.button == button)
        {
            if (state && !controllerImages.contains(keybinding.icon))
            {
                controllerImages.append(keybinding.icon);
                this->needsControllerImageDraw = true;
            } else if (!state && controllerImages.contains(keybinding.icon))
            {
                controllerImages.removeOne(keybinding.icon);
                this->needsControllerImageDraw = true;
            }
        }
    }
}

void ControllerWidget::SetAxisState(SDL_GameControllerAxis axis, int16_t state)
{
    switch (axis)
    {
        case SDL_CONTROLLER_AXIS_LEFTX:
            needsControllerImageDraw = true;
            xAxisState = state;
            break;
        case SDL_CONTROLLER_AXIS_LEFTY:
            needsControllerImageDraw = true;
            yAxisState = state;
            break;
        default:
            break;
    }
}

bool ControllerWidget::IsPluggedIn()
{
    return this->controllerPluggedCheckBox->isChecked();
}
