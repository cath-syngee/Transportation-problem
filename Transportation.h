#pragma once
#include <vector>

template <class T> //для разных типов входных векторов (наши даблы и стандартные инты)
class Transportation
{
private:
	std::vector<T> A, B;						//обьемы произв-ва и потреб-я в пунктах
	T sumA;										//суммарные объемы произ-ва и потреб-я
	T sumB;
	std::vector<std::vector<T>> C;				//матрица транспортных издержек
	T L;										//значение целевой функции
	int amountA, amountB;
	bool optPlan;								//для проверки на оптимальность
	std::pair <bool, bool> addedC;				//для определения, какие пункты добавились в С
	bool full;									//full - вывод полного решения

	std::vector<std::pair<T, std::pair<int, int>>> basisPerem_e;	//вектор базис-х перем-х, 1- знач-ние перем-й, 2- позиция в массиве С
	std::vector<std::pair<int, int>> badC;							//вектор координат неоптимальных клеток
	std::vector<T> badC_Value;

	//прототипы методов
	void solve();								//метод решения задачи					что возвращает? и что передаем? в каком виде данные (таблица из int'ов или нет)?
	bool potentialMethod();						//нахождение оптимального плана методом потенциалов
	void minElement();							//нахождение начального опорного плана методом минимального элемента
	bool notInBasis(int i, int j);				//является ли элемент [i,j] базисной переменной
	void loopBuild();							//построение цикла
	bool pathHorizontal(std::pair<int, int>, std::vector<std::pair<int, int>>&, std::vector<std::vector<int>>, int);
	bool pathVertical(std::pair<int, int>, std::vector<std::pair<int, int>>&, std::vector<std::vector<int>>, int);

public:
	Transportation(std::vector<T> A, std::vector<T> B, std::vector<std::vector<T>> C, bool full_out = false);
	T getL();
	std::vector<std::pair<T, std::pair<int, int>>> getBasis();
	std::vector<std::vector<T>> getC();
};