#include <tuple>
#include <iostream>


int main(){
    int a[4] = {1,2,3,4};
    int b[3] = {7,8,9};
    int *pa = a;
    int *pb = b;
    int **arrptr[2];
    
    arrptr[0] = &pa;
    arrptr[1] = &pb;
    std::cout<<(*arrptr[0])[1]<<" "<<(*arrptr[1])[1]<<std::endl;
}
