/*
 * Rosalie's Mupen GUI - https://github.com/Rosalie241/RMG
 *  Copyright (C) 2020 Rosalie Wanders <rosalie@mailbox.org>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3.
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QDialog>

#include "ui_MainDialog.h"
#include "Widget/ControllerWidget.hpp"

namespace UserInterface
{
class MainDialog : public QDialog, private Ui::MainDialog
{
Q_OBJECT
private:
   QTimer* inputPollTimer;
   QTimer* inputDevicePollTimer;
   QList<Widget::ControllerWidget*> controllerWidgets;
   SDL_GameController* currentController = nullptr;
   QString currentDeviceName;
   int currentDeviceNum = 0;

   void openController(QString deviceName, int deviceNum);
   void closeController();
   void removeCurrentController();
   void addController(int deviceNum);

public:
    MainDialog(QWidget *parent);
    ~MainDialog(void);

public slots:
   void on_InputPollTimer_triggered();
   void on_InputDevicePollTimer_triggered();

   void on_ControllerWidget_CurrentInputDeviceChanged(QString deviceName, int deviceNum);
   void on_tabWidget_currentChanged(int index);
};
}

#endif // MAINWINDOW_HPP
