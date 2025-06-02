#include<iostream>
#include<vector>
#include<chrono>
#include<random>
#include<thread>
#include<algorithm>
#include<fstream>

//Merge sort for single thread
void merge(std::vector<int> &arr, int low, int mid, int high) {
    std::vector<int> temp; // temporary array
    int left = low;      // starting index of left half of arr
    int right = mid + 1;   // starting index of right half of arr

    //storing elements in the temporary array in a sorted manner//

    while (left <= mid && right <= high) {
        if (arr[left] <= arr[right]) {
            temp.push_back(arr[left]);
            left++;
        }
        else {
            temp.push_back(arr[right]);
            right++;
        }
    }

    // if elements on the left half are still left //

    while (left <= mid) {
        temp.push_back(arr[left]);
        left++;
    }

    //  if elements on the right half are still left //
    while (right <= high) {
        temp.push_back(arr[right]);
        right++;
    }

    // transfering all elements from temporary to arr //
    for (int i = low; i <= high; i++) {
        arr[i] = temp[i - low];
    }
}

void mergeSort(std::vector<int> &arr, int low, int high,int depth) {
    if (low >= high) return;  //Base case
    int mid = (low + high) / 2 ;
    mergeSort(arr, low, mid,depth);  // left half
    mergeSort(arr, mid + 1, high,depth); // right half
    merge(arr, low, mid, high);  // merging sorted halves
}

//For the multi-threaded merge sort
void mergeSortParallel(std::vector<int> &arr, int low, int high,int depth)
{
    if(low>=high)  return;
    int mid=(low+high)/2;
    if(depth<=4){
    std::thread t1(mergeSortParallel,ref(arr),low,mid,depth+1);
    std::thread t2(mergeSortParallel,ref(arr),mid+1,high,depth+1);
    t1.join();   //It will wait until the thread completes it operation
    t2.join();
    }
    else
    {
 mergeSort(arr, low, mid,depth);  // left half
    mergeSort(arr, mid + 1, high,depth); // right half
    }
    merge(arr,low,mid,high);     //Merging the sorted halves
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
std::ofstream file("benchmark_results_1.csv");
file << "Size,SingleThreadTime,MultiThreadTime\n";
std::vector<int> arr(1e5);
std::vector<int> arr_cp;
 for(int i=0;i<15;i++)
 {
    
    
    std::generate(arr.begin(),arr.end(),std::rand);
         arr_cp = arr;
    //First calling for single threaded
    std::cout<<i<<"."<<"Merge sort for single thread - "<<" ";
    double singleTime = benchMark(mergeSort,arr,0);
    //Calling for multi thread
    std::cout<<i<<"."<<"Merge sort for multi thread - "<<" ";
    double multiTime = benchMark(mergeSortParallel,arr_cp,0);
    std::cout << i<<"."
                  << " | Single: " << singleTime 
                  << " sec | Multi: " << multiTime << " sec" << std::endl;

        file << "  ,  " << singleTime << "  ,  " << multiTime << "\n";
 }

file.close();
    return 0 ;
}