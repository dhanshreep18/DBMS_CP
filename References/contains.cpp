#include<iostream>
#include<vector>
#include<algorithm>
using namespace std;
template <typename T>

bool contains(vector<T> vec, const T & elem)
{
    bool result = false;
    if( find(vec.begin(), vec.end(), elem) != vec.end() )
    {
        result = true;
    }
    return result;
}
// int main()
// {
//     vector<int> numbers{ 11, 22, 33, 44, 55, 66};
//     int elem = 44;
//     if(contains(numbers, elem))
//     {
//         cout<<"Element exist in vector" <<endl;
//     }
//     else
//     {
//         cout<<"Element does not exist in vector" <<endl;
//     }
    
//     return 0;
// }