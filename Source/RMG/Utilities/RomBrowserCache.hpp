/*
 * Rosalie's Mupen GUI - https://github.com/Rosalie241/RMG
 *  Copyright (C) 2020 Rosalie Wanders <rosalie@mailbox.org>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3.
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef ROMBROWSERCACHE_HPP
#define ROMBROWSERCACHE_HPP

#include "M64P/Wrapper/Types.hpp"

#include <QString>
#include <unordered_map>

namespace Utilities
{
class RomBrowserCache
{
public:
    RomBrowserCache();
    ~RomBrowserCache();

    bool Init();
    bool Save();

    bool ContainsEntry(QString);

    bool AddEntry(QString, M64P::Wrapper::RomInfo_t);
    M64P::Wrapper::RomInfo_t GetEntry(QString);

private:
    std::unordered_map<QString, M64P::Wrapper::RomInfo_t> cachedEntries;
};
} // namespace Utilities

#endif // ROMBROWSERCACHE_HPP
