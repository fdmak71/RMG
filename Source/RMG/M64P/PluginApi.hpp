/*
 * Rosalie's Mupen GUI - https://github.com/Rosalie241/RMG
 *  Copyright (C) 2020 Rosalie Wanders <rosalie@mailbox.org>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3.
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef M64P_PLUGINAPI_HPP
#define M64P_PLUGINAPI_HPP

#include <Common/api/m64p_common.h>
#include <Common/api/m64p_custom.h>

#include <QString>

namespace M64P
{
class PluginApi
{
  public:
    PluginApi(void);
    ~PluginApi(void);

    bool Hook(m64p_dynlib_handle);

    QString GetLastError(void);

    ptr_PluginStartup Startup;
    ptr_PluginShutdown Shutdown;
    ptr_PluginConfig Config;
    ptr_PluginGetVersion GetVersion;

  private:
    QString error_Message;
};
} // namespace M64P

#endif // M64P_PLUGINAPI_HPP
