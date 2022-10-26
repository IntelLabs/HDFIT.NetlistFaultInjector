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

`include "globals.sv"

module fma(
	   input logic [3:0]  a,
	   input logic [3:0]  b,
	   input logic [3:0]  c,
	   output logic [3:0] d,
	   input logic 	      clk);
   
   logic [3:0] 		      mul;
   assign mul = a * b;

   logic [3:0] 		      mulStg2;
   `MSFF(mulStg2, mul, clk);

   logic [3:0] 		      cStg2;
   `MSFF(cStg2, c, clk);

   assign d = mulStg2 + cStg2;
     
endmodule; // fma

