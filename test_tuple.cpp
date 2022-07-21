#include <tuple>
#include <iostream>

//tbb related libraries
#include "tbb/tbb.h"

class test{
public:
    void operator()(int a, int b, int c, int* out){
        out[0] = a +b +c;
        
    }
    
};

//tuple related things...

template<typename F, typename Tuple, size_t ...S>
decltype(auto)apply_tuple_impl(F&& fn, Tuple&& t, std::index_sequence<S...>)
{
    return std::forward<F>(fn)(std::get<S>(std::forward<Tuple>(t))...);
}
template<typename F, typename Tuple>
decltype(auto) apply_from_tuple(F&& fn, Tuple && t){
    
    std::size_t constexpr tSize
    = std::tuple_size<typename std::remove_reference<Tuple>::type>::value;
    
    return apply_tuple_impl(std::forward<F>(fn),
                            std::forward<Tuple>(t),
                            std::make_index_sequence<tSize>());
}

template<class F, typename... ARGS>
class MyTask{
    static_assert(sizeof... (ARGS)>=0,
          "At least one argument needed ");
    
public:
    MyTask(tbb::task_arena &arena, ARGS... args);
    bool execute();
    
private:
    std::tuple<ARGS...>m_tuple;
    tbb::task_arena m_arena;
    
};

template<class F, typename... ARGS>
std::unique_ptr<MyTask<F,ARGS...>>make_Task(tbb::task_arena &arena, ARGS... args){
    
    return std::make_unique<MyTask<F, ARGS...>>(arena,args...);
}

template<class F, typename... ARGS>
MyTask<F, ARGS...>::MyTask(tbb::task_arena &arena, ARGS... args):m_arena(std::move(arena)),m_tuple(std::make_tuple(args...)){}

template<class F, typename... ARGS>
bool MyTask<F, ARGS...>::execute(){
    auto fun = F();
    apply_from_tuple(fun,m_tuple);

  /*
    m_arena.execute([&fun,m_tuple](){
    tbb::task_group group;
        group.run([&](){
            apply_from_tuple(fun,m_tuple);
            
        });
        group.wait();
    
    });
*/
}

//now the testing of the unpacking tuples...

int main(){
    int a = 1;
    int b = 2;
    int c = 3;
    int* out = new int[100];
   
    
    tbb::task_arena arena(1);
    auto task = make_Task<test>(arena,a,b,c,out);
    
    task->execute();
    
    std::cout<<"Output After the Execution "<<out[0]<<std::endl;
    
    return 1;
}
