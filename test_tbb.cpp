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
#include "TTree.h"
#include "TBranch.h"
#include "TLeaf.h"
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

struct read_entries{
  read_entries(double n,std::string _name)
  :_n(n),name(_name)
  {    std::cout<<" Value "<<_n<<" "<<name<<" "<<
      tbb::task_arena::current_thread_index()<<
      " "<<std::endl;
  }

  void operator()(){

  }
  
  double _n;
  std::string name;
};

struct executor_entries
{
  executor_entries(std::vector<read_entries>&t)
    :_tasks(t)
  {}

  executor_entries(executor_entries& e, tbb::split)
    :_tasks(e._tasks)
  {}

  void operator()(const tbb::blocked_range<size_t>&r)const{
    for(size_t i=r.begin();i!=r.end();++i)
      _tasks[i]();


  }
  std::vector<read_entries>& _tasks;

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

void ReadEntries(TTree *tree){

  tbb::task_scheduler_init init(2);
  auto l = tree->GetListOfBranches();

  auto nentries = tree->GetEntriesFast();
  nentries =10;
  auto tot_branches = l->GetEntriesFast();
  int _counter=0;
  std::vector<read_entries>tasks;
  for(int i = 0;i<nentries;i++){
    for(Long64_t jentry=0;jentry<tot_branches;++jentry){
      tree->GetEntry(jentry);
      auto branch = dynamic_cast<TBranch*>((*l)[jentry]);
      std::string branch_name = branch->GetName();
      auto leaves = branch->GetListOfLeaves();
      
      auto leaf = dynamic_cast<TLeaf*>((*leaves)[0]);
      auto _val = leaf->GetValue(0);
      tasks.push_back(read_entries(_val,branch_name));
      if(_counter==5){

	executor_entries exec(tasks);
	tbb::parallel_for(
			  tbb::blocked_range<size_t>(0,tasks.size()),exec);
	
	_counter=0;
	tasks.clear();
      }
      _counter++;
    }
  }

}
int main(int argc, char* argv[]){
  auto input_file = argv[1];
  std::string ntuple_name = "ccqe_data";
  auto f = TFile::Open(input_file);
  auto e = (TTree*)f->Get(ntuple_name.c_str());
  ReadEntries(e);
  // test_tbb();
  std::cout<<" END OF THE CODE "<<std::endl;
  return 1;
}
