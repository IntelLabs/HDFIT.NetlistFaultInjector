/*
 * Copyright (C) 2022 Intel Corporation
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License, as published
 * by the Free Software Foundation; either version 3 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "common.h"

#include "netlistFaultInjector.hpp"

size_t nfiErrorCnt = 0;

int NetlistFaultInjector::ModuleFiBitsCnt(size_t * bits, size_t moduleIndex)
{
	const auto it = FiSignalCntCache_.find(moduleIndex);
	if(FiSignalCntCache_.end() != it)
	{
		*bits =  it->second;
		return 0;
	}

	*bits = 0;

	// Cnt fi-signals in this module
	for(const auto &signal: modules[moduleIndex].FiSignal)
	{
		*bits += signal.Width;
	}

	// Cnt fi-signals in instances of this module
	for(const auto &inst: modules[moduleIndex].InstanceUuids)
	{
		size_t instBits = 0;
		if(ModuleFiBitsCnt(&instBits, inst.first))
		{
			nfiError("ModuleFiBitsCnt failed\n");
			return -1;
		}

		*bits += instBits;
	}

	FiSignalCntCache_[moduleIndex] = *bits;

	return 0;
}

static size_t randUL()
{
	size_t ret;
	uint8_t * retu8 = (uint8_t*) &ret;
	for(size_t byte = 0; byte < sizeof(ret); byte++)
	{
		// coverity[DC.WEAK_CRYPTO]
		retu8[byte] = rand();
	}

	return ret;
}

int NetlistFaultInjector::Init()
{
	// Count all bits in all assignments in modules
	if(ModuleFiBitsCnt(&FiBitCnt_, modulesTopIndex))
	{
		nfiError("ModuleFiBitsCnt failed\n");
		return -1;
	}

	if(0 == FiBitCnt_)
	{
		nfiError("No fi signals\n");
		return -1;
	}

	nfiDebug("Counted %lu fi bits\n", FiBitCnt_);
	nfiDebug("Cache:\n");
#if NFI_DEBUG
	for(const auto &mod: FiSignalCntCache_)
	{
		nfiDebug("%s: %lu\n", modules[mod.first].Name.c_str(), mod.second);
	}
#endif // NFI_DEBUG

	return 0;
}

int NetlistFaultInjector::RandomFiGet(std::vector<uint16_t> * moduleInstanceChain, uint32_t * assignmentUUID, size_t * width)
{
	moduleInstanceChain->clear();

	if(RandomFiGet(moduleInstanceChain, assignmentUUID, width, modulesTopIndex, modulesTopUUID))
	{
		nfiError("RandomFiGet failed\n");
		return -1;
	}

	return 0;
}

int NetlistFaultInjector::RandomFiGet(std::vector<uint16_t> * modInst, uint32_t * assignNr, size_t * width, size_t moduleIndex, uint16_t moduleUUID)
{
	modInst->push_back(moduleUUID);

	if(0 == FiBitCnt_)
	{
		nfiError("Not initialized\n");
		return -1;
	}

	// How many bits in this module?
	size_t bitsTotal = 0;
	if(ModuleFiBitsCnt(&bitsTotal, moduleIndex))
	{
		nfiError("ModuleFiBitsCnt failed\n");
		return -1;
	}

	// Choose random bit
	const size_t randomBit = randUL() % bitsTotal;

	// In which signal / instance is the random bit?
	size_t currentBit = 0;

	// Go through signals in this module
	// Cnt fi-signals in this module
	for(const auto &signal: modules[moduleIndex].FiSignal)
	{
		currentBit += signal.Width;

		if(randomBit < currentBit)
		{
			*assignNr = signal.UUID;
			*width = signal.Width;
			return 0; // found a signal
		}
	}

	// Cnt fi-signals in instances of this module
	for(const auto &inst: modules[moduleIndex].InstanceUuids)
	{
		size_t instBits = 0;
		if(ModuleFiBitsCnt(&instBits, inst.first))
		{
			nfiError("ModuleFiBitsCnt failed\n");
			return -1;
		}

		currentBit += instBits;
		if(randomBit < currentBit)
		{
			return RandomFiGet(modInst, assignNr, width, inst.first, inst.second);
		}
	}

	return 0;
}
