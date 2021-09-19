/*
 * Rosalie's Mupen GUI - https://github.com/Rosalie241/RMG
 *  Copyright (C) 2020 Rosalie Wanders <rosalie@mailbox.org>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3.
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#include <UserInterface/MainDialog.hpp>

#include <QApplication>
#include <SDL.h>

int main(int argc, char **argv)
{
    if (!SDL_WasInit(SDL_INIT_GAMECONTROLLER))
    {
        SDL_Init(SDL_INIT_GAMECONTROLLER);
    }

    if (!SDL_WasInit(SDL_INIT_HAPTIC))
    {
        SDL_Init(SDL_INIT_HAPTIC);
    }

    QApplication app(argc, argv);

    UserInterface::MainDialog dialog(nullptr);

    dialog.show();

    return app.exec();
}
