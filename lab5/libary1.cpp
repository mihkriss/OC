#include <cmath>
extern "C" float SinIntegral(float A, float B, float e); //чтобы были видны глобальные функции() находящейся в том же файле, где происходит объявление глобальной переменной
extern "C" float Derivative(float A, float deltaX);

float SinIntegral(float A, float B, float e){
    float Integral = 0;
    for(float i = A; i<B; i+=e){
        Integral += sinf(i) *e;
    }
    return Integral;
}

float Derivative(float A, float deltaX){
    return (cosf (A + deltaX) - cosf(A))/deltaX;
}