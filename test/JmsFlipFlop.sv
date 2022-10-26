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
 
// First module is a simple D Flip Flop
module jmsslaveflipflop #(
			  parameter WIDTH
			  )
   (
    input logic 	     clk,
    input logic [WIDTH-1:0]  in,
    output logic [WIDTH-1:0] out
    );
   
   always @(posedge clk)
     begin
	out <= in;
     end
endmodule

module JmsFlipFlop #(
		     parameter WIDTH,
		     parameter INITIAL = 0
		     )
   (
    input logic 	    clk,
    input logic [WIDTH-1:0] in,
    output logic [WIDTH-1:0] out
    );
   
   logic 		    iclk; // inverted clock
   logic [WIDTH-1:0] 	    masterOut; // intermediate output of Master

   if(0 != INITIAL) begin
      initial begin
	 out = INITIAL[WIDTH-1:0];
      end
   end    
   
   assign iclk = ~clk;
   
   jmsslaveflipflop #(.WIDTH(WIDTH)) dflop_instM (clk, in, masterOut);
   jmsslaveflipflop #(.WIDTH(WIDTH)) dflop_instS (iclk, masterOut, out);
   
endmodule
