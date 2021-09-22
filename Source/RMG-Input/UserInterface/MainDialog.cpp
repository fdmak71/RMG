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
        connect(widget, &Widget::ControllerWidget::CurrentInputDeviceChanged, this,
            &MainDialog::on_ControllerWidget_CurrentInputDeviceChanged);
        connect(widget, &Widget::ControllerWidget::RefreshInputDevicesButtonClicked, this,
            &MainDialog::on_ControllerWidget_RefreshInputDevicesButtonClicked);
    }

    // always add keyboard device
    for (auto& controllerWidget : this->controllerWidgets)
    {
        controllerWidget->AddInputDevice("Keyboard", -1);
    }

    // fill device list at least once
    this->on_ControllerWidget_RefreshInputDevicesButtonClicked();

    this->inputPollTimer = new QTimer(this);
    connect(this->inputPollTimer, &QTimer::timeout, this, &MainDialog::on_InputPollTimer_triggered);
    this->inputPollTimer->start(50);
}

MainDialog::~MainDialog()
{
}

void MainDialog::addInputDevice(QString deviceName, int deviceNum)
{
    for (auto& controllerWidget : this->controllerWidgets)
    {
        controllerWidget->AddInputDevice(deviceName, deviceNum);
    }
}

void MainDialog::removeInputDevice(QString deviceName, int deviceNum)
{
    for (auto& controllerWidget : this->controllerWidgets)
    {
        controllerWidget->RemoveInputDevice(deviceName, deviceNum);
    }
}

void MainDialog::openController(QString deviceName, int deviceNum)
{
    // we don't need to open a keyboard
    if (deviceNum == -1)
    {
        currentDeviceName = "";
        currentDeviceNum = -1;
        return;
    }

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
            if (deviceName == QString(name) &&
                deviceNum == i)
            {
                currentController = SDL_GameControllerOpen(i);
                currentDeviceNum = deviceNum;
                currentDeviceName = deviceName;
                return;
            }
        }
    }
}

void MainDialog::closeController()
{
    if (this->currentController != nullptr)
    {
        SDL_GameControllerClose(this->currentController);
        this->currentController = nullptr;
    }
}

void MainDialog::on_InputPollTimer_triggered()
{
    Widget::ControllerWidget* controllerWidget;
    controllerWidget = controllerWidgets.at(this->tabWidget->currentIndex());

    // if the current controller widget
    // is disabled (by the user), 
    // we don't need to do anything
    if (!controllerWidget->IsPluggedIn())
    {
        return;
    }

    // check if controller has been disconnected,
    // if so, keep trying to re-open it
    if (!SDL_GameControllerGetAttached(this->currentController))
    {
        this->closeController();
        this->openController(this->currentDeviceName, this->currentDeviceNum);
    }

    // process SDL events
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_CONTROLLERBUTTONDOWN:
            case SDL_CONTROLLERBUTTONUP:
                controllerWidget->SetButtonState((SDL_GameControllerButton)event.cbutton.button, event.cbutton.state);
                break;

            case SDL_CONTROLLERAXISMOTION:
                controllerWidget->SetAxisState((SDL_GameControllerAxis)event.caxis.axis, event.caxis.value);
                break;

            default:
                break;
        }
    }

    controllerWidget->DrawControllerImage();
}

void MainDialog::on_ControllerWidget_CurrentInputDeviceChanged(QString deviceName, int deviceNum)
{
    this->closeController();
    this->openController(deviceName, deviceNum);
}

void MainDialog::on_ControllerWidget_RefreshInputDevicesButtonClicked()
{
    struct inputDevice_t
    {
        int deviceNum;
        QString deviceName;

        bool operator== (inputDevice_t other)
        {
            return other.deviceNum == deviceNum &&
                other.deviceName == deviceName;
        }
    };

    static QList<inputDevice_t> oldDeviceList;
    QList<inputDevice_t> newDeviceList;

    // force-refresh device list in SDL
    SDL_GameControllerUpdate();

    // fill newDeviceList
    for (int i = 0; i < SDL_NumJoysticks(); i++)
    {
        const char* name = nullptr;

        // skip non-gamecontrollers (for now)
        if (!SDL_IsGameController(i))
        {
            continue;
        }
        
        name = SDL_GameControllerNameForIndex(i);
        if (name != nullptr)
        {
            inputDevice_t inputDevice = {i, QString(name)};
            newDeviceList.append(inputDevice);
        }
    }

    // compare newDeviceList with oldDeviceList
    // and signal the changes
    for (auto& inputDevice : newDeviceList)
    {
        if (!oldDeviceList.contains(inputDevice))
        {
            this->addInputDevice(inputDevice.deviceName, inputDevice.deviceNum);
        }
    }
    for (auto& inputDevice : oldDeviceList)
    {
        if (!newDeviceList.contains(inputDevice))
        {
            this->removeInputDevice(inputDevice.deviceName, inputDevice.deviceNum);
        }
    }

    // update oldDeviceList
    oldDeviceList = newDeviceList;
}

void MainDialog::on_tabWidget_currentChanged(int index)
{
    QString deviceName;
    int deviceNum;

    Widget::ControllerWidget* controllerWidget = controllerWidgets.at(index);
    controllerWidget->ClearControllerImage();

    this->closeController();

    controllerWidget->GetCurrentInputDevice(deviceName, deviceNum);

    this->openController(deviceName, deviceNum);
}
