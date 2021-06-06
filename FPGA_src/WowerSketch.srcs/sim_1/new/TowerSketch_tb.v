`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2021/01/03 20:34:30
// Design Name: 
// Module Name: TowerSketch_tb
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module TowerSketch_tb(

    );


	parameter ClockPeriod = 8 ;

	reg 		sys_clk 	; 	//system clock, 125Mhz
	reg 		rst_n 		;	//reset signal, active low
	reg [63:0]	e 			;	//element to insert
	reg 		e_valid 	;	//element valid

	//generate the system clock
	initial begin
   		sys_clk = 1;
  		forever 
    	# (ClockPeriod/2) sys_clk = ~sys_clk ;
 	end

 	//gerate the reset signal
 	initial begin
 		rst_n = 0 ;
     	#(10 * ClockPeriod) ;
    	rst_n = 1 ;
 	end

 	//generate the element
 	initial begin
 		e 		<= 64'b0		;
 		e_valid <= 1'b0		;
 		/*

 		# (12 * ClockPeriod)	
 		e 		<= {{$random},{$random}};
 		e_valid <= 1'b1		; 

 		# (ClockPeriod		)
 		e 		<= {$random}	;
 		e_valid <= 1'b1		; 

 		for (integer i = 0; i < 100; i = i +1) begin
 			# (ClockPeriod		)
 			e 		<= {{$random},{$random}}	;
 			e_valid <= 1'b1		;
 		end
*/
	# (12 * ClockPeriod)
 		# (ClockPeriod		)
 		e 		<= 64'b0		;
 		e_valid <= 1'b0		;

 		# (ClockPeriod		)
 		e 		<= {{$random},{$random}};
 		e_valid <= 1'b1		;

 		# (ClockPeriod		)
 		e 		<= 64'b0		;
 		e_valid <= 1'b0		;
 	end

 	//instantiate the MINGA_TOP
	TowerSketch TowerSketch(
	.sys_clk	(sys_clk	),		//system clock
	.rst_n 		(rst_n		),		//reset signal,active low
	.e 			(e 			),		//element to insert
	.e_valid	(e_valid 	)		//element valid signal	
    );


	
endmodule

