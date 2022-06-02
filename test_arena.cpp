#include <iostream>

#include "tbb/tbb.h"
#include "tbb/task_arena.h"
#include "tbb/tick_count.h"
#include "tbb/task_group.h"

class Functor3 {
public:

   void operator()( std::size_t i, uint32_t* array1 ) {

      array1[ i ] *= 12;

   }
}; // class Functor3


int main(){
  
    tbb::task_arena m_arena;
    uint32_t* array = new uint32_t[100];
    for(int i=0;i<100;i++)array[i] = uint32_t(i*10);
    std::cout<<"Before "<<array[3]<<std::endl;
   auto fun= Functor3 ();//(3,array);
    m_arena.execute([&fun,array](){
    tbb::task_group group;
        group.run([&](){
            fun(3,array);
        });
        group.wait();
    });
    std::cout<<"After "<<array[3]<<std::endl;
  return 0;
}
