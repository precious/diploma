#include <stdio.h>
#include <stdlib.h>

#define SIZE 12

typedef float v4sf __attribute__ ((vector_size (16)));
#define AR(t) ((float*)&t)

float *mv_mult(float mat[SIZE][SIZE], float vec[SIZE]) {
  static float ret[SIZE];
  int i, j;
  v4sf m, v, r, r0={0.,0.,0.,0.};
    for (i = 0; i < SIZE; i++) {
        r = r0;
        for (j = 0; j < SIZE; j += 4) {
            r += *(v4sf*)&mat[i][j] *  *(v4sf*)&vec[j];
            /*m = *(v4sf*)&mat[i][j]; 
            v = *(v4sf*)&vec[j];
            v = m * v;
            r = r + v;*/
        }
        ret[i] = AR(r)[0] + AR(r)[1] + AR(r)[2] + AR(r)[3];
    }
    return ret;
}
     
int main(){
      
 v4sf a={1,2,3,4}, b={5,6,7,8}, c;
 float mat[SIZE][SIZE], vec[SIZE], *res;
 int i,j;
 for(i=0;i<SIZE;i++) for(j=0;j<SIZE;j++) mat[i][j]=0.+i+j;
 for(i=0;i<SIZE;i++) vec[i]=1.;

 c = a * b;
 printf("%f, %f, %f, %f\n", AR(c)[0], AR(c)[1], AR(c)[2], AR(c)[3] );
 printf("size = %d\n",sizeof(v4sf));

 res = mv_mult( mat, vec);
 for(i=0;i<SIZE;i++) printf("%d  %f \n", i, res[i]);

 return 0;
}
/*  Visual C
__declspec(align(16)) float x[] = {1.,2.,3.,4.};

float *mv_mult(float mat[SIZE][SIZE], float vec[SIZE]) {
    static float ret[SIZE];
    float temp[4];
    int i, j;
    __m128 m, v, r;

    for (i = 0; i < SIZE; i++) {
        r = _mm_xor_ps(r, r);

        for (j = 0; j < SIZE; j += 4) {
            m = _mm_loadu_ps(&mat[i][j]);
            v = _mm_loadu_ps(&vec[j]);
            v = _mm_mul_ps(m, v);
            r = _mm_add_ps(r, v);
        }

        _mm_storeu_ps(temp, r);
        ret[i] = temp[0] + temp[1] + temp[2] + temp[3];
    }

    return ret;
}
*/
