//Гарантия того, что компилятор увидит полное определение шаблона класса Transportation
#include "Transportation.h"
#include "Transportation.cpp"				//нарушаем правила хорошего тона в программировании, но только в этом месте (иначе методы шаблонного класса не скомпилируются)

template class Transportation<int>;			//явно создаем экземпляр шаблона класса Transportation<int>
template class Transportation<double>;		//явно создаем экземпляр шаблона класса Transportation<double>



//пример использования класса Transportation, начиная с этого комментария - код удалять; но сам файл оставить в проекте
#include <vector>
#include <iostream>
int main() 
{
	setlocale(LC_ALL, "Russian");
	std::vector<double> A = { 94,85,36,11 };
	std::vector<double> B = { 23,17,15,19,40,14 };
	std::vector<std::vector<double>> C = { {6,5,6,4,2,3},{8,7,5,3,3,2}, {8,9,8,7,2,6}, {8,7,3,5,4,1} };
	double L;
	std::vector<std::pair<double, std::pair<int, int>>> basis;		//вектор базисных переменных
																	//структура: (значение, <i, j>)
	Transportation<double> problem(A, B, C);
	L = problem.getL();
	std::cout << std::endl << "result = " << L << std::endl;		//441
	basis = problem.getBasis();
	C = problem.getC();												//измененная матрица С

	std::cout << "Оптимальный план перевозок:" << std::endl;
	for (int i = 0; i < C.size(); i++)
	{
		std::cout << "Из " << i + 1 << "-го пункта производства необходимо продукт направить в ";
		for (int j = 0; j < basis.size(); j++)
		{
			if (basis[j].second.first == i && C[basis[j].second.first][basis[j].second.second] != 0) std::cout << basis[j].second.second + 1 << "-й пункт потребления (" << basis[j].first << " единиц), ";
		}
		std::cout << std::endl;
	}
	for (int j = 0; j < basis.size(); j++)
	{
		if (C[basis[j].second.first][basis[j].second.second] == 0) std::cout << "На " << basis[j].second.first + 1 << "-ом складе остался невостребованным продукт в количестве " << basis[j].first << " единиц." << std::endl;
	}

	system("pause");
	return 0;
}