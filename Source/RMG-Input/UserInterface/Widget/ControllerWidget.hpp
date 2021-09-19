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

namespace UserInterface
{
namespace Widget
{
class ControllerWidget : public QWidget, Ui::ControllerWidget
{
    Q_OBJECT
public:
    ControllerWidget(QWidget* parent);
    ~ControllerWidget();

    void ClearInputDevices();
    void AddInputDevice(QString name, int num);

public slots:
    void on_deadZoneSlider_valueChanged(int value);
    void on_analogStickRangeSlider_valueChanged(int value);
    void on_setupButton_clicked();

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
};
}
}

#endif // CONTROLLERWIDGET_H