`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2021/01/03 19:48:08
// Design Name: 
// Module Name: TowerSketch
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


module TowerSketch(
	input 			sys_clk	,		//system clock
	input 			rst_n 	,		//reset signal,active low
	input [63:0] 	e 		,		//element to insert
	input 			e_valid			//element valid signal	
    );

	reg [7:0] counter_layer1[0:64*1024-1]	; //the layer 1 counter
	reg [15:0] counter_layer2[0:32*1024-1]	; //the layer 2 counter
	reg [31:0] counter_layer3[0:16*1024-1]	; //the layer 3 counter
	//reg [1024-1:0] counter_layer1[0:7]	; //the layer 1 counter
	//reg [512-1:0] counter_layer2[0:15]	; //the layer 2 counter
	//reg [256-1:0] counter_layer3[0:31]	; //the layer 3 counter

	wire[31:0]		  hash_1 				;
	wire			  hash_1_valid			;
	wire[31:0]		  hash_2				;
	wire			  hash_2_valid			;
	wire[31:0]		  hash_3 				;
	wire			  hash_3_valid			;

	integer 		  i 					; //the layer 1 counter index
	integer 		  j 					; //the layer 2 counter index
	integer 		  k 					; //the layer 3 counter index

	//the ram 1 signal
	reg 			  ena_layer1			;	
	reg 			  wea_layer1			;	
	reg [15:0]		  addra_layer1			;		
	reg [7:0]		  dina_layer1			;
	wire[7:0] 		  douta_layer1			;		
	reg 			  enb_layer1			;	
	reg 			  web_layer1			;	
	reg [15:0]		  addrb_layer1			;		
	reg [7:0]	      dinb_layer1			;	
	wire[7:0] 		  doutb_layer1			;

	reg [7:0]		  douta_layer1_reg 		;
	reg 			  hash_1_valid_0 		;
	reg 			  hash_1_valid_1 		;
	reg 			  hash_1_valid_2 		;
	reg 			  hash_1_valid_3 		;
	reg 			  hash_1_valid_4 		;
	reg  [31:0]		  hash_1_0 				;
	reg  [31:0]		  hash_1_1 				;
	reg  [31:0]		  hash_1_2 				;
	reg  [31:0]		  hash_1_3 				;
	reg  [31:0]		  hash_1_4 				;

	//the ram 2 signal
	reg 			  ena_layer2			;	
	reg 			  wea_layer2			;	
	reg [14:0]		  addra_layer2			;		
	reg [15:0]		  dina_layer2			;
	wire[15:0] 		  douta_layer2			;		
	reg 			  enb_layer2			;	
	reg 			  web_layer2			;	
	reg [14:0]		  addrb_layer2			;		
	reg [15:0]	      dinb_layer2			;	
	wire[15:0] 		  doutb_layer2			;

	reg [7:0]		  douta_layer2_reg 		;
	reg 			  hash_2_valid_0 		;
	reg 			  hash_2_valid_1 		;
	reg 			  hash_2_valid_2 		;
	reg 			  hash_2_valid_3 		;
	reg 			  hash_2_valid_4 		;
	reg [31:0]		  hash_2_0 				;
	reg [31:0]	  	  hash_2_1 				;
	reg [31:0]	      hash_2_2 				;
	reg [31:0]	      hash_2_3 				;
	reg [31:0]	      hash_2_4 				;

	//the ram 3 signal
	reg 			  ena_layer3			;	
	reg 			  wea_layer3			;	
	reg [14:0]		  addra_layer3			;		
	reg [15:0]		  dina_layer3			;
	wire[15:0] 		  douta_layer3			;		
	reg 			  enb_layer3			;	
	reg 			  web_layer3			;	
	reg [14:0]		  addrb_layer3			;		
	reg [15:0]	      dinb_layer3			;	
	wire[15:0] 		  doutb_layer3			;

	reg [7:0]		  douta_layer3_reg 		;
	reg 			  hash_3_valid_0 		;
	reg 			  hash_3_valid_1 		;
	reg 			  hash_3_valid_2 		;
	reg 			  hash_3_valid_3 		;
	reg 			  hash_3_valid_4 		;
	reg  [31:0]		  hash_3_0 				;
	reg  [31:0]		  hash_3_1 				;
	reg  [31:0]		  hash_3_2 				;
	reg  [31:0]		  hash_3_3 				;
	reg  [31:0]		  hash_3_4 				;


	//layer 1
	always @(posedge sys_clk or negedge rst_n) begin
		if(~rst_n) begin
			ena_layer1		<= 1'b0;
			wea_layer1 		<= 1'b0;
			addra_layer1 	<= 16'b0;
			dina_layer1 	<= 8'b0;
			
		end else begin
			if (hash_1_valid == 1'b1) begin
				ena_layer1		<= 1'b1;
				wea_layer1 		<= 1'b0;
				addra_layer1 	<= hash_1%(64*1024);
				dina_layer1 	<= 8'b0;
			end
			else begin
				ena_layer1		<= 1'b0;
				wea_layer1 		<= 1'b0;
				addra_layer1 	<= 16'b0;
				dina_layer1 	<= 8'b0;
			end
		end
	end
	always @(posedge sys_clk or negedge rst_n) begin
		if(~rst_n) begin
			hash_1_valid_0 		<= 1'b0;
			hash_1_valid_1 		<= 1'b0;
			hash_1_valid_2 		<= 1'b0;
			hash_1_valid_3 		<= 1'b0;
			hash_1_valid_4 		<= 1'b0;
			hash_1_0 			<= 32'b0;
			hash_1_1 			<= 32'b0;
			hash_1_2 			<= 32'b0;
			hash_1_3 			<= 32'b0;
			hash_1_4 			<= 32'b0;
			douta_layer1_reg 	<= 8'b0;
		end 
		else begin
			hash_1_valid_0 		<= hash_1_valid;
			hash_1_valid_1 		<= hash_1_valid_0;
			hash_1_valid_2 		<= hash_1_valid_1;
			hash_1_valid_3 		<= hash_1_valid_2;
			hash_1_valid_4 		<= hash_1_valid_3;
			hash_1_0 			<= hash_1	;
			hash_1_1 			<= hash_1_0	;
			hash_1_2 			<= hash_1_1	;
			hash_1_3 			<= hash_1_2	;
			hash_1_4 			<= hash_1_3	;
			douta_layer1_reg 	<= douta_layer1;
		end
	end

	always @(posedge sys_clk or negedge rst_n) begin
		if(~rst_n) begin
			enb_layer1		<= 1'b0;
			web_layer1 		<= 1'b0;
			addrb_layer1 	<= 15'b0;
			dinb_layer1 	<= 8'b0;
		end 
		else begin
			if (hash_1_valid_4 == 1'b1 && douta_layer1_reg != {8{1'b1}}) begin
				enb_layer1		<= 1'b1;
				web_layer1 		<= 1'b1;
				addrb_layer1 	<= hash_1_4%(64*1024);
				dinb_layer1 	<=  douta_layer1 + 1;
			end
			else begin
				enb_layer1		<= 1'b0;
				web_layer1 		<= 1'b0;
				addrb_layer1 	<= 15'b0;
				dinb_layer1 	<= 8'b0;
			end
		end
	end

	//layer 2
	always @(posedge sys_clk or negedge rst_n) begin
		if(~rst_n) begin
			ena_layer2		<= 1'b0;
			wea_layer2 		<= 1'b0;
			addra_layer2 	<= 16'b0;
			dina_layer2 	<= 8'b0;
			
		end else begin
			if (hash_2_valid == 1'b1) begin
				ena_layer2		<= 1'b1;
				wea_layer2 		<= 1'b0;
				addra_layer2 	<= hash_2%(64*1024);
				dina_layer2 	<= 8'b0;
			end
			else begin
				ena_layer2		<= 1'b0;
				wea_layer2 		<= 1'b0;
				addra_layer2 	<= 16'b0;
				dina_layer2 	<= 8'b0;
			end
		end
	end
	always @(posedge sys_clk or negedge rst_n) begin
		if(~rst_n) begin
			hash_2_valid_0 		<= 1'b0;
			hash_2_valid_1 		<= 1'b0;
			hash_2_valid_2 		<= 1'b0;
			hash_2_valid_3 		<= 1'b0;
			hash_2_valid_4 		<= 1'b0;
			hash_2_0 			<= 32'b0;
			hash_2_1 			<= 32'b0;
			hash_2_2 			<= 32'b0;
			hash_2_3 			<= 32'b0;
			hash_2_4 			<= 32'b0;
			douta_layer2_reg 	<= 8'b0;
		end 
		else begin
			hash_2_valid_0 		<= hash_2_valid;
			hash_2_valid_1 		<= hash_2_valid_0;
			hash_2_valid_2 		<= hash_2_valid_1;
			hash_2_valid_3 		<= hash_2_valid_2;
			hash_2_valid_4 		<= hash_2_valid_3;
			hash_2_0 			<= hash_2	;
			hash_2_1 			<= hash_2_0	;
			hash_2_2 			<= hash_2_1	;
			hash_2_3 			<= hash_2_2	;
			hash_2_4			<= hash_2_3	;
			douta_layer2_reg 	<= douta_layer2;
		end
	end

	always @(posedge sys_clk or negedge rst_n) begin
		if(~rst_n) begin
			enb_layer2		<= 1'b0;
			web_layer2 		<= 1'b0;
			addrb_layer2 	<= 15'b0;
			dinb_layer2 	<= 8'b0;
		end 
		else begin
			if (hash_2_valid_4 == 1'b1 && douta_layer2_reg != {16{1'b1}}) begin
				enb_layer2		<= 1'b1;
				web_layer2 		<= 1'b1;
				addrb_layer2 	<= hash_2_4%(32*1024);
				dinb_layer2 	<=  douta_layer2 + 1;
			end
			else begin
				enb_layer2		<= 1'b0;
				web_layer2 		<= 1'b0;
				addrb_layer2 	<= 15'b0;
				dinb_layer2 	<= 8'b0;
			end
		end
	end

	//layer 3
	always @(posedge sys_clk or negedge rst_n) begin
		if(~rst_n) begin
			ena_layer3		<= 1'b0;
			wea_layer3 		<= 1'b0;
			addra_layer3 	<= 16'b0;
			dina_layer3 	<= 8'b0;
			
		end else begin
			if (hash_3_valid == 1'b1) begin
				ena_layer3		<= 1'b1;
				wea_layer3 		<= 1'b0;
				addra_layer3 	<= hash_3%(64*1024);
				dina_layer3	<= 8'b0;
			end
			else begin
				ena_layer3		<= 1'b0;
				wea_layer3 		<= 1'b0;
				addra_layer3 	<= 16'b0;
				dina_layer3 	<= 8'b0;
			end
		end
	end
	always @(posedge sys_clk or negedge rst_n) begin
		if(~rst_n) begin
			hash_3_valid_0 		<= 1'b0;
			hash_3_valid_1 		<= 1'b0;
			hash_3_valid_2 		<= 1'b0;
			hash_3_valid_3 		<= 1'b0;
			hash_3_valid_4 		<= 1'b0;
			hash_3_0 			<= 32'b0;
			hash_3_1 			<= 32'b0;
			hash_3_2 			<= 32'b0;
			hash_3_3 			<= 32'b0;
			hash_3_4 			<= 32'b0;
			douta_layer3_reg 	<= 8'b0;
		end 
		else begin
			hash_3_valid_0 		<= hash_3_valid;
			hash_3_valid_1 		<= hash_3_valid_0;
			hash_3_valid_2 		<= hash_3_valid_1;
			hash_3_valid_3 		<= hash_3_valid_2;
			hash_3_valid_4 		<= hash_3_valid_3;
			hash_3_0 			<= hash_3	;
			hash_3_1 			<= hash_3_0	;
			hash_3_2 			<= hash_3_1	;
			hash_3_3 			<= hash_3_2	;
			hash_3_4 			<= hash_3_3	;
			douta_layer3_reg 	<= douta_layer3;
		end
	end

	always @(posedge sys_clk or negedge rst_n) begin
		if(~rst_n) begin
			enb_layer3		<= 1'b0;
			web_layer3 		<= 1'b0;
			addrb_layer3 	<= 15'b0;
			dinb_layer3 	<= 8'b0;
		end 
		else begin
			if (hash_3_valid_4 == 1'b1 && douta_layer3_reg != {32{1'b1}}) begin
				enb_layer3		<= 1'b1;
				web_layer3 		<= 1'b1;
				addrb_layer3 	<= hash_3_4%(16*1024);
				dinb_layer3 	<=  douta_layer3 + 1;
			end
			else begin
				enb_layer3		<= 1'b0;
				web_layer3 		<= 1'b0;
				addrb_layer3 	<= 15'b0;
				dinb_layer3 	<= 8'b0;
			end
		end
	end


	

	//generate the hash of layer1
	crc32_64bit_gen hash_gen1(
    .clk		(sys_clk			), 	//system reset
	.reset_n	(rst_n 				),	//reset signal, active low
	.data		(e+64'd1 			),	//insert element
	.datavalid	(e_valid			),	//element valid
	.checksum	(hash_1				),
	.crcvalid	(hash_1_valid		)
	);

	//generate the hash of layer2
	crc32_64bit_gen hash_gen2(
    .clk		(sys_clk			), 	//system reset
	.reset_n	(rst_n 				),	//reset signal, active low
	.data		(e*2+64'd1 			),	//insert element
	.datavalid	(e_valid			),	//element valid
	.checksum	(hash_2				),
	.crcvalid	(hash_2_valid		)
	);

	//generate the hash of layer3
	crc32_64bit_gen hash_gen3(
    .clk		(sys_clk			), 	//system reset
	.reset_n	(rst_n 				),	//reset signal, active low
	.data		(e*3+64'd1 			),	//insert element
	.datavalid	(e_valid			),	//element valid
	.checksum	(hash_3				),
	.crcvalid	(hash_3_valid		)
	);

	//layer 1 ram
	ram_8_64k counter_ram_layer1 (
  	.clka		(sys_clk			),    // input wire clka
  	.ena		(ena_layer1			),      // input wire ena
  	.wea		(wea_layer1			),      // input wire [0 : 0] wea
  	.addra		(addra_layer1		),  // input wire [15 : 0] addra
  	.dina		(dina_layer1		),    //),  // output wire [7 : 0] douta
  	.douta 		(douta_layer1		),  // output wire [7 : 0] douta
  	.clkb		(clkb_layer1		),    // input wire clkb
  	.enb		(enb_layer1			),      // input wire enb
  	.web		(web_layer1			),      // input wire [0 : 0] web
  	.addrb		(addrb_layer1		),  // input wire [15 : 0] addrb
  	.dinb		(dinb_layer1		),    // input wire [7 : 0] dinb
  	.doutb		(doutb_layer1		)  // output wire [7 : 0] doutb
);

	//layer 1 ram
	ram_16_32k counter_ram_layer2 (
  	.clka		(sys_clk			),    // input wire clka
  	.ena		(ena_layer2			),      // input wire ena
  	.wea		(wea_layer2			),      // input wire [0 : 0] wea
  	.addra		(addra_layer2		),  // input wire [15 : 0] addra
  	.dina		(dina_layer2		),    //),  // output wire [7 : 0] douta
  	.douta 		(douta_layer2		),  // output wire [7 : 0] douta
  	.clkb		(clkb_layer2		),    // input wire clkb
  	.enb		(enb_layer2			),      // input wire enb
  	.web		(web_layer2			),      // input wire [0 : 0] web
  	.addrb		(addrb_layer2		),  // input wire [15 : 0] addrb
  	.dinb		(dinb_layer2		),    // input wire [7 : 0] dinb
  	.doutb		(doutb_layer2		)  // output wire [7 : 0] doutb
);

	//layer 1 ram
	ram_32_16k counter_ram_layer3 (
  	.clka		(sys_clk			),    // input wire clka
  	.ena		(ena_layer3			),      // input wire ena
  	.wea		(wea_layer3			),      // input wire [0 : 0] wea
  	.addra		(addra_layer3		),  // input wire [15 : 0] addra
  	.dina		(dina_layer3		),    //),  // output wire [7 : 0] douta
  	.douta 		(douta_layer3		),  // output wire [7 : 0] douta
  	.clkb		(clkb_layer3		),    // input wire clkb
  	.enb		(enb_layer3			),      // input wire enb
  	.web		(web_layer3			),      // input wire [0 : 0] web
  	.addrb		(addrb_layer3		),  // input wire [15 : 0] addrb
  	.dinb		(dinb_layer3		),    // input wire [7 : 0] dinb
  	.doutb		(doutb_layer3		)  // output wire [7 : 0] doutb
);



endmodule
