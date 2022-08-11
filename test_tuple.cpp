#include <tuple>
#include <iostream>

//tbb related libraries
#include "tbb/tbb.h"
#include <array>
#include <type_traits>
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

    std::tuple temp_tuple(std::move(m_tuple));
    m_arena.execute([&fun,&temp_tuple](){
    tbb::task_group group;
        group.run([&](){
            apply_from_tuple(fun,temp_tuple);
            
        });
        group.wait();
    
    });

}


//Test whether something is tuple or not....
template<typename T>
struct IsTupleImpl : std::false_type{};
  
template<typename... U>
struct IsTupleImpl<std::tuple<U...>>: std::true_type{};

template<typename T>
constexpr bool IsTuple(T* x){
 return IsTupleImpl<std::decay_t<T>>::value;
}

template<typename T>
constexpr bool IsFundamental(T x){
    return std::is_fundamental<T>::value;
}

//now the testing of the unpacking tuples...
template<typename T>
struct is_pointer { static const bool value = false; };

template<typename T>
struct is_pointer<T*> { static const bool value = true; };

//now for the arrays
template<typename T>
struct is_arrayImpl: std::false_type{};
    
template<typename T>
struct is_arrayImpl<T[]>: std::true_type{};
    
template<typename T, std::size_t N>
struct is_arrayImpl<T[N]>: std::true_type{};

template<typename T>
constexpr bool is_array(T* x){
    std::cout<<"x is "<<x<<std::endl;
    return is_arrayImpl<std::decay_t<T>>::value;
}


int main()
{
    int a = 1;
    int b = 2;
    int c = 3;
    int* out = new int[100];
    int val[100];
    bool _true = true;
    int arrSize = *(&out + 1) - out;
    std::cout<<out[0]<<" Empty array "<<std::endl;
    std::cout<<"Size of Out is "<<sizeof(out)/sizeof(out[0])<<" "<<arrSize<<std::endl;
    std::cout<<is_array(val)<<'\n'<<is_array(&a)<<std::endl;
    std::cout<<"fundamental "<<std::is_fundamental<int>::value<<" "<<IsFundamental(out)<<std::endl;
    //we just need one thread to dispatch....
    tbb::task_arena arena(1);
    auto task = make_Task<test>(arena,a,b,c,out);
    
    task->execute();
    
    std::cout<<"Output After the Execution "<<out[0]<<std::endl;
    
    
    //test on make_index_sequence
    
    auto x_ = std::make_index_sequence<10>();
    
    auto test_tuple = std::make_tuple(1,2,3,4);
    std::cout<<IsTuple(&test_tuple)<<std::endl;
    return 1;
}

