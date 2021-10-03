/*
 * Rosalie's Mupen GUI - https://github.com/Rosalie241/RMG
 *  Copyright (C) 2020 Rosalie Wanders <rosalie@mailbox.org>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3.
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#define M64P_PLUGIN_PROTOTYPES 1
#define INPUT_PLUGIN_API_VERSION 0x020100

#include <UserInterface/MainDialog.hpp>

extern "C"
{
#include <Common/api/m64p_common.h>
#include <Common/api/m64p_custom.h>
#include <Common/api/m64p_types.h>
#include <Common/api/m64p_config.h>
#include <Common/api/m64p_plugin.h>
#include <Common/api/version.h>
}

#include <QApplication>
#include <SDL.h>

//
// Basic Plugin Functions
//

EXPORT m64p_error CALL PluginStartup(m64p_dynlib_handle CoreLibHandle, void *Context, void (*DebugCallback)(void *, int, const char *))
{
    // initialize SDL

    return M64ERR_SUCCESS;
}

EXPORT m64p_error CALL PluginShutdown(void)
{
    return M64ERR_SUCCESS;
}

EXPORT m64p_error CALL PluginGetVersion(m64p_plugin_type *pluginType, int *pluginVersion, 
    int *apiVersion, const char **pluginNamePtr, int *capabilities)
{
    if (pluginType != nullptr)
    {
        *pluginType = M64PLUGIN_INPUT;
    }

    if (pluginVersion != nullptr)
    {
        *pluginVersion = 0x010000;
    }

    if (apiVersion != nullptr)
    {
        *apiVersion = INPUT_PLUGIN_API_VERSION;
    }

    if (pluginNamePtr != nullptr)
    {
        *pluginNamePtr = "Rosalie's Mupen GUI - Input Plugin";
    }

    if (capabilities != nullptr)
    {
        *capabilities = 0;
    }

    return M64ERR_SUCCESS;
}

//
// Custom Plugin Functions
//

#include <iostream>
EXPORT m64p_error CALL PluginConfig()
{
    if (!SDL_WasInit(SDL_INIT_GAMECONTROLLER) &&
        SDL_Init(SDL_INIT_GAMECONTROLLER) < 0)
    {
        return M64ERR_SYSTEM_FAIL;
    }

    if (!SDL_WasInit(SDL_INIT_HAPTIC) && 
        SDL_Init(SDL_INIT_HAPTIC) < 0)
    {
        return M64ERR_SYSTEM_FAIL;
    }

    
    UserInterface::MainDialog dialog(nullptr);

    std::cout << "dialog exec: " << dialog.exec() << std::endl;

    return M64ERR_SUCCESS;
}

//
// Input Plugin Functions
//

EXPORT void CALL ControllerCommand(int Control, unsigned char* Command)
{

}

EXPORT void CALL GetKeys(int Control, BUTTONS * Keys)
{

}

EXPORT void CALL InitiateControllers(CONTROL_INFO ControlInfo)
{
    ControlInfo.Controls->Present = 1;
}

EXPORT void CALL ReadController(int Control, unsigned char *Command)
{

}

EXPORT int CALL RomOpen(void)
{
    return 1;
}

EXPORT void CALL RomClosed(void)
{

}

EXPORT void CALL SDL_KeyDown(int keymod, int keysym)
{

}

EXPORT void CALL SDL_KeyUp(int keymod, int keysym)
{

}
