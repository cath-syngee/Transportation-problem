# Transportation problem
Template class provides Transportation problem solution

Suitable for both classic Transportation problem and Transportation problem with fuzzy input data. 
In first case is used `<int>`, in second one - `<double>`

Unfortunatly, you have to `#include "Transportation.cpp"` in order for the template class methods to be compiled

##### Example 
Input:
```c++
std::vector<int> A = { 94,85,36,11 };						//suppliers
std::vector<int> B = { 23,17,15,19,40,14 };					//consumers
std::vector<std::vector<int>> C = { {6,5,6,4,2,3},			//transportation costs table
		{8,7,5,3,3,2}, {8,9,8,7,2,6}, {8,7,3,5,4,1} };
int L;														//total transportation cost
std::vector<std::pair<int, std::pair<int, int>>> basis;		//basis variables [value, <i,j>]													

Transportation<int> problem(A, B, C);
L = problem.getL();
basis = problem.getBasis();
C = problem.getC();		

std::cout << std::endl << "result = " << L << std::endl;
```
Output:

`result = 441`