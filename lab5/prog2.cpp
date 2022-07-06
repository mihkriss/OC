#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <dlfcn.h>

using namespace std;

int main()
{
    void* adress = NULL;//адресс для доступа к библиотеке
    
    //указатели на функции 1 и 2
    float (*SinIntegral)(float A, float B, float e); //объявляем указатели на библиотеки
    float (*Derivative)(float A, float deltaX);
    
    const char* libeary_mas[]={"libd1.so", "libd2.so"}; //массив из путей к библиотекам
    int curlib; 
    int statl;
    cout<<"Войдите в стартовую библиотеку:"<<endl; //вводим стартовую библиотеку
    cout << '\t' << "1 для первой библиотеки" <<endl;
    cout << '\t' << "2 для второй библиотеки" <<endl;
    cin>>statl;
    bool cmd = 1; // для считывание текущей библиотеки, чтобы нормально сделать
    while(cmd){
        if (statl==1){
            curlib = 0; //индекс массива 
            cmd = 0;
        }
        else if(statl==2){
            curlib=1;
            cmd=0;
        }
        else{
            cout<<"Вы ввели неправильный номер. Войти снова!"<<endl;
            cin>>statl;
        }
    }
    adress = dlopen(libeary_mas[curlib],RTLD_LAZY);//RTLD_LAZY выполняется поиск только тех символов, на которые есть ссылки из кода
    if (!adress){
        cout<<"Ошибка";
        exit(EXIT_FAILURE);
    }
    
    //возвращаем адрес функции из памяти библиотеки 
    SinIntegral = (float(*)(float, float, float))dlsym(adress,"SinIntegral");//dlsym присваивает указателю на функцию, объявленному в начале, ее адрес в библиотеке
    Derivative = (float(*)(float, float))dlsym(adress, "Derivative");
    int command;
    cout<<"Пожалуйста, прочтите следующие правила, прежде чем вводить команду";
    cout << '\t' << "0 за изменение контракта;" << std:: endl;
    cout << '\t' << "1 для расчета the SinIntegral; " << std:: endl;
    cout << '\t' << "2 для расчета the Derivative; " << std:: endl;
    while(printf("Пожалуйста, введите вашу команду: ") && (scanf("%d", &command)) != EOF){ 
        if (command==0){ //смена библиотек
            dlclose(adress);//освобождает указатель на библиотеку и программа перестает ей пользоваться
            if (curlib==0){ 
                curlib=1;
                adress=dlopen(libeary_mas[curlib],RTLD_LAZY);
                if(!adress){
                    cout<<"Ошибка";
                    exit(EXIT_FAILURE);
                }
                //возвращаем адрес функции из памяти библиотеки
                SinIntegral = (float(*)(float, float, float))dlsym(adress,"SinIntegral"); //dlsym присваивает указателю на функцию, объявленному в начале, ее адрес в библиотеке
                Derivative = (float(*)(float, float))dlsym(adress, "Derivative");
   
            }
            else if(curlib==1){
                curlib=0;
                adress = dlopen(libeary_mas[curlib],RTLD_LAZY);
                if(!adress){
                    cout<<"Ошибка";
                    exit(EXIT_FAILURE);
                }
                //возвращаем адрес функции из памяти библиотеки
                SinIntegral = (float(*)(float, float, float))dlsym(adress,"SinIntegral"); //dlsym присваивает указателю на функцию, объявленному в начале, ее адрес в библиотеке
                Derivative = (float(*)(float, float))dlsym(adress, "Derivative");
 
            }
            cout << "Вы изменили контракты!" << std:: endl;
        }
        else if (command==1){
            float A, B, e;
            cin>>A>>B>>e;
            float  sinintegral =SinIntegral(A,B,e);
            if (sinintegral==-1){
                cout<<"Пожалуйста, введите еще раз\n";
            }
            else{
                cout<<"SinIntegral("<< A<<","<<B<<","<< e<<")="<<SinIntegral(A, B, e)<< endl; 
            }
        }
        else if (command==2){
            float A, deltaX;
            cin>>A>>deltaX;
            float  derivative =Derivative(A, deltaX);
            if (derivative==-1){
                cout<<"Пожалуйста, введите еще раз\n";
            }
            else{
                cout<< "Derivative("<<A<<","<<deltaX<<")="<<Derivative(A,deltaX)<<endl;
            }
        }
        else{
            cout<<"Вам нужно было ввести только 0, 1 или 2!" << std:: endl;
        }
    }
    dlclose(adress);
    return 0;
}

