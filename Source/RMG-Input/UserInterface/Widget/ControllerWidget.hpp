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

#include "ui_ControllerWidget.h"

#include <SDL.h>

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
    int16_t xAxisState = 0;
    int16_t yAxisState = 0;
    bool needsControllerImageDraw = false;

public:
    ControllerWidget(QWidget* parent);
    ~ControllerWidget();

    void ClearInputDevices();
    void AddInputDevice(QString deviceName, int deviceNum);
    void DrawControllerImage();
    void GetCurrentInputDevice(QString& deviceName, int& deviceNum);
    void SetButtonState(SDL_GameControllerButton button, int state);
    void SetAxisState(int16_t xState, int16_t yState);
    bool IsPluggedIn();


private slots:
    void on_deadZoneSlider_valueChanged(int value);
    void on_analogStickRangeSlider_valueChanged(int value);

    void on_controllerPluggedCheckBox_toggled(bool value);
    void on_setupButton_clicked();
};
}
}

#endif // CONTROLLERWIDGET_H