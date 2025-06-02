#include<iostream>
#include<vector>
#include<chrono>
#include<random>
#include<thread>
#include<algorithm>
#include<fstream>

#define MAX_THREAD 4


// For the single thread quick sort
int partition(std::vector<int> &arr, int low, int high) {
    int pivot = arr[low];
    int i = low;
    int j = high;

    while (i < j) {
        while (arr[i] <= pivot && i <= high) {
            i++;
        }

        while (arr[j] > pivot && j >= low) {
            j--;
        }
        if (i < j) std::swap(arr[i], arr[j]);
    }
    std::swap(arr[low], arr[j]);
    return j;
}

void qs(std::vector<int> &arr, int low, int high,int depth) {
    if (low >= high) return;
        int pIndex = partition(arr, low, high);
        qs(arr, low, pIndex - 1,depth);
        qs(arr, pIndex + 1, high,depth);
}


void quickSortParallel(std::vector<int> &arr,int low,int high,int depth)
{
    if(low>=high)  return;
    int pIndex = partition(arr,low,high);
if(depth<=MAX_THREAD)
{
    std::thread t1(quickSortParallel,ref(arr),low,pIndex-1,depth+1);
    std::thread t2(quickSortParallel,ref(arr),pIndex+1,high,depth+1);
    t1.join();
    t2.join();
}
else{
qs(arr, low, pIndex - 1,depth+1);
qs(arr, pIndex + 1, high,depth+1);
}
} 

double benchMark(void (*sort_fun)(std::vector<int>&,int,int,int),std::vector<int> &arr,int depth)
{
    auto st = std::chrono::high_resolution_clock::now();
    sort_fun(arr,0,arr.size()-1,depth);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> du = end - st;
    std::cout<<"Time taken -  "<<du.count()<<" seconds"<<std::endl;
    return du.count();
}

//Driver codeeee...
int main() {
std::ofstream file("benchmark_results_2.csv");
file << "Size,SingleThreadTime,MultiThreadTime\n";
std::vector<int> arr(1e5);
std::vector<int> arr_cp;
 for(int i=0;i<15;i++)
 {
    std::generate(arr.begin(),arr.end(),std::rand);
         arr_cp = arr;
    //First calling for single threaded
    std::cout<<i<<"."<<"Quick sort for single thread - "<<" ";
    double singleTime = benchMark(qs,arr,0);
    //Calling for multi thread
    std::cout<<i<<"."<<"Quick sort for multi thread - "<<" ";
    double multiTime = benchMark(quickSortParallel,arr_cp,0);
    std::cout << i<<"."
                  << " | Single: " << singleTime 
                  << " sec | Multi: " << multiTime << " sec" << std::endl;

        file << "  ,  " << singleTime << "  ,  " << multiTime << "\n";
 }

file.close();
    return 0 ;
}