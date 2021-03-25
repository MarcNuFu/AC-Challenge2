#include <stdio.h>
#include <stdlib.h>

// Variable used to generate pseudo-random numbers
unsigned int seed;

// Function to generate pseudo-random numbers
int myRandom() {
  seed = (214013*seed+2531011);
  return (seed>>13);
}


void LongNumInit( char *L, unsigned N )
{
  for ( int i=0; i< N; i++ ) 
  {
    L[i] = myRandom() % 10;  // digito decimal
  }
}

void LongNumPrint( char *L, unsigned N, char *Name )
{
  printf("%s:", Name);
  for ( int i=N; i>0; i-- ) 
  {
    printf("%d", L[i-1]);
  }
  printf("\n");
}


void LongNumSet( char *L, unsigned N, char digit )
{
  for ( int i=0; i< N; i++ ) 
  {
    L[i] = digit;
  }
}


void LongNumCopy( char *Vin, char *Vout, unsigned N )
{
  for ( int i=0; i< N; i++ ) 
  {
    Vout[i] = Vin[i];
  }
}

char LongNumAddition( char *Vin1, char *Vin2, char *Vout, unsigned N, int offset)
{
  char CARRY = 0;
  for ( int i=offset; i< (N+offset); i++ ) 
  {
    char R = Vin1[i] + Vin2[i] + CARRY;
    if ( R <= 9 )
    {
      Vout[i] = R; CARRY = 0;
    }
    else
    {
      Vout[i] = R-10; CARRY = 1;
    }
  }
  return CARRY;
}


char LongNumAddDigit( char *V, char digit, unsigned N )
{
  int i=0;
  char R = V[0] + digit;
  if (R < 10)
  {
    V[0] = R; return 0; // No carry
  }
  
  V[0] = R-10;
  // add carry, maybe iteratively for all digits
  char CARRY = 1;
  i = 1;
  while ( CARRY && i < N ) 
  {
    if ( V[i] < 9 )
    {
      V[i] = V[i] + 1; CARRY = 0;
    }
    else 
    {
      V[i] = 0; i++;  // CARRY remains set to 1
    }
  }
  return CARRY;
}


char LongNumHorizAdd( char *Vin, char *Vout, unsigned N )
{
  char CARRY = 0;
  LongNumSet ( Vout, N, 0 );
  for ( int i=0; i< N; i++ ) 
  {
    LongNumAddDigit ( Vout, Vin[i], N );
  }
  return 0; // CARRY can never be set
}

char LongNumConstMult(unsigned char *Vout, unsigned N, unsigned char *Vin, char digit, int offset)
{
  for ( int i=offset; i< (offset+N); i++ ) 
  {
    Vout[i] = Vin[i-offset] * digit; // max number = 9*9 (7 bytes)
  }
  return 0; 
}

char LongNumAdditionV2(unsigned char * __restrict Vin1,unsigned int *Vout, unsigned N, int offset, char digit)
{
  for ( int i=offset; i< (N+offset); i++ ) 
  {
    Vout[i] += Vin1[i-offset] * digit;
  }
  return 0;
}


char LongNumAdditionV3(unsigned char * __restrict Vin1,unsigned char *Vout, unsigned N, int offsetUnroll, int offsetVin1, char digit)
{
  for ( int i=(offsetUnroll+offsetVin1); i< (N+offsetUnroll); i++ ) 
  {
    Vout[i] += Vin1[i-offsetUnroll] * digit;
  }

  return 0;
}

char LongNumAdditionV4(unsigned char * __restrict Vin1,unsigned char *Vout, unsigned N, int offsetUnroll, int offsetVin1, char digit)
{
  for ( int i=(offsetUnroll+offsetVin1); i< (N+offsetUnroll); i++ ) 
  {
    Vout[i] = Vin1[i-offsetUnroll] * digit;
  }
  Vout[N+offsetUnroll] = 0;
  Vout[N+offsetUnroll+1] = 0;
  
  return 0;
}

char Sum3(unsigned char * __restrict Vin, unsigned char * __restrict Vin2, unsigned char * __restrict Vin3, unsigned int *Vout, unsigned N, int offset)
{
  for ( int i=offset; i< (N+offset); i++ ) 
  {
    Vout[i] += Vin[i-offset] + Vin2[i-offset] + Vin3[i-offset];
  }
  return 0;
}

void LongNumSetINT( int *L, unsigned N, int digit )
{
  for ( int i=0; i< N; i++ ) 
  {
    L[i] = digit;
  }
}

char reduceNumbers(unsigned int *Vin, unsigned char *Vout, unsigned N)
{
  unsigned int CARRY = 0;
  for ( int i=0; i< N; i++ ) 
  {
    unsigned int R = Vin[i] + CARRY;
    CARRY = R/10;
    R = R - ((CARRY << 3) + (CARRY << 1));
    Vout[i] = R;
  }
  return CARRY;
}

void LongNumMultiply( unsigned char *Vin1, unsigned char *Vin2, unsigned char *VoutH, unsigned char *VoutL,  unsigned N)
{   
  unsigned Vin10index = 0;
  unsigned Vin20index = 0;
  
  while(Vin1[Vin10index] == 0)
    Vin10index++;
  while(Vin2[Vin20index] == 0)
    Vin20index++;
  
  unsigned restoDiv9 = (N-Vin20index)%9;
  
  unsigned int *RES = (unsigned int*) malloc( 2*N*sizeof(unsigned int) ); 

  LongNumSetINT  ( RES, 2*N, 0 );    // Set RES to 0
  
  for ( int i=Vin20index; i<(restoDiv9+Vin20index); i++ ) // Se ejecutan las iteraciones necesarias para poder ejecutar 3 veces la instruccion LongNumAdditionV3 en el siguiente bucle
  {
    LongNumAdditionV2 ( Vin1, RES, N, i, Vin2[i]); // Vin1*Vin2[i] -> RES (Desplazamiento ya aplicado)
  }

  unsigned char *TEMP = (unsigned char*) malloc( (N+8)*sizeof(unsigned char) );
  LongNumSet  ( TEMP, (N+5), 0 ); // Set TEMP to 0
  
  unsigned char *TEMP2 = (unsigned char*) malloc( (N+8)*sizeof(unsigned char) );
  LongNumSet  ( TEMP2, (N+5), 0 ); // Set TEMP2 to 0
  
  unsigned char *TEMP3 = (unsigned char*) malloc( (N+8)*sizeof(unsigned char) );
  LongNumSet  ( TEMP3, (N+5), 0 ); // Set TEMP3 to 0
  
  for ( int i=(Vin20index+restoDiv9); i<N; i+=9 ) // Numero max Vin1[i]*Vin2[i] = 81, se puede almacenar el valor 3 sumas de Vin1*Vin2 en un unsigned char 255/81 -> 3
  { 
    LongNumAdditionV4 ( Vin1, TEMP, N, 0, Vin10index, Vin2[i]);   //  Vin1*Vin2[i]   -> TEMP 
    LongNumAdditionV3 ( Vin1, TEMP, N, 1, Vin10index, Vin2[i+1]); //  Vin1*Vin2[i+1] -> TEMP
    LongNumAdditionV3 ( Vin1, TEMP, N, 2, Vin10index, Vin2[i+2]); //  Vin1*Vin2[i+2] -> TEMP 
    
    LongNumAdditionV4 ( Vin1, TEMP2, N, 3, Vin10index, Vin2[i+3]);  //  Vin1*Vin2[i+3] -> TEMP 
    LongNumAdditionV3 ( Vin1, TEMP2, N, 4, Vin10index, Vin2[i+4]);  //  Vin1*Vin2[i+4] -> TEMP 
    LongNumAdditionV3 ( Vin1, TEMP2, N, 5, Vin10index, Vin2[i+5]);  //  Vin1*Vin2[i+5] -> TEMP
    
    LongNumAdditionV4 ( Vin1, TEMP3, N, 6, Vin10index, Vin2[i+6]);  //  Vin1*Vin2[i+6] -> TEMP 
    LongNumAdditionV3 ( Vin1, TEMP3, N, 7, Vin10index, Vin2[i+7]);  //  Vin1*Vin2[i+7] -> TEMP
    LongNumAdditionV3 ( Vin1, TEMP3, N, 8, Vin10index, Vin2[i+8]);  //  Vin1*Vin2[i+8] -> TEMP 
    
    Sum3 ( TEMP, TEMP2, TEMP3, RES, (N+8), i); // TEMP + TEMP1 + TEMP2 + RES -> RES (Desplazamiento ya aplicado) RES es tipo int, se puede alamcenar el valor de 53024287 sumas de Vin1*Vin2
  }
  
  unsigned char *RES2 = (unsigned char*) malloc( 2*N*sizeof(unsigned char) );
  LongNumSet  ( RES2, 2*N, 0 );
  reduceNumbers(RES, RES2, 2*N); // Se obtiene el carry de cada numero y se suma al siguiente, se almacena en RES2 (unsigned char)
  
  
  // Result goes to VoutH-VoutL
  LongNumCopy ( RES2,   VoutL, N );  // Copy RES2   -> VoutL
  LongNumCopy ( RES2+N, VoutH, N );  // Copy RES2+N -> VoutH
  
  free(TEMP); free(TEMP2); free(TEMP3);
  free(RES); free(RES2);
}


int main (int argc, char **argv)
{
  int i, sum1, sum2, sum3, N=10000, Rep=50; // El numero maximo para N antes de la aparicion de posibles fallos es ((2^32)-1)/81 = 53024287, 81 = max numero posible de una multiplicacion de 2 numeros de 1 cifra
  seed = 12345;

  // obtain parameters at run time
  if (argc>1) { N    = atoi(argv[1]); }
  if (argc>2) { Rep  = atoi(argv[2]); }

  printf("Challenge #2: Vector size is %d. Repeat %d times\n", N, Rep);

  // Create Long Nums
  unsigned char *V1= (unsigned char*) malloc( N*sizeof(unsigned char) );
  unsigned char *V2= (unsigned char*) malloc( N*sizeof(unsigned char) );
  unsigned char *V3= (unsigned char*) malloc( N*sizeof(unsigned char) );
  unsigned char *V4= (unsigned char*) malloc( N*sizeof(unsigned char) );
 
  LongNumInit ( V1, N ); LongNumInit ( V2, N ); LongNumInit ( V3, N );

  // Repeat
  for (i=0; i<Rep; i++)
  {
    LongNumAddition ( V1, V2, V4, N, 0 );
    LongNumMultiply ( V3, V4, V2, V1, N);
    LongNumHorizAdd ( V1, V2, N );
    LongNumAddDigit ( V3, V2[0], N );
  }

  // Print last 32 digits of Long Numbers
  LongNumPrint( V1, 32, "V1" );
  LongNumPrint( V2, 32, "V2" );
  LongNumPrint( V3, 32, "V3" );
  LongNumPrint( V4, 32, "V4" );

  free(V1); free(V2); free(V3); free(V4);
  return 0;
}
