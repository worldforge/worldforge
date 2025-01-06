/*
 Copyright (C) 2023 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef EMBER_ASSETSSYNCNEEDEDSTATE_H
#define EMBER_ASSETSSYNCNEEDEDSTATE_H

#include "StateBase.h"
#include "AccountAvailableState.h"
#include "services/server/ServerServiceSignals.h"
#include "framework/AutoCloseConnection.h"
#include <Eris/ServerInfo.h>

namespace Ember {

class AssetsSyncNeededState : public virtual StateBase<AccountAvailableState> {
public:
	AssetsSyncNeededState(IState& parentState, Eris::Connection& connection);

	~AssetsSyncNeededState() override;

private:

	void processServerInfo(const Eris::ServerInfo& info);

	void syncComplete(AssetsSync::UpdateResult result, std::string assetPath);

	AutoCloseConnection serverInfoConnection;

	Eris::Connection& mConnection;

	std::vector<std::string> mAssetsPaths;
};
}

#endif //EMBER_ASSETSSYNCNEEDEDSTATE_H
