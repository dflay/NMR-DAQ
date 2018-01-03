// call from python with single argument which is filename of data to be plotted
// Code reads corresponding binary file and pipes directly to gnuplot
//
// for example >./b.exe test1.bin will plot FID data in test1.dat

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NMAX   10000000                 // max number of FID array elements

int main(int argc, char *argv[]){

  FILE   *fp0,*fp1;
  // FILE   *fp2;
  long   i,j,ii,il,ih;
  long   nrange,N,m0,m1,nzc;
  double tt,fid;
  double nu_samp,dt;
  double dum0,dum1,dum2,dum3,f0,f1,*intercept,*slope,*intercepte,*slopee;
  double vave,tmax;


  double *v  = malloc(sizeof(double)*NMAX);                       // voltage of nmr fid
  double *t  = malloc(sizeof(double)*NMAX);                       // time of nmr fid
  double *ve = malloc(sizeof(double)*NMAX);                       // noise estimate of nmr fid
  const int SIZE = 100000; 
  double tzc0[SIZE],tzc1[SIZE],phizc0[SIZE];
  // double phizc1[SIZE];     // time of zero crossings

  void   linefit2(double *, double *, double *, long, double *, double *, double *, double *);

  nu_samp = 10.0e6;                  // sample at 10 MHz
  dt      = 1.0/nu_samp;                  // sampling interval
 
  tmax = 0.007;
  // tmax = 0.200;
  dum0 = 0.0;   dum1 = 0.0;   dum2 = 0.0;   dum2 = 0.0;
  intercept = &dum0; slope = &dum1;  intercepte = &dum2; slopee = &dum3; 
    
  fp1 = popen("gnuplot -persist","w");             // pipe output directly to gnuplot
  fprintf(fp1,"set multiplot layout 2,2\n");        

  for (ii=1; ii<argc; ii++){ 
    i=0; j=0; vave=0.0;
    fp0=fopen(argv[ii],"r");                       // binary file with FID
    while( ((il=fgetc(fp0))!=EOF) && (i<NMAX) ){   // get low order byte, stop at 10 msec 
      ih=fgetc(fp0);                               // get high order byte 
      fid = ((ih*256+il)-32768.0)*2.5/32768.0;     // rough conversion of FID to volts
      t[i]  = i*dt;
      v[i]  = fid;
      ve[i] = 2.0e-3;
      if (t[i]<tmax){ 
        vave = vave + fid; 
        j++;
      }
      i++;
    }
    fclose(fp0);
    N = i;                                         // number of samples
    vave = vave/j;                                 // get estimate of offset

    nrange = 20;
    i = (long)(nrange/2); j = 0;
    if (i<10) i=10;                                // first few samples sometimes off
    while ( (i<(N-nrange)) && (t[i]<tmax) ){       // zero crossings in first 5 ms
      if (((v[i-1]-vave)*(v[i]-vave))<=0){         // zero crossing 
        tzc0[j]=(t[i-1]+t[i])/2.0;
        m0 =  i-(long)(nrange/2); 
        m1 = m1+(long)(nrange/2); 
        linefit2(&t[m0], &v[m0], &ve[m0], nrange, intercept, slope, intercepte, slopee); 
        tzc1[j]  = *intercept;
        phizc0[j] = j/2.0; 
        //     fprintf(fp,"%ld %11.8lf %11.8lf %lf\n",j,tzc0[j]*1000.0,tzc1[j]*1000.0,0.0);
        //     printf("%ld %11.8lf %11.8lf 0.0\n",j,tzc0[j]*1000.0,tzc1[j]*1000.0);
        j++;
        i=i+nrange/2;
      }
      i++;
    }
    nzc = j;
    f0 = -9999.9; f1 = -9999.9;
    if ((nzc>10) && (nzc<tmax*1.0e5*2.0)){             // enough zero crossings 
      f0 = (0.5*(nzc-1.0-2.0))/(tzc0[nzc-1]-tzc0[2]);
      linefit2(&tzc1[1], &phizc0[1], &ve[1], nzc-2, intercept, slope, intercepte, slopee); 
      f1 = *slope;
    }
    if (f1>0.0){                                     // make a better estimate
      nrange=(1.0/f1)*(0.1)*nu_samp;                 // skip over 10% of cycle
      i = (long)(nrange/2); j = 0;
      if (i<10) i=10;                                // first few samples sometimes off
      while ( (i<(N-nrange)) && (t[i]<tmax) ){       // zero crossings in first 5 ms
        if (((v[i-1]-vave)*(v[i]-vave))<=0){         // zero crossing 
          tzc0[j]=(t[i-1]+t[i])/2.0;
          m0 =  i-(long)(nrange/2); 
          m1 = m1+(long)(nrange/2); 
          linefit2(&t[m0], &v[m0], &ve[m0], nrange, intercept, slope, intercepte, slopee); 
          tzc1[j]  = *intercept;
          phizc0[j] = j/2.0; 
          //      fprintf(fp,"%ld %11.8lf %11.8lf %lf\n",j,tzc0[j]*1000.0,tzc1[j]*1000.0,0.0);  
  	  //      printf("%ld %11.8lf %11.8lf 0.0\n",j,tzc0[j]*1000.0,tzc1[j]*1000.0);
          j++;
          i=i+nrange/2;
        }
        i++;
      }
      nzc = j;
      f0 = -9999.9; f1 = -9999.9;
      if (nzc>10){                                    // enough zero crossings 
        f0 = (0.5*(nzc-1.0-2.0))/(tzc0[nzc-1]-tzc0[2]);
        linefit2(&tzc1[1], &phizc0[1], &ve[1], nzc-2, intercept, slope, intercepte, slopee); 
        f1 = *slope;
        if (fabs(f0-f1)>20){ f0 = -9999.9; f1 = -9999.9;} // disagreement, poor result
      }
    }

    //    printf("Pulse %s frequency estimates: %10.2lf %10.2lf\n",argv[ii],f0,f1);

    fprintf(fp1,"set xrange [0.0:10.0]\n");
    fprintf(fp1,"set yrange [-2.5:2.5]\n");
    fprintf(fp1,"set title \" %s %9.1lf \"\n",argv[ii],f1);
    fprintf(fp1,"set xlabel \"Time (ms)\"\n");
    fprintf(fp1,"set ylabel \"FID (V)\"\n");
    fprintf(fp1,"set grid\n");
    //    fprintf(fp1,"set label \" %s Freq = %9.1lf\" at screen 0.5, screen 0.8\n",argv[ii],f1);
    fprintf(fp1,"plot \"-\" with lines\n");

    i=0;
    fp0=fopen(argv[ii],"r");                         // binary file with FID   
    while( ((il=fgetc(fp0))!=EOF) && (i<NMAX) ){     // get low order byte, stop at 10 msec 
      ih=fgetc(fp0);                                 // get high order byte 
      fid = ((ih*256+il)-32768.0)*2.5/32768.0;       // rough conversion of FID to volts
      tt  = i*1.0e-7*1.0e3;                          // get time in milliseconds
      if ((i>10) && ((i%25)==0)) fprintf(fp1,"%lf %lf\n",tt,fid);
      i++;
    }
    fprintf(fp1,"e\n");
    fclose(fp0);
  }
  fflush(fp1);
  pclose(fp1);

  return 0;
}
////////////////////////////////////////////////////////////////////

void linefit2(double *x, double *y, double *ye, long N, double *intercept, double *slope, double *intercepte, 
              double *slopee){

  long   i;
  double s1,sx,sy,sxy,sxx;
  double denom,det;

  double a,b,ae,be;
  // double s,r;

  sxx = 0.0;
  sxy = 0.0;
  sy = 0.0;
  sx = 0.0;
  s1 = 0.0;

  // find matrix elements
  for (i=0; i<N; i++){
    denom = N*pow(ye[i],2.0);
    s1 = s1 +         1.0/denom;
    sx = sx +        x[i]/denom;
    sy = sy +        y[i]/denom;
    sxy = sxy + x[i]*y[i]/denom;
    sxx = sxx + x[i]*x[i]/denom;
  }

  // y = a + bx
  b = (s1*sxy-sx*sy)/(s1*sxx-sx*sx);
  a = (sy-b*sx)/s1;
  det = sxx*s1-sx*sx;
  ae = sqrt(sxx/(N*det));
  be = sqrt(s1 /(N*det));


  *slope       = b;
  *intercept   = -a/b;
  *slopee      = be;
  *intercepte  = ae;

  //  printf("Fit2 a=%lf +/- %lf   b=%lf +/- %lf\n",a,ae,b,be);

  return;
}

////////////////////////////////////////////////////////////////////
