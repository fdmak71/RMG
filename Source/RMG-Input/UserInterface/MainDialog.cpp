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
    }

    // always add keyboard device
    for (auto& controllerWidget : this->controllerWidgets)
    {
        controllerWidget->AddInputDevice("Keyboard", -1);
    }

    this->inputPollTimer = new QTimer(this);
    connect(this->inputPollTimer, &QTimer::timeout, this, &MainDialog::on_InputPollTimer_triggered);
    this->inputPollTimer->start(50);

    this->inputDevicePollTimer = new QTimer(this);
    connect(this->inputDevicePollTimer, &QTimer::timeout, this, &MainDialog::on_InputDevicePollTimer_triggered);
    this->inputDevicePollTimer->start(5000);
}

MainDialog::~MainDialog()
{
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

void MainDialog::removeCurrentController()
{
    for (auto& controllerWidget : this->controllerWidgets)
    {
        controllerWidget->RemoveInputDevice(currentDeviceName, currentDeviceNum);
    }
}

void MainDialog::addController(int deviceNum)
{
    QString deviceName = SDL_GameControllerNameForIndex(deviceNum);
    for (auto& controllerWidget : this->controllerWidgets)
    {
        controllerWidget->AddInputDevice(deviceName, deviceNum);
    }
}

#include <iostream>
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

            /*case SDL_CONTROLLERDEVICEREMOVED:
            {
                int instanceId = event.cdevice.which;
                SDL_GameController* controller = SDL_GameControllerFromInstanceID(instanceId);

                std::cout << "removed: " << SDL_GameControllerName(controller) << std::endl;
                //this->removeCurrentController();
            }    break;

            case SDL_CONTROLLERDEVICEADDED:
                this->addController(event.cdevice.which);
                break;*/

            default:
                break;
        }
    }

    controllerWidget->DrawControllerImage();
}

#include <QList>
void MainDialog::on_InputDevicePollTimer_triggered()
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

    QList<inputDevice_t> devicesToBeAdded;
    QList<inputDevice_t> devicesToBeRemoved;

    // fill newDeviceList
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
            inputDevice_t inputDevice = {i, QString(name)};
            newDeviceList.append(inputDevice);
        }
    }

    // compare newDeviceList with oldDeviceList
    for (int i = 0; i < newDeviceList.count(); i++)
    {
        auto inputDevice = newDeviceList.at(i);
        if (!oldDeviceList.contains(inputDevice))
        {
            devicesToBeAdded.append(inputDevice);
        }
    }
    for (int i = 0; i< oldDeviceList.count(); i++)
    {
        auto inputDevice = oldDeviceList.at(i);
        if (!newDeviceList.contains(inputDevice))
        {
            devicesToBeRemoved.append(inputDevice);
        }
    }

    // apply changes
    for (auto& controllerWidget : this->controllerWidgets)
    {
        for (auto& inputDevice : devicesToBeAdded) 
        {
            controllerWidget->AddInputDevice(inputDevice.deviceName, inputDevice.deviceNum);
        }
        for (auto& inputDevice : devicesToBeRemoved)
        {
            controllerWidget->RemoveInputDevice(inputDevice.deviceName, inputDevice.deviceNum);
        }
    }

    oldDeviceList = newDeviceList;
}

void MainDialog::on_ControllerWidget_CurrentInputDeviceChanged(QString deviceName, int deviceNum)
{
    std::cout << "on_ControllerWidget_CurrentInputDeviceChanged" << std::endl;

    this->closeController();
    this->openController(deviceName, deviceNum);
}


void MainDialog::on_tabWidget_currentChanged(int index)
{
    QString deviceName;
    int deviceNum = 0;

    Widget::ControllerWidget* controllerWidget = controllerWidgets.at(index);
    controllerWidget->ClearControllerImage();

    this->closeController();

    controllerWidget->GetCurrentInputDevice(deviceName, deviceNum);

    this->openController(deviceName, deviceNum);
}
