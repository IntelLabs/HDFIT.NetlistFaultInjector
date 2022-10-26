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

#include "RtlFile.h"

#include "common.h"

size_t nfiErrorCnt = 0;

typedef struct {
	std::string File;
	std::string TopModule;
} userConfig_t;

static int argParse(userConfig_t * config, int argc, char ** argv)
{
	if(3 != argc)
	{
		nfiError("No fileName / topModule supplied\n");
		return -1;
	}

	config->File = argv[1];
	config->TopModule = argv[2];

	return 0;
}

int main(int argc, char ** argv)
{
	userConfig_t userConfig;
	if(argParse(&userConfig, argc, argv))
	{
		nfiFatal("argParse failed\n");
	}

	// Get file
	RtlFile rtlFile;
	if(rtlFile.Get(userConfig.File.c_str(), userConfig.TopModule))
	{
		nfiFatal("fileGet failed\n");
	}

	if(rtlFile.FiSignalsCreate(RtlFile::FI_MODE_FLIP))
	{
		nfiFatal("Failed to insert FiSignals\n");
	}

	if(rtlFile.WriteBack())
	{
		nfiFatal("WriteBack failed\n");
	}

	if(nfiErrorCnt)
	{
		nfiFatal("There were %lu errors\n", nfiErrorCnt);
	}

	return 0;
}

