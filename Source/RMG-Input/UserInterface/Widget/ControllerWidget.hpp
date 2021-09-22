/*
 * Rosalie's Mupen GUI - https://github.com/Rosalie241/RMG
 *  Copyright (C) 2020 Rosalie Wanders <rosalie@mailbox.org>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3.
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef CONTROLLERWIDGET_H
#define CONTROLLERWIDGET_H

#include "CustomButton.hpp"

using namespace UserInterface::Widget;

#include "ui_ControllerWidget.h"

#include <SDL.h>

enum N64CONTROLLER_STATE
{
    N64CONTROLLER_BUTTON_A = 0,
    N64CONTROLLER_BUTTON_B = 1,
    N64CONTROLLER_BUTTON_START = 2,
    N64CONTROLLER_BUTTON_DPAD_UP = 3,
    N64CONTROLLER_BUTTON_DPAD_DOWN = 4,
    N64CONTROLLER_BUTTON_DPAD_LEFT = 5,
    N64CONTROLLER_BUTTON_DPAD_RIGHT = 6,
    N64CONTROLLER_BUTTON_CBUTTONS_UP = 7,
    N64CONTROLLER_BUTTON_CBUTTONS_DOWN = 8,
    N64CONTROLLER_BUTTON_CBUTTONS_LEFT = 9,
    N64CONTROLLER_BUTTON_CBUTTONS_RIGHT = 10,
    N64CONTROLLER_BUTTON_LEFTTRIGGER = 11,
    N64CONTROLLER_BUTTON_RIGHTTRIGGER = 12,
    N64CONTROLLER_BUTTON_ZTRIGGER = 13,
    N64CONTROLLER_NUM = 14
};

namespace UserInterface
{
namespace Widget
{
class ControllerWidget : public QWidget, Ui::ControllerWidget
{
    Q_OBJECT

private:
    QList<QString> inputDeviceNameList;
    QPixmap controllerPixmap;
    QList<QString> controllerImages;
    int controllerState[N64CONTROLLER_NUM] = {0};
    int16_t xAxisState = 0;
    int16_t yAxisState = 0;
    bool needsControllerImageDraw = false;

    void initializeButtons();

public:
    ControllerWidget(QWidget* parent);
    ~ControllerWidget();

    void AddInputDevice(QString deviceName, int deviceNum);
    void RemoveInputDevice(QString deviceName, int deviceNum);

    void DrawControllerImage();
    void ClearControllerImage();

    void GetCurrentInputDevice(QString& deviceName, int& deviceNum);
    void SetButtonState(SDL_GameControllerButton button, int state);
    void SetAxisState(SDL_GameControllerAxis axis, int16_t state);
    bool IsPluggedIn();

private slots:
    void on_deadZoneSlider_valueChanged(int value);
    void on_analogStickRangeSlider_valueChanged(int value);
    
    void on_inputDeviceComboBox_currentIndexChanged(int value);
    void on_inputDeviceRefreshButton_clicked();
    
    void on_controllerPluggedCheckBox_toggled(bool value);
    void on_setupButton_clicked();

public slots:
    void on_CustomButton_released(CustomButton* button);
    void on_CustomButton_TimerFinished(CustomButton* button);
    void on_CustomButton_TimerStopped(CustomButton* button);

signals:
    void CurrentInputDeviceChanged(QString deviceName, int deviceNum);
    void RefreshInputDevicesButtonClicked();
};
}
}

#endif // CONTROLLERWIDGET_H