#include "tbb/task_group.h"
#include "tbb/global_control.h"
#include "tbb/task_arena.h"

#include<functional>
#include<iostream>
#include <memory>

namespace Task{
template<class FUNCTOR, typename... ARGS>
class MyTask{
    static_assert(sizeof... (ARGS)>0,
                  "At least one argument needed ");
    
public:
    MyTask(tbb::task_arena &arena, std::size_t arrSize, ARGS... args);
    
    bool execute();
    
    bool CopyDeviceToHost();
    
    
private:
    tbb::task_arena m_arena;
    std::size_t m_size;
    std::tuple<ARGS... >m_tuple;
    std::tuple<ARGS... >m_objects;
    
    
    
};

template<class FUNCTOR, typename... ARGS>
std::unique_ptr<MyTask<FUNCTOR,ARGS... >>make_Task(tbb::task_arena& arena, std::size_t arraySizes, ARGS... args){
    
    return std::make_unique<MyTask<FUNCTOR,ARGS...>>(arena,arraySizes,args...);
}

template<class FUNCTOR, typename... ARGS>
MyTask<FUNCTOR,ARGS...>::MyTask(tbb::task_arena &arena, std::size_t arrSize, ARGS... args):m_arena(std::move (arena)),m_size(arrSize),m_tuple(args ...)
{
    
}
template<class FUNCTOR, typename... ARGS>
bool MyTask<FUNCTOR,ARGS...>::execute(){
    
   // FUNCTOR fun(ARGS...);
    FUNCTOR fun(std::get<0>(m_tuple),std::get<1>(m_tuple));
    m_arena.execute([&fun](){
        tbb::task_group group;
        std::atomic<unsigned int>count{0};
        group.run([&](){
            fun.RunArray();
        });
        group.wait();
    });
     
    return true;
}


}

class Functor1{
public:
    void operator()(std::size_t i, uint32_t*array1){
        array1[i]*=12;
    }
    
    
};

class Functor2{
public:
    Functor2( std::size_t i, uint32_t* array1):m_size(i),m_array(std::move(array1)){
        
    }
    void RunArray(){
        m_array[m_size] *=12;
    }
private:
    size_t m_size;
    uint32_t* m_array;
    
};

int main(){
    
    
    tbb::task_arena arena(1);
    tbb::task_group tg;
    
    uint32_t myarray[100];
    
    for(int i=0;i<100;i++)myarray[i] = i;
   

    auto task = Task::make_Task<Functor2>(arena,100,2,myarray);
    task->execute();
   // auto fun = Functor1();
   // Functor2(2,myarray);
    std::cout<<myarray[2]<<" "<<std::endl;
    
    
    
    
}
