/*
 * Rosalie's Mupen GUI - https://github.com/Rosalie241/RMG
 *  Copyright (C) 2020 Rosalie Wanders <rosalie@mailbox.org>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3.
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#include "MainDialog.hpp"
#include "SDL_gamecontroller.h"
#include "Widget/ControllerWidget.hpp"

#include <SDL.h>
#include <QTimer>

using namespace UserInterface;
#include <iostream>

MainDialog::MainDialog(QWidget* parent) : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint)
{
    this->setupUi(this);
    this->setWindowIcon(QIcon(":Resource/RMG.png"));

    // each tab needs its own ControllerWidget
    for (int i = 0; i < this->tabWidget->count(); i++)
    {
        Widget::ControllerWidget* widget = new Widget::ControllerWidget(this);
        this->tabWidget->widget(i)->layout()->addWidget(widget);
        controllerWidgets.push_back(widget);
    }

    this->searchForInputDevices();

    this->inputPollTimer = new QTimer(this);
    connect(this->inputPollTimer, &QTimer::timeout, this, &MainDialog::on_InputPollTimer_triggered);
    this->inputPollTimer->start(50);
}

MainDialog::~MainDialog()
{
}

void MainDialog::searchForInputDevices()
{
    // always add keyboard device
    for (auto& controllerWidget : this->controllerWidgets)
    {
        controllerWidget->ClearInputDevices();
        controllerWidget->AddInputDevice("Keyboard", -1);
    }

    // check if there are any game controllers
    for (int i = 0; i < SDL_NumJoysticks(); i++)
    {
        const char* name = nullptr;

        // skip non-gamecontrollers
        if (!SDL_IsGameController(i))
        {
            continue;
        }
        
        name = SDL_GameControllerNameForIndex(i);
        if (name != nullptr)
        {
            for (int y = 0; y < controllerWidgets.count(); y++)
            {
                Widget::ControllerWidget* controllerWidget = controllerWidgets.at(y);
                controllerWidget->AddInputDevice(name, i);
            }
        }
    }
}

SDL_GameController* MainDialog::openController(QString deviceName, int deviceNum)
{
    struct controller_t
    {
        QString deviceName;
        int deviceNum;

        bool operator==(controller_t b)
        {
            return deviceName == b.deviceName && 
                    deviceNum == b.deviceNum;
        }
    };

    QList<controller_t> controllerList;
    bool useDeviceIndex = false;

    // find each controller    
    for (int i = 0; i < SDL_NumJoysticks(); i++)
    {
        const char* name = nullptr;

        // skip non-gamecontrollers
        if (!SDL_IsGameController(i))
        {
            continue;
        }
        
        name = SDL_GameControllerNameForIndex(i);
        if (name != nullptr)
        {
            controller_t controller = {QString(name), i};
            controllerList.append(controller);
        }
    }

    // check if controllerList has duplicate names
    // if it has duplicates, use the device index aswell
    QList<controller_t> tmpControllerList;
    for (auto& controller : controllerList)
    {
        if (tmpControllerList.contains(controller))
        {
            useDeviceIndex = true;
            break;
        }

        tmpControllerList.append(controller);
    }
    tmpControllerList.clear();

    // loop open each device and see if any match
    // if so, try to open the controller
    for (auto& controller : controllerList)
    {
        if (useDeviceIndex)
        {
            if (controller.deviceName == deviceName &&
                controller.deviceNum == deviceNum)
            {
                return SDL_GameControllerOpen(deviceNum);
            }
        }
        else
        {
            if (controller.deviceName == deviceName)
            {
                return SDL_GameControllerOpen(deviceNum);
            }
        }
    }

    // when we haven't found anything,
    // return nullptr
    return nullptr;
}

static QString currentDeviceName;
static int currentDeviceNum;
static SDL_GameController* currentController = nullptr;

void MainDialog::on_InputPollTimer_triggered()
{
    Widget::ControllerWidget* controllerWidget;
    QString deviceName;
    int deviceNum;

    controllerWidget = controllerWidgets.at(this->tabWidget->currentIndex());

    // if the current controller widget 
    // is disabled (by the user), 
    // we don't need to do anything
    if (!controllerWidget->IsPluggedIn())
    {
        return;
    }

    // update controller state
    SDL_GameControllerUpdate();

    // query currently selected input device
    controllerWidget->GetCurrentInputDevice(deviceName, deviceNum);
    if (currentDeviceNum != deviceNum ||
        currentDeviceName != deviceName)
    {
        // close previous controller
        // if it exists
        if (currentController != nullptr)
        {
            SDL_GameControllerClose(currentController);
        }

        currentDeviceNum = deviceNum;
        currentDeviceName = deviceName;
        currentController = openController(deviceName, deviceNum);
    }

    // we don't need to poll a controller
    // when the user has keyboard selected
    if (currentDeviceNum == -1)
    {
        return;
    }

    // if we didn't open a controller yet (or failed to do so),
    // try to open the controller again
    if (currentController == nullptr)
    {
        currentController = openController(deviceName, deviceNum);
        if (currentController == nullptr)
        { // return when we've failed again
            return;
        }
    }

    // make sure controller is still attached
    if (!SDL_GameControllerGetAttached(currentController))
    {
        SDL_GameControllerClose(currentController);
        currentController = nullptr;
        // search for controllers again
        this->searchForInputDevices();
        return;
    }

    SDL_GameControllerButton buttons[] = {
        SDL_CONTROLLER_BUTTON_A,
        SDL_CONTROLLER_BUTTON_B,
        SDL_CONTROLLER_BUTTON_X,
        SDL_CONTROLLER_BUTTON_Y,
        SDL_CONTROLLER_BUTTON_DPAD_DOWN,
        SDL_CONTROLLER_BUTTON_DPAD_LEFT,
        SDL_CONTROLLER_BUTTON_DPAD_RIGHT,
        SDL_CONTROLLER_BUTTON_DPAD_UP,
        SDL_CONTROLLER_BUTTON_START,
        SDL_CONTROLLER_BUTTON_LEFTSHOULDER,
        SDL_CONTROLLER_BUTTON_RIGHTSHOULDER
    };

    for (auto& button : buttons)
    {
        int buttonState = SDL_GameControllerGetButton(currentController, button);
        controllerWidget->SetButtonState(button, buttonState);
    }

    // update axis state
    int16_t xAxisState = 0, yAxisState = 0;
    xAxisState = SDL_GameControllerGetAxis(currentController, SDL_CONTROLLER_AXIS_LEFTX);
    yAxisState = SDL_GameControllerGetAxis(currentController, SDL_CONTROLLER_AXIS_LEFTY);
    controllerWidget->SetAxisState(xAxisState, yAxisState);

    // re-draw controller image if needed
    controllerWidget->DrawControllerImage();
}
