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
#include <QSvgRenderer>
#include <iostream>

#define SDL_AXIS_PEAK 32768

using namespace UserInterface::Widget;

ControllerWidget::ControllerWidget(QWidget* parent) : QWidget(parent)
{
    this->setupUi(this);

    this->deadZoneSlider->setValue(15);
    this->analogStickRangeSlider->setValue(100);
    this->controllerPluggedCheckBox->setChecked(false);

    this->DrawControllerImage();

    this->initializeButtons();
}

ControllerWidget::~ControllerWidget()
{

}

void ControllerWidget::initializeButtons()
{
    CustomButton* buttonList[] =
    {
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
    };

    for (auto& button : buttonList)
    {
        button->Initialize(this);
        button->setText(" ");
    }
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
    this->controllerImageWidget->UpdateImage();
}

void ControllerWidget::ClearControllerImage()
{
    this->controllerImageWidget->ClearControllerState();
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
    this->controllerImageWidget->SetDeadzone(value);
}

void ControllerWidget::on_analogStickRangeSlider_valueChanged(int value)
{
    QString text = QString::number(value);
    text += "%";
    this->analogStickRangeLabel->setText(text);
}

void ControllerWidget::on_inputDeviceComboBox_currentIndexChanged(int value)
{
    QString deviceName = this->inputDeviceNameList[value];
    int deviceNum = this->inputDeviceComboBox->itemData(value).toInt();

    this->ClearControllerImage();

    emit this->CurrentInputDeviceChanged(deviceName, deviceNum);
}

void ControllerWidget::on_inputDeviceRefreshButton_clicked()
{
    emit this->RefreshInputDevicesButtonClicked();
}

void ControllerWidget::on_controllerPluggedCheckBox_toggled(bool value)
{
    QWidget* widgetList[] =
    {
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
        this->inputDeviceComboBox,
        this->inputDeviceRefreshButton
    };

    for (auto& widget : widgetList)
    {
        widget->setEnabled(value);
    }

    this->ClearControllerImage();
}

void ControllerWidget::on_setupButton_clicked()
{
    CustomButton* buttonList[] =
    {
        this->startButton,
    };

    for (auto& button : buttonList)
    {
        button->setFocus(Qt::OtherFocusReason);
        //button->activateWindow();
        button->click();
    }
}

void ControllerWidget::on_CustomButton_released(CustomButton* button)
{
    if (this->currentButton != nullptr)
    {
        this->currentButton->StopTimer();
        this->currentButton->setText(" ");
    }

    this->currentButton = button;
    button->StartTimer();
}

void ControllerWidget::on_CustomButton_TimerFinished(CustomButton* button)
{
    if (this->currentButton == button)
    {
        this->currentButton = nullptr;
    }

    QString text = " ";

    SDL_GameControllerButton buttonNum = (SDL_GameControllerButton)button->GetButton();
    if ((int)buttonNum != -1)
    {
        text = SDL_GameControllerGetStringForButton(buttonNum);
    }

    button->setText(text);
}

void ControllerWidget::SetButtonState(SDL_GameControllerButton sdlButton, int state)
{
    if (this->currentButton != nullptr)
    {
        if (state)
        {
            this->currentButton->StopTimer();
            this->currentButton->SetButton(sdlButton);
            this->currentButton->setText(SDL_GameControllerGetStringForButton(sdlButton));
            this->currentButton = nullptr;
        }
        return;
    }

    const struct
    {
         enum N64CONTROLLER_BUTTON button;
        CustomButton* buttonWidget;
    } buttons[] =
    {
        { N64CONTROLLER_BUTTON_A, this->aButton, },
        { N64CONTROLLER_BUTTON_B, this->bButton, },
        { N64CONTROLLER_BUTTON_START, this->startButton },
        { N64CONTROLLER_BUTTON_DPAD_UP, this->dpadUpButton },
        { N64CONTROLLER_BUTTON_DPAD_DOWN, this->dpadDownButton },
        { N64CONTROLLER_BUTTON_DPAD_LEFT, this->dpadLeftButton },
        { N64CONTROLLER_BUTTON_DPAD_RIGHT, this->dpadRightButton },
        { N64CONTROLLER_BUTTON_CBUTTONS_UP, this->cbuttonUpButton },
        { N64CONTROLLER_BUTTON_CBUTTONS_DOWN, this->cbuttonDownButton },
        { N64CONTROLLER_BUTTON_CBUTTONS_LEFT, this->cbuttonLeftButton  },
        { N64CONTROLLER_BUTTON_CBUTTONS_RIGHT, this->cbuttonRightButton },
        { N64CONTROLLER_BUTTON_LEFTTRIGGER, this->leftTriggerButton },
        { N64CONTROLLER_BUTTON_RIGHTTRIGGER, this->rightTriggerButton },
    };

    for (auto& button : buttons)
    {
        if (button.buttonWidget->GetButton() == sdlButton)
        {
            this->controllerImageWidget->SetButtonState(button.button, state);
        }
    }
}

void ControllerWidget::SetAxisState(SDL_GameControllerAxis axis, int16_t state)
{
    const double statePercentage = (double)state / SDL_AXIS_PEAK * 100;

    switch (axis)
    {
        case SDL_CONTROLLER_AXIS_LEFTX:
            this->controllerImageWidget->SetXAxisState(statePercentage);
            break;
        case SDL_CONTROLLER_AXIS_LEFTY:
            this->controllerImageWidget->SetYAxisState(statePercentage);
            break;
        default:
            break;
    }
}

bool ControllerWidget::IsPluggedIn()
{
    return this->controllerPluggedCheckBox->isChecked();
}
