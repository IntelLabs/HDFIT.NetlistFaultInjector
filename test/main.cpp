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


#include <sys/time.h>
#include <stdint.h>
#include <vector>

#define VL_THREADED 1 // for Verilator header below
#include "obj_dir/fma_netlist.h"

#include "../netlistFaultInjector.hpp"
#include "../common.h"

#define FI_SINGLE_BIT 0 // see RtlFile.cpp TODO: Should provide autogen variable to supply this info

typedef struct {
	uint8_t a;
	uint8_t b;
	uint8_t c;
} sample_t;

int testRun(const std::vector<sample_t> &samples, const std::vector<uint8_t> &expected, const std::vector<uint16_t> &modInst, uint32_t assignNr, uint8_t fiSignal)
{
	Vfma_netlist fma;

	// Set fi
	const size_t modInstancesMax = sizeof(fma.GlobalFiModInstNr) / sizeof(fma.GlobalFiModInstNr[0]);
	if(modInst.size() > modInstancesMax)
	{
		nfiError("Supplied more mod instances than possible\n");
		return -1;
	}

	for(size_t inst = 0; inst < modInstancesMax; inst++)
	{
		if(modInst.size() > inst)
		{
			fma.GlobalFiModInstNr[inst] = modInst[inst];
		}
		else
		{
			fma.GlobalFiModInstNr[inst] = 0;
		}
	}

	fma.GlobalFiNumber = assignNr;
	fma.GlobalFiSignal = fiSignal;

	// simulate!
	std::vector<uint8_t> results(samples.size());
	size_t currentSample = 0;
	for(size_t cycle = 0; cycle < samples.size() * 2 + 2; cycle++)
	{
		fma.clk = fma.clk ? 0 : 1;

		// Set regular inputs
		if(!fma.clk && (currentSample < samples.size()))
		{
			fma.a = samples[currentSample].a;
			fma.b = samples[currentSample].b;
			fma.c = samples[currentSample].c;

			currentSample++;
		}

		fma.eval();

		if(cycle > 2)
		{
			results[(cycle - 2) / 2] = fma.d;
		}
	}

	nfiDebug("Results: ");
	for(size_t sample = 0; sample < samples.size(); sample++)
	{
		nfiDebug("%u, ", results[sample]);

		if(expected[sample] != results[sample])
		{
			nfiError("Result does not match, %u != %u\n", expected[sample], results[sample]);
			return -1;
		}
	}
	nfiDebug("\n");

	return 0;
}

int main(int argc, char ** argv)
{
	// Init rand
	timeval t1;
	gettimeofday(&t1, NULL);
	srand(t1.tv_usec * t1.tv_sec);

	std::vector<sample_t> samples;

	samples.push_back({0, 0, 0});
	samples.push_back({2, 0, 0});
	samples.push_back({0, 2, 0});
	samples.push_back({2, 2, 0});
	samples.push_back({0, 0, 2});
	samples.push_back({1, 1, 1});
	samples.push_back({1, 2, 3});
	samples.push_back({3, 2, 1});

	// No fi
	std::vector<uint8_t> expectedNoFi(samples.size());
	for(size_t sample = 0; sample < samples.size(); sample++)
	{
		expectedNoFi[sample] = samples[sample].a * samples[sample].b + samples[sample].c;
	}

	if(testRun(samples, expectedNoFi, std::vector<uint16_t>{0}, 0, 0))
	{
		nfiFatal("testRun failed\n");
	}

#if FI_SINGLE_BIT
	const uint8_t fiSignal = 0; // i.e. 1 << 0
#else // !FI_SINGLE_BIT
	const uint8_t fiSignal = 1; // i.e. 8'b00000001
#endif // !FI_SINGLE_BIT

	// Flip mul[1] in top module
	std::vector<uint8_t> expectedMul1Flip(samples.size());
	for(size_t sample = 0; sample < samples.size(); sample++)
	{
		uint8_t mul = samples[sample].a * samples[sample].b;
		mul ^= (1 << 1);

		expectedMul1Flip[sample] = mul + samples[sample].c;
	}

	if(testRun(samples, expectedMul1Flip, std::vector<uint16_t>{1}, 33, fiSignal))
	{
		nfiFatal("testRun failed\n");
	}

	// Flip bit in c flipflop "flip"
	std::vector<uint8_t> expectedCstg2Flip(samples.size());
	for(size_t sample = 0; sample < samples.size(); sample++)
	{
		uint8_t cCorr = samples[sample].c;
		cCorr ^= (1 << 0);

		expectedMul1Flip[sample] = samples[sample].a * samples[sample].b + cCorr;
	}

	if(testRun(samples, expectedMul1Flip, std::vector<uint16_t>{1, 62, 59}, 3, fiSignal))
	{
		nfiFatal("testRun failed\n");
	}

	// Flip bit in mul flipflop "flop"
	std::vector<uint8_t> expectedMulstg2Flip(samples.size());
	for(size_t sample = 0; sample < samples.size(); sample++)
	{
		uint8_t mul = samples[sample].a * samples[sample].b;
		mul ^= (1 << 2);

		expectedMulstg2Flip[sample] = mul + samples[sample].c;
	}

	if(testRun(samples, expectedMulstg2Flip, std::vector<uint16_t>{1, 61, 60}, 5, fiSignal))
	{
		nfiFatal("testRun failed\n");
	}

	// Inject random faults
	NetlistFaultInjector netlistFaultInjector;
	if(netlistFaultInjector.Init())
	{
		nfiFatal("Verifi.Init() failed\n");
	}

	// Check for errors / warnings before closing

	if(nfiErrorCnt)
	{
		nfiFatal("There were %lu errors\n", nfiErrorCnt);
	}


	nfiInfo("Test successful\n");


	return 0;
}
