/*
 * Rosalie's Mupen GUI - https://github.com/Rosalie241/RMG
 *  Copyright (C) 2020 Rosalie Wanders <rosalie@mailbox.org>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3.
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#include "RomBrowserCache.hpp"
#include "M64P/Wrapper/Types.hpp"

#include <QFile>

using namespace Utilities;

RomBrowserCache::RomBrowserCache()
{
}

RomBrowserCache::~RomBrowserCache()
{
	this->Save();
}

bool RomBrowserCache::Init()
{
	return true;
}

bool RomBrowserCache::Save()
{
	QFile cacheFile("RomBrowserCache.cache");

	if (!cacheFile.open(QIODevice::WriteOnly))
	{
		return false;
	}

	for (auto& entry : this->cachedEntries)
	{
		auto romInfo = entry.second;

		cacheFile.write((const char*)&romInfo, sizeof(romInfo));
	}

	cacheFile.close();
	return true;
}

bool RomBrowserCache::ContainsEntry(QString filename)
{
	return this->cachedEntries.find(filename) != this->cachedEntries.end();
}

bool RomBrowserCache::AddEntry(QString filename, M64P::Wrapper::RomInfo_t romInfo)
{
	this->cachedEntries.emplace(std::pair<QString, M64P::Wrapper::RomInfo_t>(filename, romInfo));
	return true;
}

M64P::Wrapper::RomInfo_t RomBrowserCache::GetEntry(QString filename)
{
	return this->cachedEntries.find(filename)->second;
}
