/*
gcc burninsse.c cpuidc.o cpuida.o -lrt -lc -lm -msse -O3 -o burninsse32
./burninsse32

gcc burninsse.c cpuidc64.o cpuida64.o -m64 -lrt -lc -lm -O3 -o burninsse64
./burninsse64


Affinity Setting
taskset 0x00000001 ./burninsse32
*/

 #include <stdio.h>
 #include <stdlib.h>
 #include "cpuidh.h"
 #include <malloc.h>
 #include <mm_malloc.h>

 char   compiler[80] = "                Via Ubuntu 64 Bit Compiler";
 char   heading[40]  = "64 Bit MFLOPS Burn-In Test 1.0";
// char   compiler[80] = "                Via Ubuntu 32 Bit Compiler";
// char   heading[40]  = "32 Bit MFLOPS Burn-In Test 1.0";

 FILE    *outfile;
 int     endit;
 int     section = 3;
 int     opwd;

 float   *x_cpu;                  // Pointer to CPU arrays
 size_t  size_x;
 int     words     = 100000;      // E Number of words in arrays
 int     repeats   = 2500;        // R Number of repeat passes
 int     repeat2   = 0;
 int     testlog       = 0;
 char    testcpu[15] = "";
 char    logfile[20] = "";
 int     passes = 4;              // repeat all passes * loopsecs
 int     loopsecs = 15;
 float   xval = 0.999950f;
 float   aval = 0.000020f;
 float   bval = 0.999980f;
 float   cval = 0.000011f;
 float   dval = 1.000011f;
 float   eval = 0.000012f;
 float   fval = 0.999992f;
 float   gval = 0.000013f;
 float   hval = 1.000013f;
 float   jval = 0.000014f;
 float   kval = 0.999994f;
 float   lval = 0.000015f;
 float   mval = 1.000015f;
 float   oval = 0.000016f;
 float   pval = 0.999996f;
 float   qval = 0.000017f;
 float   rval = 1.000017f;
 float   sval = 0.000018f;
 float   tval = 1.000018f;
 float   uval = 0.000019f;
 float   vval = 1.000019f;
 float   wval = 0.000021f;
 float   yval = 1.000021f;

 void triadplus2(int n, float a, float b, float c, float d, float e, float f, float g, float h, float j, float k, float l, float m, float o, float p, float q, float r, float s, float t, float u, float v, float w, float y, float *x)
 {
     int i;

     for(i=0; i<n; i++)
     x[i] = (x[i]+a)*b-(x[i]+c)*d+(x[i]+e)*f-(x[i]+g)*h+(x[i]+j)*k-(x[i]+l)*m+(x[i]+o)*p-(x[i]+q)*r+(x[i]+s)*t-(x[i]+u)*v+(x[i]+w)*y;
 } 

 void triadplus(int n, float a, float b, float c, float d, float e, float f, float *x)
 {
     int i;

     for(i=0; i<n; i++)
     x[i] = (x[i]+a)*b-(x[i]+c)*d+(x[i]+e)*f;
 }

 void triad(int n, float a, float b, float *x)
 {
     int i;

     for(i=0; i<n; i++)
     x[i] = (x[i]+a)*b;
 }

 void runTests()
 {
    int  i;
    
    for (i=0; i<repeats; i++)
    {
       // calculations in CPU
       if (section == 1)
       {
          triad(words, aval, xval, x_cpu);
       }
       if (section == 2)
       {
          triadplus(words, aval, bval, cval, dval, eval, fval, x_cpu);
       }
       if (section == 3)
       {
          triadplus2(words, aval, bval, cval, dval, eval, fval, gval, hval, jval, kval, lval, mval, oval, pval, qval, rval, sval, tval, uval, vval, wval, yval,  x_cpu);
       }   
 
    }
 }


 int main(int argc, char *argv[])
 {
         
    int     i, g, p, r;
    int     param1;
    float   fpmops;
    float   mflops;
    int     isok1 = 0;
    int     isok2 = 0;
    int     count1 = 0;
    float   errors[2][10];
    int     erdata[5][10];
    float   newdata = 0.999999f;
    float   one;
       
    for (i=1; i<9; i=i+2)
    {
       if (argc > i)
       {
          switch (toupper(argv[i][0]))
          {
               case 'K':
                param1 = 0;
                if (argc > i+1)
                {
                   sscanf(argv[i+1],"%d", &param1);
                   if (param1 > 0) words = param1 * 1000;
                }
                break;

                case 'S':
                param1 = 0;
                if (argc > i+1)
                {
                   sscanf(argv[i+1],"%d", &param1);
                   if (param1 > 0) section = param1;
                   if (section > 3) section = 3;
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

                case 'M':
                param1 = 0;
                if (argc > i+1)
                {
                   sscanf(argv[i+1],"%d", &param1);
                   if (param1 > 0) passes = param1;
                   passes = passes * 4;
                }
                break;
         }
       }
    }
    
    sprintf (testcpu, "test%d", testlog);
    sprintf (logfile, "log%d.txt", testlog);
    local_time();
    outfile = fopen(logfile,"a+");
    if (outfile == NULL)
    {
        printf (" Cannot open results file \n\n");
        printf(" Press Enter\n");
        g  = getchar();
        exit (0);
    }
    if (section == 1)
    {
       opwd = 2;
    }
    if (section == 2)
    {
       opwd = 8;
    }
    if (section == 3)
    {
       opwd = 32;
    }   

    getDetails();       
    printf("  %s %s\n", heading, timeday);
    if (testlog < 2)
    {
        for (i=0; i<10; i++)
        {
            printf("%s\n", configdata[i]);
        }
    }
    printf("\n");
    fprintf (outfile, "\n"); 
    fprintf (outfile, "##############################################\n\n");
    for (i=1; i<10; i++)
    {
        fprintf(outfile, "%s \n", configdata[i]);
    }
    fprintf (outfile, "\n");
    fprintf (outfile, "##############################################\n\n");
    fprintf(outfile, "  %s %s\n", heading, timeday);
    fprintf(outfile, "  %s\n\n", compiler);
    fprintf(outfile, " Using %d KBytes, %d Operations Per Word, For Approximately %d Minutes\n", 4*words/1000, opwd, passes*loopsecs/60);
    printf("  Reporting approximately every %d seconds, repeated %d times\n", loopsecs, passes);
    fprintf(outfile, "\n   Pass    4 Byte  Ops/   Repeat    Seconds   MFLOPS          First   All\n");
    fprintf(outfile,   "            Words  Word   Passes                            Results  Same\n\n");
    printf( "\n  System   4 Byte  Ops/   Repeat    Seconds   MFLOPS          First   All\n");
    printf(  "    Pass    Words  Word   Passes                            Results  Same\n\n");
    fflush(outfile);

    isok1  = 0;
    
    size_x = words * sizeof(float);
    
    for (p=0; p<passes; p++)
    {
        // Allocate arrays for host CPU
        x_cpu = (float *)_mm_malloc(size_x, 16);
        if (x_cpu  == NULL)
        {
            printf(" ERROR WILL EXIT\n");
            printf(" Press Enter\n");
            g  = getchar();
            exit(1);
        }
    
        // Data for array
        for (i=0; i<words; i++)
        {
          x_cpu[i] = newdata;
        }
        if (p == 0)
        {
            start_time();
            do
            {
                runTests();
                end_time();
                repeat2 = repeat2 + 1;
            }
            while (secs < (float)loopsecs);
            one = x_cpu[0];
            if (one == newdata)
            {
                   isok2 = 1;
                   isok1 = 1;
            }
        }
        else
        {
            start_time();
            for (r=0; r<repeat2; r++)
            {
                runTests();
            }                
            end_time();
        }

        fpmops = (float)words * (float)opwd;
        mflops = (float)repeats * repeat2 * fpmops / 1000000.0f / (float)secs;
        
        // Print results
        fprintf(outfile, "%7d %9d %5d %8d %10.2f %8.0f ", p+1, words, opwd, repeats * repeat2, secs, mflops);
        printf("%3d%5d%9d %5d %8d %10.2f %8.0f ", testlog, p+1,words, opwd, repeats * repeat2, secs, mflops);
        isok1  = 0;
        for (i=0; i<words; i++)
        {
          if (one != x_cpu[i])
          {
             isok1 = 1;
             if (count1 < 10)
             {
                errors[0][count1] = x_cpu[i];
                errors[1][count1] = one;
                erdata[0][count1] = i;
                erdata[1][count1] = words;                          
                erdata[2][count1] = opwd;
                erdata[3][count1] = repeats * repeat2;
                count1 = count1 + 1;
             }
          }
        }
        if (isok1 == 0)
        {
          fprintf(outfile, " %13.9f   Yes\n", x_cpu[0]);
          printf(" %13.9f   Yes\n", x_cpu[0]);
        }
        else
        {
          fprintf(outfile, "   See later   No\n");
          printf("   See log     No\n");
        }
        fflush(outfile);

    // Cleanup
    _mm_free(x_cpu);

    }
    local_time();
    fprintf(outfile, "\n                   End at %s\n", timeday);
    printf("\n                   End at %s\n", timeday);    
    fprintf(outfile, "\n");
    printf("\n");
    


    fprintf(outfile,"\n");

    if (isok2 > 0)
    {
       fprintf(outfile," ERROR - At least one first result of 0.999999 - no calculations?\n\n");
       printf(" ERROR - At least one first result of 0.999999 - no calculations?\n\n");
    }
    if (count1 > 0)
    {
       fprintf(outfile," First Unexpected Results\n");
       for (i=0; i<count1; i++)
       {
         fprintf(outfile,"%7s %9d %5d %8d word %9d was %10.6f not %10.6f\n",
           testcpu, erdata[1][i], erdata[2][i], erdata[3][i], erdata[0][i], errors[0][i], errors[1][i]);
       }
       fprintf(outfile,"\n");
    }

    fclose (outfile);
//    printf(" Press Enter\n");
//    g  = getchar();
    return 0;
 }


