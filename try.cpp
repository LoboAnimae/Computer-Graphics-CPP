#include <cmath>
#include <iostream>
using namespace std;
#pragma region


double dot(double *v0, double *v1)
{
    double *vector1 = v0, *vector2 = v1; // This is the memory address of the x value
    double result = (*(vector1) * *(vector2)) + (*(vector1 + 1) * *(vector2 + 1)) + (*(vector1 + 2) * *(vector2 + 2));
    return result;
}

int main()
{

    double v0[3] = {-0.44444, 0.54131561, 0.95248761}, v1[3] = {-0.644841651, -0.68549135448, 0.9487961616};
    cout << dot(v0, v1) << endl;
    
    
}