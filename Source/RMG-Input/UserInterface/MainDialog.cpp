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

using namespace UserInterface;
#include <iostream>
MainDialog::MainDialog(QWidget* parent) : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint)
{
    this->setupUi(this);
    this->setWindowIcon(QIcon(":Resource/RMG.png"));

    QList<Widget::ControllerWidget*> controllerWidgets;

    // each tab needs its own ControllerWidget
    for (int i = 0; i < this->tabWidget->count(); i++)
    {
        Widget::ControllerWidget* widget = new Widget::ControllerWidget(this);
        this->tabWidget->widget(i)->layout()->addWidget(widget);
        controllerWidgets.push_back(widget);
    }

    for (auto& controllerWidget : controllerWidgets)
    {
        controllerWidget->ClearInputDevices();
        controllerWidget->AddInputDevice("Keyboard", -1);
    }

    for (int i = 0; i < SDL_NumJoysticks(); i++)
    {
        const char* name = nullptr;

        // skip non-gamecontrollers
        if (!SDL_IsGameController(i))
        {
            continue;
        }
        
        name = SDL_GameControllerNameForIndex(i);

        SDL_GameController* controller = SDL_GameControllerOpen(i);

            //std::cout << guid << std::endl;

        if (name != nullptr)
        {
            for (int y = 0; y < controllerWidgets.count(); y++)
            {
                Widget::ControllerWidget* controllerWidget = controllerWidgets.at(y);

                int playerIndex = SDL_GameControllerGetPlayerIndex(controller);

                std::cout << "playerIndex: " << playerIndex << std::endl;
                std::cout << "i:" << i << " controllername:" << name << std::endl;
                controllerWidget->AddInputDevice(name, i);

                //std::cout << "i:" << i << " controllername:" << name << std::endl;

                //controllerWidget->AddInputDevice(name, i);
            }
        }

        SDL_GameControllerClose(controller);
    }
}

MainDialog::~MainDialog()
{

}
