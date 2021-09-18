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

    void resizeEvent(QResizeEvent *event) override;
};
}
}

#endif // CONTROLLERWIDGET_H