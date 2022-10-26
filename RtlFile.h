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

#ifndef RTLFILE_H_
#define RTLFILE_H_

#include <string>
#include <map>
#include <vector>

class RtlFile {
public:
	RtlFile();
	virtual ~RtlFile();

	// Prevent copying (alternatively, implement copy/asgn duplicating Content_)
	RtlFile & operator=(const RtlFile&) = delete; // assignment operator
	RtlFile(const RtlFile &rtlFile) = delete; // copy constructor

	int Get(const char * fileName, const std::string &topModule);

	typedef enum {
		FI_MODE_STUCK_HIGH,
		FI_MODE_STUCK_LOW,
		FI_MODE_FLIP
	} fiMode_t;

	int FiSignalsCreate(fiMode_t fiMode);
	int WriteBack() const;

private:
	std::string Name_;
	char * Content_ = nullptr;
	size_t Size_ = 0;

	std::string TopModule_;

	typedef struct {
		std::string Replacement;
		const char * End;
	} diff_t;

	int DiffApply(std::map<const char *, diff_t> &diff);

	typedef enum {
		SIGNAL_TYPE_WIRE,
		SIGNAL_TYPE_REG,
		SIGNAL_TYPE_INPUT,
		SIGNAL_TYPE_OUTPUT,
		SIGNAL_TYPE_NROF
	} signalType_t;

	static const std::string &signalTypeStr(signalType_t type);

	static constexpr const char * SignalTypes[SIGNAL_TYPE_NROF] = {"wire", "reg", "input", "output"};

	typedef struct {
		signalType_t Type;
		std::string Name;
		size_t Width;
		size_t ElemCnt; // i.e. array elements
		size_t UUID;
	} signal_t;

	static constexpr char GlobalFiSignal_[] = "GlobalFiSignal";
	static constexpr char GlobalFiNumber_[] = "GlobalFiNumber";
	static constexpr char GlobalFiModInstNumber_[] = "GlobalFiModInstNr";
	static constexpr size_t GlobalFiModInstNumberTop_ = 1;
	static constexpr char FiEnableStr[] = "fiEnable";

	static constexpr char FiSignalsLibraryNameAppend_[] = "FiSignals.cpp";

	typedef struct {
		std::string Name;
		std::vector<signal_t> FiSignal;
		std::vector<std::pair<void *, size_t>> InstanceUuids; // <module_t * instanceOfModulePointedTo, uuid>
	} module_t;

	static int ModuleFind(std::string * name, const char ** start, const char ** end, const char * pFile, size_t nFile);

	static int ModuleFi(
			bool isTop, const std::string &fiPrefix,
			fiMode_t fiMode,
			module_t * module, std::map<const char *, diff_t> * diff,
			const char * start, const char * stop);

	static int ModuleInstancesHandle(
			std::map<std::string, module_t>::iterator &currentModule,
			std::map<std::string, module_t> * modules, std::map<const char *, diff_t> * diff,
			const char * moduleStart, const char * moduleEnd,
			const std::string &topModule,
			size_t hierarchyDepth);

	static constexpr const char * blockCommentStartStr[] = {"(*", "/*"};

	static constexpr const char * blockCommentEndStr[] = {"*)", "*/"};

	static bool PosInsideComment(const char * pos, const char * start, const char * end);
	static const char * SignalDeclarationGet(const std::string &signalName, const char * inModuleStart);
	static int SubSignalWidthGet(const std::string &inSubSignal, const char * inModuleStart);
	static int SignalDeclarationParse(signal_t * signal, const char * declaration);
	static signalType_t TypeGet(const char ** declStart, const char * in, const char * inModuleStart);

	static int FiEnableInputAdd(std::map<const char *, diff_t> * diff, const char * start, const char * stop);
	static int GlobalSignalsToTopAdd(std::map<const char *, diff_t> * diff, const char * start, const char * stop, size_t fiSignalWidth, size_t hierarchyDepth);

	typedef enum {
			FI_NEEDLE_ASSIGN,
			FI_NEEDLE_ASSIGN_NON_BLOCKIN,
			FI_NEEDLE_NROF
		} fiNeedle_t;

	static constexpr char fiNeedles[FI_NEEDLE_NROF][8] = {"assign ", "<="};

	static const char * NextNeedle(size_t * needleNr, const char * pHaystack, size_t nHaystack);
	static int NeedleCorrupt(fiMode_t fiMode, module_t * module, std::map<const char *, diff_t> * diff,
			const std::string &fiPrefix,
			const char * moduleStart, const char * moduleEnd, const char * needle, fiNeedle_t needleNr);

	static int LibraryCreate(const std::map<std::string, module_t> &modules, const std::string &topName);
	static int MapOffsetsCalculate(std::map<std::string, size_t> * offsets, const std::map<std::string, module_t> &modules);
	static int HierarchyDepthGet(const std::map<std::string, module_t> &modules, const std::string &topName);
};

#endif /* RTLFILE_H_ */
