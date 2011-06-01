/*

 nasm -f elf64 -g -F stabs int64.asm   ;for int64.o
 gcc intburn.c cpuidc64.c  int64.o -m64 -lrt -lc -lm -O3 -o intburn64
 ./intburn64

*/

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include "cpuidh.h"
 #include <math.h>
 #include <malloc.h>
 #include <mm_malloc.h>

//  ************** PROTOPTYPES *****************


void checkData();



// ************* GLOBAL VARIABLES **********

long long *xx;
long long sumCheck[8];

long long mempasses;
long long outerLoop;
long long calcResult =  { 0x0000000000000000 }; 

long long usePattern1;
long long usePattern2;
long long isdata[16];
long long sbdata[16];
long long passbytes;
long long pattern1[16];
long long pattern2[16];
long long tempPattern;
long long printPattern;

FILE    *outfile;

char    logfile[80];
char    iparam[100] = "";
char    writeMsg1[256];
char    writeMsg2[256];
char    version[30] = "64 Bit Version 1.0";
char    commandLine[1024] = "";
char    patternSB[20];
char    patternIs[20];
char    testmsg[30][256];
char    errormsg[64] = " ";

int     RrunSecs = 1;
int     testPasses = 1;
int     n1;
int     errors;
int     errord;
int     errorp;
int     i0Start;
int     testlog = 0;
int     errword[16];

unsigned int useMemK = 4; // 2048;

double maxMIPS0;
double maxMIPS1;
double maxMIPS2;
double millionBytes;
double results[30][10];
double n1Count;
double timeCount;


int main(int argc, char *argv[])
{
    int   g, i, j, p;
    int   param1;
    float tp;
    char  val[20];

    for (i=1; i<7; i=i+2)
    {
       if (argc > i)
       {
          strcat(iparam, argv[i]); 
          switch (toupper(argv[i][0]))
          {
               case 'K':
                param1 = 0;
                if (argc > i+1)
                {
                   sscanf(argv[i+1],"%d", &param1);
                   sprintf(val, " %d, ", param1); 
                   strcat(iparam, val); 
                   if (param1 > 0) useMemK = param1;
                }
                break;
                
                case 'S':
                param1 = 0;
                if (argc > i+1)
                {
                   sscanf(argv[i+1],"%d", &param1);
                   sprintf(val, " %d, ", param1); 
                   strcat(iparam, val); 
                   if (param1 > 0) RrunSecs = param1;
                }
                break;

                case 'L':
                param1 = 0;
                if (argc > i+1)
                {
                   sscanf(argv[i+1],"%d", &param1);
                   sprintf(val, " %d, ", param1); 
                   strcat(iparam, val); 
                   if (param1 > 0) testlog = param1;
                   if (testlog > 99) testlog = 0;
                }
                break;

          }
       }
    }
    sprintf (logfile, "Burnlog%d.txt", testlog);
    if (RrunSecs > 10)
    {
        tp = (float)RrunSecs / 10.0;
        testPasses = (int)tp;
        RrunSecs = (int)((float)RrunSecs / (float)testPasses);
    }
    
    pattern1[0]  = 0x0000000000000000;
    pattern1[1]  = 0xFFFFFFFFFFFFFFFF;
    pattern1[2]  = 0xA5A5A5A5A5A5A5A5;
    pattern1[3]  = 0x5555555555555555;
    pattern1[4]  = 0x3333333333333333;
    pattern1[5]  = 0xF0F0F0F0F0F0F0F0;
    pattern1[6]  = 0x0000000000000000;
    pattern1[7]  = 0xFFFFFFFFFFFFFFFF;
    pattern1[8]  = 0xA5A5A5A5A5A5A5A5;
    pattern1[9]  = 0x5555555555555555;
    pattern1[10] = 0x3333333333333333;
    pattern1[11] = 0xF0F0F0F0F0F0F0F0;

    pattern2[0]  = 0x0000000000000000;
    pattern2[1]  = 0xFFFFFFFFFFFFFFFF;
    pattern2[2]  = 0x5A5A5A5A5A5A5A5A;
    pattern2[3]  = 0xAAAAAAAAAAAAAAAA;
    pattern2[4]  = 0xCCCCCCCCCCCCCCCC;
    pattern2[5]  = 0x0F0F0F0F0F0F0F0F;
    pattern2[6]  = 0xFFFFFFFFFFFFFFFF;
    pattern2[7]  = 0x0000000000000000;
    pattern2[8]  = 0x5A5A5A5A5A5A5A5A;
    pattern2[9]  = 0xAAAAAAAAAAAAAAAA;
    pattern2[10] = 0xCCCCCCCCCCCCCCCC;
    pattern2[11] = 0x0F0F0F0F0F0F0F0F;

    outfile = fopen(logfile,"a+");
    if (outfile == NULL)
    {
        printf ("Cannot open results file \n\n");
        printf(" Press Enter\n");
        g  = getchar();
        exit (0);
    }
    printf("\n");
    getDetails();
    local_time();
    for (i=1; i<10; i++)
    {
        printf("%s\n", configdata[i]);
    }
    printf("\n");
    fprintf (outfile, " ###############################################################\n\n");                     
    for (i=1; i<10; i++)
    {
        fprintf(outfile, "%s \n", configdata[i]);
    }
    fprintf (outfile, "\n");
 
    printf(" ##########################################\n"); 

    fprintf(outfile, "###############################################################\n \n");
    fprintf(outfile, "           Linux %s Integer Reliability Test\n\n", version);
    fprintf(outfile, "                   Copyright (C) Roy Longbottom 2011\n\n");
    strcpy(writeMsg2, " Batch Command ");
    strncat(writeMsg2, iparam, 100);
    fprintf(outfile, " %s\n\n", writeMsg2);
    printf(" %s\n\n", writeMsg2);
    sprintf(writeMsg1, " Test %d KB at %dx%d seconds per test", useMemK, RrunSecs, testPasses);
    fprintf(outfile, " %s, Start at %s\n", writeMsg1, timeday);
    printf(" %s, Start at %s\n", writeMsg1, timeday);
    fflush(outfile);

    sprintf(errormsg, " ");
    xx = (long long *)_mm_malloc((long long)useMemK*1024+256, 16);
    if (xx == NULL)
    {
       printf(" ERROR WILL EXIT\n");
       fprintf(outfile, " Failed to allocate memory\n");
       printf(" Press Enter\n");
       g  = getchar();
       exit(1);
    }
    fprintf(outfile, " Write/Read\n");
    printf(" Write/Read\n");
    fflush(outfile);
    maxMIPS2 = 0;
    maxMIPS1 = 0;
    for (j=6; j<12; j++)
    {
        printPattern = pattern1[j];
        usePattern1 = pattern2[j];
        usePattern2 = pattern1[j];
        millionBytes = (double) useMemK * 1.024 / 1000;
        mempasses = useMemK * 8; // 1024 / 128;
        results[j][0] = 0;
        n1Count = 0;
        timeCount = 0;

        for (p=0; p<testPasses; p++)
        {
            n1 = 0;
            start_time();
            do
            {
                tempPattern = usePattern1;
                usePattern1 = usePattern2;
                usePattern2 = tempPattern;
                xx[0]   = usePattern1;
                xx[1]   = usePattern1;
                xx[2]   = usePattern1;
                xx[3]   = usePattern1;
                xx[4]   = usePattern2;
                xx[5]   = usePattern2;
                xx[6]   = usePattern2;
                xx[7]   = usePattern2;
                _writeData();                
                checkData();
                n1 = n1 + 1;
                end_time();
            }
            while (secs < (double)RrunSecs && errors == 0);
            n1Count = n1Count + (double)n1;
            timeCount = timeCount + secs;
            printf(" L%2dP %3d Test%2d of 6, Pattern %16.16llX, %4.1f Secs,  %6.0f MB/S\n",
                       testlog, p+1, j-5, printPattern, secs, (millionBytes * 2 * (double)n1 / secs));
        }
        results[j][0] = millionBytes * 2 * n1Count / timeCount;
        maxMIPS0 = n1Count * 54 * (double)mempasses / 1000000 / timeCount;
        if (maxMIPS0 > maxMIPS1) maxMIPS1 = maxMIPS0;

        sprintf(patternSB, "%16.16llX", usePattern1);
        if (errors == 0)
        {
             sprintf(testmsg[j], " Pattern %16.16llX Result OK %10.0f passes", printPattern, n1Count);
        }
        else
        {
            sprintf(testmsg[j], " Pattern %s  ERROR", patternSB);
        }
        fprintf(outfile, " %2d %7.0f MB/sec %s\n", j-5, results[j][0], testmsg[j]);
        if (errors > 0)
        {
            fprintf(outfile, " %dK 4 byte words checked and %d errors found\n", useMemK/4, errors);
            if (errors < 12)
            {
                fprintf(outfile, " %d errors\n", errord);
            }
            else
            {
                fprintf(outfile, " First 12 errors\n");
            } 
            for (i=0; i<errord; i++)
            {
                fprintf(outfile, " Pattern %16.16llX Was %16.16llX Word %d\n", sbdata[i], isdata[i], errword[i]);
            }
            sprintf(errormsg, "      ERRORS - See %s logfile ", logfile);
        }


        fflush(outfile);
    }
    printf("%s\n", errormsg);
    fprintf(outfile, " Max %6.0f 64 bit MIPS\n", maxMIPS1);
    fprintf(outfile, " Read\n");
    printf(" Read\n");
    fflush(outfile);

    sprintf(errormsg, " ");
    for (j=0; j<6; j++)
    {

        printPattern = pattern1[j];
        usePattern1 = pattern1[j];   
        usePattern2 = pattern2[j];   
        
        millionBytes = (double) useMemK * 1.024 / 1000;


        xx[0]   = usePattern1;
        xx[1]   = usePattern1;
        xx[2]   = usePattern1;
        xx[3]   = usePattern1;
        xx[4]   = usePattern2;
        xx[5]   = usePattern2;
        xx[6]   = usePattern2;
        xx[7]   = usePattern2;

        mempasses = useMemK * 8;
        _writeData();
        checkData();

        if (errors > 0)
        {
            fprintf(outfile, "    ERROR found checking data after generation - Test %d\n", j+1);
            fprintf(outfile, " %dK 4 byte words checked and %d errors found\n", useMemK/4, errors);
            if (errors < 12)
            {
                fprintf(outfile, " %d errors\n", errord);
            }
            else
            {
                fprintf(outfile, " First 12 errors\n");
            } 
            for (i=0; i<errord; i++)
            {
                fprintf(outfile, " Pattern %16.16llX Was %16.16llX Word %d\n", sbdata[i], isdata[i], errword[i]);
            }
            sprintf(errormsg, "      ERRORS - See %s logfile ", logfile);
            fflush(outfile);
        }

        results[j][0] = 0;

        passbytes = 128;
        mempasses = useMemK * 1024 / passbytes;

        outerLoop = 1;
        start_time();
        _readData();
        end_time();

        outerLoop = 100;
        if (secs > 0.001) outerLoop = (long long)(0.1/secs);
        if (outerLoop < 1) outerLoop = 1;
        n1Count = 0;
        timeCount = 0;
        for (p=0; p<testPasses; p++)
        {
            n1 = 0;
            start_time();
            do
            {
                _readData();
                n1 = n1 + 1;
                end_time();
            }
            while (secs < (double)RrunSecs);
            n1Count = n1Count + (double)n1;
            timeCount = timeCount + secs;
            printf(" L%2dP %3d Test%2d of 6, Pattern %16.16llX, %4.1f Secs,  %6.0f MB/S\n",
                       testlog, p+1, j+1, printPattern, secs, (millionBytes * (double)outerLoop * (double)n1 / secs));
        }
        results[j][0] = millionBytes * (double)outerLoop * n1Count / timeCount;
        maxMIPS0 = n1Count * 19 * (double)mempasses * (double)outerLoop / 1000000 / timeCount;
        if (maxMIPS0 > maxMIPS2) maxMIPS2 = maxMIPS0;
        sprintf(patternSB, "%16.16llX", usePattern1);
        sprintf(patternIs, "%16.16llX",   calcResult);
        sprintf(testmsg[j], " Pattern %16.16llX Result OK %10.0f passes", printPattern, n1Count * (double)outerLoop);
        if (strcmp(patternSB, patternIs) != 0)
        {
            sprintf(testmsg[j], " Pattern %s %c ERROR Was %s", patternSB, 9, patternIs);
            errorp = 1;
        }
        fprintf(outfile, " %2d %7.0f MB/sec %s\n", j+1, results[j][0], testmsg[j]);
        fflush(outfile);        
        checkData();
        if (errors == 0)
        {
            if (errorp > 0)
            {
                fprintf(outfile, " %dK 4 byte words checked but no errors found\n", useMemK/4);
            }
        }
        else
        {
            if (errorp == 0)
            {
                fprintf(outfile, " No Errors in Reliability/Speed Test FAILED ON END COMPARE\n");
            }
            fprintf(outfile, " %dK 4 byte words checked and %d errors found\n", useMemK/4, errors);
            if (errors < 12)
            {
                fprintf(outfile, " %d errors\n", errord);
            }
            else
            {
                fprintf(outfile, " First 12 errors\n");
            } 
            for (i=0; i<errord; i++)
            {
                fprintf(outfile, " Pattern %16.16llX Was %16.16llX Word %d\n", sbdata[i], isdata[i], errword[i]);
            }
        }
        fflush(outfile);
        if (errors > 0)
        {
            sprintf(errormsg, "      ERRORS - See %s logfile ", logfile);
        }
    }
    fprintf(outfile, " Max %6.0f 64 bit MIPS\n", maxMIPS2);
    fprintf(outfile, "\n");
    printf("%s\n\n", errormsg);
    local_time();
    fprintf(outfile, "             Reliability Test Ended %s", timeday);
    fprintf(outfile, "\n");
    fflush(outfile);
    fclose(outfile);
//    printf("\n Press Enter\n");
    _mm_free(xx);
//    g  = getchar();
}

void checkData()
{
    int i;
    unsigned int m;

    sumCheck[0] = usePattern1;
    sumCheck[1] = usePattern2;

    mempasses = useMemK * 8; // 1024 / 128;            
    _errorcheck();
    
    errors = 0;
    errord = 0;

    if (sumCheck[0] != usePattern1 ||
        sumCheck[1] != usePattern1 || 
        sumCheck[2] != usePattern2 ||
        sumCheck[3] != usePattern2 ||  errorp == 1)
    {
        for (m=0; m<useMemK*128; m=m+8)
        {
           if (sumCheck[0] != usePattern1 ||
               sumCheck[1] != usePattern1 || errorp == 1)
           {    
               for (i=i0Start; i<4; i++)
               {
                  if (xx[m+i] != usePattern1)
                  {
                      errors = errors + 1;
                      if (errors < 13)
                      {
                          isdata[errord] = xx[m+i];
                          sbdata[errord] = usePattern1;
                          errword[errord] = m+i;
                          errord = errord + 1;
                      }
                  }
               }
           }
           i0Start = 0;
           if (sumCheck[3] != usePattern2 ||
               sumCheck[4] != usePattern2 || errorp == 1) 
           {
               for (i=0; i<4; i++)
               {
                  if (xx[m+i+4] != usePattern2)
                  {
                      errors = errors + 1;
                      if (errors < 13)
                      {
                          isdata[errord] = xx[m+i+4];
                          sbdata[errord] = usePattern2;
                          errword[errord] = m+i+4;
                          errord = errord + 1;
                      }
                  }
               }
           }
        }
    }    
}



















