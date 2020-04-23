/*#include "stdio.h"*/
#include "stdarg.h"
#include "string.h"
#include "standard.h"
#include <stdlib.h>
/* #include <libkern/OSByteOrder.h> */
#
#define SWAP_SHORT(Var)  Var = *(short*)         SwapEndian((void*)&Var, sizeof(short))
#define SWAP_USHORT(Var) Var = *(unsigned short*)SwapEndian((void*)&Var, sizeof(short))
#define SWAP_LONG(Var)   Var = *(long*)          SwapEndian((void*)&Var, sizeof(long))
#define SWAP_ULONG(Var)  Var = *(unsigned long*) SwapEndian((void*)&Var, sizeof(long))
#define SWAP_RGB(Var)    Var = *(int*)           SwapEndian((void*)&Var, 3)
#define SWAP_FLOAT(Var)  Var = *(float*)         SwapEndian((void*)&Var, sizeof(float))
#define SWAP_DOUBLE(Var) Var = *(double*)        SwapEndian((void*)&Var, sizeof(double))

extern void *SwapEndian(void* Addr, const int Nb);
static size_t byteSwap(void *ptr, size_t nitems, size_t size, int flags);
/* 
   Useful procedures that can be used for all programs
*/

/*
  error -- prints error message with arguements "..." with format "fmt" 
  and then exits program. From K&R.
*/

void error( char *fmt, ... )
{
	va_list args;

	va_start(args,fmt);
	fprintf(stderr,"error:  ");
	vfprintf(stderr,fmt,args);
	fprintf(stderr, "\n");
	va_end(args);
	exit(1);
}

void warning(FILE *fp, char *fmt, ... )
{
	va_list args;

	va_start(args,fmt);
	fprintf(stderr,"\nWarning:  ");
	if(fp !=NULL) fprintf(fp,"\nWarning:  ");
	vfprintf(stderr,fmt,args);
	if(fp !=NULL) vfprintf(fp,fmt,args);
	fprintf(stderr, "\n");
	if(fp !=NULL) fprintf(fp, "\n");
	va_end(args);
}


/* getline:  read a line, return length.  From K&R */

int getlineobsolete(char *line, int imax)
{
	if(fgets(line,imax,stdin) == NULL)
		return 0;
	else
		return strlen(line);
}
 
/* fgetline:  read a line from a file, return length. */

int fgetline(FILE *fp, char *line, int imax)
{
	if( feof(fp) == EOF ) error("%s","fgetline -- EOF encountered");
	if( ferror(fp) != 0 ) error("%s","fgetline -- file error");

	if(fgets(line,imax,fp) == NULL) {
		return 0;
	}else {
		return strlen(line);
	}
}


/* 
   Search through file for next occurence of BEGINDATA n, 
   where n = number of column. Return line count
*/

int goToBeginningOfData(FILE *fp, int lineCount, int *ncolumns)
{
	int lineLength;  
	char line[LINEMAX];
	char *string;

	while( TRUE ) {  /* Loop to read lines */
 
		lineLength = fgetline(fp,line,LINEMAX); /* Read line */
		lineCount++;
		if(lineLength > 0) {
			if( strchr(line,ENDDATA) != NULL ) /* Read data */
				error("%s","goToBeginningOfData -- & with no matching # "); 
			else if( (string = strchr(line,BEGINDATA) ) != NULL ) { 
				if( sscanf((string+1),"%i",ncolumns) != 1) /*If BEGINDATA,get */
					error("%s",
					      "goToBeginningOfData -- # found without n columns ");
				return lineCount;   /* Found so exit */
			}  
		}
	}
}


/* 
   Read through file returning data strings.
   If end of data eod = TRUE else eod = FALSE.
*/

int getDataString(FILE *fp, int lineCount, char *line,int *eod)
{
	int lineLength;  
	int count;
	count = 0;
	while( TRUE ) {  /* Loop to read lines */
 
		lineLength = fgetline(fp,line,LINEMAX); /* Read line */
		lineCount++;
		count++;
		if( strchr(line,COMMENT) == NULL &&   
		    strpbrk(line,
			    "0123456789.ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz")
		    != NULL ) {
			*eod = FALSE;
			return lineCount; /* Data */ 
		}
         
		if( strchr(line,ENDDATA) != NULL &&
		    strchr(line,COMMENT) == NULL ) { /* End of data */
			*eod = TRUE;
			return lineCount;
		}
		if( count >= 100000) error("getDataString: Missing end of data");
	}
}


/* Search through file for next occurence of ENDDATA */

int goToEndOfData(FILE *fp, int lineCount, int *ndata)
{
	int lineLength;  
	char line[LINEMAX];

	while( TRUE ) {  /* Loop to read lines */
 
		lineLength = fgetline(fp,line,LINEMAX); /* Read line */
		lineCount++;
		if( strchr(line,COMMENT) == NULL &&   
		    strpbrk(line,"0123456789.EDed") != NULL ) (*ndata)++; /*Count data*/

		if( strchr(line,ENDDATA) != NULL ) return lineCount;
	}
}

/* Search through file for next occurence of ENDDATA */

int goToNextStringOccurence(FILE *fp, int lineCount, char *string,char *line)
{
	int lineLength;  

	while( TRUE ) {  /* Loop to read lines */
		lineLength = fgetline(fp,line,LINEMAX); /* Read line */
		lineCount++;
		if(feof(fp) != 0) return -1;
		if( strstr(line,string) != NULL) return lineCount;
	}
}


/* Open file for input, exit if error occurs */

FILE *openInputFile(char *filename)
{
	FILE *fp;

	if( (fp = fopen(filename,"r")) == NULL )  /* Error if unable to open file */
		error("\n\n%s |%s|\n","openInputFile -- Unable to open file: ",filename);
	return fp;
}


size_t byteSwap(void *ptr, size_t nitems, size_t size, int flags) {
	long long i;	
	short int *ptr16;
	unsigned long *ptrLong;
	double *ptrDouble;
	size_t n;
	/*
	  No need to swap byte data 
	*/
	switch(flags) {
	case BYTEFLAG :
		n = (size_t) nitems;
		break;
	case FLOAT32FLAG :
		n=(nitems*size)/sizeof(float);
		ptrLong = (unsigned long *)ptr;
		for(i = 0; i < n; i++) ptrLong[i] = SWAP_ULONG(ptrLong[i]);
		break;		
	case FLOAT64FLAG :
		n = (nitems*size)/sizeof(double);
		ptrDouble = (double *)ptr;
		for(i = 0; i < n; i++) ptrDouble[i] = SWAP_DOUBLE(ptrDouble[i]);
		break;		
	case INT32FLAG :
		n = (nitems*size)/sizeof(long int);
		ptrLong = (unsigned long *)ptr;
		for(i = 0; i < n; i++) ptrLong[i] = SWAP_ULONG(ptrLong[i]);
		break;		
	case INT16FLAG :
		n = (nitems*size)/sizeof(short int);
		ptr16 = (short int *)ptr;
		for(i = 0; i < n; i++) ptr16[i] = SWAP_USHORT(ptr16[i]);
		break;		
	case INT16FLAGNOSWAP :
		n = (nitems*size)/sizeof(short int);
		break;		
	}
	return((size_t) n);	
}	

/*
  Read raw data and byteswap if needed
*/ 
size_t freadBS(void *ptr, size_t nitems, size_t size, FILE *fp,int flags)
{
	int swap;
	size_t retVal;
	/* Use fread to read the data as requested */
	retVal = fread(ptr,nitems,size,fp);
#ifdef i386
	swap = TRUE;
#endif
#ifdef x86_64
	swap = TRUE;
#endif
	if(swap == TRUE) byteSwap(ptr, nitems, size, flags);
	else error("No current noSwap architecture defined  - code needs update\n");
	return retVal;
}
/*
  Read raw data and byteswap if needed
*/ 
size_t fwriteBS(void *ptr, size_t nitems, size_t size, FILE *fp,int flags)
{
	size_t retVal;
	int swap;
	/*
	  ++++++++++++++++++++++++++++++start i386++++++++++++++++++++++++++++++++++++++++
	*/
	if(flags != FLOAT32FLAG && flags != FLOAT64FLAG && flags != INT32FLAG && flags != INT16FLAG && flags != BYTEFLAG) error("fwriteBS: invalid flag");	
	swap = FALSE;
#ifdef i386
	swap=TRUE;
#endif
#ifdef x86_64
	swap=TRUE;
#endif 
	if(swap == FALSE) error("No current noSwap architecture defined  - code needs update\n");
	if(swap == TRUE)  byteSwap(ptr, nitems, size, flags);
	retVal=fwrite(ptr,nitems,size,fp);
	/* Swap back */
	if(swap == TRUE)  byteSwap(ptr, nitems, size, flags);
	return(retVal);
}


static long _TestEndian=1;

int IsLittleEndian(void) {
	return *(char*)&_TestEndian;
}

/******************************************************************************
  FUNCTION: SwapEndian
  PURPOSE: Swap the byte order of a structure
  EXAMPLE: float F=123.456;; SWAP_FLOAT(F);
******************************************************************************/

void *SwapEndian(void* Addr, const int Nb) {
	static char Swapped[16];
	switch (Nb) {
	case 2:	Swapped[0]=*((char*)Addr+1);
		Swapped[1]=*((char*)Addr  );
		break;
	case 3:	// As far as I know, 3 is used only with RGB images
		Swapped[0]=*((char*)Addr+2);
		Swapped[1]=*((char*)Addr+1);
		Swapped[2]=*((char*)Addr  );
		break;
	case 4:	Swapped[0]=*((char*)Addr+3);
		Swapped[1]=*((char*)Addr+2);
		Swapped[2]=*((char*)Addr+1);
		Swapped[3]=*((char*)Addr  );
		break;
	case 8:	Swapped[0]=*((char*)Addr+7);
		Swapped[1]=*((char*)Addr+6);
		Swapped[2]=*((char*)Addr+5);
		Swapped[3]=*((char*)Addr+4);
		Swapped[4]=*((char*)Addr+3);
		Swapped[5]=*((char*)Addr+2);
		Swapped[6]=*((char*)Addr+1);
		Swapped[7]=*((char*)Addr  );
		break;
	case 16:Swapped[0]=*((char*)Addr+15);
		Swapped[1]=*((char*)Addr+14);
		Swapped[2]=*((char*)Addr+13);
		Swapped[3]=*((char*)Addr+12);
		Swapped[4]=*((char*)Addr+11);
		Swapped[5]=*((char*)Addr+10);
		Swapped[6]=*((char*)Addr+9);
		Swapped[7]=*((char*)Addr+8);
		Swapped[8]=*((char*)Addr+7);
		Swapped[9]=*((char*)Addr+6);
		Swapped[10]=*((char*)Addr+5);
		Swapped[11]=*((char*)Addr+4);
		Swapped[12]=*((char*)Addr+3);
		Swapped[13]=*((char*)Addr+2);
		Swapped[14]=*((char*)Addr+1);
		Swapped[15]=*((char*)Addr  );
		break;
	}
	return (void*)Swapped;
}

