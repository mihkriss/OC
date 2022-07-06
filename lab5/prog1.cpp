#include <iostream>
using namespace std;
extern "C" float SinIntegral(float A, float B, float e);
extern "C" float Derivative(float A, float deltaX);

int main()
{
    int cmd;
    cout << "Пожалуйста, введите флаг:\n";
    while (scanf("%d",&cmd) > 0 ){
        if (cmd == 1){
            cout<<"Пожалуйста, введите ваши даты:\n";
            float A, B, e;
            cin>>A>>B>>e;
            cout<<"SinIntegral("<< A<<","<<B<<","<< e<<")="<<SinIntegral(A, B, e)<< endl;
        }
        else if (cmd == 2){
            cout<<"Пожалуйста, введите ваши даты:\n";
            float A, deltaX;
            cin>>A>>deltaX;
            cout<< "Derivative("<<A<<","<<deltaX<<")="<<Derivative(A,deltaX)<<endl;
        }
    }
    return 0;
}
