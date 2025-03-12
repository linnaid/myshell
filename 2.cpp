#include <stdio.h>
#include <iostream>
using namespace std;

namespace a
{
    int n = 10;
    int b = 1;
}
//int b = 9;

int n = 0;
using a::b;

void pmt(int& a)
{
    a += 2;
    
}

// int& amm(int a)
// {
//     a += 2;
//     return a;
// }

int main()
{
    //int n = 1;
    //int b = 10;
    int m = 0;
    pmt(m);
    cout << m << endl;
    // amm(m);
    //cout << m << endl;
    //printf("%d %d\n",n,b);
    return 0;
}