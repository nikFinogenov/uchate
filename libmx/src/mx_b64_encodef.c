#include "../inc/libmx.h"

const unsigned char b64_chr[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
unsigned int b64_encodef(char *InFile, char *OutFile) {

	FILE *pInFile = fopen(InFile,"rb");
	FILE *pOutFile = fopen(OutFile,"wb");
	
	unsigned int i=0;
	unsigned int j=0;
	unsigned int c=0;
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
		s[j++]=c;
		if (j==3) {
			fputc(b64_chr[ (s[0]&255)>>2 ],pOutFile);
			fputc(b64_chr[ ((s[0]&0x03)<<4)+((s[1]&0xF0)>>4) ],pOutFile);
			fputc(b64_chr[ ((s[1]&0x0F)<<2)+((s[2]&0xC0)>>6) ],pOutFile);
			fputc(b64_chr[ s[2]&0x3F ],pOutFile);
			j=0; i+=4;
		}
	}
	
	if (j) {
		if (j==1)
			s[1] = 0;
		fputc(b64_chr[ (s[0]&255)>>2 ],pOutFile);
		fputc(b64_chr[ ((s[0]&0x03)<<4)+((s[1]&0xF0)>>4) ],pOutFile);
		if (j==2)
			fputc(b64_chr[ ((s[1]&0x0F)<<2) ],pOutFile);
		else
			fputc('=',pOutFile);
		fputc('=',pOutFile);
		i+=4;
	}
	
	fclose(pInFile);
	fclose(pOutFile);
	
	return i;
}
