/*
 * Rosalie's Mupen GUI - https://github.com/Rosalie241/RMG
 *  Copyright (C) 2020 Rosalie Wanders <rosalie@mailbox.org>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3.
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef M64P_WRAPPER_PLUGIN_HPP
#define M64P_WRAPPER_PLUGIN_HPP

#include "../PluginApi.hpp"
#include "Types.hpp"

#include <Common/api/m64p_types.h>

#include <QString>

namespace M64P
{
namespace Wrapper
{
class Plugin
{
  public:
    Plugin(void);
    ~Plugin(void);

    bool Init(QString, m64p_dynlib_handle);
    bool HasInit(void);

    bool Startup(void);
    bool Shutdown(void);

    bool HasConfig(void);
    bool OpenConfig(void);

    m64p_dynlib_handle GetHandle(void);
    m64p_plugin_type GetType(void);

    Plugin_t GetPlugin_t(void);

    QString GetLastError(void);

  private:
    QString error_Message;

    bool has_Init = false;

    QString fileName;

    m64p_dynlib_handle handle;
    m64p_plugin_type type;

    m64p_dynlib_handle coreHandle;

    M64P::PluginApi plugin;

    Plugin_t plugin_t;
    bool plugin_t_Get(void);
};
} // namespace Wrapper
} // namespace M64P

#endif // M64P_WRAPPER_PLUGIN_HPP
