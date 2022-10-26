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

`ifndef GLOBALS_SV_  // guard
	`define GLOBALS_SV_
	
	//`define DOUBLE_PREC

	`define MSFF_NAME msff_inst`__LINE__
	`define MSFF(OUT, IN, CLK) JmsFlipFlop #(.WIDTH($bits(OUT))) `MSFF_NAME (CLK, IN, OUT)
	`define MSFF_INIT(OUT, IN, CLK, INIT) JmsFlipFlop #(.WIDTH($bits(OUT)), .INITIAL(INIT)) `MSFF_NAME (CLK, IN, OUT)

	`ifdef DOUBLE_PREC
		parameter EXP_BIAS =  10'sd1023;
		typedef logic [10:0] exponent_t; // -1023 biased exponent
		
		typedef logic signed [51:0] accMant_t;  // Signed mantissa with leading 1
		typedef logic signed [51:0] inMultMant_t;  // Signed mantissa with leading 1
	`else // !DOUBLE_PREC
		parameter EXP_BIAS =  8'sd127;
		typedef logic [7:0] exponent_t; // -127 biased exponent
		
		typedef logic signed [24:0] accMant_t;  // Signed mantissa with leading 1
		typedef logic signed [11:0] inMultMant_t;  // Signed mantissa with leading 1
	`endif // DOUBLE_PREC
	
	
	typedef struct packed {
		exponent_t Exp;
		accMant_t Mant;
	} acc_t;

	typedef struct packed {
		exponent_t Exp;
		inMultMant_t Mant;
	} inMult_t;

	typedef enum {CFG_S8 = 0, CFG_HF = 2, CFG_BF = 3} cfg_t;

	parameter CFG_MASK_XF = 2'b10;	

`endif // guard GLOBALS_SV_
