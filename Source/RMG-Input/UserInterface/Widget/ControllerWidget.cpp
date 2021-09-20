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
#include <SDL.h>

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

    QPixmap image = QIcon(":Resource/Controller.svg").pixmap(QSize(w, h));
    this->imageLabel->setPixmap(image);

    this->deadZoneSlider->setValue(25);
    this->analogStickRangeSlider->setValue(100);
    this->controllerPluggedCheckBox->setChecked(false);
}

ControllerWidget::~ControllerWidget()
{

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
    QPixmap image1 = QIcon(":Resource/Controller.svg").pixmap(tmpSize);

    QList<QString> images(tmpList);

    for (auto& keybinding : keybindings2)
    {
        if (keybinding.button == button)
        {
            if (state && !images.contains(keybinding.icon))
            {
                images.append(keybinding.icon);
            } else if (!state && images.contains(keybinding.icon))
            {
                images.removeOne(keybinding.icon);
            }
        }
    }

    if (tmpList != images) {
        QList<QPixmap> pixMaps;

        for (auto& str : images)
        {
            pixMaps.append(QIcon(str).pixmap(tmpSize));
        }

        QPixmap tmp = createImageWithOverlay(image1, pixMaps);
        this->imageLabel->setPixmap(tmp);

        tmpList = images;
    }
}

bool ControllerWidget::IsPluggedIn()
{
    return this->controllerPluggedCheckBox->isChecked();
}

#include <QPainter>

QPixmap createImageWithOverlay(QPixmap& baseImage, QList<QPixmap> overlayImages)
{
    QPixmap imageWithOverlay = QPixmap(baseImage);
    QPainter painter(&imageWithOverlay);

    // paint all overlayImages on top of baseImage
    for (auto& overlayImage : overlayImages)
    {
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        painter.drawPixmap(0, 0, overlayImage);
    }

    painter.end();

    return imageWithOverlay;
}
