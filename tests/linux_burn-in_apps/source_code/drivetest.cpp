/* 
   g++ drivetest.cpp cpuidc64.o cpuida64.o -m64 -lrt -lc -lm -O3 -o drivestress64
 ./drivestress64

 g++ drivetest.cpp cpuidc.o cpuida.o -lrt -lc -lm -O3 -o drivestress32
 ./drivestress32

*/

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include "cpuidh.h"
 #include <dirent.h>
 #include <ctype.h>
 #include <unistd.h>
 #include <sys/statvfs.h>
 #include <math.h>
 #include <fcntl.h>
 #include <malloc.h>
 #include <mm_malloc.h>

 #define TRUE  1
 #define FALSE 0


// ************* GLOBAL VARIABLES **********
 
char   version[30] = "64-Bit Version 1.1";
// char   version[30] = "32-Bit Version 1.1";

FILE    *outfile;

int     handle;
int     patterns = 164;
int     sequences[11][400];
int     files = 4;
int     baseData[400];
int     repeats = 1;
int     blockSize = 16384; // 65536 bytes
int     dataSize = 65536;
int     endFile;
int     *dataInOut;
int     testMins = 2;
int     passCount;
int     passCount2;
int     errors;
int     repeatTestSecs = 1;
int     readBlocks = 20;
int     testlog    = 0;
int     fileCache = FALSE;

char    filePath[PATH_MAX] = "";
char    fileNames[4][PATH_MAX];
char    logfile[20] = "";

double  Mbytes;
double  totalMins;
double  countMins;


int writeFiles(int dsize);
int readFiles(int dsize);
int checkData(int p, int frc, int rep);
int readFromBuffer(int dsize);


int main(int argc, char *argv[])
{

    int      f, i, j, k, m, g;
    int      param1;
    double   totalMB;
    double   freeMB;
    int      jfa, jfi, mfi;
    int      mad[400];
    int      jf[11];
    int      outputPatterns = FALSE;
    int      nb, nc, nh;  
    int      im, in, iw, ii, ib, nk; 

    for (i=1; i<15; i=i+2)
    {
       if (argc > i)
       {
          switch (toupper(argv[i][0]))
          {
                case 'R':
                param1 = 0;
                if (argc > i+1)
                {
                   sscanf(argv[i+1],"%d", &param1);
                   if (param1 > 0)
                   {
                      repeats = param1;
                   }
                }
                break;

                case 'M':
                param1 = 0;
                if (argc > i+1)
                {
                   sscanf(argv[i+1],"%d", &param1);
                   if (param1 > 0)
                   {
                      testMins = param1;
                   }
                }
                break;

                case 'L':
                param1 = 0;
                if (argc > i+1)
                {
                   sscanf(argv[i+1],"%d", &param1);
                   if (param1 > 0) testlog = param1;
                   if (testlog > 99) testlog = 0;
                }
                break;

                case 'S':
                param1 = 0;
                if (argc > i+1)
                {
                   sscanf(argv[i+1],"%d", &param1);
                   if (param1 > 0)
                   {
                      repeatTestSecs = param1;
                   }
                }
                break;

                case 'F':
                if (argc > i+1)
                {
                   sscanf(argv[i+1],"%s", filePath);
                   strcat(filePath, "/");
                }
                break;

                case 'C': // use Linux File Cache
                 fileCache = TRUE;
                 i = i - 1;
                break;            

                case 'O': // Log patterns
                 outputPatterns = TRUE;
                 i = i - 1;
                 break;            

          }
       }
    }
    Mbytes = (double)(64 * patterns * repeats) / 1024;  

    sprintf (logfile, "IOStress%d.txt", testlog);
    outfile = fopen(logfile,"a+");
    if (outfile == NULL)
    {
        printf (" Cannot open results file \n\n");
        printf(" Press Enter\n");
        g  = getchar();
        exit (0);
    }
    printf("\n");

    getDetails();

    for (i=1; i<10; i++)
    {
        printf("%s\n", configdata[i]);
    }
    printf("\n");

    dataInOut = (int*)_mm_malloc(dataSize, 512);
    if (dataInOut == NULL)
    {
        printf(" Memory allocation failed - Exit\n");
        fprintf(outfile, " Memory allocation failed - Exit\n");
        fclose(outfile);
        printf(" Press Enter\n");
        g  = getchar();
        exit (0);
    }        

    

    fprintf (outfile, " ###############################################################\n\n");                     
    for (i=1; i<10; i++)
    {
        fprintf(outfile, "%s \n", configdata[i]);
    }
    fprintf (outfile, "\n");
 
    printf(" ##########################################\n"); 
    fprintf (outfile, " ###############################################################\n");                     

    printf("\n");
    fprintf (outfile, "\n");


    char thispath[PATH_MAX];

    if (strcmp(filePath, "") != 0)
    { 
        printf (" Selected File Path: \n %s\n", filePath);
        fprintf (outfile, " Selected File Path: \n %s\n", filePath);
        strcpy(thispath, filePath);
    }
    else
    {
        if (getcwd(thispath, sizeof thispath) == NULL)
        {
            printf(" Current Directory Path not available\n");
            fprintf(outfile, " Current Directory Path not available\n");
        }
        else
        {
            printf (" Current Directory Path: \n %s\n", thispath);
            fprintf (outfile, " Current Directory Path: \n %s\n", thispath);
        }
    }



    struct statvfs driveSpace;
    if (statvfs(thispath, &driveSpace))
    {
        printf(" Free Disk Space not available - Exit\n");
        fprintf(outfile, " Free Disk Space not available - Exit\n");
        fclose(outfile);
        _mm_free(dataInOut);
        printf(" Press Enter\n");
        g  = getchar();
        exit (0);
    }
    else
    {  
      totalMB = (double)driveSpace.f_frsize * (double)driveSpace.f_blocks / 1048576.0;
      freeMB  = (double)driveSpace.f_frsize * (double)driveSpace.f_bfree  / 1048576.0;
      printf (" Total MB %7.0f, Free MB %7.0f, Used MB %7.0f\n", totalMB, freeMB, totalMB - freeMB);
      fprintf (outfile, " Total MB %7.0f, Free MB %7.0f, Used MB %7.0f\n", totalMB, freeMB, totalMB - freeMB);
    }

    if (freeMB - (Mbytes * 4) < 10)
    {
        printf (" Exit - Not Enough Free Space\n\n");
        fprintf (outfile, " Exit - Not Enough Free Space\n\n");
        fclose(outfile);
        _mm_free(dataInOut);
        printf(" Press Enter\n");
        g  = getchar();
        exit (0);
    }

    local_time();
    errors = 0;

    printf("\n Linux Storage Stress Test %s, %s\n", version, timeday);      
    printf("                Copyright (C) Roy Longbottom 2011\n\n");    
    printf(" File size%8.2f MB x 4 files, minimum reading time %d minutes\n\n", Mbytes, testMins);
    fprintf(outfile, "\n Linux Storage Stress Test %s, %s\n", version, timeday);      
    fprintf(outfile, "                Copyright (C) Roy Longbottom 2011\n\n");    
    fprintf(outfile, " File size%8.2f MB x 4 files, minimum reading time %d minutes\n\n", Mbytes, testMins);

    fflush(outfile);

    //************        Pattern Generation      ***************

     nb = 32;
     nh = nb / 2; 
     ii = nb;
     nc = nb + 2;

     baseData[0] = 0;
     
     for (i=1; i<nb; i++)
     {
         baseData[i] = (int)pow(2.0, (double)(i-1));         
     }
     for (m=1; m<nh+1; m++ )
     {
         in = m - 1;
         im = 2 * m;
         iw = 0;
         nk = nc - im;
         for (i=2; i<nk+1; i=i+im)
         {
             ib = i + in;
             for (j=i; j<ib+1; j++)
             {
                 iw = iw + baseData[j-1];
             }
             ii = ii + 1;
             baseData[ii - 1] = iw;
         }
     }
     for (i=0; i<ii; i++)
     {
         baseData[ii+i] = -(baseData[i]+1);
     }

     //************   Reading Sequence Generation   ***************

     for ( i=1; i<files+1; i++)
     {
          jf[i] = i;         
     }
     for (j=1; j<patterns+1; j++)
     {
         for (k=1; k<files+1; k++)
         {
             mad[k] = k-1;
         }
         for (i=1; i<files+1; i++)

         {
             jfa = jf[i];              
             sequences[i][j] = mad[jfa];
             if (i < jfa || i > jfa)
             {
                 jfi = i + 1;
                 for (m=jfi; m<jfa+1; m++)
                 {
                     mfi = jfa - m + jfi;
                     mad[mfi] = mad[mfi-1];
                 }
             }
             if (files > jfa)
             {
                 jf[i] = jfa + 1;
             }
             else
             {
                 jf[i] = i;
             }  
          }
     }
     if (outputPatterns)
     {

        fprintf (outfile, " Patterns Generated\n\n Number         Pattern        Or"
                                        " Number         Pattern        Or\n\n");
        for (i=0; i<patterns; i=i+2)
        {
             j = i + 1;
             fprintf(outfile,"%7d%16d%10X%7d%16d%10X\n", i+1, baseData[i], baseData[i], j+1, baseData[j], baseData[j]); 
        }
        fprintf(outfile,"\n\n");
    
        fprintf(outfile," Sequences\n\n");
    
        for (j=1; j<patterns+1; j++)
        {
            fprintf (outfile, "%5d      ", j);     
            for (m=1; m<files+1; m++)
            {
                 fprintf (outfile, "%3d", sequences[m][j]);
            }
            fprintf (outfile, "\n");
        }
        fprintf(outfile,"\n\n");
    
        fflush(outfile);
    }

   // ################ Writing ################

    for (f=0; f<files; f++)
    {
        sprintf(fileNames[f], "%sztestz%d%d", filePath, testlog, f);
    }


    if (!writeFiles(dataSize))
    {

        for (f=0; f<endFile; f++)
        {
            remove(fileNames[f]);
        }       
        fclose(outfile);
        _mm_free(dataInOut);
        printf("\n Press Enter\n");
        g  = getchar();
        exit (0);
    }

   // ################ Reading ################

    local_time();
    if (fileCache)
    {
         printf("\n          Start Cached Reading %s\n", timeday);
         fprintf(outfile, "\n          Start Cached Reading %s\n", timeday);
    }
    else
    {
         printf("\n              Start Reading %s\n", timeday);
         fprintf(outfile, "\n              Start Reading %s\n", timeday);
    }

    totalMins = 0;
    passCount = 0;
    start_time();
    countMins = 0.25;

    do
    {
        if (!readFiles(dataSize))
        {
            if (errors > 9) totalMins = (double)testMins + 1;
        }
    }
    while (totalMins < (double)testMins);


   // ################ Reading From Buffer ################

    if (errors < 10)
    {
        local_time();
        if (fileCache)
        {
             printf("\n        Start Cached Repeat Read %s\n", timeday);
             fprintf(outfile, "\n        Start Cached Repeat Read %s\n", timeday);
        }
        else
        {
             printf("\n            Start Repeat Read %s\n", timeday);
             fprintf(outfile, "\n            Start Repeat Read %s\n", timeday);
        }
        printf(" Passes in %d second(s) for each of %d blocks of 64KB:\n\n", repeatTestSecs, patterns); 
        fprintf(outfile, " Passes in %d second(s) for each of %d blocks of 64KB:\n\n", repeatTestSecs, patterns); 

        passCount2 = 0;
        totalMins = 0;
        if (!readFromBuffer(dataSize))
        {
            printf("\n Press Enter\n");
            g  = getchar();
        }
        printf("\n %d read passes of 64KB blocks in %8.2f minutes\n", passCount2, totalMins);
        fprintf(outfile, "\n %d read passes of 64KB blocks in %8.2f minutes\n", passCount2, totalMins);

    }
    if (errors == 0)
    {
        printf("\n  No errors found during reading tests\n");
        fprintf(outfile, "\n  No errors found during reading tests\n");
    }
    else
    {
        printf("\n ERRORS found during reading tests, see above and log\n");
        fprintf(outfile, "\n ERRORS found during reading tests, see above\n");
        printf("\n Press Enter TO REMOVE DATA FILES\n or Close Terminal To Keep Files");
        g  = getchar();
    }        
    
    local_time();
    fprintf(outfile, "\n              End of test %s", timeday);        
    fprintf(outfile,"\n");
    printf("\n");
    for (f=0; f<files; f++)
    {
        remove(fileNames[f]);
    }       
    
    _mm_free(dataInOut);

    fflush(outfile);
    fclose(outfile);

    return 1;
    
} // main

int writeFiles(int dsize)
{
    int f, g, r, p, b;
    
    for (f=0; f<files; f++)
    {
        start_time();
        if (fileCache)
        {
             handle = open(fileNames[f], O_WRONLY | O_CREAT | O_TRUNC,
                                  S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP );
        }
        else
        {
             handle = open(fileNames[f], O_WRONLY | O_CREAT | O_TRUNC | O_DIRECT,
                                  S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP );
        }
        if (handle == -1)
        {
            printf (" Cannot open data file for writing\n\n");
            fprintf (outfile, " Cannot open data file for writing\n\n");
            return 0;
        }
        printf(" File %d %7.2f MB ", f+1, Mbytes);
        fflush(stdout);
        fprintf(outfile, " File %d %7.2f MB ", f+1, Mbytes);
        for (r=0; r<repeats; r++)
        {
            for (p=0; p<patterns; p++)
            {
                for (b=0; b<blockSize; b++)
                {
                    dataInOut[b] = baseData[p];
                }
                dataInOut[0] = f;

                if (write(handle, dataInOut, dsize) != dsize )
                {
                    endFile = f + 1;
                    printf (" Error writing file\n\n");
                    fprintf (outfile, " Error writing file\n\n");
                    close(handle);
                    return 0;
                 }

            }
            end_time();
        }
        close(handle);
        printf("written in %7.2f seconds \n", secs);             
        fprintf(outfile, "written in %7.2f seconds \n", secs);             
    }
    printf("\n");
    fprintf(outfile, "\n");
    return 1;
    
} // writeFiles

int readFiles(int dsize)
{
    int hFile[10];

    int f, t, r, p, fr;
    
    for (f=0; f<files; f++)
    {
        if (fileCache)
        {
             hFile[f] = open(fileNames[f], O_RDONLY);
        }
        else
        {
             hFile[f] = open(fileNames[f], O_RDONLY | O_DIRECT);
        }
        if (hFile[f] == -1)
        {
            printf (" Cannot open data file for reading\n\n");
            fprintf (outfile, " Cannot open data file for reading\n\n");
            endFile = f + 1;
            return 0;
        } 
    }
    passCount = passCount + 1;
    for (r=0; r<repeats; r++)
    {
        for (p=0; p<patterns; p++)
        {
            for (f=0; f<files; f++)
            {
                fr = sequences[f+1][p+1];
                if (read(hFile[fr], dataInOut, dsize) == -1)
                {
                    endFile = files;
                    printf (" Error reading file %d\n\n", fr);
                    fprintf (outfile, " Error reading file %d\n\n", fr);
                    fflush(outfile);
                    errors = errors + 1;
                    if (errors > 9)
                    {
                        for (t=0; t<files; t++)
                        {
                            close(hFile[t]);
                        }
                        return 0;
                    }
                }
                if (dataInOut[0] != fr)
                {
                    endFile = files;
                    printf (" Wrong File Read szzztestz%d instead of szzztestz%d \n\n", dataInOut[0], fr);
                    fprintf (outfile, " Wrong File Read szzztestz%d instead of szzztestz%d \n\n", dataInOut[0], fr);
                    fflush(outfile);
                    errors = errors + 1;
                    if (errors > 9)
                    {
                        for (t=0; t<files; t++)
                        {
                            close(hFile[t]);
                        }
                        return 0;
                    }
                }
                if (!checkData(p, fr, 0))
                {
                    endFile = files;
                    fflush(outfile);
                    if (errors > 9)
                    {
                        for (t=0; t<files; t++)
                        {
                            close(hFile[t]);
                        }
                        return 0;
                    }                
                }
            } // for f
            
        } // for p
    } // for r
    end_time();
    totalMins = secs / 60;
    if (totalMins > countMins)
    {
        countMins = countMins + 0.25;
        printf(" Read passes %5d  x %d Files x %7.2f MB in %8.2f minutes\n",
                                passCount, files, Mbytes, totalMins);
        fprintf(outfile, " Read passes %5d x %d Files x %7.2f MB in %8.2f minutes\n",
                                passCount, files, Mbytes, totalMins); 
        fflush(outfile);
    }        
                 

    for (f=0; f<files; f++)
    {
        close(hFile[f]);
    }
    return 1;
    
} // readFiles

int checkData(int p, int frc, int rep)
{
    int   b;
    int   orSum;
    int   andSum;
    
    orSum = baseData[p];
    andSum = baseData[p];
    dataInOut[0] = orSum;
    for (b=0; b<blockSize; b=b+8)
    {
        orSum = orSum  | dataInOut[b  ] | dataInOut[b+1]
                       | dataInOut[b+2] | dataInOut[b+3] 
                       | dataInOut[b+4] | dataInOut[b+5] 
                       | dataInOut[b+6] | dataInOut[b+7];
       andSum = andSum & dataInOut[b  ] & dataInOut[b+1]
                       & dataInOut[b+2] & dataInOut[b+3] 
                       & dataInOut[b+4] & dataInOut[b+5] 
                       & dataInOut[b+6] & dataInOut[b+7];
    }
    if (orSum != baseData[p] || andSum != baseData[p])
    {
        for (b=0; b<blockSize; b++)
        {
            if (dataInOut[b] != baseData[p])
            {
                if (rep == 1)
                {
                     printf("\n");
                     fprintf(outfile, "\n");
                }
                printf(" Pass%4d file szzztestz%d word %2d, data error was %8.8X expected %8.8X\n",
                                      passCount, frc, b, dataInOut[b], baseData[p]); 
                fprintf(outfile, " Pass%4d file szzztestz%d word %2d, data error was %8.8X expected %8.8X\n",
                                      passCount, frc, b, dataInOut[b], baseData[p]); 
                errors = errors + 1;
                if (errors > 9) return 0;
            }
        }
        return 0;
    }
    return 1;

} // checkData


int readFromBuffer(int dsize)
{
    int    r, p;
    int    printCount;
    int    thisPass;

    if (fileCache)
    {
        handle = open(fileNames[1], O_RDONLY);
    }
    else
    {
         handle = open(fileNames[1], O_RDONLY | O_DIRECT);
    }
    if (handle == -1)
    {
        printf (" Cannot open data file for reading\n\n");
        fprintf (outfile, " Cannot open data file for reading\n\n");
        return 0;
    } 

    printCount = 0;
    for (p=0; p<patterns; p++)
    {
        start_time();
        thisPass = 0;
        passCount = 0;
        do
        {
            for (r=0; r<readBlocks; r++)
            {                
                lseek(handle,  p*dsize, SEEK_SET);
                if (read(handle, dataInOut, dsize) == -1)
                {
                    printf (" Error reading file 1\n\n");
                    fprintf (outfile, " Error reading file 1\n\n");
                    fflush(outfile);
                    errors = errors + 1;
                    if (errors > 9)
                    {
                        close(handle);
                        return 0;
                    }
                }
            }
            passCount = passCount + readBlocks;
            passCount2 = passCount2 + readBlocks;
            thisPass = thisPass + readBlocks;    
            if (!checkData(p, 1, 1))
            {
                fflush(outfile);
                if (errors > 9)
                {
                   close(handle);
                   return 0;
                }                
            }
            end_time();
        }
        while(secs < (double)repeatTestSecs);
        printf(" %6d", thisPass);
        fprintf(outfile, " %6d", thisPass);
        fflush(stdout);
        fflush(outfile);
        printCount = printCount + 1;
        if (printCount == 11)
        {
            printf("\n");
            fprintf(outfile, "\n");
            fflush(outfile);
            printCount = 0;
        }        
        totalMins = totalMins + secs / 60;
    }
    printf("\n");
    fprintf(outfile, "\n");
    close(handle);
    return 1;

} // readFromBuffer


