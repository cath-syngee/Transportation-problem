#include "Transportation.h"

#include <iostream>
#include <vector>
#include <algorithm>

template <typename T>
Transportation<T>::Transportation(std::vector<T> A, std::vector<T> B, std::vector<std::vector<T>> C, bool full_out) :
	A(A), B(B), C(C), amountA(A.size()), amountB(B.size()), L(0), full(full_out), addedC(std::make_pair(false, false)), optPlan(false)
{
	sumA = 0;
	sumB = 0;

	for (int i = 0; i < amountA; i++)
	{
		sumA += A.at(i);
	}

	for (int i = 0; i < amountB; i++)
	{
		sumB += B.at(i);
	}

	solve();
}

template <typename T>
T Transportation<T>::getL()
{
	return L;
}

template <typename T>
std::vector<std::pair<T, std::pair<int, int>>> Transportation<T>::getBasis() 
{
	return basisPerem_e;
}

template <typename T>
std::vector<std::vector<T>> Transportation<T>::getC()
{
	return C;
}

template <typename T>
void Transportation<T>::solve()
{
	if (sumA != sumB)
	{
		if (full) std::cout << std::endl << "Задача открытая, так как условие баланса нарушено" << std::endl;
		if (sumA > sumB)
		{
			if (full) std::cout << "Cуммарный объем производства sumA = " << sumA << ", а суммарный объем потребления sumB = " << sumB << std::endl;
			if (full) std::cout << "Чтобы получить закрытую модель, добавляем фиктивный пункт потребления B(j+1) с объемом потребления = " << (sumA - sumB) << std::endl;
			B.push_back(sumA - sumB);
			for (int i = 0; i < amountA; i++)
			{
				C[i].push_back(0);
			}
			addedC.second = true;
		}
		else
		{
			if (full) std::cout << "Cуммарный объем производства sumA = " << sumA << ", а суммарный объем потребления sumB = " << sumB << std::endl;
			if (full) std::cout << "Чтобы получить закрытую модель, добавляем фиктивный пункт производства A(i+1) с объемом производства = " << (sumB - sumA) << std::endl;
			A.push_back(sumB - sumA);
			std::vector<T> c_newB(amountB, 0);
			C.push_back(c_newB);
			addedC.first = true;
		}
	}
	else
	{
		if (full) std::cout << std::endl << "Задача закрытая, условие баланса выполняется, так как суммарные объемы производства и потребления равны (=" << sumA << ")" << std::endl;
	}

	if (full)
	{
		std::cout << std::endl << "Текущая таблица:" << std::endl;
		std::cout << "A: ";
		for (int i = 0; i < A.size(); i++)
		{
			std::cout << A.at(i) << "  ";
		}

		std::cout << std::endl << "B: ";
		for (int i = 0; i < B.size(); i++)
		{
			std::cout << B.at(i) << "  ";
		}

		std::cout << std::endl << "Матрица транспортных издержек (С):" << std::endl;
		for (int i = 0; i < C.size(); i++)
		{
			for (int j = 0; j < C[i].size(); j++)
			{
				std::cout << C[i][j] << "  ";
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
	};

	minElement();												//начальный опорный план
	if (full)
	{
		for (int i = 0; i < basisPerem_e.size(); i++)
		{
			L += basisPerem_e[i].first * C[basisPerem_e[i].second.first][basisPerem_e[i].second.second];
		}
		std::cout << std::endl << "Значение целевой функции для этого опорного плана: L = " << L << std::endl;
	};

	optPlan = potentialMethod();

	while (!optPlan)
	{
		loopBuild();
		badC.clear();
		badC_Value.clear();
		optPlan = potentialMethod();
	}

	L = 0;
	if (full) std::cout << std::endl << std::endl << "Полученный опорный план является оптимальным, так как для всех клеток выполняется условие `ai + `bj = cij" << std::endl;
	for (int i = 0; i < basisPerem_e.size(); i++)
	{
		L += basisPerem_e[i].first * C[basisPerem_e[i].second.first][basisPerem_e[i].second.second];
	}
	std::cout << std::endl << "Минимальные затраты составят: L = " << L << std::endl;

	if (!full)
	{
		std::cout << std::endl << "Оптимальный план перевозок:" << std::endl;
		for (int i = 0; i < C.size(); i++)
		{
			std::cout << "Из " << i + 1 << "-го пункта производства необходимо продукт направить в ";
			for (int j = 0; j < basisPerem_e.size(); j++)
			{
				if (basisPerem_e[j].second.first == i && C[basisPerem_e[j].second.first][basisPerem_e[j].second.second] != 0) std::cout << basisPerem_e[j].second.second + 1 << "-й пункт потребления (" << basisPerem_e[j].first << " единиц), ";
			}
			std::cout << std::endl;
		}
		for (int j = 0; j < basisPerem_e.size(); j++)
		{
			if (C[basisPerem_e[j].second.first][basisPerem_e[j].second.second] == 0) std::cout << "На " << basisPerem_e[j].second.first + 1 << "-ом складе остался невостребованным продукт в количестве " << basisPerem_e[j].first << " единиц." << std::endl;
		}
	}
}

template <typename T>
bool Transportation<T>::potentialMethod()
{
	int sizeA = C.size();
	int sizeB = C[0].size();
	std::vector<std::pair<int, bool>> alpha, beta;		//векторы потенциалов с их значениями и посчитаны ли они
	int count = sizeA + sizeB - 1;

	alpha.resize(sizeA);
	beta.resize(sizeB);
	alpha[0].second = true;								//a1=0

	if (full) std::cout << std::endl << "Проверим оптимальность опорного плана. Найдем предварительные потенциалы `Ai и `Bj по занятым клеткам таблицы, в которых `Ai+`Bj=Cij, полагая, что `A1=0" << std::endl;

	int max_iter = count;	//если вырожденность на этапе вычисл-я знач-й потенциалов -> решить ее
	while (count != 0)
	{
		for (int i = 0; i < basisPerem_e.size(); i++)
		{
			if (alpha[basisPerem_e[i].second.first].second == true && beta[basisPerem_e[i].second.second].second == false)
			{
				beta[basisPerem_e[i].second.second].first = C[basisPerem_e[i].second.first][basisPerem_e[i].second.second] - alpha[basisPerem_e[i].second.first].first;
				if (full) std::cout << "`a" << basisPerem_e[i].second.first + 1 << " + `b" << basisPerem_e[i].second.second + 1 << " = " << alpha[basisPerem_e[i].second.first].first + beta[basisPerem_e[i].second.second].first << "; " << alpha[basisPerem_e[i].second.first].first << " + `b" << basisPerem_e[i].second.second + 1 << " = " << alpha[basisPerem_e[i].second.first].first + beta[basisPerem_e[i].second.second].first << "; `b" << basisPerem_e[i].second.second + 1 << " = " << beta[basisPerem_e[i].second.second].first << std::endl;
				beta[basisPerem_e[i].second.second].second = true;
				count--;
			}
		}

		for (int i = 0; i < basisPerem_e.size(); i++)
		{
			if (alpha[basisPerem_e[i].second.first].second == false && beta[basisPerem_e[i].second.second].second == true)
			{
				alpha[basisPerem_e[i].second.first].first = C[basisPerem_e[i].second.first][basisPerem_e[i].second.second] - beta[basisPerem_e[i].second.second].first;
				if (full) std::cout << "`a" << basisPerem_e[i].second.first + 1 << " + `b" << basisPerem_e[i].second.second + 1 << " = " << alpha[basisPerem_e[i].second.first].first + beta[basisPerem_e[i].second.second].first << "; `a" << basisPerem_e[i].second.first + 1 << " + " << beta[basisPerem_e[i].second.second].first << " = " << alpha[basisPerem_e[i].second.first].first + beta[basisPerem_e[i].second.second].first << "; `a" << basisPerem_e[i].second.first + 1 << " = " << alpha[basisPerem_e[i].second.first].first << std::endl;
				alpha[basisPerem_e[i].second.first].second = true;
				count--;
			}
		}

		//для проверки на вырожденность решения
		max_iter--;
		if (max_iter == 0)
		{
			if (full)
			{
				std::cout << "На данном этапе возникла ситуация, когда для оставшихся занятых клеток не известно ни одного из потенциалов.";
				std::cout << "Это результат вырожденности решения. Для его преодоления в одну из клеток нужно внести нулевую поставку, таким образом, такая клетка станет условно занятой.";
			}

			//выбираем из тех строк/столбцов, у которых есть потенциалы, клетку с наименьшей стоимостью в матрице С
			int min_cost = C[0][0];
			int temp_i, temp_j;
			for (int i = 0; i < alpha.size(); i++)
			{
				if (alpha[i].second == true)
				{
					for (int j = 0; j < C[0].size(); j++)
					{
						if (beta[j].second == false)
						{
							if (min_cost > C[i][j])
							{
								min_cost = C[i][j];
								temp_i = i;
								temp_j = j;
							}
						}
					}
				}
			}

			for (int i = 0; i < beta.size(); i++)
			{
				if (beta[i].second == true)
				{
					for (int j = 0; j < C.size(); j++)
					{
						if ((alpha[j].second == false) && (min_cost > C[j][i]))
						{
							min_cost = C[j][i];
							temp_i = j;
							temp_j = i;
						}
					}
				}
			}

			if (full)
			{
				std::cout << std::endl << "Среди клеток, в которых может быть размещена нулевая поставка, наименьший тариф имеет клетка " << min_cost << " (" << temp_i + 1 << ", " << temp_j + 1 << "). В ней размещаем нулевую поставку и она становится условно занятой.";
			}

			for (int i = 0; i < basisPerem_e.size(); i++)
			{
				if (basisPerem_e[i].first == 0)
				{
					if (full) std::cout << std::endl << "Ранее поставленный псевдоноль в ячейке (" << basisPerem_e[i].second.first + 1 << ", " << basisPerem_e[i].second.second + 1 << ") убираем." << std::endl;
					basisPerem_e.erase(basisPerem_e.begin() + i);
				}
			}
			basisPerem_e.push_back(std::make_pair(0, std::make_pair(temp_i, temp_j)));
			max_iter = count;
		}
	}

	//вывод потенциалов:
	if (full)
	{
		std::cout << std::endl << "`a: ";
		for (int i = 0; i < sizeA; i++) std::cout << alpha[i].first << "  ";
		std::cout << std::endl << "`b: ";
		for (int i = 0; i < sizeB; i++) std::cout << beta[i].first << "  ";

	}

	bool flag = true;		//оптимальность плана
	for (int i = 0; i < sizeA; i++)
		for (int j = 0; j < sizeB; j++)
		{
			if (alpha[i].first + beta[j].first - C[i][j] > 0)
			{
				badC.push_back(std::make_pair(i, j));
				badC_Value.push_back(int(alpha[i].first + beta[j].first - C[i][j]));
				flag = false;
			}
		}

	L = 0;
	for (unsigned int i = 0; i < basisPerem_e.size(); i++)
	{
		L += basisPerem_e[i].first * C[basisPerem_e[i].second.first][basisPerem_e[i].second.second];
	}
	if (full) std::cout << std::endl << "Минимальные затраты составят: L = " << L << std::endl;

	if (!badC.empty() && full) std::cout << std::endl << "Опорный план не оптимален, так как существуют клетки, для которых `ai + `bj > cij" << std::endl;

	return flag;
}

template <typename T>
void Transportation<T>::minElement()
{
	if (full) std::cout << "Поиск начального опорного плана с помощью метода минимального элемента." << std::endl;
	if (full) std::cout << "Суть метода в том, что из таблицы стоимостей С выбирается наименьшая, и в клетку, соответствующую ей, помещается меньшее из ai и bj" << std::endl;

	T minCost, maxCost, cost;						//cost - псевдостоимость текущей клетки C~
	std::pair<int, int> minPosition;
	bool flagA = true;
	bool flagB = true;								//флаги, что исчерпаны ресурсы до нуля

	if (addedC.first == true)						//добавили пункт А
	{
		minCost = C[0][0];
		maxCost = C[0][0];
		for (int k = 0; k < (A.size() + B.size() - 1); k++) //кол-во итераций = нужное кол-во базисных клеток
		{
			for (int i = 0; i < A.size(); i++) if (A[i] != 0) flagA = false;
			for (int j = 0; j < B.size(); j++) if (B[j] != 0) flagB = false;

			if (flagA && flagB)		//все ресурсы исчерпаны, план вырожден
			{
				for (int i = 0; i < A.size() - 1; i++)
				{
					for (int j = 0; j < B.size(); j++)
					{
						if (notInBasis(i, j))
						{
							if (C[i][j] < minCost)
							{
								minCost = C[i][j];
								minPosition.first = i;
								minPosition.second = j;
							}
							if (C[i][j] > maxCost) maxCost = C[i][j] + 1;
						}
					}
				}
				cost = 0;
				basisPerem_e.push_back(std::make_pair(cost, minPosition));

				if (full) std::cout << "Искомый элемент: С(" << minPosition.first + 1 << ", " << minPosition.second + 1 << "), его запасы =" << A[minPosition.first] << ", потребности =" << B[minPosition.second] << std::endl;
				minCost = maxCost;
				if (full) std::cout << "Поскольку минимальным является " << cost << ", то вычитаем его" << std::endl << std::endl;
				continue;
			}

			bool flag = true;
			for (int a = 0; a < A.size() - 1; a++)
			{
				if (A[a] != 0) flag = false;
			}

			if (flag && !flagB)		//если все реальные А исчерпаны, а В не исчерпаны
			{
				for (int j = 0; j < B.size(); j++)
				{
					if (A.back() != 0 && B[j] != 0)
					{
						if (notInBasis(A.size() - 1, j))
						{
							if (C[A.size() - 1][j] < minCost)
							{
								minCost = C[A.size() - 1][j];
								minPosition.first = A.size() - 1;
								minPosition.second = j;
							}
							if (C[A.size() - 1][j] > maxCost) maxCost = C[A.size() - 1][j] + 1;
						}
					}
				}

				cost = std::min(A[minPosition.first], B[minPosition.second]);		//можно min заменить на:  !(b<a)?a:b
				basisPerem_e.push_back(std::make_pair(cost, minPosition));
				if (full) std::cout << "Искомый элемент: С(" << minPosition.first + 1 << ", " << minPosition.second + 1 << "), его запасы =" << A[minPosition.first] << ", потребности =" << B[minPosition.second] << std::endl;
				A[minPosition.first] = A[minPosition.first] - cost;
				B[minPosition.second] = B[minPosition.second] - cost;
				minCost = maxCost;
				if (full) std::cout << "Поскольку минимальным является " << cost << ", то вычитаем его" << std::endl << std::endl;
			}
			else			//если реал А и В неисчерпаны
			{
				for (int i = 0; i < A.size() - 1; i++)
				{
					for (int j = 0; j < B.size(); j++)
					{
						if (notInBasis(i, j))
						{
							if (A[i] != 0 && B[j] != 0)
							{
								if (C[i][j] < minCost)
								{
									minCost = C[i][j];
									minPosition.first = i;
									minPosition.second = j;
								}
								if (C[i][j] > maxCost) maxCost = C[i][j] + 1;
							}
						}
					}
				}
				cost = std::min(A[minPosition.first], B[minPosition.second]);		//можно min заменить на:  !(b<a)?a:b
				basisPerem_e.push_back(std::make_pair(cost, minPosition));
				if (full) std::cout << "Искомый элемент: С(" << minPosition.first + 1 << ", " << minPosition.second + 1 << "), его запасы =" << A[minPosition.first] << ", потребности =" << B[minPosition.second] << std::endl;
				A[minPosition.first] = A[minPosition.first] - cost;
				B[minPosition.second] = B[minPosition.second] - cost;
				minCost = maxCost;
				if (full) std::cout << "Поскольку минимальным является " << cost << ", то вычитаем его" << std::endl << std::endl;
			}
			flagA = true;
			flagB = true;
		}
	}

	else if (addedC.second == true)					// добавили пункт В
	{
		minCost = C[0][0];
		maxCost = C[0][0];
		for (int k = 0; k < (A.size() + B.size() - 1); k++) //кол-во итераций = нужное кол-во базисных клеток
		{
			for (int i = 0; i < A.size(); i++) if (A[i] != 0) flagA = false;
			for (int j = 0; j < B.size(); j++) if (B[j] != 0) flagB = false;
			if (flagA && flagB)
			{
				for (int i = 0; i < A.size(); i++)
				{
					for (int j = 0; j < B.size() - 1; j++)
					{
						if (notInBasis(i, j))
						{
							if (C[i][j] < minCost)
							{
								minCost = C[i][j];
								minPosition.first = i;
								minPosition.second = j;
							}
							if (C[i][j] > maxCost) maxCost = C[i][j] + 1;
						}
					}
				}
				cost = std::min(A[minPosition.first], B[minPosition.second]);		//можно min заменить на:  !(b<a)?a:b
				basisPerem_e.push_back(std::make_pair(cost, minPosition));
				if (full) std::cout << "Искомый элемент: С(" << minPosition.first + 1 << ", " << minPosition.second + 1 << "), его запасы =" << A[minPosition.first] << ", потребности =" << B[minPosition.second] << std::endl;
				A[minPosition.first] = A[minPosition.first] - cost;
				B[minPosition.second] = B[minPosition.second] - cost;
				minCost = maxCost;
				if (full) std::cout << "Поскольку минимальным является " << cost << ", то вычитаем его" << std::endl << std::endl;
				continue;
			}

			bool flag = true;
			for (int b = 0; b < B.size() - 1; b++)
			{
				if (B[b] != 0) flag = false;
			}

			if (flag)
			{
				for (int i = 0; i < A.size(); i++)
				{
					if (A[i] != 0 && B[B.size() - 1] != 0)
					{
						if (notInBasis(i, B.size() - 1))
						{
							if (C[i][B.size() - 1] < minCost)
							{
								minCost = C[i][B.size() - 1];
								minPosition.first = i;
								minPosition.second = B.size() - 1;
							}
							if (C[i][B.size() - 1] > maxCost) maxCost = C[i][B.size() - 1] + 1;
						}
					}
				}

				cost = std::min(A[minPosition.first], B[minPosition.second]);		//можно min заменить на:  !(b<a)?a:b
				basisPerem_e.push_back(std::make_pair(cost, minPosition));
				if (full) std::cout << "Искомый элемент: С(" << minPosition.first + 1 << ", " << minPosition.second + 1 << "), его запасы =" << A[minPosition.first] << ", потребности =" << B[minPosition.second] << std::endl;
				A[minPosition.first] = A[minPosition.first] - cost;
				B[minPosition.second] = B[minPosition.second] - cost;
				minCost = maxCost;
				if (full) std::cout << "Поскольку минимальным является " << cost << ", то вычитаем его" << std::endl << std::endl;
			}
			else
			{
				for (int i = 0; i < A.size(); i++)
				{
					for (int j = 0; j < B.size() - 1; j++)
					{
						if (A[i] != 0 && B[j] != 0)
						{
							if (notInBasis(i, j))
							{
								if (C[i][j] < minCost)
								{
									minCost = C[i][j];
									minPosition.first = i;
									minPosition.second = j;
								}
								if (C[i][j] > maxCost) maxCost = C[i][j] + 1;
							}
						}
					}
				}
				cost = std::min(A[minPosition.first], B[minPosition.second]);		//можно min заменить на:  !(b<a)?a:b
				basisPerem_e.push_back(std::make_pair(cost, minPosition));
				if (full) std::cout << "Искомый элемент: С(" << minPosition.first + 1 << ", " << minPosition.second + 1 << "), его запасы =" << A[minPosition.first] << ", потребности =" << B[minPosition.second] << std::endl;
				A[minPosition.first] = A[minPosition.first] - cost;
				B[minPosition.second] = B[minPosition.second] - cost;
				minCost = maxCost;
				if (full) std::cout << "Поскольку минимальным является " << cost << ", то вычитаем его" << std::endl << std::endl;
			}
			flagA = true;
			flagB = true;
		}
	}

	else											// задача закрытая
	{
		minCost = C[0][0];
		maxCost = C[0][0];
		for (int k = 0; k < (A.size() + B.size() - 1); k++) //кол-во итераций = нужное кол-во базисных клеток
		{
			for (int i = 0; i < A.size(); i++) if (A[i] != 0) flagA = false;
			for (int j = 0; j < B.size(); j++) if (B[j] != 0) flagB = false;
			if (flagA && flagB)
			{
				for (int i = 0; i < A.size(); i++)
				{
					for (int j = 0; j < B.size(); j++)
					{
						if (notInBasis(i, j))
						{
							if (C[i][j] < minCost)
							{
								minCost = C[i][j];
								minPosition.first = i;
								minPosition.second = j;
							}
							if (C[i][j] > maxCost) maxCost = C[i][j] + 1;
						}
					}
				}
				cost = std::min(A[minPosition.first], B[minPosition.second]);		//можно min заменить на:  !(b<a)?a:b
				basisPerem_e.push_back(std::make_pair(cost, minPosition));
				if (full) std::cout << "Искомый элемент: С(" << minPosition.first + 1 << ", " << minPosition.second + 1 << "), его запасы =" << A[minPosition.first] << ", потребности =" << B[minPosition.second] << std::endl;
				A[minPosition.first] = A[minPosition.first] - cost;
				B[minPosition.second] = B[minPosition.second] - cost;
				minCost = maxCost;
				if (full) std::cout << "Поскольку минимальным является " << cost << ", то вычитаем его" << std::endl << std::endl;
				continue;
			}

			for (int i = 0; i < A.size(); i++)
			{
				for (int j = 0; j < B.size(); j++)
				{
					if (A[i] != 0 && B[j] != 0)
					{
						if (notInBasis(i, j))
						{
							if (C[i][j] < minCost)
							{
								minCost = C[i][j];
								minPosition.first = i;
								minPosition.second = j;
							}
							if (C[i][j] > maxCost) maxCost = C[i][j] + 1;
						}
					}
				}
			}
			cost = std::min(A[minPosition.first], B[minPosition.second]);		//можно min заменить на:  !(b<a)?a:b
			basisPerem_e.push_back(std::make_pair(cost, minPosition));
			if (full) std::cout << "Искомый элемент: С(" << minPosition.first + 1 << ", " << minPosition.second + 1 << "), его запасы =" << A[minPosition.first] << ", потребности =" << B[minPosition.second] << std::endl;
			A[minPosition.first] = A[minPosition.first] - cost;
			B[minPosition.second] = B[minPosition.second] - cost;
			minCost = maxCost;
			if (full) std::cout << "Поскольку минимальным является " << cost << ", то вычитаем его" << std::endl << std::endl;
			flagA = true;
			flagB = true;
		}
	}

	//вывод опорного плана
	if (full)
	{
		bool printed = false;
		std::cout << "Мы получили начальный опорный план: " << std::endl;
		for (int i = 0; i < C.size(); i++)
		{
			for (int j = 0; j < C[0].size(); j++)
			{
				for (int b = 0; b < basisPerem_e.size(); b++)
				{
					if (basisPerem_e[b].second.first == i && basisPerem_e[b].second.second == j)
					{
						std::cout << C[i][j] << "[" << basisPerem_e[b].first << "] ";
						printed = true;
					}
				}
				if (printed)
				{
					printed = false;
					continue;
				}
				else std::cout << C[i][j] << "     ";
			}
			std::cout << std::endl;
		}

		std::cout << std::endl << "Базисных переменных должно быть m+n-1=" << C.size() + C[0].size() - 1 << ", их " << basisPerem_e.size() << std::endl;
		if ((C.size() + C[0].size() - 1) == basisPerem_e.size()) std::cout << "Следовательно, опорный план является невырожденным" << std::endl;
		else
		{
			std::cout << "Следовательно, опорный план является вырожденным" << std::endl;
			//такого произойти не должно
		}
	}
}

template <typename T>
bool Transportation<T>::notInBasis(int i, int j)
{
	for (int b = 0; b < basisPerem_e.size(); b++)
	{
		if (basisPerem_e[b].second.first == i && basisPerem_e[b].second.second == j) return false;
	}
	return true;
}

template <typename T>
void Transportation<T>::loopBuild() 
{
	T tmpMaxMin;
	std::pair<int, int> positionLoopBegin, positionFreeCell;
	bool oddSign, looped;																//знак нечетности для чередования + и - в цикле: true=минус, false=плюс
	std::vector<std::vector<int>> loopMap(C.size(), std::vector <int>(C[0].size()));	//в loopMap эл-ты: все нули, минус =-1, плюс =1

	tmpMaxMin = C[badC[0].first][badC[0].second];
	positionLoopBegin = std::make_pair(badC[0].first, badC[0].second);

	if (badC.capacity() == 1)
	{
		if (full) std::cout << "Данная клетка: " << tmpMaxMin << " (" << positionLoopBegin.first + 1 << ", " << positionLoopBegin.second + 1 << "). Строим цикл, начиная с нее" << std::endl;
	}
	else
	{
		if (full) std::cout << "Эти клетки: ";
		tmpMaxMin = badC_Value[0];
		if (full) std::cout << C[badC[0].first][badC[0].second] << " (" << badC[0].first + 1 << ", " << badC[0].second + 1 << ")  ";
		for (int i = 1; i < badC.size(); i++)
		{
			if (full) std::cout << C[badC[i].first][badC[i].second] << " (" << badC[i].first + 1 << ", " << badC[i].second + 1 << ")  ";
			if (tmpMaxMin < badC_Value[i])
			{
				tmpMaxMin = badC_Value[i];
				positionLoopBegin = std::make_pair(badC[i].first, badC[i].second);
			}
		}
		if (full) std::cout << std::endl << "Выбираем из них максимальную: " << tmpMaxMin << " (" << positionLoopBegin.first + 1 << ", " << positionLoopBegin.second + 1 << "). Строим цикл, начиная с нее" << std::endl;
		tmpMaxMin = C[positionLoopBegin.first][positionLoopBegin.second];
	}

	loopMap[positionLoopBegin.first][positionLoopBegin.second] = 1;
	oddSign = true;
	basisPerem_e.push_back(std::make_pair(0, positionLoopBegin));
	std::vector<std::pair<int, int>> path;

	for (int i = 0; i < basisPerem_e.size() - 1; i++)
	{
		loopMap[basisPerem_e[i].second.first][basisPerem_e[i].second.second] = 8;	//отмечаем на карте все базисные клетки магическим числом 8, нужно для построения рекурсивного цикла (одно из условий проверяет клетку)
	}
	loopMap[basisPerem_e.back().second.first][basisPerem_e.back().second.second] = 5;	//магическое число для первой клетки в цикле - 5

	int maxIter = C.size()*C[0].size();
	path.clear();
	pathHorizontal(positionLoopBegin, path, loopMap, maxIter);

	//отмечаем клетки цикла
	for (int p = 0; p < path.size(); p++)
	{
		if (!oddSign) loopMap[path[p].first][path[p].second] = -1;
		else loopMap[path[p].first][path[p].second] = 1;
		oddSign = !oddSign;
	}

	//выводим массив, где рисуется цикл
	if (full)
	{
		std::cout << std::endl << "Полученный цикл:" << std::endl;
		for (int i = 0; i < loopMap.size(); i++)
		{
			for (int j = 0; j < loopMap[0].size(); j++)
			{
				switch (loopMap[i][j])
				{
				case -1:
					std::cout << "[-] ";
					break;
				case 1:
					std::cout << "[+] ";
					break;
				default:
					std::cout << " .  ";
					break;
				}
			}
			std::cout << std::endl;
		}
	}

	//находим минимальный из басисных клеток с минусом
	for (int m = 0; m < basisPerem_e.size(); m++)
	{
		if (basisPerem_e[m].second.first == path[1].first && basisPerem_e[m].second.second == path[1].second)
		{
			tmpMaxMin = basisPerem_e[m].first;
			positionFreeCell = std::make_pair(basisPerem_e[m].second.first, basisPerem_e[m].second.second);
		}
	}

	for (int i = 0; i < loopMap.size(); i++)
	{
		for (int j = 0; j < loopMap[0].size(); j++)
		{
			if (loopMap[i][j] == -1)
			{
				for (int b = 0; b < basisPerem_e.size(); b++)
				{
					if (basisPerem_e[b].second.first == i && basisPerem_e[b].second.second == j && basisPerem_e[b].first < tmpMaxMin)
					{
						tmpMaxMin = basisPerem_e[b].first;
						positionFreeCell = std::make_pair(basisPerem_e[b].second.first, basisPerem_e[b].second.second);
					}
				}
			}
		}
	}

	if (full)
	{
		std::cout << std::endl << "Из клеток с [-] выбираем наименьшую, то есть " << tmpMaxMin << " (" << positionFreeCell.first + 1 << ", " << positionFreeCell.second + 1 << ")" << std::endl;
		std::cout << "Прибавляем " << tmpMaxMin << " к клеткам с [+] и вычитаем из клеток с [-]" << std::endl;
	}

	//изменяем значение базис-х клеток, попавших в цикл (+/-)
	for (int i = 0; i < loopMap.size(); i++)
	{
		for (int j = 0; j < loopMap[0].size(); j++)
		{
			if (loopMap[i][j] == -1)
			{
				for (int b = 0; b < basisPerem_e.size(); b++)
					if (basisPerem_e[b].second.first == i && basisPerem_e[b].second.second == j)
					{
						basisPerem_e[b].first = basisPerem_e[b].first - tmpMaxMin;
					}
			}

			if (loopMap[i][j] == 1)
			{
				for (int b = 0; b < basisPerem_e.size(); b++)
					if (basisPerem_e[b].second.first == i && basisPerem_e[b].second.second == j)
					{
						basisPerem_e[b].first = basisPerem_e[b].first + tmpMaxMin;
					}
			}
		}
	}

	//удаляем клетку освободившуюся из базисных
	for (int b = 0; b < basisPerem_e.size(); b++)
		if (basisPerem_e[b].second.first == positionFreeCell.first && basisPerem_e[b].second.second == positionFreeCell.second)
		{
			basisPerem_e.erase(basisPerem_e.begin() + b);
			loopMap[positionFreeCell.first][positionFreeCell.second] = 0;
		}

	if (full)
	{
		bool printed = false;
		std::cout << std::endl << "В результате получим новый опорный план:" << std::endl;
		for (int i = 0; i < C.size(); i++)
		{
			for (int j = 0; j < C[0].size(); j++)
			{
				for (int b = 0; b < basisPerem_e.size(); b++)
				{
					if (basisPerem_e[b].second.first == i && basisPerem_e[b].second.second == j)
					{
						std::cout << C[i][j] << "[" << basisPerem_e[b].first << "] ";
						printed = true;
					}
				}

				if (printed)
				{
					printed = false;
					continue;
				}
				else std::cout << C[i][j] << "     ";
			}
			std::cout << std::endl;
		}
	}
}

template <typename T>
bool Transportation<T>::pathHorizontal(std::pair<int, int> position, std::vector<std::pair<int, int>>& path, std::vector<std::vector<int>> loopMap, int maxIter)
{
	maxIter = maxIter--;
	if (maxIter == 0)
	{
		return false;
	}
	for (int j = 0; j < loopMap[0].size(); j++)
	{
		if (position.second == j) continue;
		if (loopMap[position.first][j] != 8) continue;		//если клетка не является базисной
		if (pathVertical(std::make_pair(position.first, j), path, loopMap, maxIter))
		{
			path.push_back(std::make_pair(position.first, j));
			return true;
		}
	}
	return false;
}

template <typename T>
bool Transportation<T>::pathVertical(std::pair<int, int> position, std::vector<std::pair<int, int>>& path, std::vector<std::vector<int>> loopMap, int maxIter)
{
	for (int i = 0; i < loopMap.size(); i++)
	{
		if (loopMap[i][position.second] == 5)		//если вернулись в клетку, с которой начали строить цикл
		{
			path.push_back(std::make_pair(i, position.second));
			return true;
		}
		if (i == position.first) continue;
		if (loopMap[i][position.second] == 0) continue;
		if (pathHorizontal(std::make_pair(i, position.second), path, loopMap, maxIter))
		{
			path.push_back(std::make_pair(i, position.second));
			return true;
		}
	}
	return false;
}