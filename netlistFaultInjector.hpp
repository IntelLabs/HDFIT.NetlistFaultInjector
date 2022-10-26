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

#ifndef NETLISTFAULTINJECTOR_H_
#define NETLISTFAULTINJECTOR_H_

#include <string>
#include <vector>
#include <map>

	typedef enum {
		SIGNAL_TYPE_WIRE,
		SIGNAL_TYPE_REG,
		SIGNAL_TYPE_INPUT,
		SIGNAL_TYPE_OUTPUT,
		SIGNAL_TYPE_NROF
	} signalType_t;

	typedef struct {
		signalType_t Type;
		size_t Width;
		size_t ElemCnt; // i.e. array elements
		size_t UUID;
	} signal_t;

	// TODO: All this string and vector stuff causes the compilation to take forever.
	typedef struct {
		std::string Name;
		std::vector<signal_t> FiSignal;
		std::vector<std::pair<size_t, size_t>> InstanceUuids; // <index of module of this instance, uuid>
	} module_t;

	extern const std::vector<module_t> modules;
	extern const size_t modulesTopIndex;
	extern const size_t modulesTopUUID;

	class NetlistFaultInjector {
	public:
		int Init(); // NOTE: Assumes srand was called outside
		int RandomFiGet(std::vector<uint16_t> * moduleInstanceChain, uint32_t * assignmentUUID, size_t * width);


	private:
		std::map<size_t, size_t> FiSignalCntCache_; // <index, fi-signal cnt>
		int ModuleFiBitsCnt(size_t * bits, size_t moduleIndex);
		size_t FiBitCnt_ = 0;

		int RandomFiGet(std::vector<uint16_t> * modInst, uint32_t * assignNr, size_t * width, size_t moduleIndex, uint16_t moduleUUID);
	};

#endif /* NETLISTFAULTINJECTOR_H_ */
