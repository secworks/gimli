//======================================================================
//
// gimli_core.v
// -------------
// Verilog 2001 implementation of the SHA-256 hash function.
// This is the internal core with wide interfaces.
//
//
// Author: Joachim Strombergson
// Copyright (c) 2013, Secworks Sweden AB
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or
// without modification, are permitted provided that the following
// conditions are met:
//
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in
//    the documentation and/or other materials provided with the
//    distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//======================================================================

`default_nettype none

module gimli_core(
                   input wire            clk,
                   input wire            reset_n,

                   input wire            init,
                   input wire            next,
                   input wire            mode,

                   input wire [383 : 0]  block,

                   output wire [383 : 0] new_block,
                   output wire           ready
                  );


  //----------------------------------------------------------------
  // Internal constant and parameter definitions.
  //----------------------------------------------------------------
  localparam CTRL_IDLE = 0;
  localparam CTRL_INIT = 1;
  localparam CTRL_DONE = 15;


  //----------------------------------------------------------------
  // Registers including update variables and write enable.
  //----------------------------------------------------------------
  reg [383 : 0] block_reg;
  reg [383 : 0] block_new;
  reg           block_we;

  reg [4 : 0] round_ctr_reg;
  reg [4 : 0] round_ctr_new;
  reg         round_ctr_inc;
  reg         round_ctr_rst;
  reg         round_ctr_we;

  reg         ready_reg;
  reg         ready_new;
  reg         ready_we;

  reg [3 : 0] gimli_ctrl_reg;
  reg [3 : 0] gimli_ctrl_new;
  reg         gimli_ctrl_we;


  //----------------------------------------------------------------
  // Wires.
  //----------------------------------------------------------------


  //----------------------------------------------------------------
  // Concurrent connectivity for ports etc.
  //----------------------------------------------------------------
  assign ready     = ready_reg;
  assign new_block = block_reg;


  //----------------------------------------------------------------
  // reg_update
  //----------------------------------------------------------------
  always @ (posedge clk)
    begin : reg_update
      if (!reset_n) begin
	ready_reg      <= 1'h1;
	block_reg      <= 384'h0;
	round_ctr_reg  <= 5'h0;
	gimli_ctrl_reg <= CTRL_IDLE;
      end

      else begin
	if (ready_we) begin
	  ready_reg <= ready_new;
	end

	if (block_we) begin
	  block_reg <= block_new;
	end

	if (round_ctr_we) begin
	  round_ctr_reg <= round_ctr_new;
	end

	if (gimli_ctrl_we) begin
	  gimli_ctrl_reg <= gimli_ctrl_new;
	end
      end
    end // reg_update


  //----------------------------------------------------------------
  // round_ctr
  //----------------------------------------------------------------
  always @*
    begin : round_ctr
      round_ctr_new = 5'h0;
      round_ctr_we  = 1'h0;

      if (round_ctr_inc) begin
	round_ctr_new = round_ctr_reg + 1'h1;
	round_ctr_we  = 1'h1;
      end

      else if (round_ctr_rst) begin
	round_ctr_new = 5'h0;
	round_ctr_we  = 1'h1;
      end
    end


  //----------------------------------------------------------------
  // gimli_ctrl_fsm
  //----------------------------------------------------------------
  always @*
    begin : gimli_ctrl_fsm
      ready_new      = 1'h0;
      ready_we       = 1'h0;
      block_we       = 1'h0;
      round_ctr_inc  = 1'h0;
      round_ctr_rst  = 1'h0;
      gimli_ctrl_new = CTRL_IDLE;
      gimli_ctrl_we  = 1'h0;

      block_new = block;

      case (gimli_ctrl_ref)
	CTRL_IDLE: begin
	  if (init) begin
	    ready_new      = 1'h0;
	    ready_we       = 1'h1;
	    gimli_ctrl_new = CTRL_INIT;
	    gimli_ctrl_we  = 1'h1;
	  end
	end


	CTRL_INIT: begin
	  block_we       = 1'h1;
	  gimli_ctrl_new = CTRL_DONE;
	  gimli_ctrl_we  = 1'h1;
	end


	CTRL_DONE: begin
	  ready_new      = 1'h1;
	  ready_we       = 1'h1;
	  gimli_ctrl_new = CTRL_IDLE;
	  gimli_ctrl_we  = 1'h1;
	end


	default: begin
	end
      endcase // case (gimli_ctrl_ref)


    end // gimli_ctrl_fsm

endmodule // gimli_core

//======================================================================
// EOF gimli_core.v
//======================================================================
