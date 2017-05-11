/*
    Author : Ahmed Hdeawy
    at  11-5-2017
*/

#include <iostream>
#include <sstream>
#include <math.h>

using namespace std;

void shiftLeft (int myarray[], int size, int shiftBy);
void concatenateArray(int arr1[], int arr2[], int newArr[]);
void permutationChoice2(int key[], int newArr[]);
void permutationChoice1Left(int key[], int newArr[]);
void permutationChoice1Right(int key[], int newArr[]);
void generateKey(int key[], int keys[16][48]);
void initial_permutation(int M[], int IP[]);
void decimalToBinary(int n, int rem[]);
void Expand32into48(int original[], int expanded[]);
void xorArray(int arr1[], int arr2[], int result[], int size);
int combine(int a, int b);
int getNumberinS1(int row, int col);
int getSBoxNumber(int row, int column, int s);
void permutation_F_function(int sBox[], int newArr[]);
void concatenateLeftAndRight(int arr1[], int arr2[], int newArr[]);
void reversePermutation(int original[], int inversed[]);


int main()
{

    int i, j, k, c, f;

   /*================== Generate Key =====================*/
    // 1- 64 bit key (including parity-check bits)
    int key[64] = {0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1};
    int keys[16][48];
    generateKey(key, keys);

    // the array keys[16][48]  contains 16 keys

    /*================== Message ===========================*/
    int IP[64], Ln[32], Rn[32];
    int Message[64] = {0,0,0,0,0,0,0,1,0,0,1,0,0,0,1,1,0,1,0,0,0,1,0,1,0,1,1,0,0,1,1,1,1,0,0,0,1,0,0,1,1,0,1,0,1,0,1,1,1,1,0,0,1,1,0,1,1,1,1,0,1,1,1,1};

    initial_permutation(Message, IP);

    // divide IP into  L0 R0
    for(i = 0; i < 32; i++)  { Ln[i] = IP[i]; }   // L0
    for(i = 32, j = 0; i < 64 && j < 32; i++, j++) { Rn[j] = IP[i]; }  // R0

    int sBoxAfter[32]; int finalSBox[32];
    int Left16AndRight16[64];

    for(i = 0; i < 16; i++)
    {

        // Store Ln Once
        int temp_Ln[32] = {0};
        for( k = 0; k < 32; k++) { temp_Ln[k] = Ln[k]; }

        int temp_Rn[32] = {0};
        for( k = 0; k < 32; k++) { temp_Rn[k] = Rn[k]; }


        // 1- Left Side  L(i) = R(i-1)
        for( k = 0; k < 32; k++) { Ln[k] = temp_Rn[k]; }  // L1 = R0, L2 = R1  ......etc

        // 2- Right Side [ Li-1 + f(Ri-1, Ki) ]

        // We calculate E(R0) from R0
        int E_R[48];
        Expand32into48(Rn, E_R);  // E_R[48]

        // Get Ki fro Keys[16][48]
        int Kn[48] = {0};
        for( j = 0; j < 48; j++){ Kn[j] = keys[i][j]; }


        // we XOR the output E(Rn-1) with the key Kn:
        int xored[48];
        xorArray(E_R, Kn, xored, 48);   // Now  xored[48]


        // We now have 48 bits, or eight groups of six bits.
        for(k = 0, c = 0; k < 48 && c < 32; k = k+6, c = c+4)
        {
            int rowNumber = 0;
            rowNumber = (pow(2, 0) * xored[k+5]) + (pow(2, 1) * xored[k]);

            int columnNumber = 0;
            columnNumber = (pow(2, 0) * xored[k+4]) + (pow(2, 1) * xored[k+3]) + (pow(2, 2) * xored[k+2]) + (pow(2, 3) * xored[k+1]);

            int BoxNumber = getSBoxNumber(rowNumber, columnNumber, k); // 5

            // get decimal from binary BoxNumber
            int _4bit[4] = {0};
            decimalToBinary(BoxNumber, _4bit);  // 5 == 1010 [ 4 3 2 1 ]

            sBoxAfter[c] = _4bit[3];    sBoxAfter[c+1] = _4bit[2];   // sBoxAfter[0] = _4bit[3];
            sBoxAfter[c+2] = _4bit[1];  sBoxAfter[c+3] = _4bit[0];     // sBoxAfter[3] = _4bit[0];

        }

        // The final stage in the calculation of f is to do a permutation P of the S-box output to obtain the final value of f:
        // f = P(S1(B1)S2(B2)...S8(B8))

        permutation_F_function(sBoxAfter, finalSBox);

        // Now Final function f  is finalSBox array

        // Finishing Right Side  Ri = Li-1 + finalSBox

        //int xoredRightSide[32];
        xorArray(temp_Ln, finalSBox, Rn, 32);

        if(i == 15)
        {
                concatenateLeftAndRight(Ln, Rn, Left16AndRight16);
        }
    }

    for(j = 0; j < 64; j++)
    {
        cout << Left16AndRight16[j];
    }
    cout << endl << endl;

    /*======= After this Loop  we have [ Left16AndRight16 ] Array that contains 64 bit */
    // Now Do inverse permutation

    int cipher64Bit[64];
    reversePermutation(Left16AndRight16, cipher64Bit);

    // Now The output result is a 64-bit ciphertext!

     for(j = 0; j < 64; j++)
    {
        cout << cipher64Bit[j];
    }

    return 0;
}
//functions

int getNumberinS1(int row, int col)
{
    int S1[4][16] = {14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7, 0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8, 4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0, 15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13};

    return S1[row][col];
}

int combine(int a, int b) {
   int times = 1;
   while (times <= b)
      times *= 10;

   int result = a*times + b;
   if(result == 0 || result == 1)
   {
       return result * 10;
   }else{
        return result;
   }

}

void decimalToBinary(int n, int rem[])
{
    int num = n;
    int i = 0;
    while (num != 0)
    {
        //cout << num << " - " << rem[i] << "-- ";
        rem[i] = num % 2;
        num = num / 2;
        i++;
    }
}


void Expand32into48(int original[], int expanded[])
{
    int x[48] = { original[31], original[0], original[1], original[2], original[3], original[4], original[3], original[4], original[5], original[6], original[7], original[8], original[7], original[8], original[9], original[10], original[11], original[12], original[11], original[12], original[13], original[14], original[15], original[16], original[15], original[16], original[17], original[18], original[19], original[20], original[19], original[20], original[21], original[22], original[23], original[24], original[23], original[24], original[25], original[26], original[27], original[28], original[27], original[28], original[29], original[30], original[31], original[0] };

        for(int i=0; i<48; i++)
        {
            expanded[i] = x[i];
        }

}

void xorArray(int arr1[], int arr2[], int result[], int size)
{
    for(int i = 0; i < size; i++)
    {
        if(arr1[i] == arr2[i])
        {
            result[i] = 0;
        }else{
            result[i] = 1;
        }
    }
}


int getSBoxNumber(int row, int column, int s)
{
    int S1[4][16] = { 14, 4,  13,  1,   2, 15,  11,  8,   3, 10,   6, 12,   5,  9,   0,  7, 0, 15,   7,  4,  14,  2,  13,  1,  10,  6,  12, 11,   9,  5,   3,  8, 4,  1,  14,  8,  13,  6,   2, 11,  15, 12,   9,  7,   3, 10,   5,  0, 15, 12,   8,  2,   4,  9,   1,  7,   5, 11,   3, 14,  10,  0,   6, 13  };
    int S2[4][16] = { 15,  1,   8, 14,   6, 11,   3,  4,   9,  7,   2, 13,  12,  0,   5, 10, 3, 13,   4  ,7,  15,  2  , 8, 14 , 12 , 0  , 1, 10  , 6,  9  ,11,  5, 0 ,14 ,  7 ,11 , 10 , 4 , 13 , 1  , 5,  8 , 12 , 6 ,  9 , 3 ,  2 ,15, 13,  8  ,10,  1  , 3 ,15,   4  ,2,  11  ,6,   7 ,12,   0  ,5,  14  ,9 };
    int S3[4][16] = { 10,  0  , 9, 14   ,6,  3  ,15,  5   ,1, 13  ,12,  7 , 11,  4  , 2,  8,
                      13,  7 ,  0 , 9  , 3 , 4 ,  6 ,10  , 2 , 8  , 5 ,14  ,12 ,11 , 15 , 1,
                      13 , 6,   4 , 9 ,  8 ,15,   3  ,0 , 11 , 1 ,  2 ,12 ,  5 ,10,  14,  7,
                      1, 10,  13  ,0 ,  6  ,9,   8  ,7,   4 ,15 , 14  ,3 , 11 , 5,   2 ,12 };
    int S4[4][16] = { 7, 13  ,14,  3   ,0,  6   ,9, 10   ,1,  2   ,8,  5  ,11, 12   ,4, 15,
                     13,  8  ,11 , 5  , 6 ,15  , 0 , 3  , 4 , 7  , 2 ,12  , 1 ,10  ,14  ,9,
                     10 , 6 ,  9 , 0 , 12 ,11 ,  7 ,13 , 15  ,1 ,  3 ,14 ,  5 , 2  , 8  ,4,
                      3 ,15,   0  ,6,  10  ,1,  13 , 8,   9  ,4,   5 ,11,  12  ,7 ,  2 ,14 };
    int S5[4][16] = { 2, 12,   4,  1,   7 ,10  ,11,  6   ,8 , 5   ,3, 15  ,13 , 0  ,14,  9,
                     14, 11  , 2, 12  , 4,  7  ,13 , 1  , 5 , 0  ,15 ,10  , 3 , 9  , 8 , 6,
                      4 , 2 ,  1 ,11 , 10 ,13  , 7  ,8 , 15 , 9 , 12 , 5 ,  6  ,3 ,  0 ,14,
                     11  ,8,  12  ,7,   1 ,14 ,  2 ,13,   6 ,15,   0  ,9,  10  ,4,   5 , 3 };
    int S6[4][16] = { 12,  1  ,10, 15  , 9,  2  , 6,  8  , 0, 13  , 3,  4  ,14,  7   ,5, 11,
                     10, 15  , 4  ,2  , 7 ,12  , 9 , 5  , 6 , 1  ,13 ,14  , 0 ,11   ,3  ,8,
                      9, 14 , 15  ,5 ,  2 , 8 , 12  ,3 ,  7  ,0  , 4 ,10 ,  1 ,13  ,11  ,6,
                      4 , 3,   2 ,12,   9  ,5,  15 ,10,  11 ,14 ,  1  ,7,   6  ,0 ,  8 ,13 };
    int S7[4][16] = { 4, 11   ,2 ,14  ,15,  0   ,8 ,13   ,3, 12   ,9,  7   ,5, 10   ,6,  1,
                     13 , 0  ,11,  7  , 4 , 9  , 1, 10  ,14 , 3  , 5 ,12  , 2 ,15  , 8 , 6,
                      1 , 4 , 11 ,13 , 12  ,3 ,  7 ,14 , 10 ,15 ,  6  ,8 ,  0  ,5 ,  9  ,2,
                      6 ,11,  13  ,8,   1  ,4,  10  ,7,   9  ,5,   0 ,15,  14  ,2,   3 ,12 };
    int S8[4][16] = { 13,  2  , 8,  4  , 6, 15  ,11 , 1  ,10 , 9  , 3, 14  , 5,  0  ,12,  7,
                      1 ,15  ,13  ,8  ,10 , 3  , 7 , 4  ,12 , 5  , 6 ,11  , 0 ,14  , 9 , 2,
                      7 ,11 ,  4  ,1  , 9 ,12 , 14  ,2 ,  0  ,6 , 10 ,13 , 15  ,3 ,  5 , 8,
                      2  ,1,  14  ,7 ,  4 ,10,   8 ,13,  15 ,12,   9  ,0,   3  ,5,   6 ,11 };



        if(s == 0)
        {
            return S1[row][column];
        }else if( s == 6)
        {
            return S2[row][column];
        }else if( s == 12)
        {
            return S3[row][column];
        }else if( s == 18)
        {
            return S4[row][column];
        }else if( s == 24)
        {
            return S5[row][column];
        }else if( s == 30)
        {
            return S6[row][column];
        }else if( s == 36)
        {
            return S7[row][column];
        }else if( s == 42)
        {
            return S8[row][column];
        }

}


void permutation_F_function(int sBox[], int newArr[])
{

    int x[32] = { sBox[15], sBox[6], sBox[19], sBox[20], sBox[28], sBox[11], sBox[27], sBox[16], sBox[0], sBox[14], sBox[22], sBox[26], sBox[4], sBox[17], sBox[30], sBox[9], sBox[1], sBox[7], sBox[23], sBox[13], sBox[31], sBox[26], sBox[2], sBox[8], sBox[18], sBox[12], sBox[29], sBox[5], sBox[21], sBox[10], sBox[3], sBox[24] };

        for(int i=0; i < 32; i++)
        {
            newArr[i] = x[i];
        }

}

void reversePermutation(int original[], int inversed[])
{
    int x[64] = { original[39], original[7], original[47], original[15], original[55], original[23], original[63], original[31], original[38], original[6], original[46], original[14], original[54], original[22], original[62], original[30], original[37], original[5], original[45], original[13], original[53], original[21], original[61], original[29], original[36], original[4], original[44], original[12], original[52], original[20], original[60], original[28], original[35], original[3], original[43], original[11], original[51], original[19], original[59], original[27], original[34], original[2], original[42], original[10], original[50], original[18], original[58], original[26], original[33], original[1], original[41], original[9], original[49], original[17], original[57], original[25], original[32], original[0], original[40], original[8], original[48], original[16], original[56], original[24] };
    for(int i=0; i < 64; i++)
        {
            inversed[i] = x[i];
        }
}

void concatenateLeftAndRight(int arr1[], int arr2[], int newArr[])
{
    int i, j;
    for(i = 0; i < 32; i++)
    {
        newArr[i] = arr1[i];
    }
    for(i = 32, j = 0; i < 64 && j < 32; i++, j++)
    {
        newArr[i] = arr2[j];
    }
}

void generateKey(int key[], int keys[16][48])
{
    int i, j, k;
    int C[28], D[28], concate[56], C_D[48];

    // 1- permuted Choice-1
    permutationChoice1Left(key, C);  // C == Left  == 28 bit
    permutationChoice1Right(key, D); // D == Right == 28 bit

    // 2- Left Shift
    for(i = 1, k = 0; i <= 16 && k < 16; i++, k++)
    {
        if(i==1 || i==2 || i==9 || i==16)
        {
            shiftLeft(C, 28, 1);
            shiftLeft(D, 28, 1);
        }else
        {
            shiftLeft(C, 28, 2);
            shiftLeft(D, 28, 2);
        }

        // 3- Concate array will be 56 bit
        concatenateArray(C, D, concate); //concate[56]

        // 4- permuted Choice-2
        permutationChoice2(concate, C_D);  // C_D[48]

        // 5- Store Key by key in k[16][16] array
        for(j = 0; j < 48; j++)
        {
            keys[k][j] = C_D[j];
        }

        /*=========== Now we have 16 keys ===========*/
    }

}

void initial_permutation(int M[], int IP[])
{
    int x[64] = {M[57], M[49], M[41], M[33], M[25], M[17], M[9], M[1], M[59], M[51], M[43], M[35], M[27], M[19], M[11], M[3], M[61], M[53], M[45], M[37], M[29], M[21], M[13], M[5], M[63], M[55], M[47], M[39], M[31], M[23], M[15], M[7], M[56], M[48], M[40], M[32], M[24], M[16], M[8], M[0], M[58], M[50], M[42], M[34], M[26], M[18], M[10], M[2], M[60], M[52], M[44], M[36], M[28], M[20], M[12], M[4], M[62], M[54], M[46], M[38], M[30], M[22], M[14], M[6]};

        for(int i=0; i < 64; i++)
        {
            IP[i] = x[i];
        }
}

void shiftLeft(int myarray[], int size, int shiftBy)
{
    if(shiftBy == 1)
    {
        int temp = myarray[0];
        for (int i=0; i<(size - 1); i++)
        {
            myarray[i] = myarray[i+1] ;
        }
        myarray[size - 1] = temp;
        //cout << myarray[size - 1] << endl;
    }
    else if(shiftBy == 2)
    {
        int temp0 = myarray[0];
        //cout << myarray[0] << endl;
        int temp1 = myarray[1];

        for (int i = 0; i < (size - 2); i = i+2)
        {
            myarray[i] = myarray[i+2];
            //cout << myarray[i] << " - " << myarray[i+2] << endl;
            myarray[i+1] = myarray[i+3];
        }
        myarray[size - 1] = temp1;
        myarray[size - 2] = temp0;
    }



}

void concatenateArray(int arr1[], int arr2[], int newArr[])
{
    int i, j;
    for(i = 0; i < 28; i++)
    {
        newArr[i] = arr1[i];
    }
    for(i = 28, j=0; i < 56 && j < 28; i++, j++)
    {
        newArr[i] = arr2[j];
    }
}

void permutationChoice2(int key[], int newArr[])
{
    int x[48] = {key[13], key[16], key[10], key[23], key[0], key[4], key[2], key[27], key[14], key[5], key[20], key[9], key[22], key[18], key[11], key[3], key[25], key[7], key[15], key[6], key[26], key[19], key[12], key[1], key[40], key[51], key[30], key[36], key[46], key[54], key[29], key[39], key[50], key[44], key[32], key[47], key[43], key[48], key[38], key[55], key[33], key[52], key[45], key[41], key[49], key[35], key[28], key[31] };

        for(int i=0; i<48; i++)
        {
            newArr[i] = x[i];
        }
}

void permutationChoice1Left(int key[], int newArr[])
{
    int x[28] = {key[56], key[48], key[40], key[32], key[24], key[16], key[8], key[0], key[57], key[49], key[41], key[33], key[25], key[17], key[9], key[1], key[58], key[50], key[42], key[34], key[26], key[18], key[10], key[2], key[59], key[51], key[43], key[35]};

        for(int i=0; i<28; i++)
        {
            newArr[i] = x[i];
        }
}

void permutationChoice1Right(int key[], int newArr[])
{
    int y[28] = {key[62], key[54], key[46], key[38], key[30], key[22], key[14], key[6], key[61], key[53], key[45], key[37], key[29], key[21], key[13], key[5], key[60], key[52], key[44], key[36], key[28], key[20], key[12], key[4], key[27], key[19], key[11], key[3]};

        for(int i=0; i<28; i++)
        {
            newArr[i] = y[i];
        }
}



