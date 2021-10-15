LD	R1,	0x01
	 .loop		#this is a comment
    ADD    R1,      0x01
	
	CMP R1, 0x09  #hello world
BEQ end
  BRA	  	loop
	  
	  #hello world
	  #go home    man
 
.end
		HALT
