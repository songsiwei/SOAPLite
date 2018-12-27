#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define PI2 9.86960440108936
#define PI 3.14159265359
#define PIHalf 1.57079632679490
//===========================================================
float myExp(float x,float* xxx){
  if(x>10){
    return 0;}
  else if (x > 0){
    float myFloat = 100*x;
    int myInt = (int) myFloat;
    float myFloor = xxx[myInt];
    return 100*(xxx[myInt+1] - myFloor)*(x - 0.01 * (float) myInt);
  }
  else{
    return exp(x);
  }
}
//===========================================================
int getCrosNum(int n){return n*(n+1)/2;}
//===========================================================
float* getReIm2(float* x, float* y, float* c3, int Asize){
  for(int i = 0; i < Asize; i++){
    c3[2*i  ] = x[i]*x[i]-y[i]*y[i];
    c3[2*i+1] = 2*y[i]*x[i];
  }
}
//===========================================================
void getReIm3(float* x, float* y, float* c2, float* c3, int Asize){
  for(int i = 0; i < Asize; i++){
    c3[2*i  ] = x[i]*c2[2*i] - y[i]*c2[2*i + 1];
    c3[2*i+1] = x[i]*c2[2*i+1] + y[i]*c2[2*i  ];
  }
}
//===========================================================
void getMulReIm(float* c1, float* c2, float* c3, int Asize){
  for(int i = 0; i < Asize; i++){
    c3[2*i  ] = c1[2*i  ]*c2[2*i  ] - c1[2*i+1]*c2[2*i+1];
    c3[2*i+1] = c1[2*i  ]*c2[2*i+1] + c1[2*i+1]*c2[2*i  ];
  }
}
//===========================================================
void getMulDouble(float* c1, float* c3, int Asize){
  for(int i = 0; i < Asize; i++){
    c3[2*i  ] = c1[2*i]*c1[2*i] - c1[2*i+1]*c1[2*i+1];
    c3[2*i+1] = 2*c1[2*i]*c1[2*i+1];
  }
}
//================================================================
int getFilteredPos(float* x, float* y, float* z, float* Apos, float* Hpos, int* typeNs, float rCutSqr, int Ihpos, int Itype){

  int shiftType = 0; int count = 0;
  float X = 0; float Y = 0; float Z = 0;

    for(int i = 0; i < Itype ; i++){
      shiftType += typeNs[i];
    }

    for(int i = 0; i < typeNs[Itype]; i++){
      X = Apos[3*shiftType + 3*i    ] - Hpos[3*Ihpos    ];
      Y = Apos[3*shiftType + 3*i + 1] - Hpos[3*Ihpos + 1];
      Z = Apos[3*shiftType + 3*i + 2] - Hpos[3*Ihpos + 2];
      if( X*X + Y*Y + Z*Z < rCutSqr ){
        x[count] = X;
        y[count] = Y;
        z[count] = Z;
        count++;
      }
    }
  return count;
}
//================================================================
float* getRsZs(float* x, float* y, float* z,float* r2,float* r4,float* r6,float* r8,float* z2,float* z4,float* z6,float* z8, int size){
  for(int i = 0; i < size; i++){
    r2[i] = x[i]*x[i] + y[i]*y[i] + z[i]*z[i];
    r4[i] = r2[i]*r2[i]; r6[i] = r2[i]*r4[i]; r8[i] = r4[i]*r4[i];
    z2[i] = z[i]*z[i]; z4[i] = z2[i]*z2[i]; z6[i] = z2[i]*z4[i]; z8[i] = z4[i]*z4[i];
  }
}
//================================================================
void getAlphaBeta(float* aOa, float* bOa, float* alphas, float* betas, int Ns,int lMax, float oOsigma, float oOsigma3O2){

  int  NsNs = Ns*Ns;
  float  oneO1alpha;      float  oneO1alpha2; float  oneO1alpha3;
  float  oneO1alpha4; float  oneO1alpha5; float  oneO1alpha6;
  float  oneO1alpha7; float  oneO1alpha8; float  oneO1alpha9;
  float  oneO1alpha10;
  float  oneO1alphaSqrt;// = (float*) malloc(Ns*sizeof(float));
  float  oneO1alphaSqrtX;

  // MY POEWR MISSING (see beggning functions);

  for(int k = 0; k < Ns; k++){
    oneO1alpha = 1.0/(1.0 + oOsigma*alphas[k]);
    oneO1alphaSqrt = sqrt(oneO1alpha);
    aOa[k] = alphas[k]*oneO1alpha; //got alpha_0k
    oneO1alphaSqrtX = oneO1alphaSqrt*oneO1alpha;
    for(int n = 0; n < Ns; n++){ bOa[n*Ns + k] = oOsigma3O2*betas[n*Ns + k]*oneO1alphaSqrtX;} // got beta_0nk
  }
  if(lMax > 0){
    for(int k = 0; k < Ns; k++){
      oneO1alpha = 1.0/(1.0 + oOsigma*alphas[Ns + k]); oneO1alphaSqrt = sqrt(oneO1alpha);
      aOa[Ns + k] = alphas[Ns + k]*oneO1alpha; //got alpha_1k
      oneO1alpha2 = oneO1alpha*oneO1alpha; oneO1alphaSqrtX = oneO1alphaSqrt*oneO1alpha2;
      for(int n = 0; n < Ns; n++){ bOa[NsNs + n*Ns + k] = oOsigma3O2*betas[NsNs + n*Ns + k]*oneO1alphaSqrtX;} // got beta_1nk
    }
  } if(lMax > 1){
    int shift1 = 2*Ns; int shift2 = 2*NsNs;
    for(int k = 0; k < Ns; k++){
      oneO1alpha = 1.0/(1.0 + oOsigma*alphas[shift1 + k]); oneO1alphaSqrt = sqrt(oneO1alpha);
      aOa[shift1 + k] = alphas[shift1 + k]*oneO1alpha; //got alpha_2k
      oneO1alpha3 = oneO1alpha*oneO1alpha*oneO1alpha; oneO1alphaSqrtX = oneO1alphaSqrt*oneO1alpha3;
      for(int n = 0; n < Ns; n++){bOa[shift2 + n*Ns + k] = oOsigma3O2*betas[shift2 + n*Ns + k]*oneO1alphaSqrtX;} // got beta_2nk
    }
  } if(lMax > 2){
    int shift1 = 3*Ns; int shift2 = 3*NsNs;
    for(int k = 0; k < Ns; k++){
      oneO1alpha = 1.0/(1.0 + oOsigma*alphas[shift1 + k]); oneO1alphaSqrt = sqrt(oneO1alpha);
      aOa[shift1 + k] = alphas[shift1 + k]*oneO1alpha; //got alpha_3k
      oneO1alpha4 = oneO1alpha*oneO1alpha*oneO1alpha*oneO1alpha; oneO1alphaSqrtX = oneO1alphaSqrt*oneO1alpha4;
      for(int n = 0; n < Ns; n++){bOa[shift2 + n*Ns + k] = oOsigma3O2*betas[shift2 + n*Ns + k]*oneO1alphaSqrtX;} // got beta_3nk
    }
  } if(lMax > 3){
    int shift1 = 4*Ns; int shift2 = 4*NsNs;
    for(int k = 0; k < Ns; k++){
      oneO1alpha = 1.0/(1.0 + oOsigma*alphas[shift1 + k]); oneO1alphaSqrt = sqrt(oneO1alpha);
      aOa[shift1 + k] = alphas[shift1 + k]*oneO1alpha; //got alpha_4k
      oneO1alpha5 = pow(oneO1alpha,5); oneO1alphaSqrtX = oneO1alphaSqrt*oneO1alpha5;
      for(int n = 0; n < Ns; n++){ bOa[shift2 + n*Ns + k] = oOsigma3O2*betas[shift2 + n*Ns + k]*oneO1alphaSqrtX;} // got beta_4nk
    }
  } if(lMax > 4){
    int shift1 = 5*Ns; int shift2 = 5*NsNs;
    for(int k = 0; k < Ns; k++){
      oneO1alpha = 1.0/(1.0 + oOsigma*alphas[shift1 + k]); oneO1alphaSqrt = sqrt(oneO1alpha);
      aOa[shift1 + k] = alphas[shift1 + k]*oneO1alpha; //got alpha_5k
      oneO1alpha6 = pow(oneO1alpha,6); oneO1alphaSqrtX = oneO1alphaSqrt*oneO1alpha6;
      for(int n = 0; n < Ns; n++){ bOa[shift2 + n*Ns + k] = oOsigma3O2*betas[shift2 + n*Ns + k]*oneO1alphaSqrtX;} // got beta_5nk
    }
  } if(lMax > 5){
    int shift1 = 6*Ns; int shift2 = 6*NsNs;
    for(int k = 0; k < Ns; k++){
      oneO1alpha = 1.0/(1.0 + oOsigma*alphas[shift1 + k]); oneO1alphaSqrt = sqrt(oneO1alpha);
      aOa[shift1 + k] = alphas[shift1 + k]*oneO1alpha; //got alpha_6k
      oneO1alpha7 = pow(oneO1alpha,7); oneO1alphaSqrtX = oneO1alphaSqrt*oneO1alpha7;
      for(int n = 0; n < Ns; n++){bOa[shift2 + n*Ns + k] = oOsigma3O2*betas[shift2 + n*Ns + k]*oneO1alphaSqrtX;} // got beta_6nk
    }
  } if(lMax > 6){
    int shift1 = 7*Ns; int shift2 = 7*NsNs;
    for(int k = 0; k < Ns; k++){
      oneO1alpha = 1.0/(1.0 + oOsigma*alphas[shift1 + k]); oneO1alphaSqrt = sqrt(oneO1alpha);
      aOa[shift1 + k] = alphas[shift1 + k]*oneO1alpha; //got alpha_7k
      oneO1alpha8 = pow(oneO1alpha,8); oneO1alphaSqrtX = oneO1alphaSqrt*oneO1alpha8;
      for(int n = 0; n < Ns; n++){bOa[shift2 + n*Ns + k] = oOsigma3O2*betas[shift2 + n*Ns + k]*oneO1alphaSqrtX;} // got beta_7nk
    }
  } if(lMax > 7){
    int shift1 = 8*Ns; int shift2 = 8*NsNs;
    for(int k = 0; k < Ns; k++){
      oneO1alpha = 1.0/(1.0 + oOsigma*alphas[shift1 + k]); oneO1alphaSqrt = sqrt(oneO1alpha);
      aOa[shift1 + k] = alphas[shift1 + k]*oneO1alpha; //got alpha_8k
      oneO1alpha9 = pow(oneO1alpha,9); oneO1alphaSqrtX = oneO1alphaSqrt*oneO1alpha9;
      for(int n = 0; n < Ns; n++){bOa[shift2 + n*Ns + k] = oOsigma3O2*betas[shift2 + n*Ns + k]*oneO1alphaSqrtX;} // got beta_8nk
    }
  }if(lMax > 8){
    int shift1 = 9*Ns; int shift2 = 9*NsNs;
    for(int k = 0; k < Ns; k++){
      oneO1alpha = 1.0/(1.0 + oOsigma*alphas[shift1 + k]); oneO1alphaSqrt = sqrt(oneO1alpha);
      aOa[shift1 + k] = alphas[shift1 + k]*oneO1alpha; //got alpha_9k
      oneO1alpha10 = pow(oneO1alpha,10); oneO1alphaSqrtX = oneO1alphaSqrt*oneO1alpha10;
      for(int n = 0; n < Ns; n++){bOa[shift2 + n*Ns + k] = oOsigma3O2*betas[shift2 + n*Ns + k]*oneO1alphaSqrtX;} // got beta_9nk
    }
  }
}
//================================================================
void getCfactors(float* preCoef, int Asize, float* x, float* y, float* z, float* z2, float* z4, float* z6, float* z8, float* r2, float* r4, float* r6, float* r8, float* ReIm2, float* ReIm3, float* ReIm4, float* ReIm5, float* ReIm6, float* ReIm7, float* ReIm8, float* ReIm9,int totalAN, int lMax, int t2, int t3, int t4, int t5, int t6, int t7, int t8, int t9, int t10, int t11, int t12, int t13, int t14, int t15, int t16, int t17, int t18, int t19, int t20, int t21, int t22, int t23, int t24, int t25, int t26, int t27, int t28, int t29, int t30, int t31, int t32, int t33, int t34, int t35, int t36, int t37, int t38, int t39, int t40, int t41, int t42, int t43, int t44, int t45, int t46, int t47, int t48, int t49, int t50, int t51, int t52, int t53, int t54, int t55, int t56, int t57, int t58, int t59, int t60, int t61, int t62, int t63, int t64, int t65, int t66, int t67, int t68, int t69, int t70, int t71, int t72, int t73, int t74, int t75, int t76, int t77, int t78, int t79, int t80, int t81, int t82, int t83, int t84, int t85, int t86, int t87, int t88, int t89, int t90, int t91, int t92, int t93, int t94, int t95, int t96, int t97, int t98, int t99){
  float c20c;float c30c;float c31c;float c40c;float c41c;float c42c;
  float c50c;float c51c;float c52c;float c53c;float c60c;float c61c;
  float c62c;float c63c;float c64c;float c70c;float c71c;float c72c;
  float c73c;float c74c;float c75c;float c80c;float c81c;float c82c;
  float c83c;float c84c;float c85c;float c86c;float c90c;float c91c;
  float c92c;float c93c;float c94c;float c95c;float c96c;float c97c;

    getReIm2(x, y, ReIm2,Asize);
    getReIm3(x, y, ReIm2, ReIm3, Asize);
    getMulDouble(ReIm2, ReIm4, Asize);
    getMulReIm(ReIm2,ReIm3, ReIm5, Asize);
    getMulDouble(ReIm3, ReIm6, Asize);
    getMulReIm(ReIm3,ReIm4, ReIm7, Asize);
    getMulDouble(ReIm4, ReIm8, Asize);
    getMulReIm(ReIm4,ReIm5, ReIm9, Asize);
    int i2;
    //printf("AAA\n");

  for(int i = 0; i < Asize; i++){
    i2 = 2*i;
    c20c=3*z2[i]-r2[i];
    if(lMax > 2){
      c30c=5*z2[i]-3*r2[i];
      c31c=5*z2[i]-r2[i];
    }
    if(lMax > 3){
      c40c=35*z4[i]-30*z2[i]*r2[i]+3*r4[i];
      c41c=7*z2[i]-3*r2[i];
      c42c=7*z2[i]-r2[i];
    }
    if(lMax > 4){
      c50c=63*z4[i]-70*z2[i]*r2[i]+15*r4[i];
      c51c=21*z4[i]-14*z2[i]*r2[i]+r4[i];
      c52c=3*z2[i]-r2[i];
      c53c=9*z2[i]-r2[i];
    }
    //printf("BBB\n");
    if(lMax > 5){
      c60c=231*z6[i] - 315*z4[i]*r2[i] + 105*z2[i]*r4[i] - 5*r6[i];
      c61c=33*z4[i] - 30*z2[i]*r2[i] + 5*r4[i];
      c62c=33*z4[i] - 18*z2[i]*r2[i] + r4[i];
      c63c=11*z2[i] - 3*r2[i];
      c64c=11*z2[i] - r2[i];
    }
    if(lMax > 6){
      c70c=429*z6[i]-693*z4[i]*r2[i]+315*z2[i]*r4[i]-35*r6[i];
      c71c=429*z6[i]-495*z4[i]*r2[i]+135*z2[i]*r4[i]-5*r6[i];
      c72c=143*z4[i]-110*z2[i]*r2[i]+15*r4[i];
      c73c=143*z4[i]-66*z2[i]*r2[i]+3*r4[i];
      c74c=13*z2[i]-3*r2[i];
      c75c=13*z2[i]-r2[i];
    }
    if(lMax > 7){
      c80c=6435*z8[i]-12012*z6[i]*r2[i]+6930*z4[i]*r4[i]-1260*z2[i]*r6[i]+35*r8[i];
      c81c=715*z6[i]-1001*z4[i]*r2[i]+385*z2[i]*r4[i]-35*r6[i];
      c82c=143*z6[i]-143*z4[i]*r2[i]+33*z2[i]*r4[i]-r6[i];
      c83c=39*z4[i]-26*z2[i]*r2[i]+3*r4[i];
      c84c=65*z4[i]-26*z2[i]*r2[i]+r4[i];
      c85c=5*z2[i]-r2[i];
      c86c=15*z2[i]-r2[i];
    }
    if(lMax > 8){
      c90c=12155*z8[i]-25740*z6[i]*r2[i]+18018*z4[i]*r4[i] -4620*z2[i]*r6[i]+315*r8[i];
      c91c=2431*z8[i]-4004*z6[i]*r2[i]+2002*z4[i]*r4[i]-308*z2[i]*r6[i] + 7*r8[i];
      c92c=221*z6[i]-273*z4[i]*r2[i]+91*z2[i]*r4[i]-7*r6[i];
      c93c=221*z6[i]-195*z4[i]*r2[i]+39*z2[i]*r4[i]-r6[i];
      c94c=17*z4[i]-10*z2[i]*r2[i]+r4[i];
      c95c=85*z4[i]-30*z2[i]*r2[i]+r4[i];
      c96c=17*z2[i]-3*r2[i];
      c97c=17*z2[i]-r2[i];
    }
    //printf("CCC\n");
      /*c20  */  preCoef[        +i] = c20c;
      /*c21Re*/  preCoef[totalAN+i] = z[i]*x[i];
    //printf("DCC\n");
      /*c21Im*/  preCoef[t2+i] = z[i]*y[i];
      /*c22Re*/  preCoef[t3+i] =      ReIm2[2*i];
      /*c22Im*/  preCoef[t4+i] =      ReIm2[i2+1];
    if(lMax > 2){
      /*c30  */  preCoef[t5+i] = c30c*z[i];
      /*c31Re*/  preCoef[t6+i] =       x[i]*c31c;
      /*c31Im*/  preCoef[t7+i] =       y[i]*c31c;
      /*c32Re*/  preCoef[t8+i] = z[i]*ReIm2[i2];
      /*c32Im*/  preCoef[t9+i] = z[i]*ReIm2[i2+1];
      /*c33Re*/  preCoef[t10+i] =      ReIm3[i2  ];
      /*c33Im*/  preCoef[t11+i] =      ReIm3[i2+1];
    }
    //printf("DDD\n");
    if(lMax > 3){
      /*c40  */  preCoef[t12+i] = c40c;
      /*c41Re*/  preCoef[t13+i] = z[i]*x[i]*c41c;
      /*c41Im*/  preCoef[t14+i] = z[i]*y[i]*c41c;
      /*c42Re*/  preCoef[t15+i] =      ReIm2[i2  ]*c42c;
      /*c42Im*/  preCoef[t16+i] =      ReIm2[i2+1]*c42c;
      /*c43Re*/  preCoef[t17+i] = z[i]*ReIm3[i2  ];
      /*c43Im*/  preCoef[t18+i] = z[i]*ReIm3[i2+1];
      /*c44Re*/  preCoef[t19+i] =      ReIm4[i2  ];
      /*c44Im*/  preCoef[t20+i] =      ReIm4[i2+1];
    }
    if(lMax > 4){
      /*c50  */  preCoef[t21+i] = c50c*z[i];
      /*c51Re*/  preCoef[t22+i] =      x[i]*c51c;
      /*c51Im*/  preCoef[t23+i] =      y[i]*c51c;
      /*c52Re*/  preCoef[t24+i] = z[i]*ReIm2[i2  ]*c52c;
      /*c52Im*/  preCoef[t25+i] = z[i]*ReIm2[i2+1]*c52c;
      /*c53Re*/  preCoef[t26+i] =      ReIm3[i2  ]*c53c;
      /*c53Im*/  preCoef[t27+i] =      ReIm3[i2+1]*c53c;
      /*c54Re*/  preCoef[t28+i] = z[i]*ReIm4[i2  ];
      /*c54Im*/  preCoef[t29+i] = z[i]*ReIm4[i2+1];
      /*c55Re*/  preCoef[t30+i] =      ReIm5[i2  ];
      /*c55Im*/  preCoef[t31+i] =      ReIm5[i2+1];
    }
    if(lMax > 5){
      /*c60  */  preCoef[t32+i] = c60c;
      /*c61Re*/  preCoef[t33+i] = z[i]*x[i]*c61c;
      /*c61Im*/  preCoef[t34+i] = z[i]*y[i]*c61c;
      /*c62Re*/  preCoef[t35+i] =      ReIm2[i2  ]*c62c;
      /*c62Im*/  preCoef[t36+i] =      ReIm2[i2+1]*c62c;
      /*c63Re*/  preCoef[t37+i] = z[i]*ReIm3[i2  ]*c63c;
      /*c63Im*/  preCoef[t38+i] = z[i]*ReIm3[i2+1]*c63c;
      /*c64Re*/  preCoef[t39+i] =      ReIm4[i2  ]*c64c;
      /*c64Im*/  preCoef[t40+i] =      ReIm4[i2+1]*c64c;
      /*c65Re*/  preCoef[t41+i] = z[i]*ReIm5[i2  ];
      /*c65Im*/  preCoef[t42+i] = z[i]*ReIm5[i2+1];
      /*c66Re*/  preCoef[t43+i] =      ReIm6[i2  ];
      /*c66Im*/  preCoef[t44+i] =      ReIm6[i2+1];
    }
    //printf("DDD\n");
    if(lMax > 6){
      /*c70  */  preCoef[t45+i] = c70c*z[i];
      /*c71Re*/  preCoef[t46+i] = x[i]*c71c;
      /*c71Im*/  preCoef[t47+i] = y[i]*c71c;
      /*c72Re*/  preCoef[t48+i] = z[i]*ReIm2[i2  ]*c72c;
      /*c72Im*/  preCoef[t49+i] = z[i]*ReIm2[i2+1]*c72c;
      /*c73Re*/  preCoef[t50+i] =      ReIm3[i2  ]*c73c;
      /*c73Im*/  preCoef[t51+i] =      ReIm3[i2+1]*c73c;
      /*c74Re*/  preCoef[t52+i] = z[i]*ReIm4[i2  ]*c74c;
      /*c74Im*/  preCoef[t53+i] = z[i]*ReIm4[i2+1]*c74c;
      /*c75Re*/  preCoef[t54+i] =      ReIm5[i2  ]*c75c;
      /*c75Im*/  preCoef[t55+i] =      ReIm5[i2+1]*c75c;
      /*c76Re*/  preCoef[t56+i] = z[i]*ReIm6[i2  ];
      /*c76Im*/  preCoef[t57+i] = z[i]*ReIm6[i2+1];
      /*c77Re*/  preCoef[t58+i] =      ReIm7[i2  ];
      /*c77Im*/  preCoef[t59+i] =      ReIm7[i2+1];
    }
    if(lMax > 7){
      /*c80  */  preCoef[t60+i] = c80c;
      /*c81Re*/  preCoef[t61+i] = z[i]*x[i]*c81c;
      /*c81Im*/  preCoef[t62+i] = z[i]*y[i]*c81c;
      /*c82Re*/  preCoef[t63+i] =      ReIm2[i2  ]*c82c;
      /*c82Im*/  preCoef[t64+i] =      ReIm2[i2+1]*c82c;
      /*c83Re*/  preCoef[t65+i] = z[i]*ReIm3[i2  ]*c83c;
      /*c83Im*/  preCoef[t66+i] = z[i]*ReIm3[i2+1]*c83c;
      /*c84Re*/  preCoef[t67+i] =      ReIm4[i2  ]*c84c;
      /*c84Im*/  preCoef[t68+i] =      ReIm4[i2+1]*c84c;
      /*c85Re*/  preCoef[t69+i] = z[i]*ReIm5[i2  ]*c85c;
      /*c85Im*/  preCoef[t70+i] = z[i]*ReIm5[i2+1]*c85c;
      /*c86Re*/  preCoef[t71+i] =      ReIm6[i2  ]*c86c;
      /*c86Im*/  preCoef[t72+i] =      ReIm6[i2+1]*c86c;
      /*c87Re*/  preCoef[t73+i] = z[i]*ReIm7[i2  ];
      /*c87Im*/  preCoef[t74+i] = z[i]*ReIm7[i2+1];
      /*c88Re*/  preCoef[t75+i] =      ReIm8[i2  ];
      /*c88Im*/  preCoef[t76+i] =      ReIm8[i2+1];
    }
    if(lMax > 8){
      /*c90  */  preCoef[t77+i] = c90c*z[i];
      /*c91Re*/  preCoef[t78+i] = x[i]*c91c;
      /*c91Im*/  preCoef[t79+i] = y[i]*c91c;
      /*c92Re*/  preCoef[t80+i] = z[i]*ReIm2[i2  ]*c92c;
      /*c92Im*/  preCoef[t81+i] = z[i]*ReIm2[i2+1]*c92c;
      /*c93Re*/  preCoef[t82+i] =      ReIm3[i2  ]*c93c;
      /*c93Im*/  preCoef[t83+i] =      ReIm3[i2+1]*c93c;
      /*c94Re*/  preCoef[t84+i] = z[i]*ReIm4[i2  ]*c94c;
      /*c94Im*/  preCoef[t85+i] = z[i]*ReIm4[i2+1]*c94c;
      /*c95Re*/  preCoef[t86+i] =      ReIm5[i2  ]*c95c;
      /*c95Im*/  preCoef[t87+i] =      ReIm5[i2+1]*c95c;
      /*c96Re*/  preCoef[t88+i] = z[i]*ReIm6[i2  ]*c96c;
      /*c96Im*/  preCoef[t89+i] = z[i]*ReIm6[i2+1]*c96c;
      /*c97Re*/  preCoef[t90+i] =      ReIm7[i2  ]*c97c;
      /*c97Im*/  preCoef[t91+i] =      ReIm7[i2+1]*c97c;
      /*c98Re*/  preCoef[t92+i] = z[i]*ReIm8[i2  ];
      /*c98Im*/  preCoef[t93+i] = z[i]*ReIm8[i2+1];
      /*c99Re*/  preCoef[t94+i] =      ReIm9[i2  ];
      /*c99Im*/  preCoef[t95+i] =      ReIm9[i2+1];
    }
  }
    //printf("EEE\n");
}
//================================================================
int getC(float* C, float* preCoef, float* x, float* y, float* z,float* r2, float* bOa, float* aOa, float* exes,  int totalAN, int Asize, int Ns, int Ntypes, int lMax, int posI, int typeJ, int Nx2, int Nx3, int Nx4, int Nx5, int Nx6, int Nx7, int Nx8, int Nx9, int Nx10, int Nx11, int Nx12, int Nx13, int Nx14, int Nx15, int Nx16, int Nx17, int Nx18, int Nx19, int Nx20, int Nx21, int Nx22, int Nx23, int Nx24, int Nx25, int Nx26, int Nx27, int Nx28, int Nx29, int Nx30, int Nx31, int Nx32, int Nx33, int Nx34, int Nx35, int Nx36, int Nx37, int Nx38, int Nx39, int Nx40, int Nx41, int Nx42, int Nx43, int Nx44, int Nx45, int Nx46, int Nx47, int Nx48, int Nx49, int Nx50, int Nx51, int Nx52, int Nx53, int Nx54, int Nx55, int Nx56, int Nx57, int Nx58, int Nx59, int Nx60, int Nx61, int Nx62, int Nx63, int Nx64, int Nx65, int Nx66, int Nx67, int Nx68, int Nx69, int Nx70, int Nx71, int Nx72, int Nx73, int Nx74, int Nx75, int Nx76, int Nx77, int Nx78, int Nx79, int Nx80, int Nx81, int Nx82, int Nx83, int Nx84, int Nx85, int Nx86, int Nx87, int Nx88, int Nx89, int Nx90, int Nx91, int Nx92, int Nx93, int Nx94, int Nx95, int Nx96, int Nx97, int Nx98, int Nx99, int t2, int t3, int t4, int t5, int t6, int t7, int t8, int t9, int t10, int t11, int t12, int t13, int t14, int t15, int t16, int t17, int t18, int t19, int t20, int t21, int t22, int t23, int t24, int t25, int t26, int t27, int t28, int t29, int t30, int t31, int t32, int t33, int t34, int t35, int t36, int t37, int t38, int t39, int t40, int t41, int t42, int t43, int t44, int t45, int t46, int t47, int t48, int t49, int t50, int t51, int t52, int t53, int t54, int t55, int t56, int t57, int t58, int t59, int t60, int t61, int t62, int t63, int t64, int t65, int t66, int t67, int t68, int t69, int t70, int t71, int t72, int t73, int t74, int t75, int t76, int t77, int t78, int t79, int t80, int t81, int t82, int t83, int t84, int t85, int t86, int t87, int t88, int t89, int t90, int t91, int t92, int t93, int t94, int t95, int t96, int t97, int t98, int t99,float* xxx){



  if(Asize == 0){return 0;}
  float sumMe = 0; int NsNs = Ns*Ns;  int NsJ = 100*Ns*typeJ; int LNsNs;
  int LNs; int NsTsI = 100*Ns*Ntypes*posI;
  for(int k = 0; k < Ns; k++){
    sumMe = 0; for(int i = 0; i < Asize; i++){ sumMe += myExp(aOa[k]*r2[i],xxx);}
    for(int n = 0; n < Ns; n++){ C[NsTsI + NsJ + n] += bOa[n*Ns + k]*sumMe; }
  } if(lMax > 0) { LNsNs=NsNs; LNs=Ns;
    for(int k = 0; k < Ns; k++){
      for(int i = 0; i < Asize; i++){exes[i] = myExp(aOa[LNs + k]*r2[i],xxx);}//myExp
      sumMe = 0;/*c10*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*z[i];}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Ns + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c11Re*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*x[i];}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx2 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c11Im*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*y[i];}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx3 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
    }} if(lMax > 1) { LNsNs=2*NsNs; LNs=2*Ns;
    for(int k = 0; k < Ns; k++){
      for(int i = 0; i < Asize; i++){exes[i] = myExp(aOa[LNs + k]*r2[i],xxx);}//myExp
      sumMe = 0;/*c20*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*preCoef[i];}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx4 + n] += bOa[LNsNs + n*Ns + k]*sumMe; }
      sumMe = 0;/*c21Re*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*preCoef[totalAN + i];}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx5 + n] += bOa[LNsNs + n*Ns + k]*sumMe; }
      sumMe = 0;/*c21Im*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*preCoef[t2+ i];}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx6 + n] += bOa[LNsNs + n*Ns + k]*sumMe; }
      sumMe = 0;/*c22Re*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*preCoef[t3+ i];}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx7 + n] += bOa[LNsNs + n*Ns + k]*sumMe; }
      sumMe = 0;/*c22Im*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*preCoef[t4+ i];}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx8 + n] += bOa[LNsNs + n*Ns + k]*sumMe; }
    }} if(lMax > 2) { LNsNs=3*NsNs; LNs=3*Ns;
    for(int k = 0; k < Ns; k++){
      for(int i = 0; i < Asize; i++){exes[i] = myExp(aOa[LNs + k]*r2[i],xxx);}//myExp
      sumMe = 0;/*c30*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t5+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx9 + n] += bOa[LNsNs + n*Ns + k]*sumMe; }
      sumMe = 0;/*c31Re*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t6+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx10 + n] += bOa[LNsNs + n*Ns + k]*sumMe; }
      sumMe = 0;/*c31Im*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t7+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx11 + n] += bOa[LNsNs + n*Ns + k]*sumMe; }
      sumMe = 0;/*c32Re*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t8+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx12 + n] += bOa[LNsNs + n*Ns + k]*sumMe; }
      sumMe = 0;/*c32Im*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t9+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx13 + n] += bOa[LNsNs + n*Ns + k]*sumMe; }
      sumMe = 0;/*c33Re*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t10+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx14 + n] += bOa[LNsNs + n*Ns + k]*sumMe; }
      sumMe = 0;/*c33Im*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t11+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx15 + n] += bOa[LNsNs + n*Ns + k]*sumMe; }
    }} if(lMax > 3) { LNsNs=4*NsNs; LNs=4*Ns;
    for(int k = 0; k < Ns; k++){
      for(int i = 0; i < Asize; i++){exes[i] = myExp(aOa[LNs + k]*r2[i],xxx);}//myExp
      sumMe = 0;/*c40*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t12+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx16 + n] += bOa[LNsNs + n*Ns + k]*sumMe; }
      sumMe = 0;/*c41Re*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t13+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx17 + n] += bOa[LNsNs + n*Ns + k]*sumMe; }
      sumMe = 0;/*c41Im*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t14+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx18 + n] += bOa[LNsNs + n*Ns + k]*sumMe; }
      sumMe = 0;/*c42Re*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t15+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx19 + n] += bOa[LNsNs + n*Ns + k]*sumMe; }
      sumMe = 0;/*c42Im*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t16+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx20 + n] += bOa[LNsNs + n*Ns + k]*sumMe; }
      sumMe = 0;/*c43Re*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t17+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx21 + n] += bOa[LNsNs + n*Ns + k]*sumMe; }
      sumMe = 0;/*c43Im*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t18+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx22 + n] += bOa[LNsNs + n*Ns + k]*sumMe; }
      sumMe = 0;/*c44Re*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t19+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx23 + n] += bOa[LNsNs + n*Ns + k]*sumMe; }
      sumMe = 0;/*c44Im*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t20+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx24 + n] += bOa[LNsNs + n*Ns + k]*sumMe; }

    }} if(lMax > 4) { LNsNs=5*NsNs; LNs=5*Ns;
    for(int k = 0; k < Ns; k++){
      for(int i = 0; i < Asize; i++){exes[i] = myExp(aOa[LNs + k]*r2[i],xxx);}//myExp
      sumMe = 0;/*c50*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t21+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx25 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c51Re*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t22+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx26 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c51Im*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t23+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx27 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c52Re*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t24+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx28 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c52Im*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t25+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx29 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c53Re*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t26+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx30 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c53Im*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t27+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx31 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c54Re*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t28+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx32 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c54Im*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t29+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx33 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c55Re*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t30+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx34 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c55Im*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t31+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx35 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
    }} if(lMax > 5) { LNsNs=6*NsNs; LNs=6*Ns;
    for(int k = 0; k < Ns; k++){
      for(int i = 0; i < Asize; i++){exes[i] = myExp(aOa[LNs + k]*r2[i],xxx);}//myExp
      sumMe = 0;/*c60*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t32+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx36 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c61Re*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t33+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx37 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c61Im*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t34+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx38 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c62Re*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t35+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx39 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c62Im*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t36+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx40 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c63Re*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t37+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx41 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c63Im*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t38+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx42 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c64Re*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t39+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx43 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c64Im*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t40+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx44 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c65Re*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t41+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx45 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c65Im*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t42+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx46 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c66Re*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t43+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx47 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c66Im*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t44+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx48 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
    }} if(lMax > 6) { LNsNs=7*NsNs; LNs=7*Ns;
    for(int k = 0; k < Ns; k++){
      for(int i = 0; i < Asize; i++){exes[i] = myExp(aOa[LNs + k]*r2[i],xxx);}//myExp
      sumMe = 0;/*c70*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t45+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx49 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c71Re*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t46+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx50 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c71Im*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t47+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx51 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c72Re*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t48+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx52 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c72Im*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t49+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx53 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c73Re*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t50+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx54 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c73Im*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t51+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx55 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c74Re*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t52+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx56 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c74Im*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t53+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx57 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c75Re*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t54+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx58 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c75Im*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t55+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx59 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c76Re*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t56+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx60 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c76Im*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t57+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx61 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c77Re*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t58+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx62 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c77Im*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t59+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx63 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
    }} if(lMax > 7) { LNsNs=8*NsNs; LNs=8*Ns;
    for(int k = 0; k < Ns; k++){
      for(int i = 0; i < Asize; i++){exes[i] = myExp(aOa[LNs + k]*r2[i],xxx);}//myExp
      sumMe = 0;/*c80*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t60+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx64 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c81Re*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t61+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx65 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c81Im*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t62+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx66 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c82Re*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t63+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx67 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c82Im*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t64+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx68 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c83Re*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t65+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx69 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c83Im*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t66+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx70 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c84Re*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t67+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx71 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c84Im*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t68+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx72 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c85Re*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t69+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx73 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c85Im*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t70+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx74 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c86Re*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t71+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx75 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c86Im*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t72+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx76 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c87Re*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t73+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx77 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c87Im*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t74+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx78 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c88Re*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t75+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx79 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c88Im*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t76+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx80 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
    }} if(lMax > 8) { LNsNs=9*NsNs; LNs=9*Ns;
    for(int k = 0; k < Ns; k++){
      for(int i = 0; i < Asize; i++){exes[i] = myExp(aOa[LNs + k]*r2[i],xxx);}//myExp
      sumMe = 0;/*c90*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t77+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx81 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c91Re*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t78+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx82 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c91Im*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t79+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx83 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c92Re*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t80+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx84 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c92Im*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t81+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx85 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c93Re*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t82+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx86 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c93Im*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t83+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx87 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c94Re*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t84+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx88 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c94Im*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t85+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx89 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c95Re*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t86+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx90 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c95Im*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t87+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx91 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c96Re*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t88+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx92 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c96Im*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t89+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx93 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c97Re*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t90+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx94 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c97Im*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t91+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx95 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c98Re*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t92+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx96 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c98Im*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t93+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx97 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c99Re*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t94+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx98 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
      sumMe = 0;/*c99Im*/ for(int i = 0; i < Asize; i++){sumMe += exes[i]*(preCoef[t95+i]);}
      for(int n = 0; n < Ns; n++){C[NsTsI + NsJ + Nx99 + n] += bOa[LNsNs + n*Ns + k]*sumMe;}
    }}
}
//=======================================================================
/**
 * Used to calculate the partial power spectrum.
 */
void getP(float* soapMat, float* Cnnd, int Ns, int Ts, int Hs, int lMax){
  int NsTs100 = Ns*Ts*100;
  int Ns100 = Ns*100;
  int NsNs = (Ns*(Ns+1))/2;
  int NsNsLmax = NsNs*(lMax+1) ;
  int NsNsLmaxTs = NsNsLmax*getCrosNum(Ts);

  int shiftN = 0;
  int shiftT = 0;

  for(int i = 0; i < Hs*NsNs*(lMax+1)*3; i++){soapMat[i] = 0.0;}

//  float   cs0  = pow(PIHalf,2);
//  float   cs1  = pow(2.7206990464,2);
//  float cs2  = 2*pow(1.9238247452,2); float   cs3  = pow(1.7562036828,2); float cs4  = 2*pow(4.3018029072,2);
//  float cs5  = 2*pow(2.1509014536,2); float   cs6  = pow(2.0779682205,2); float cs7  = 2*pow(1.7995732672,2);
//  float cs8  = 2*pow(5.6907503408,2); float cs9  = 2*pow(2.3232390981,2); float   cs10 = pow(0.5890486225,2);
//  float cs11 = 2*pow(2.6343055241,2); float cs12 = 2*pow(1.8627352998,2); float cs13 = 2*pow(6.9697172942,2);
//  float cs14 = 2*pow(2.4641671809,2); float   cs15 = pow(0.6512177548,2); float cs16 = 2*pow(1.7834332706,2);
//  float cs17 = 2*pow(9.4370418280,2); float cs18 = 2*pow(1.9263280966,2); float cs19 = 2*pow(8.1727179596,2);
//  float cs20 = 2*pow(2.5844403427,2); float   cs21 = pow(0.3539741687,2); float cs22 = 2*pow(2.2940148014,2);
//  float cs23 = 2*pow(1.8135779397,2); float cs24 = 2*pow(3.6271558793,2); float cs25 = 2*pow(1.9866750947,2);
//  float cs26 = 2*pow(9.3183321738,2); float cs27 = 2*pow(2.6899707945,2); float   cs28 = pow(0.3802292509,2);
//  float cs29 = 2*pow(0.3556718963,2); float cs30 = 2*pow(0.8712146618,2); float cs31 = 2*pow(0.6160417952,2);
//  float cs32 = 2*pow(4.0863589798,2); float cs33 = 2*pow(2.0431794899,2); float cs34 = 2*pow(10.418212089,2);
//  float cs35 = 2*pow(2.7843843014,2); float   cs36 = pow(0.0505981185,2); float cs37 = 2*pow(0.4293392727,2);
//  float cs38 = 2*pow(1.7960550366,2); float cs39 = 2*pow(4.8637400313,2); float cs40 = 2*pow(1.8837184141,2);
//  float cs41 = 2*pow(13.583686661,2); float cs42 = 2*pow(2.0960083567,2); float cs43 = 2*pow(11.480310577,2);
//  float cs44 = 2*pow(2.8700776442,2); float   cs45 = pow(0.0534917379,2); float cs46 = 2*pow(0.2537335916,2);
//  float cs47 = 2*pow(2.3802320735,2); float cs48 = 2*pow(1.8179322747,2); float cs49 = 2*pow(16.055543121,2);
//  float cs50 = 2*pow(1.9190044477,2); float cs51 = 2*pow(4.9548481782,2); float cs52 = 2*pow(2.1455121971,2);
//  float cs53 = 2*pow(12.510378411,2); float cs54 = 2*pow(2.9487244699,2);
float cs0=2.4674011003; float cs1=7.4022033011; float cs2=7.4022033005;
float cs3=3.0842513755; float cs4=37.0110165048; float cs5=9.2527541262;
float cs6=4.3179519254; float cs7=6.4769278880; float cs8=64.7692788826;
float cs9=10.7948798139; float cs10=0.3469782797; float cs11=13.8791311886;
float cs12=6.9395655942; float cs13=97.1539183221; float cs14=12.1442397908;
float cs15=0.4240845642; float cs16=6.3612684614; float cs17=178.1155169268;
float cs18=7.4214798715; float cs19=133.5866376943; float cs20=13.3586637700;
float cs21=0.1252977121; float cs22=10.5250078181; float cs23=6.5781298867;
float cs24=26.3125195455; float cs25=7.8937558638; float cs26=173.6626290026;
float cs27=14.4718857505; float cs28=0.1445742832; float cs29=0.2530049956;
float cs30=1.5180299739; float cs31=0.7590149869; float cs32=33.3966594236;
float cs33=8.3491648559; float cs34=217.0782862628; float cs35=15.5055918758;
float cs36=0.0025601696; float cs37=0.3686644222; float cs38=6.4516273890;
float cs39=47.3119341841; float cs40=7.0967901272; float cs41=369.0330866085;
float cs42=8.7865020627; float cs43=263.5950618888; float cs44=16.4746913675;
float cs45=0.0028613660; float cs46=0.1287614710; float cs47=11.3310094474;
float cs48=6.6097555108; float cs49=515.5609298206; float cs50=7.3651561406;
float cs51=49.1010409380; float cs52=9.2064451758; float cs53=313.0191359728;
float cs54=17.3899519988;

  // The power spectrum is multiplied by an l-dependent prefactor that comes
  // from the normalization of the Wigner D matrices. This prefactor is
  // mentioned in the arrata of the original SOAP paper: On representing
  // chemical environments, Phys. Rev. B 87, 184115 (2013). Here the square
  // root of the prefactor in the dot-product kernel is used, so that after a
  // possible dot-product the full prefactor is recovered.

   // SUM M's UP!
  float prel0 = PI*sqrt(8.0/(1.0));
  for(int i = 0; i < Hs; i++){
    shiftT = 0;
    for(int j = 0; j < Ts; j++){
     for(int jd = j; jd < Ts; jd++){
       shiftN = 0;
      for(int k = 0; k < Ns; k++){
        for(int kd = k; kd < Ns; kd++){
          soapMat[NsNsLmaxTs*i + NsNsLmax*shiftT + 0 + shiftN] = prel0*(
            cs0*Cnnd[NsTs100*i + Ns100*j + 0 + k]*Cnnd[NsTs100*i + Ns100*jd + 0 + kd]);
          shiftN++;
        }
      }
	  shiftT++;
    }
   }
  } if(lMax > 0){
    float prel1 = PI*sqrt(8.0/(2.0*1.0+1.0));
    for(int i = 0; i < Hs; i++){
    shiftT = 0;
      for(int j = 0; j < Ts; j++){
     for(int jd = j; jd < Ts; jd++){
       shiftN = 0;
        for(int k = 0; k < Ns; k++){
          for(int kd = k; kd < Ns; kd++){
              soapMat[NsNsLmaxTs*i+NsNsLmax*shiftT+ NsNs + shiftN] = prel1*(
                cs1*Cnnd[NsTs100*i + Ns100*j + 1*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 1*Ns + kd]
               +cs2*Cnnd[NsTs100*i + Ns100*j + 2*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 2*Ns + kd]
               +cs2*Cnnd[NsTs100*i + Ns100*j + 3*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 3*Ns + kd]);
            shiftN++;
          }
        }
		shiftT++;
      }
      }
    }
  }  if(lMax > 1){
    float prel2 = PI*sqrt(8.0/(2.0*2.0+1.0));
    for(int i = 0; i < Hs; i++){
    shiftT = 0;
      for(int j = 0; j < Ts; j++){
     for(int jd = j; jd < Ts; jd++){
       shiftN = 0;
        for(int k = 0; k < Ns; k++){
          for(int kd = k; kd < Ns; kd++){
              soapMat[NsNsLmaxTs*i+NsNsLmax*shiftT+ 2*NsNs + shiftN] = prel2*(
                cs3*Cnnd[NsTs100*i + Ns100*j + 4*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 4*Ns + kd]
               +cs4*Cnnd[NsTs100*i + Ns100*j + 5*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 5*Ns + kd]
               +cs4*Cnnd[NsTs100*i + Ns100*j + 6*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 6*Ns + kd]
               +cs5*Cnnd[NsTs100*i + Ns100*j + 7*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 7*Ns + kd]
               +cs5*Cnnd[NsTs100*i + Ns100*j + 8*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 8*Ns + kd]);
            shiftN++;
          }
        }
		shiftT++;
      }
      }
    }
  }  if(lMax > 2){
    float prel3 = PI*sqrt(8.0/(2.0*3.0+1.0));
    for(int i = 0; i < Hs; i++){
    shiftT = 0;
      for(int j = 0; j < Ts; j++){
     for(int jd = j; jd < Ts; jd++){
       shiftN = 0;
        for(int k = 0; k < Ns; k++){
          for(int kd = k; kd < Ns; kd++){
              soapMat[NsNsLmaxTs*i+NsNsLmax*shiftT+ 3*NsNs + shiftN] = prel3*(
                cs6*Cnnd[NsTs100*i + Ns100*j + 9*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 9*Ns + kd]
               +cs7*Cnnd[NsTs100*i + Ns100*j + 10*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 10*Ns + kd]
               +cs7*Cnnd[NsTs100*i + Ns100*j + 11*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 11*Ns + kd]
               +cs8*Cnnd[NsTs100*i + Ns100*j + 12*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 12*Ns + kd]
               +cs8*Cnnd[NsTs100*i + Ns100*j + 13*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 13*Ns + kd]
               +cs9*Cnnd[NsTs100*i + Ns100*j + 14*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 14*Ns + kd]
               +cs9*Cnnd[NsTs100*i + Ns100*j + 15*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 15*Ns + kd]);
            shiftN++;
          }
        }
		shiftT++;
      }
      }
    }
  }  if(lMax > 3){
    float prel4 = PI*sqrt(8.0/(2.0*4.0+1.0));
    for(int i = 0; i < Hs; i++){
    shiftT = 0;
      for(int j = 0; j < Ts; j++){
     for(int jd = j; jd < Ts; jd++){
       shiftN = 0;
        for(int k = 0; k < Ns; k++){
          for(int kd = k; kd < Ns; kd++){
              soapMat[NsNsLmaxTs*i+NsNsLmax*shiftT+ 4*NsNs + shiftN] = prel4*(
                cs10*Cnnd[NsTs100*i + Ns100*j + 16*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 16*Ns + kd]
               +cs11*Cnnd[NsTs100*i + Ns100*j + 17*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 17*Ns + kd]
               +cs11*Cnnd[NsTs100*i + Ns100*j + 18*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 18*Ns + kd]
               +cs12*Cnnd[NsTs100*i + Ns100*j + 19*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 19*Ns + kd]
               +cs12*Cnnd[NsTs100*i + Ns100*j + 20*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 20*Ns + kd]
               +cs13*Cnnd[NsTs100*i + Ns100*j + 21*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 21*Ns + kd]
               +cs13*Cnnd[NsTs100*i + Ns100*j + 22*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 22*Ns + kd]
               +cs14*Cnnd[NsTs100*i + Ns100*j + 23*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 23*Ns + kd]
               +cs14*Cnnd[NsTs100*i + Ns100*j + 24*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 24*Ns + kd]);
            shiftN++;
          }
        }
		shiftT++;
      }
      }
    }
  }  if(lMax > 4) {
    float prel5 = PI*sqrt(8.0/(2.0*5.0+1.0));
    for(int i = 0; i < Hs; i++){
    shiftT = 0;
      for(int j = 0; j < Ts; j++){
     for(int jd = j; jd < Ts; jd++){
       shiftN = 0;
        for(int k = 0; k < Ns; k++){
          for(int kd = k; kd < Ns; kd++){
              soapMat[NsNsLmaxTs*i+NsNsLmax*shiftT+ 5*NsNs + shiftN] = prel5*(
                cs15*Cnnd[NsTs100*i + Ns100*j + 25*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 25*Ns + kd]
               +cs16*Cnnd[NsTs100*i + Ns100*j + 26*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 26*Ns + kd]
               +cs16*Cnnd[NsTs100*i + Ns100*j + 27*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 27*Ns + kd]
               +cs17*Cnnd[NsTs100*i + Ns100*j + 28*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 28*Ns + kd]
               +cs17*Cnnd[NsTs100*i + Ns100*j + 29*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 29*Ns + kd]
               +cs18*Cnnd[NsTs100*i + Ns100*j + 30*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 30*Ns + kd]
               +cs18*Cnnd[NsTs100*i + Ns100*j + 31*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 31*Ns + kd]
               +cs19*Cnnd[NsTs100*i + Ns100*j + 32*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 32*Ns + kd]
               +cs19*Cnnd[NsTs100*i + Ns100*j + 33*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 33*Ns + kd]
               +cs20*Cnnd[NsTs100*i + Ns100*j + 34*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 34*Ns + kd]
               +cs20*Cnnd[NsTs100*i + Ns100*j + 35*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 35*Ns + kd]);
            shiftN++;
          }
        }
		shiftT++;
      }
      }
    }
  }  if(lMax > 5){
    float prel6 = PI*sqrt(8.0/(2.0*6.0+1.0));
    for(int i = 0; i < Hs; i++){
    shiftT = 0;
      for(int j = 0; j < Ts; j++){
     for(int jd = j; jd < Ts; jd++){
       shiftN = 0;
        for(int k = 0; k < Ns; k++){
          for(int kd = k; kd < Ns; kd++){
              soapMat[NsNsLmaxTs*i+NsNsLmax*shiftT+ 6*NsNs + shiftN] = prel6*(
                cs21*Cnnd[NsTs100*i + Ns100*j + 36*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 36*Ns + kd]
               +cs22*Cnnd[NsTs100*i + Ns100*j + 37*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 37*Ns + kd]
               +cs22*Cnnd[NsTs100*i + Ns100*j + 38*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 38*Ns + kd]
               +cs23*Cnnd[NsTs100*i + Ns100*j + 39*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 39*Ns + kd]
               +cs23*Cnnd[NsTs100*i + Ns100*j + 40*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 40*Ns + kd]
               +cs24*Cnnd[NsTs100*i + Ns100*j + 41*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 41*Ns + kd]
               +cs24*Cnnd[NsTs100*i + Ns100*j + 42*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 42*Ns + kd]
               +cs25*Cnnd[NsTs100*i + Ns100*j + 43*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 43*Ns + kd]
               +cs25*Cnnd[NsTs100*i + Ns100*j + 44*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 44*Ns + kd]
               +cs26*Cnnd[NsTs100*i + Ns100*j + 45*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 45*Ns + kd]
               +cs26*Cnnd[NsTs100*i + Ns100*j + 46*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 46*Ns + kd]
               +cs27*Cnnd[NsTs100*i + Ns100*j + 47*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 47*Ns + kd]
               +cs27*Cnnd[NsTs100*i + Ns100*j + 48*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 48*Ns + kd]);
            shiftN++;
          }
        }
		shiftT++;
      }
      }
    }
  }  if(lMax > 6){
    float prel7 = PI*sqrt(8.0/(2.0*7.0+1.0));
    for(int i = 0; i < Hs; i++){
    shiftT = 0;
      for(int j = 0; j < Ts; j++){
     for(int jd = j; jd < Ts; jd++){
       shiftN = 0;
        for(int k = 0; k < Ns; k++){
          for(int kd = k; kd < Ns; kd++){
              soapMat[NsNsLmaxTs*i+NsNsLmax*shiftT+ 7*NsNs + shiftN] = prel7*(
                cs28*Cnnd[NsTs100*i + Ns100*j + 49*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 49*Ns + kd]
               +cs29*Cnnd[NsTs100*i + Ns100*j + 50*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 50*Ns + kd]
               +cs29*Cnnd[NsTs100*i + Ns100*j + 51*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 51*Ns + kd]
               +cs30*Cnnd[NsTs100*i + Ns100*j + 52*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 52*Ns + kd]
               +cs30*Cnnd[NsTs100*i + Ns100*j + 53*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 53*Ns + kd]
               +cs31*Cnnd[NsTs100*i + Ns100*j + 54*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 54*Ns + kd]
               +cs31*Cnnd[NsTs100*i + Ns100*j + 55*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 55*Ns + kd]
               +cs32*Cnnd[NsTs100*i + Ns100*j + 56*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 56*Ns + kd]
               +cs32*Cnnd[NsTs100*i + Ns100*j + 57*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 57*Ns + kd]
               +cs33*Cnnd[NsTs100*i + Ns100*j + 58*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 58*Ns + kd]
               +cs33*Cnnd[NsTs100*i + Ns100*j + 59*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 59*Ns + kd]
               +cs34*Cnnd[NsTs100*i + Ns100*j + 60*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 60*Ns + kd]
               +cs34*Cnnd[NsTs100*i + Ns100*j + 61*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 61*Ns + kd]
               +cs35*Cnnd[NsTs100*i + Ns100*j + 62*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 62*Ns + kd]
               +cs35*Cnnd[NsTs100*i + Ns100*j + 63*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 63*Ns + kd]);
            shiftN++;
          }
        }
		shiftT++;
      }
      }
    }
  }  if(lMax > 7){
    float prel8 = PI*sqrt(8.0/(2.0*8.0+1.0));
    for(int i = 0; i < Hs; i++){
    shiftT = 0;
      for(int j = 0; j < Ts; j++){
     for(int jd = j; jd < Ts; jd++){
       shiftN = 0;
        for(int k = 0; k < Ns; k++){
          for(int kd = k; kd < Ns; kd++){
              soapMat[NsNsLmaxTs*i+NsNsLmax*shiftT+ 8*NsNs + shiftN] = prel8*(
                cs36*Cnnd[NsTs100*i + Ns100*j + 64*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 64*Ns + kd]
               +cs37*Cnnd[NsTs100*i + Ns100*j + 65*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 65*Ns + kd]
               +cs37*Cnnd[NsTs100*i + Ns100*j + 66*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 66*Ns + kd]
               +cs38*Cnnd[NsTs100*i + Ns100*j + 67*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 67*Ns + kd]
               +cs38*Cnnd[NsTs100*i + Ns100*j + 68*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 68*Ns + kd]
               +cs39*Cnnd[NsTs100*i + Ns100*j + 69*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 69*Ns + kd]
               +cs39*Cnnd[NsTs100*i + Ns100*j + 70*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 70*Ns + kd]
               +cs40*Cnnd[NsTs100*i + Ns100*j + 71*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 71*Ns + kd]
               +cs40*Cnnd[NsTs100*i + Ns100*j + 72*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 72*Ns + kd]
               +cs41*Cnnd[NsTs100*i + Ns100*j + 73*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 73*Ns + kd]
               +cs41*Cnnd[NsTs100*i + Ns100*j + 74*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 74*Ns + kd]
               +cs42*Cnnd[NsTs100*i + Ns100*j + 75*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 75*Ns + kd]
               +cs42*Cnnd[NsTs100*i + Ns100*j + 76*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 76*Ns + kd]
               +cs43*Cnnd[NsTs100*i + Ns100*j + 77*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 77*Ns + kd]
               +cs43*Cnnd[NsTs100*i + Ns100*j + 78*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 78*Ns + kd]
               +cs44*Cnnd[NsTs100*i + Ns100*j + 79*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 79*Ns + kd]
               +cs44*Cnnd[NsTs100*i + Ns100*j + 80*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 80*Ns + kd]);
            shiftN++;
          }
        }
		shiftT++;
      }
      }
    }
  }  if(lMax > 8){
    float prel9 = PI*sqrt(8.0/(2.0*9.0+1.0));
    for(int i = 0; i < Hs; i++){
    shiftT = 0;
      for(int j = 0; j < Ts; j++){
     for(int jd = j; jd < Ts; jd++){
       shiftN = 0;
        for(int k = 0; k < Ns; k++){
          for(int kd = k; kd < Ns; kd++){
              soapMat[NsNsLmaxTs*i+NsNsLmax*shiftT+ 9*NsNs + shiftN] = prel9*(
                cs45*Cnnd[NsTs100*i + Ns100*j + 81*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 81*Ns + kd]
               +cs46*Cnnd[NsTs100*i + Ns100*j + 82*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 82*Ns + kd]
               +cs46*Cnnd[NsTs100*i + Ns100*j + 83*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 83*Ns + kd]
               +cs47*Cnnd[NsTs100*i + Ns100*j + 84*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 84*Ns + kd]
               +cs47*Cnnd[NsTs100*i + Ns100*j + 85*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 85*Ns + kd]
               +cs48*Cnnd[NsTs100*i + Ns100*j + 86*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 86*Ns + kd]
               +cs48*Cnnd[NsTs100*i + Ns100*j + 87*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 87*Ns + kd]
               +cs49*Cnnd[NsTs100*i + Ns100*j + 88*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 88*Ns + kd]
               +cs49*Cnnd[NsTs100*i + Ns100*j + 89*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 89*Ns + kd]
               +cs50*Cnnd[NsTs100*i + Ns100*j + 90*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 90*Ns + kd]
               +cs50*Cnnd[NsTs100*i + Ns100*j + 91*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 91*Ns + kd]
               +cs51*Cnnd[NsTs100*i + Ns100*j + 92*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 92*Ns + kd]
               +cs51*Cnnd[NsTs100*i + Ns100*j + 93*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 93*Ns + kd]
               +cs52*Cnnd[NsTs100*i + Ns100*j + 94*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 94*Ns + kd]
               +cs52*Cnnd[NsTs100*i + Ns100*j + 95*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 95*Ns + kd]
               +cs53*Cnnd[NsTs100*i + Ns100*j + 96*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 96*Ns + kd]
               +cs53*Cnnd[NsTs100*i + Ns100*j + 97*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 97*Ns + kd]
               +cs54*Cnnd[NsTs100*i + Ns100*j + 98*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 98*Ns + kd]
               +cs54*Cnnd[NsTs100*i + Ns100*j + 99*Ns + k]*Cnnd[NsTs100*i + Ns100*jd + 99*Ns + kd]);
            shiftN++;
          }
        }
		shiftT++;
      }
      }
    }
   }
}
//===========================================================================================
//===========================================================================================
int soap(float* c, float* Apos,float* Hpos,float* alphas,float* betas, int* typeNs, float rCut, int totalAN,int Nt,int Ns, int lMax, int Hs, float sigma);
int soap(float* c, float* Apos,float* Hpos, float* alphas,float* betas, int* typeNs, float rCut, int totalAN,int Nt,int Ns, int lMax, int Hs, float sigma){

  float oOsigma = 1/sigma;
  float oOsigma3O2 = sqrt(oOsigma*oOsigma*oOsigma);

  //printf("xxx\n");
  float NsNs = Ns*Ns;
  float* x  = (float*) malloc(sizeof(float)*totalAN);
  float* y  = (float*) malloc(sizeof(float)*totalAN);
  float* z  = (float*) malloc(sizeof(float)*totalAN);
  float* z2 = (float*) malloc(sizeof(float)*totalAN);
  float* z4 = (float*) malloc(sizeof(float)*totalAN);
  float* z6 = (float*) malloc(sizeof(float)*totalAN);
  float* z8 = (float*) malloc(sizeof(float)*totalAN);
  float* r2 = (float*) malloc(sizeof(float)*totalAN);
  float* r4 = (float*) malloc(sizeof(float)*totalAN);
  float* r6 = (float*) malloc(sizeof(float)*totalAN);
  float* r8 = (float*) malloc(sizeof(float)*totalAN);
  float* ReIm2 = (float*) malloc(2*sizeof(float)*totalAN);// 2 -> Re + ixIm
  float* ReIm3 = (float*) malloc(2*sizeof(float)*totalAN);// 2 -> Re + ixIm
  float* ReIm4 = (float*) malloc(2*sizeof(float)*totalAN);// 2 -> Re + ixIm
  float* ReIm5 = (float*) malloc(2*sizeof(float)*totalAN);// 2 -> Re + ixIm
  float* ReIm6 = (float*) malloc(2*sizeof(float)*totalAN);// 2 -> Re + ixIm
  float* ReIm7 = (float*) malloc(2*sizeof(float)*totalAN);// 2 -> Re + ixIm
  float* ReIm8 = (float*) malloc(2*sizeof(float)*totalAN);// 2 -> Re + ixIm
  float* ReIm9 = (float*) malloc(2*sizeof(float)*totalAN);// 2 -> Re + ixIm
  float* exes = (float*) malloc (sizeof(float)*totalAN);
  float* preCoef = (float*) malloc(96*sizeof(float)*totalAN);
  float* bOa = (float*) malloc((lMax+1)*NsNs*sizeof(float));
  float* aOa = (float*) malloc((lMax+1)*Ns*sizeof(float));
  int Asize;
  //printf("xyx\n");

  int Nx2 = 2*Ns; int Nx3 = 3*Ns; int Nx4 = 4*Ns; int Nx5 = 5*Ns;
  int Nx6 = 6*Ns; int Nx7 = 7*Ns; int Nx8 = 8*Ns; int Nx9 = 9*Ns;
  int Nx10 = 10*Ns; int Nx11 = 11*Ns; int Nx12 = 12*Ns; int Nx13 = 13*Ns;
  int Nx14 = 14*Ns; int Nx15 = 15*Ns; int Nx16 = 16*Ns; int Nx17 = 17*Ns;
  int Nx18 = 18*Ns; int Nx19 = 19*Ns; int Nx20 = 20*Ns; int Nx21 = 21*Ns;
  int Nx22 = 22*Ns; int Nx23 = 23*Ns; int Nx24 = 24*Ns; int Nx25 = 25*Ns;
  int Nx26 = 26*Ns; int Nx27 = 27*Ns; int Nx28 = 28*Ns; int Nx29 = 29*Ns;
  int Nx30 = 30*Ns; int Nx31 = 31*Ns; int Nx32 = 32*Ns; int Nx33 = 33*Ns;
  int Nx34 = 34*Ns; int Nx35 = 35*Ns; int Nx36 = 36*Ns; int Nx37 = 37*Ns;
  int Nx38 = 38*Ns; int Nx39 = 39*Ns; int Nx40 = 40*Ns; int Nx41 = 41*Ns;
  int Nx42 = 42*Ns; int Nx43 = 43*Ns; int Nx44 = 44*Ns; int Nx45 = 45*Ns;
  int Nx46 = 46*Ns; int Nx47 = 47*Ns; int Nx48 = 48*Ns; int Nx49 = 49*Ns;
  int Nx50 = 50*Ns; int Nx51 = 51*Ns; int Nx52 = 52*Ns; int Nx53 = 53*Ns;
  int Nx54 = 54*Ns; int Nx55 = 55*Ns; int Nx56 = 56*Ns; int Nx57 = 57*Ns;
  int Nx58 = 58*Ns; int Nx59 = 59*Ns; int Nx60 = 60*Ns; int Nx61 = 61*Ns;
  int Nx62 = 62*Ns; int Nx63 = 63*Ns; int Nx64 = 64*Ns; int Nx65 = 65*Ns;
  int Nx66 = 66*Ns; int Nx67 = 67*Ns; int Nx68 = 68*Ns; int Nx69 = 69*Ns;
  int Nx70 = 70*Ns; int Nx71 = 71*Ns; int Nx72 = 72*Ns; int Nx73 = 73*Ns;
  int Nx74 = 74*Ns; int Nx75 = 75*Ns; int Nx76 = 76*Ns; int Nx77 = 77*Ns;
  int Nx78 = 78*Ns; int Nx79 = 79*Ns; int Nx80 = 80*Ns; int Nx81 = 81*Ns;
  int Nx82 = 82*Ns; int Nx83 = 83*Ns; int Nx84 = 84*Ns; int Nx85 = 85*Ns;
  int Nx86 = 86*Ns; int Nx87 = 87*Ns; int Nx88 = 88*Ns; int Nx89 = 89*Ns;
  int Nx90 = 90*Ns; int Nx91 = 91*Ns; int Nx92 = 92*Ns; int Nx93 = 93*Ns;
  int Nx94 = 94*Ns; int Nx95 = 95*Ns; int Nx96 = 96*Ns; int Nx97 = 97*Ns;
  int Nx98 = 98*Ns; int Nx99 = 99*Ns;
  int t2 = 2*totalAN;  int t3 = 3*totalAN;  int t4 = 4*totalAN;
  int t5 = 5*totalAN;  int t6 = 6*totalAN;  int t7 = 7*totalAN;
  int t8 = 8*totalAN;  int t9 = 9*totalAN;  int t10 = 10*totalAN;
  int t11 = 11*totalAN;  int t12 = 12*totalAN;  int t13 = 13*totalAN;
  int t14 = 14*totalAN;  int t15 = 15*totalAN;  int t16 = 16*totalAN;
  int t17 = 17*totalAN;  int t18 = 18*totalAN;  int t19 = 19*totalAN;
  int t20 = 20*totalAN;  int t21 = 21*totalAN;  int t22 = 22*totalAN;
  int t23 = 23*totalAN;  int t24 = 24*totalAN;  int t25 = 25*totalAN;
  int t26 = 26*totalAN;  int t27 = 27*totalAN;  int t28 = 28*totalAN;
  int t29 = 29*totalAN;  int t30 = 30*totalAN;  int t31 = 31*totalAN;
  int t32 = 32*totalAN;  int t33 = 33*totalAN;  int t34 = 34*totalAN;
  int t35 = 35*totalAN;  int t36 = 36*totalAN;  int t37 = 37*totalAN;
  int t38 = 38*totalAN;  int t39 = 39*totalAN;  int t40 = 40*totalAN;
  int t41 = 41*totalAN;  int t42 = 42*totalAN;  int t43 = 43*totalAN;
  int t44 = 44*totalAN;  int t45 = 45*totalAN;  int t46 = 46*totalAN;
  int t47 = 47*totalAN;  int t48 = 48*totalAN;  int t49 = 49*totalAN;
  int t50 = 50*totalAN;  int t51 = 51*totalAN;  int t52 = 52*totalAN;
  int t53 = 53*totalAN;  int t54 = 54*totalAN;  int t55 = 55*totalAN;
  int t56 = 56*totalAN;  int t57 = 57*totalAN;  int t58 = 58*totalAN;
  int t59 = 59*totalAN;  int t60 = 60*totalAN;  int t61 = 61*totalAN;
  int t62 = 62*totalAN;  int t63 = 63*totalAN;  int t64 = 64*totalAN;
  int t65 = 65*totalAN;  int t66 = 66*totalAN;  int t67 = 67*totalAN;
  int t68 = 68*totalAN;  int t69 = 69*totalAN;  int t70 = 70*totalAN;
  int t71 = 71*totalAN;  int t72 = 72*totalAN;  int t73 = 73*totalAN;
  int t74 = 74*totalAN;  int t75 = 75*totalAN;  int t76 = 76*totalAN;
  int t77 = 77*totalAN;  int t78 = 78*totalAN;  int t79 = 79*totalAN;
  int t80 = 80*totalAN;  int t81 = 81*totalAN;  int t82 = 82*totalAN;
  int t83 = 83*totalAN;  int t84 = 84*totalAN;  int t85 = 85*totalAN;
  int t86 = 86*totalAN;  int t87 = 87*totalAN;  int t88 = 88*totalAN;
  int t89 = 89*totalAN;  int t90 = 90*totalAN;  int t91 = 91*totalAN;
  int t92 = 92*totalAN;  int t93 = 93*totalAN;  int t94 = 94*totalAN;
  int t95 = 95*totalAN;  int t96 = 96*totalAN;  int t97 = 97*totalAN;
  int t98 = 98*totalAN;  int t99 = 99*totalAN;
  //printf("xzx\n");
float*  xxx = malloc(sizeof(float)*1000);

  xxx[0]=1;xxx[1]=0.9900498337491681;xxx[2]=0.9801986733067553;xxx[3]=0.9704455335485082;xxx[4]=0.9607894391523232;
  xxx[5]=0.951229424500714;xxx[6]=0.9417645335842487;xxx[7]=0.9323938199059483;xxx[8]=0.9231163463866358;xxx[9]=0.9139311852712282;
  xxx[10]=0.9048374180359595;xxx[11]=0.8958341352965282;xxx[12]=0.8869204367171575;xxx[13]=0.8780954309205613;xxx[14]=0.8693582353988059;
  xxx[15]=0.8607079764250578;xxx[16]=0.8521437889662113;xxx[17]=0.8436648165963837;xxx[18]=0.835270211411272;xxx[19]=0.8269591339433623;
  xxx[20]=0.8187307530779818;xxx[21]=0.8105842459701871;xxx[22]=0.8025187979624785;xxx[23]=0.794533602503334;xxx[24]=0.7866278610665535;
  xxx[25]=0.7788007830714049;xxx[26]=0.7710515858035663;xxx[27]=0.7633794943368531;xxx[28]=0.7557837414557255;xxx[29]=0.7482635675785653;
  xxx[30]=0.7408182206817179;xxx[31]=0.7334469562242892;xxx[32]=0.7261490370736909;xxx[33]=0.7189237334319262;xxx[34]=0.7117703227626097;
  xxx[35]=0.7046880897187134;xxx[36]=0.697676326071031;xxx[37]=0.6907343306373547;xxx[38]=0.6838614092123558;xxx[39]=0.6770568744981647;
  xxx[40]=0.6703200460356393;xxx[41]=0.6636502501363194;xxx[42]=0.6570468198150567;xxx[43]=0.6505090947233165;xxx[44]=0.6440364210831414;
  xxx[45]=0.6376281516217733;xxx[46]=0.631283645506926;xxx[47]=0.6250022682827008;xxx[48]=0.6187833918061408;xxx[49]=0.6126263941844161;
  xxx[50]=0.6065306597126334;xxx[51]=0.6004955788122659;xxx[52]=0.5945205479701944;xxx[53]=0.5886049696783552;xxx[54]=0.5827482523739896;
  xxx[55]=0.5769498103804866;xxx[56]=0.5712090638488149;xxx[57]=0.5655254386995371;xxx[58]=0.559898366565402;xxx[59]=0.5543272847345071;
  xxx[60]=0.5488116360940264;xxx[61]=0.5433508690744998;xxx[62]=0.5379444375946745;xxx[63]=0.5325918010068972;xxx[64]=0.5272924240430485;
  xxx[65]=0.522045776761016;xxx[66]=0.5168513344916992;xxx[67]=0.5117085777865424;xxx[68]=0.5066169923655895;xxx[69]=0.5015760690660555;
  xxx[70]=0.4965853037914095;xxx[71]=0.4916441974609651;xxx[72]=0.4867522559599717;xxx[73]=0.4819089900902024;xxx[74]=0.4771139155210344;
  xxx[75]=0.4723665527410147;xxx[76]=0.4676664270099092;xxx[77]=0.4630130683112281;xxx[78]=0.4584060113052235;xxx[79]=0.4538447952823558;
  xxx[80]=0.4493289641172216;xxx[81]=0.4448580662229411;xxx[82]=0.4404316545059992;xxx[83]=0.4360492863215356;xxx[84]=0.4317105234290797;
  xxx[85]=0.4274149319487267;xxx[86]=0.4231620823177488;xxx[87]=0.418951549247639;xxx[88]=0.4147829116815814;xxx[89]=0.4106557527523455;
  xxx[90]=0.4065696597405991;xxx[91]=0.402524224033636;xxx[92]=0.3985190410845141;xxx[93]=0.3945537103716011;xxx[94]=0.3906278353585211;
  xxx[95]=0.3867410234545012;xxx[96]=0.3828928859751121;xxx[97]=0.3790830381033988;xxx[98]=0.3753110988513996;xxx[99]=0.3715766910220457;
  xxx[100]=0.3678794411714423;xxx[101]=0.3642189795715233;xxx[102]=0.3605949401730783;xxx[103]=0.3570069605691474;xxx[104]=0.3534546819587802;
  xxx[105]=0.3499377491111553;xxx[106]=0.3464558103300574;xxx[107]=0.3430085174187066;xxx[108]=0.3395955256449391;xxx[109]=0.3362164937067333;
  xxx[110]=0.3328710836980796;xxx[111]=0.3295589610751891;xxx[112]=0.3262797946230395;xxx[113]=0.3230332564222529;xxx[114]=0.3198190218163038;
  xxx[115]=0.3166367693790532;xxx[116]=0.3134861808826053;xxx[117]=0.310366941265485;xxx[118]=0.3072787386011313;xxx[119]=0.3042212640667041;
  xxx[120]=0.3011942119122021;xxx[121]=0.2981972794298874;xxx[122]=0.2952301669240142;xxx[123]=0.2922925776808594;xxx[124]=0.2893842179390506;
  xxx[125]=0.2865047968601901;xxx[126]=0.2836540264997704;xxx[127]=0.2808316217783798;xxx[128]=0.2780373004531941;xxx[129]=0.2752707830897523;
  xxx[130]=0.2725317930340126;xxx[131]=0.2698200563846868;xxx[132]=0.2671353019658503;xxx[133]=0.264477261299824;xxx[134]=0.261845668580326;
  xxx[135]=0.2592402606458915;xxx[136]=0.2566607769535559;xxx[137]=0.2541069595528003;xxx[138]=0.2515785530597565;xxx[139]=0.2490753046316682;
  xxx[140]=0.2465969639416064;xxx[141]=0.2441432831534371;xxx[142]=0.2417140168970364;xxx[143]=0.2393089222437546;xxx[144]=0.2369277586821218;
  xxx[145]=0.2345702880937977;xxx[146]=0.2322362747297588;xxx[147]=0.2299254851867238;xxx[148]=0.2276376883838127;xxx[149]=0.2253726555394387;
  xxx[150]=0.2231301601484298;xxx[151]=0.2209099779593782;xxx[152]=0.2187118869522147;xxx[153]=0.2165356673160071;xxx[154]=0.2143811014269779;
  xxx[155]=0.212247973826743;xxx[156]=0.2101360712007647;xxx[157]=0.2080451823570205;xxx[158]=0.2059750982048834;xxx[159]=0.2039256117342134;
  xxx[160]=0.2018965179946554;xxx[161]=0.1998876140751445;xxx[162]=0.1978986990836147;xxx[163]=0.1959295741269093;xxx[164]=0.1939800422908919;
  xxx[165]=0.1920499086207541;xxx[166]=0.1901389801015205;xxx[167]=0.1882470656387468;xxx[168]=0.18637397603941;xxx[169]=0.1845195239929893;
  xxx[170]=0.1826835240527347;xxx[171]=0.1808657926171221;xxx[172]=0.1790661479114932;xxx[173]=0.1772844099698778;xxx[174]=0.1755204006169969;
  xxx[175]=0.1737739434504451;xxx[176]=0.1720448638230505;xxx[177]=0.1703329888254094;xxx[178]=0.1686381472685955;xxx[179]=0.1669601696670407;
  xxx[180]=0.1652988882215865;xxx[181]=0.163654136802704;xxx[182]=0.1620257509338807;xxx[183]=0.1604135677751727;xxx[184]=0.1588174261069207;
  xxx[185]=0.1572371663136276;xxx[186]=0.1556726303679973;xxx[187]=0.1541236618151314;xxx[188]=0.1525901057568839;xxx[189]=0.1510718088363708;
  xxx[190]=0.149568619222635;xxx[191]=0.1480803865954624;xxx[192]=0.1466069621303502;xxx[193]=0.1451481984836237;xxx[194]=0.1437039497777029;
  xxx[195]=0.1422740715865136;xxx[196]=0.140858420921045;xxx[197]=0.1394568562150509;xxx[198]=0.1380692373108928;xxx[199]=0.1366954254455238;
  xxx[200]=0.1353352832366127;xxx[201]=0.1339886746688049;xxx[202]=0.1326554650801217;xxx[203]=0.131335521148493;xxx[204]=0.1300287108784259;
  xxx[205]=0.1287349035878042;xxx[206]=0.1274539698948207;xxx[207]=0.1261857817050388;xxx[208]=0.1249302121985824;xxx[209]=0.1236871358174548;
  xxx[210]=0.1224564282529819;xxx[211]=0.1212379664333817;xxx[212]=0.1200316285114567;xxx[213]=0.1188372938524097;xxx[214]=0.1176548430217792;
  xxx[215]=0.116484157773497;xxx[216]=0.1153251210380625;xxx[217]=0.1141776169108365;xxx[218]=0.1130415306404498;xxx[219]=0.1119167486173289;
  xxx[220]=0.1108031583623339;xxx[221]=0.1097006485155114;xxx[222]=0.108609108824958;xxx[223]=0.107528430135795;xxx[224]=0.1064585043792528;
  xxx[225]=0.1053992245618643;xxx[226]=0.104350484754765;xxx[227]=0.1033121800831002;xxx[228]=0.1022842067155374;xxx[229]=0.1012664618538834;
  xxx[230]=0.1002588437228037;xxx[231]=0.09926125155964566;xxx[232]=0.09827358560436154;xxx[233]=0.09729574708953276;xxx[234]=0.09632763823049303;
  xxx[235]=0.09536916221554961;xxx[236]=0.09442022319630235;xxx[237]=0.09348072627805847;xxx[238]=0.09255057751034329;xxx[239]=0.09162968387750484;
  xxx[240]=0.09071795328941251;xxx[241]=0.08981529457224763;xxx[242]=0.08892161745938634;xxx[243]=0.08803683258237255;xxx[244]=0.0871608514619813;
  xxx[245]=0.08629358649937049;xxx[246]=0.08543495096732123;xxx[247]=0.08458485900156469;xxx[248]=0.08374322559219596;xxx[249]=0.08290996657517266;
  xxx[250]=0.0820849986238988;xxx[251]=0.08126823924089167;xxx[252]=0.08045960674953244;xxx[253]=0.07965902028589801;xxx[254]=0.07886639979067495;
  xxx[255]=0.07808166600115313;xxx[256]=0.07730474044329974;xxx[257]=0.07653554542391151;xxx[258]=0.07577400402284548;xxx[259]=0.07502004008532698;
  xxx[260]=0.07427357821433388;xxx[261]=0.0735345437630571;xxx[262]=0.07280286282743559;xxx[263]=0.0720784622387661;xxx[264]=0.07136126955638605;
  xxx[265]=0.0706512130604296;xxx[266]=0.06994822174465536;xxx[267]=0.06925222530934599;xxx[268]=0.06856315415427791;xxx[269]=0.06788093937176144;
  xxx[270]=0.06720551273974976;xxx[271]=0.06653680671501686;xxx[272]=0.06587475442640295;xxx[273]=0.06521928966812753;xxx[274]=0.06457034689316847;
  xxx[275]=0.06392786120670757;xxx[276]=0.0632917683596407;xxx[277]=0.06266200474215315;xxx[278]=0.06203850737735829;xxx[279]=0.06142121391500013;
  xxx[280]=0.06081006262521795;xxx[281]=0.06020499239237354;xxx[282]=0.05960594270893937;xxx[283]=0.05901285366944784;xxx[284]=0.05842566596450083;
  xxx[285]=0.05784432087483846;xxx[286]=0.05726876026546736;xxx[287]=0.0566989265798469;xxx[288]=0.05613476283413373;xxx[289]=0.05557621261148306;
  xxx[290]=0.05502322005640723;xxx[291]=0.05447572986918986;xxx[292]=0.05393368730035602;xxx[293]=0.05339703814519708;xxx[294]=0.05286572873835037;
  xxx[295]=0.05233970594843238;xxx[296]=0.05181891717272583;xxx[297]=0.05130331033191911;xxx[298]=0.0507928338648985;xxx[299]=0.05028743672359186;
  xxx[300]=0.04978706836786394;xxx[301]=0.04929167876046215;xxx[302]=0.04880121836201296;xxx[303]=0.04831563812606777;xxx[304]=0.04783488949419837;
  xxx[305]=0.04735892439114091;xxx[306]=0.04688769521998849;xxx[307]=0.04642115485743125;xxx[308]=0.0459592566490442;xxx[309]=0.04550195440462157;
  xxx[310]=0.0450492023935578;xxx[311]=0.04460095534027454;xxx[312]=0.04415716841969286;xxx[313]=0.04371779725275094;xxx[314]=0.0432827979019659;
  xxx[315]=0.04285212686704019;xxx[316]=0.04242574108051139;xxx[317]=0.04200359790344555;xxx[318]=0.04158565512117316;xxx[319]=0.04117187093906774;
  xxx[320]=0.04076220397836621;xxx[321]=0.04035661327203115;xxx[322]=0.0399550582606539;xxx[323]=0.03955749878839872;xxx[324]=0.03916389509898707;
  xxx[325]=0.03877420783172201;xxx[326]=0.03838839801755205;xxx[327]=0.03800642707517431;xxx[328]=0.0376282568071762;xxx[329]=0.03725384939621581;
  xxx[330]=0.03688316740123999;xxx[331]=0.0365161737537404;xxx[332]=0.03615283175404641;xxx[333]=0.0357931050676553;xxx[334]=0.03543695772159864;
  xxx[335]=0.03508435410084502;xxx[336]=0.03473525894473856;xxx[337]=0.03438963734347271;xxx[338]=0.03404745473459934;xxx[339]=0.0337086768995724;
  xxx[340]=0.03337326996032608;xxx[341]=0.03304120037588693;xxx[342]=0.03271243493901982;xxx[343]=0.03238694077290704;xxx[344]=0.03206468532786077;
  xxx[345]=0.03174563637806794;xxx[346]=0.03142976201836771;xxx[347]=0.03111703066106086;xxx[348]=0.03080741103275108;xxx[349]=0.03050087217121748;
  xxx[350]=0.0301973834223185;xxx[351]=0.02989691443692631;xxx[352]=0.029599435167892;xxx[353]=0.02930491586704075;xxx[354]=0.02901332708219705;
  xxx[355]=0.02872463965423942;xxx[356]=0.0284388247141845;xxx[357]=0.0281558536803001;xxx[358]=0.02787569825524702;xxx[359]=0.02759833042324929;
  xxx[360]=0.02732372244729256;xxx[361]=0.02705184686635042;xxx[362]=0.02678267649263818;xxx[363]=0.02651618440889418;xxx[364]=0.02625234396568796;
  xxx[365]=0.02599112877875535;xxx[366]=0.02573251272635994;xxx[367]=0.02547646994668102;xxx[368]=0.02522297483522721;xxx[369]=0.02497200204227615;
  xxx[370]=0.02472352647033939;xxx[371]=0.02447752327165267;xxx[372]=0.02423396784569111;xxx[373]=0.02399283583670917;xxx[374]=0.023754103131305;
  xxx[375]=0.02351774585600911;xxx[376]=0.023283740374897;xxx[377]=0.02305206328722557;xxx[378]=0.02282269142509297;xxx[379]=0.02259560185112186;
  xxx[380]=0.02237077185616559;xxx[381]=0.02214817895703731;xxx[382]=0.02192780089426161;xxx[383]=0.02170961562984857;xxx[384]=0.02149360134508992;
  xxx[385]=0.02127973643837717;xxx[386]=0.02106799952304143;xxx[387]=0.02085836942521472;xxx[388]=0.02065082518171257;xxx[389]=0.02044534603793765;
  xxx[390]=0.02024191144580439;xxx[391]=0.02004050106168401;xxx[392]=0.01984109474437029;xxx[393]=0.01964367255306529;xxx[394]=0.01944821474538539;
  xxx[395]=0.01925470177538692;xxx[396]=0.01906311429161164;xxx[397]=0.01887343313515149;xxx[398]=0.01868563933773277;xxx[399]=0.01849971411981924;
  xxx[400]=0.01831563888873418;xxx[401]=0.01813339523680108;xxx[402]=0.01795296493950285;xxx[403]=0.01777432995365944;xxx[404]=0.01759747241562339;
  xxx[405]=0.01742237463949351;xxx[406]=0.01724901911534626;xxx[407]=0.01707738850748479;xxx[408]=0.01690746565270528;xxx[409]=0.01673923355858063;
  xxx[410]=0.01657267540176125;xxx[411]=0.01640777452629264;xxx[412]=0.01624451444194987;xxx[413]=0.01608287882258843;xxx[414]=0.0159228515045117;
  xxx[415]=0.01576441648485449;xxx[416]=0.01560755791998283;xxx[417]=0.01545226012390952;xxx[418]=0.01529850756672552;xxx[419]=0.01514628487304698;
  xxx[420]=0.0149955768204777;xxx[421]=0.01484636833808683;xxx[422]=0.01469864450490178;xxx[423]=0.01455239054841612;xxx[424]=0.01440759184311235;
  xxx[425]=0.01426423390899926;xxx[426]=0.01412230241016396;xxx[427]=0.0139817831533383;xxx[428]=0.0138426620864795;xxx[429]=0.01370492529736494;
  xxx[430]=0.01356855901220093;xxx[431]=0.0134335495942453;xxx[432]=0.01329988354244377;xxx[433]=0.01316754749007975;xxx[434]=0.01303652820343774;
  xxx[435]=0.01290681258047986;xxx[436]=0.01277838764953576;xxx[437]=0.01265124056800531;xxx[438]=0.01252535862107438;xxx[439]=0.01240072922044341;
  xxx[440]=0.01227733990306844;xxx[441]=0.01215517832991493;xxx[442]=0.01203423228472377;xxx[443]=0.01191448967278965;xxx[444]=0.01179593851975156;
  xxx[445]=0.01167856697039544;xxx[446]=0.01156236328746854;xxx[447]=0.01144731585050571;xxx[448]=0.01133341315466739;xxx[449]=0.01122064380958908;
  xxx[450]=0.01110899653824231;xxx[451]=0.01099846017580688;xxx[452]=0.01088902366855444;xxx[453]=0.01078067607274308;xxx[454]=0.01067340655352293;
  xxx[455]=0.01056720438385265;xxx[456]=0.01046205894342679;xxx[457]=0.0103579597176137;xxx[458]=0.01025489629640402;xxx[459]=0.01015285837336976;
  xxx[460]=0.01005183574463358;xxx[461]=0.00995181830784842;xxx[462]=0.009852796061187257;xxx[463]=0.009754759102342903;xxx[464]=0.009657697627537777;
  xxx[465]=0.009561601930543505;xxx[466]=0.009466462401710323;xxx[467]=0.009372269527006058;xxx[468]=0.009279013887064744;xxx[469]=0.009186686156244664;
  xxx[470]=0.009095277101695816;xxx[471]=0.009004777582436559;xxx[472]=0.008915178548439553;xxx[473]=0.008826471039726723;xxx[474]=0.00873864618547329;
  xxx[475]=0.008651695203120634;xxx[476]=0.008565609397498061;xxx[477]=0.00848038015995326;xxx[478]=0.008395998967491471;xxx[479]=0.008312457381923119;
  xxx[480]=0.00822974704902003;xxx[481]=0.008147859697679982;xxx[482]=0.008066787139099614;xxx[483]=0.007986521265955502;xxx[484]=0.007907054051593441;
  xxx[485]=0.007828377549225767;xxx[486]=0.007750483891136692;xxx[487]=0.007673365287895489;xxx[488]=0.007597014027577567;xxx[489]=0.00752142247499327;
  xxx[490]=0.007446583070924338;xxx[491]=0.007372488331368012;xxx[492]=0.007299130846788583;xxx[493]=0.007226503281376463;xxx[494]=0.007154598372314579;
  xxx[495]=0.007083408929052118;xxx[496]=0.007012927832585425;xxx[497]=0.006943148034746115;xxx[498]=0.006874062557496248;xxx[499]=0.006805664492230543;
  xxx[500]=0.006737946999085467;xxx[501]=0.006670903306255274;xxx[502]=0.006604526709314805;xxx[503]=0.006538810570549064;xxx[504]=0.006473748318289405;
  xxx[505]=0.006409333446256383;xxx[506]=0.00634555951290911;xxx[507]=0.006282420140801118;xxx[508]=0.006219909015942573;xxx[509]=0.006158019887168897;
  xxx[510]=0.006096746565515633;xxx[511]=0.006036082923599565;xxx[512]=0.005976022895005943;xxx[513]=0.005916560473681857;xxx[514]=0.005857689713335622;
  xxx[515]=0.005799404726842141;xxx[516]=0.005741699685654202;xxx[517]=0.005684568819219595;xxx[518]=0.005628006414404065;xxx[519]=0.005572006814919993;
  xxx[520]=0.005516564420760772;xxx[521]=0.005461673687640779;xxx[522]=0.00540732912644096;xxx[523]=0.005353525302659903;xxx[524]=0.005300256835870402;
  xxx[525]=0.005247518399181385;xxx[526]=0.005195304718705231;xxx[527]=0.005143610573030379;xxx[528]=0.00509243079269919;xxx[529]=0.005041760259690979;
  xxx[530]=0.004991593906910217;xxx[531]=0.004941926717679818;xxx[532]=0.004892753725239476;xxx[533]=0.004844070012248967;xxx[534]=0.004795870710296421;
  xxx[535]=0.004748150999411473;xxx[536]=0.004700906107583276;xxx[537]=0.004654131310283272;xxx[538]=0.004607821929992752;xxx[539]=0.004561973335735096;
  xxx[540]=0.004516580942612666;xxx[541]=0.004471640211348332;xxx[542]=0.004427146647831511;xxx[543]=0.004383095802668776;xxx[544]=0.004339483270738895;
  xxx[545]=0.00429630469075234;xxx[546]=0.004253555744815125;xxx[547]=0.004211232157997035;xxx[548]=0.004169329697904112;xxx[549]=0.004127844174255436;
  xxx[550]=0.004086771438464067;xxx[551]=0.004046107383222199;xxx[552]=0.004005847942090417;xxx[553]=0.003965989089091065;xxx[554]=0.003926526838305624;
  xxx[555]=0.00388745724347613;xxx[556]=0.003848776397610539;xxx[557]=0.003810480432592037;xxx[558]=0.003772565518792205;xxx[559]=0.003735027864688067;
  xxx[560]=0.003697863716482929;xxx[561]=0.003661069357731005;xxx[562]=0.003624641108965756;xxx[563]=0.003588575327331948;xxx[564]=0.003552868406221362;
  xxx[565]=0.003517516774912128;xxx[566]=0.003482516898211663;xxx[567]=0.003447865276103127;xxx[568]=0.00341355844339543;xxx[569]=0.003379592969376712;
  xxx[570]=0.003345965457471272;xxx[571]=0.003312672544899893;xxx[572]=0.003279710902343573;xxx[573]=0.003247077233610586;xxx[574]=0.00321476827530687;
  xxx[575]=0.003182780796509667;xxx[576]=0.003151111598444441;xxx[577]=0.003119757514164992;xxx[578]=0.003088715408236769;xxx[579]=0.003057982176423307;
  xxx[580]=0.003027554745375815;xxx[581]=0.002997430072325829;xxx[582]=0.002967605144780944;xxx[583]=0.00293807698022355;xxx[584]=0.002908842625812584;
  xxx[585]=0.00287989915808824;xxx[586]=0.002851243682679632;xxx[587]=0.002822873334015336;xxx[588]=0.002794785275036844;xxx[589]=0.002766976696914851;
  xxx[590]=0.002739444818768368;xxx[591]=0.002712186887386643;xxx[592]=0.002685200176953821;xxx[593]=0.002658481988776367;xxx[594]=0.002632029651013198;
  xxx[595]=0.002605840518408498;xxx[596]=0.00257991197202718;xxx[597]=0.002554241418992998;xxx[598]=0.002528826292229256;xxx[599]=0.0025036640502021;
  xxx[600]=0.002478752176666358;xxx[601]=0.002454088180413917;xxx[602]=0.002429669595024595;xxx[603]=0.00240549397861951;xxx[604]=0.002381558913616871;
  xxx[605]=0.002357862006490233;xxx[606]=0.002334400887529133;xxx[607]=0.002311173210602129;xxx[608]=0.002288176652922169;xxx[609]=0.002265408914814322;
  xxx[610]=0.002242867719485801;xxx[611]=0.002220550812798294;xxx[612]=0.002198455963042531;xxx[613]=0.002176580960715126;xxx[614]=0.002154923618297613;
  xxx[615]=0.002133481770037708;xxx[616]=0.002112253271732714;xxx[617]=0.00209123600051511;xxx[618]=0.002070427854640261;xxx[619]=0.002049826753276235;
  xxx[620]=0.002029430636295734;xxx[621]=0.00200923746407006;xxx[622]=0.001989245217265163;xxx[623]=0.001969451896639701;xxx[624]=0.001949855522845121;
  xxx[625]=0.001930454136227709;xxx[626]=0.001911245796632638;xxx[627]=0.001892228583209938;xxx[628]=0.001873400594222423;xxx[629]=0.001854759946855503;
  xxx[630]=0.001836304777028907;xxx[631]=0.001818033239210271;xxx[632]=0.001799943506230591;xxx[633]=0.001782033769101492;xxx[634]=0.001764302236834335;
  xxx[635]=0.001746747136261118;xxx[636]=0.001729366711857156;xxx[637]=0.001712159225565523;xxx[638]=0.001695122956623251;xxx[639]=0.001678256201389246;
  xxx[640]=0.001661557273173934;xxx[641]=0.001645024502070575;xxx[642]=0.001628656234788281;xxx[643]=0.001612450834486684;xxx[644]=0.001596406680612247;
  xxx[645]=0.001580522168736217;xxx[646]=0.001564795710394167;xxx[647]=0.001549225732927156;xxx[648]=0.001533810679324463;xxx[649]=0.001518549008067883;
  xxx[650]=0.001503439192977572;xxx[651]=0.001488479723059429;xxx[652]=0.001473669102353995;xxx[653]=0.001459005849786858;xxx[654]=0.001444488499020543;
  xxx[655]=0.001430115598307874;xxx[656]=0.001415885710346802;xxx[657]=0.001401797412136674;xxx[658]=0.001387849294835929;xxx[659]=0.001374039963621212;
  xxx[660]=0.001360368037547893;xxx[661]=0.001346832149411974;xxx[662]=0.001333430945613359;xxx[663]=0.001320163086020503;xxx[664]=0.001307027243836386;
  xxx[665]=0.001294022105465848;xxx[666]=0.001281146370384211;xxx[667]=0.001268398751007239;xxx[668]=0.001255777972562369;xxx[669]=0.00124328277296124;
  xxx[670]=0.001230911902673481;xxx[671]=0.001218664124601752;xxx[672]=0.00120653821395804;xxx[673]=0.001194532958141175;xxx[674]=0.001182647156615573;
  xxx[675]=0.001170879620791174;xxx[676]=0.001159229173904591;xxx[677]=0.001147694650901425;xxx[678]=0.001136274898319766;xxx[679]=0.001124968774174837;
  xxx[680]=0.001113775147844803;xxx[681]=0.001102692899957702;xxx[682]=0.001091720922279511;xxx[683]=0.001080858117603318;xxx[684]=0.001070103399639604;
  xxx[685]=0.001059455692907609;xxx[686]=0.001048913932627788;xxx[687]=0.001038477064615328;xxx[688]=0.00102814404517473;xxx[689]=0.001017913840995438;
  xxx[690]=0.00100778542904851;xxx[691]=0.000997757796484312;xxx[692]=0.0009878299405312295;xxx[693]=0.0009780008683953946;xxx[694]=0.0009682695971614017;
  xxx[695]=0.0009586351536940199;xxx[696]=0.0009490965745408727;xxx[697]=0.0009396529058360961;xxx[698]=0.000930303203204949;xxx[699]=0.0009210465316693785;
  xxx[700]=0.0009118819655545162;xxx[701]=0.0009028085883961136;xxx[702]=0.0008938254928488927;xxx[703]=0.0008849317805958146;xxx[704]=0.0008761265622582417;
  xxx[705]=0.0008674089573070025;xxx[706]=0.0008587780939743365;xxx[707]=0.0008502331091667194;xxx[708]=0.000841773148378549;xxx[709]=0.0008333973656066964;
  xxx[710]=0.0008251049232659038;xxx[711]=0.0008168949921050283;xxx[712]=0.0008087667511241114;xxx[713]=0.0008007193874922814;xxx[714]=0.0007927520964664684;
  xxx[715]=0.0007848640813109316;xxx[716]=0.0007770545532175816;xxx[717]=0.0007693227312271008;xxx[718]=0.0007616678421508473;xxx[719]=0.0007540891204935333;
  xxx[720]=0.0007465858083766792;xxx[721]=0.0007391571554628196;xxx[722]=0.0007318024188804728;xxx[723]=0.0007245208631498506;xxx[724]=0.0007173117601093135;
  xxx[725]=0.000710174388842549;xxx[726]=0.0007031080356064829;xxx[727]=0.0006961119937599021;xxx[728]=0.0006891855636927931;xxx[729]=0.0006823280527563766;
  xxx[730]=0.0006755387751938444;xxx[731]=0.0006688170520717817;xxx[732]=0.0006621622112122764;xxx[733]=0.0006555735871256958;xxx[734]=0.0006490505209441411;
  xxx[735]=0.0006425923603555573;xxx[736]=0.0006361984595385052;xxx[737]=0.000629868179097574;xxx[738]=0.0006236008859994445;xxx[739]=0.0006173959535095834;
  xxx[740]=0.0006112527611295723;xxx[741]=0.0006051706945350532;xxx[742]=0.0005991491455142981;xxx[743]=0.000593187511907387;xxx[744]=0.0005872851975459907;
  xxx[745]=0.0005814416121937556;xxx[746]=0.0005756561714872761;xxx[747]=0.0005699282968776604;xxx[748]=0.0005642574155726738;xxx[749]=0.0005586429604794611;
  xxx[750]=0.0005530843701478336;xxx[751]=0.0005475810887141261;xxx[752]=0.0005421325658456086;xxx[753]=0.0005367382566854548;xxx[754]=0.0005313976217982529;
  xxx[755]=0.0005261101271160638;xxx[756]=0.0005208752438850122;xxx[757]=0.0005156924486124138;xxx[758]=0.0005105612230144218;xxx[759]=0.0005054810539642003;
  xxx[760]=0.0005004514334406104;xxx[761]=0.0004954718584774093;xxx[762]=0.0004905418311129505;xxx[763]=0.0004856608583403892;xxx[764]=0.0004808284520583803;
  xxx[765]=0.0004760441290222693;xxx[766]=0.0004713074107957654;xxx[767]=0.0004666178237030984;xxx[768]=0.0004619748987816513;xxx[769]=0.0004573781717350623;
  xxx[770]=0.000452827182886797;xxx[771]=0.0004483214771341776;xxx[772]=0.000443860603902874;xxx[773]=0.0004394441171018455;xxx[774]=0.0004350715750787321;
  xxx[775]=0.0004307425405756875;xxx[776]=0.0004264565806856538;xxx[777]=0.00042221326680907;xxx[778]=0.0004180121746110129;xxx[779]=0.0004138528839787616;
  xxx[780]=0.0004097349789797868;xxx[781]=0.0004056580478201565;xxx[782]=0.0004016216828033581;xxx[783]=0.0003976254802895258;xxx[784]=0.0003936690406550783;
  xxx[785]=0.0003897519682527545;xxx[786]=0.0003858738713720506;xxx[787]=0.0003820343622000467;xxx[788]=0.0003782330567826258;xxx[789]=0.000374469574986078;
  xxx[790]=0.0003707435404590882;xxx[791]=0.0003670545805950984;xxx[792]=0.0003634023264950478;xxx[793]=0.0003597864129304827;xxx[794]=0.000356206478307034;
  xxx[795]=0.0003526621646282557;xxx[796]=0.0003491531174598264;xxx[797]=0.0003456789858941049;xxx[798]=0.0003422394225150394;xxx[799]=0.0003388340833634261;
  xxx[800]=0.0003354626279025119;xxx[801]=0.000332124718983941;xxx[802]=0.0003288200228140399;xxx[803]=0.000325548208920438;xxx[804]=0.0003223089501190186;
  xxx[805]=0.0003191019224812033;xxx[806]=0.0003159268053015553;xxx[807]=0.0003127832810657107;xxx[808]=0.0003096710354186262;xxx[809]=0.0003065897571331437;
  xxx[810]=0.0003035391380788668;xxx[811]=0.0003005188731913479;xxx[812]=0.0002975286604415808;xxx[813]=0.0002945682008057998;xxx[814]=0.0002916371982355737;
  xxx[815]=0.000288735359628203;xxx[816]=0.0002858623947974087;xxx[817]=0.0002830180164443136;xxx[818]=0.000280201940128712;xxx[819]=0.0002774138842406257;
  xxx[820]=0.0002746535699721425;xxx[821]=0.0002719207212895348;xxx[822]=0.0002692150649056578;xxx[823]=0.0002665363302526181;xxx[824]=0.0002638842494547179;
  xxx[825]=0.0002612585573016675;xxx[826]=0.0002586589912220635;xxx[827]=0.0002560852912571316;xxx[828]=0.0002535372000347304;xxx[829]=0.000251014462743614;
  xxx[830]=0.0002485168271079518;xxx[831]=0.0002460440433620985;xxx[832]=0.0002435958642256188;xxx[833]=0.0002411720448785589;xxx[834]=0.0002387723429369641;
  xxx[835]=0.0002363965184286407;xxx[836]=0.0002340443337691579;xxx[837]=0.0002317155537380892;xxx[838]=0.0002294099454554917;xxx[839]=0.0002271272783586154;
  xxx[840]=0.0002248673241788482;xxx[841]=0.000222629856918889;xxx[842]=0.0002204146528301471;xxx[843]=0.0002182214903903678;xxx[844]=0.0002160501502814794;
  xxx[845]=0.0002139004153676615;xxx[846]=0.0002117720706736309;xxx[847]=0.0002096649033631454;xxx[848]=0.0002075787027177175;xxx[849]=0.0002055132601155443;
  xxx[850]=0.0002034683690106442;xxx[851]=0.0002014438249122027;xxx[852]=0.0001994394253641228;xxx[853]=0.0001974549699247794;xxx[854]=0.0001954902601469746;
  xxx[855]=0.0001935450995580938;xxx[856]=0.000191619293640457;xxx[857]=0.0001897126498118675;xxx[858]=0.0001878249774063536;xxx[859]=0.0001859560876551017;
  xxx[860]=0.0001841057936675792;xxx[861]=0.0001822739104128454;xxx[862]=0.0001804602547010481;xxx[863]=0.0001786646451651052;xxx[864]=0.0001768869022425666;
  xxx[865]=0.0001751268481576584;xxx[866]=0.0001733843069035056;xxx[867]=0.0001716591042245305;xxx[868]=0.0001699510675990275;xxx[869]=0.0001682600262219108;
  xxx[870]=0.0001665858109876332;xxx[871]=0.0001649282544732767;xxx[872]=0.0001632871909218081;xxx[873]=0.0001616624562255048;xxx[874]=0.0001600538879095432;
  xxx[875]=0.0001584613251157513;xxx[876]=0.0001568846085865224;xxx[877]=0.0001553235806488899;xxx[878]=0.0001537780851987589;xxx[879]=0.0001522479676852964;
  xxx[880]=0.0001507330750954765;xxx[881]=0.0001492332559387774;xxx[882]=0.0001477483602320336;xxx[883]=0.0001462782394844371;xxx[884]=0.000144822746682688;
  xxx[885]=0.0001433817362762932;xxx[886]=0.0001419550641630111;xxx[887]=0.0001405425876744414;xxx[888]=0.0001391441655617586;xxx[889]=0.0001377596579815859;
  xxx[890]=0.0001363889264820114;xxx[891]=0.0001350318339887429;xxx[892]=0.0001336882447914002;xxx[893]=0.0001323580245299439;xxx[894]=0.0001310410401812393;
  xxx[895]=0.0001297371600457538;xxx[896]=0.0001284462537343878;xxx[897]=0.0001271681921554341;xxx[898]=0.0001259028475016698;xxx[899]=0.0001246500932375751;
  xxx[900]=0.0001234098040866796;xxx[901]=0.0001221818560190345;xxx[902]=0.0001209661262388099;xxx[903]=0.0001197624931720147;xxx[904]=0.0001185708364543388;
  xxx[905]=0.000117391036919118;xxx[906]=0.0001162229765854152;xxx[907]=0.0001150665386462238;xxx[908]=0.0001139216074567861;xxx[909]=0.0001127880685230291;
  xxx[910]=0.0001116658084901148;xxx[911]=0.0001105547151311046;xxx[912]=0.0001094546773357366;xxx[913]=0.0001083655850993148;xxx[914]=0.000107287329511708;
  xxx[915]=0.0001062198027464587;xxx[916]=0.0001051628980500009;xxx[917]=0.0001041165097309842;xxx[918]=0.0001030805331497045;xxx[919]=0.0001020548647076406;
  xxx[920]=0.0001010394018370932;xxx[921]=0.0001000340429909296;xxx[922]=9.903868763242697e-05;xxx[923]=9.805323622522013e-05;xxx[924]=9.707759022334712e-05;
  xxx[925]=9.611165206139469e-05;xxx[926]=9.515532514474172e-05;xxx[927]=9.420851383989959e-05;xxx[928]=9.32711234649488e-05;xxx[929]=9.234306028007055e-05;
  xxx[930]=9.142423147817327e-05;xxx[931]=9.051454517561092e-05;xxx[932]=8.961391040299518e-05;xxx[933]=8.872223709609823e-05;xxx[934]=8.783943608684635e-05;
  xxx[935]=8.696541909440292e-05;xxx[936]=8.610009871634035e-05;xxx[937]=8.524338841989959e-05;xxx[938]=8.439520253333736e-05;xxx[939]=8.355545623735804e-05;
  xxx[940]=8.272406555663223e-05;xxx[941]=8.190094735139904e-05;xxx[942]=8.1086019309152e-05;xxx[943]=8.027919993640778e-05;xxx[944]=7.948040855055677e-05;
  xxx[945]=7.868956527179456e-05;xxx[946]=7.790659101513453e-05;xxx[947]=7.713140748249839e-05;xxx[948]=7.636393715488688e-05;xxx[949]=7.560410328462769e-05;
  xxx[950]=7.48518298877006e-05;xxx[951]=7.4107041736139e-05;xxx[952]=7.336966435050708e-05;xxx[953]=7.263962399245182e-05;xxx[954]=7.191684765732888e-05;
  xxx[955]=7.120126306690273e-05;xxx[956]=7.049279866211783e-05;xxx[957]=6.979138359594336e-05;xxx[958]=6.909694772628816e-05;xxx[959]=6.840942160898655e-05;
  xxx[960]=6.77287364908539e-05;xxx[961]=6.705482430281112e-05;xxx[962]=6.638761765307773e-05;xxx[963]=6.572704982043295e-05;xxx[964]=6.507305474754294e-05;
  xxx[965]=6.442556703435542e-05;xxx[966]=6.378452193155948e-05;xxx[967]=6.314985533411063e-05;xxx[968]=6.252150377482026e-05;xxx[969]=6.189940441800879e-05;
  xxx[970]=6.128349505322202e-05;xxx[971]=6.067371408901044e-05;xxx[972]=6.007000054676936e-05;xxx[973]=5.947229405464145e-05;xxx[974]=5.888053484147942e-05;
  xxx[975]=5.829466373086881e-05;xxx[976]=5.771462213521033e-05;xxx[977]=5.714035204986106e-05;xxx[978]=5.657179604733388e-05;xxx[979]=5.600889727155467e-05;
  xxx[980]=5.545159943217695e-05;xxx[981]=5.489984679895225e-05;xxx[982]=5.435358419615749e-05;xxx[983]=5.381275699707714e-05;xxx[984]=5.327731111854062e-05;
  xxx[985]=5.274719301551385e-05;xxx[986]=5.222234967574478e-05;xxx[987]=5.170272861446196e-05;xxx[988]=5.118827786912642e-05;xxx[989]=5.067894599423484e-05;
  xxx[990]=5.017468205617528e-05;xxx[991]=4.967543562813372e-05;xxx[992]=4.918115678505129e-05;xxx[993]=4.869179609863181e-05;xxx[994]=4.820730463239883e-05;
  xxx[995]=4.772763393680188e-05;xxx[996]=4.725273604437187e-05;xxx[997]=4.67825634649237e-05;xxx[998]=4.631706918080762e-05;xxx[999]=4.585620664220731e-05;
  xxx[1000]=4.539992976248485e-05;

  float* cnnd = (float*) malloc(100*Nt*Ns*Hs*sizeof(float));
  float rCutrCut = rCut*rCut;
  for(int i = 0; i < 100*Nt*Ns*Hs; i++){cnnd[i] = 0.0;}

  //MAKESURE TO NULLIFY THE CNs!!!!!!!
  //Triple Check the implementation, Triple times. Then Triple that again.
  getAlphaBeta(aOa,bOa,alphas,betas,Ns,lMax,oOsigma,oOsigma3O2);
  for(int i = 0; i < Hs; i++){
    for(int j = 0; j < Nt; j++){
      Asize = getFilteredPos(x, y, z, Apos, Hpos,typeNs, rCutrCut, i, j);
      getRsZs(x, y, z, r2,r4,r6,r8,z2,z4,z6,z8, Asize);
      getCfactors(preCoef,Asize,x,y,z,z2,z4,z6,z8,r2,r4,r6,r8,ReIm2,ReIm3,ReIm4,ReIm5,ReIm6,ReIm7,ReIm8,ReIm9, totalAN, lMax, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15, t16, t17, t18, t19, t20, t21, t22, t23, t24, t25, t26, t27, t28, t29, t30, t31, t32, t33, t34, t35, t36, t37, t38, t39, t40, t41, t42, t43, t44, t45, t46, t47, t48, t49, t50, t51, t52, t53, t54, t55, t56, t57, t58, t59, t60, t61, t62, t63, t64, t65, t66, t67, t68, t69, t70, t71, t72, t73, t74, t75, t76, t77, t78, t79, t80, t81, t82, t83, t84, t85, t86, t87, t88, t89, t90, t91, t92, t93, t94, t95, t96, t97, t98, t99);
      getC(cnnd,preCoef,x,y,z,r2,bOa,aOa,exes,totalAN,Asize,Ns,Nt, lMax, i, j, Nx2, Nx3, Nx4, Nx5, Nx6, Nx7, Nx8, Nx9, Nx10, Nx11, Nx12, Nx13, Nx14, Nx15, Nx16, Nx17, Nx18, Nx19, Nx20, Nx21, Nx22, Nx23, Nx24, Nx25, Nx26, Nx27, Nx28, Nx29, Nx30, Nx31, Nx32, Nx33, Nx34, Nx35, Nx36, Nx37, Nx38, Nx39, Nx40, Nx41, Nx42, Nx43, Nx44, Nx45, Nx46, Nx47, Nx48, Nx49, Nx50, Nx51, Nx52, Nx53, Nx54, Nx55, Nx56, Nx57, Nx58, Nx59, Nx60, Nx61, Nx62, Nx63, Nx64, Nx65, Nx66, Nx67, Nx68, Nx69, Nx70, Nx71, Nx72, Nx73, Nx74, Nx75, Nx76, Nx77, Nx78, Nx79, Nx80, Nx81, Nx82, Nx83, Nx84, Nx85, Nx86, Nx87, Nx88, Nx89, Nx90, Nx91, Nx92, Nx93, Nx94, Nx95, Nx96, Nx97, Nx98, Nx99, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15, t16, t17, t18, t19, t20, t21, t22, t23, t24, t25, t26, t27, t28, t29, t30, t31, t32, t33, t34, t35, t36, t37, t38, t39, t40, t41, t42, t43, t44, t45, t46, t47, t48, t49, t50, t51, t52, t53, t54, t55, t56, t57, t58, t59, t60, t61, t62, t63, t64, t65, t66, t67, t68, t69, t70, t71, t72, t73, t74, t75, t76, t77, t78, t79, t80, t81, t82, t83, t84, t85, t86, t87, t88, t89, t90, t91, t92, t93, t94, t95, t96, t97, t98, t99,xxx);
    }
  }
  free(x);
  free(y);
  free(z);
  free(z2);
  free(z4);
  free(z6);
  free(z8);
  free(r2);
  free(r4);
  free(r6);
  free(r8);
  free(ReIm2);
  free(ReIm3);
  free(ReIm4);
  free(ReIm5);
  free(ReIm6);
  free(ReIm7);
  free(ReIm8);
  free(ReIm9);
  free(exes);
  free(preCoef);
  free(bOa);
  free(aOa);

//  float* soapMat = (float*) malloc(Hs*3*(Ns*(Ns+1))/2*(lMax+1)*sizeof(float));// 3 -> aa, ab, bb
  getP(c, cnnd, Ns, Nt, Hs, lMax);
  free(cnnd);
  return 0;
}
