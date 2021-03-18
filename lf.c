#include <stdio.h>
#include <string.h>

unsigned int asciitohex(char);

void main() {
	loadFile();
}

int loadFile() {
	/*
	Function: Load File
	Operation: Load a Motorola S19 file into memory
	Returns: Integer error code
	Date: 20/02/2021
	Version: 1.0
	Change log:
	v1.0 - Implemented basic file load that will read in the length of the file
	Produced by: Jack Walker
	*/
	
	/*Step 1 - Open channel on serial port*/
	/*Step 2 - Parse incoming data into memory*/
	/*Step 3 - Output some form of progress indicator*/
	/*Step 4 - Output when file fully read*/	
		
	unsigned char *scsr, *scdr, data, status, charcount, flag, topNibble, bottomNibble, *addrPtr;
	unsigned int lineLen, origin, hexData, sum, checksum, i;
	int linecount;
	
	charcount = 0, linecount = 0, flag = 0,	status = 0, sum = 0;

	scsr = (unsigned char *) 0x2e;
	scdr = (unsigned char *) 0x2f;
	
	do {
		while(((*scsr) & 0x20) == 0x00);
		
		data = *scdr;
		
		if (data == 'S') {
			flag = 0; /*Reset char count */	
			
			while(((*scsr) & 0x20) == 0x00);
			
			data = *scdr;
			if (data == '1') {
				charcount = 0;				
				putchar('~'); /*Optional to save time remove*/				
				flag = 1;  /*Tell to start counting characters*/				
				linecount++;
			}
			if (data == '9') {
				status = 1; /*Temp stop if S9 detected*/
			}
		}
		
		if (flag == 1) {						
			/*Extract length of line*/	
			while(((*scsr) & 0x20) == 0x00);
			topNibble = asciitohex(*scdr);						
			
			while(((*scsr) & 0x20) == 0x00);
			bottomNibble = asciitohex(*scdr);
			
			/*Shift top nibble to top half of byte and add bottom nibble to form whole byte*/
			lineLen = topNibble << 4 | bottomNibble;
			
			sum += lineLen;
						
			/*Extract start address of line*/
			while(((*scsr) & 0x20) == 0x00);
			topNibble = asciitohex(*scdr);						
			
			while(((*scsr) & 0x20) == 0x00);
			bottomNibble = asciitohex(*scdr);
			
			/*Shift top nibble to top half of byte and add bottom nibble to form whole byte*/
			origin = topNibble << 4 | bottomNibble;
			
			sum += origin;
			
			while(((*scsr) & 0x20) == 0x00);
			topNibble = asciitohex(*scdr);						
			
			while(((*scsr) & 0x20) == 0x00);
			bottomNibble = asciitohex(*scdr);
			
			sum += (topNibble << 4 | bottomNibble);
			
			/*Shift value already stored into the top byte and add the other formed byte to the bottom byte*/
			origin = (origin << 8) + (topNibble << 4 | bottomNibble);
			
			addrPtr = (char *)origin;
			
			/*Loop through data coming in and write direct to memory, starting at the address stored in origin*/		
			for (i = 0; i < lineLen - 3; i++) {
				while(((*scsr) & 0x20) == 0x00);
				topNibble = asciitohex(*scdr);						
			
				while(((*scsr) & 0x20) == 0x00);
				bottomNibble = asciitohex(*scdr);
				
				*addrPtr = topNibble << 4 | bottomNibble;
				sum += *addrPtr;
				addrPtr++;
			}
			
			/*Extract checksum*/
			while(((*scsr) & 0x20) == 0x00);
			topNibble = asciitohex(*scdr);						
			
			while(((*scsr) & 0x20) == 0x00);
			bottomNibble = asciitohex(*scdr);
			
			/*Shift top nibble to top half of byte and add bottom nibble to form whole byte*/
			checksum = topNibble << 4 | bottomNibble;
			
			/*Mask to 8 bit and then invert*/
			sum = sum | 0xff00;
			sum = ~sum;
			
			/*printf("Line len: %02x\t Origin: %04x\t Checksum: %02x\t Calculated checksum: %02x\n\r", lineLen, origin, checksum, sum);*/
											
			flag = 0;
		}	
			
	} while (status == 0);
	
	printf("\n\rDownload completed S %d lines\n\r", linecount);
	printf("Last S1 Address + data + checksum = %02x\n\r", ((charcount - 1) / 2) - 1);
	return(0);
}

/*'0' in ascii is 0x30 and 'a' in ascii is 0x41*/
unsigned int asciitohex(char c) {
	if ('0' <= c && c <= '9') {
		return c - '0';
	}
	if ('A' <= c && c <= 'F') {
		return c + 10 - 'A';
	}
}








