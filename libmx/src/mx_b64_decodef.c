#include "../inc/libmx.h"

unsigned int mx_b64_decodef(char *InFile, char *OutFile) {

	FILE *pInFile = fopen(InFile,"rb");
	FILE *pOutFile = fopen(OutFile,"wb");
	
	unsigned int c=0;
	unsigned int j=0;
	unsigned int k=0;
	unsigned int s[4];
	
	if ((pInFile==NULL) || (pOutFile==NULL) ) {
		if (pInFile!=NULL){fclose(pInFile);}
		if (pOutFile!=NULL){fclose(pOutFile);}
		return 0;
	}
	
	while((int)c!=EOF) {
		c=fgetc(pInFile);
		if ((int)c==EOF)
		break;
		s[j++]=mx_b64_int(c);
		if (j==4) {
			fputc(((s[0]&255)<<2)+((s[1]&0x30)>>4),pOutFile);
			if (s[2]!=64) {
				fputc(((s[1]&0x0F)<<4)+((s[2]&0x3C)>>2),pOutFile);
				if ((s[3]!=64)) {
					fputc(((s[2]&0x03)<<6)+(s[3]),pOutFile); k+=3;
				} else {
					k+=2;
				}
			} else {
				k+=1;
			}
			j=0;
		}
	}

	fclose(pInFile);
	fclose(pOutFile);
	
	return k;
}
