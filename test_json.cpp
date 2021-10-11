#include <string>
#include <cstddef>
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <iomanip>
#include <unistd.h>
#include <algorithm>
#include "TH1D.h"
#include "TChain.h"
#include "TH2D.h"
#include "TFile.h"
#include "mpi.h"
#include "tbb/tbb.h"
#include "tbb/parallel_for_each.h"
#include "tbb/task_scheduler_init.h"

#include<nlohmann/json.hpp>

struct mytask{
  mytask(size_t n)
    :_n(n)
  {}

  void operator()(){
    for (int i=0;i<100000;i++){}
    std::cerr<<"["<<_n<<"]"<<std::endl;
    
  }
  size_t _n;

};

struct executor
{
  executor(std::vector<mytask>&t)
    :_tasks(t)
  {}

  executor(executor& e, tbb::split)
    :_tasks(e._tasks)
  {}

  void operator()(const tbb::blocked_range<size_t>&r)const{
    for(size_t i=r.begin();i!=r.end();++i)
      _tasks[i]();

    }
  std::vector<mytask>& _tasks;
  
};

void test_tbb(){
 
  tbb::task_scheduler_init init; // Automatic number of threads
  // Uncomment below if explicit number of threads...
  // tbb:task_scheduler_init init(2);

  std::vector<mytask>tasks;
  for(int i=0;i<1000;++i)
    tasks.push_back(mytask(i));

  
  executor exec(tasks);
  tbb::parallel_for(tbb::blocked_range<size_t>(0,tasks.size()),exec);
  
  std::cerr<<std::endl;
		   
}

void test_json(){
  using json = nlohmann::json;

  json data;

  
  std::vector<double>N = {1,2,4.5,6.7,6.5};

  data["blaa"] = N;

  data["beta"] = 10.0;


  std::cout<<data<<std::endl;
  
}

int main(int argc, char* argv[]){

  //test_json();
  test_tbb();
  std::cout<<" END OF THE CODE "<<std::endl;
  return 1;
}
