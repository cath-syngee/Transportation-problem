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
		if (full) std::cout << std::endl << "������ ��������, ��� ��� ������� ������� ��������" << std::endl;
		if (sumA > sumB)
		{
			if (full) std::cout << "C�������� ����� ������������ sumA = " << sumA << ", � ��������� ����� ����������� sumB = " << sumB << std::endl;
			if (full) std::cout << "����� �������� �������� ������, ��������� ��������� ����� ����������� B(j+1) � ������� ����������� = " << (sumA - sumB) << std::endl;
			B.push_back(sumA - sumB);
			for (int i = 0; i < amountA; i++)
			{
				C[i].push_back(0);
			}
			addedC.second = true;
		}
		else
		{
			if (full) std::cout << "C�������� ����� ������������ sumA = " << sumA << ", � ��������� ����� ����������� sumB = " << sumB << std::endl;
			if (full) std::cout << "����� �������� �������� ������, ��������� ��������� ����� ������������ A(i+1) � ������� ������������ = " << (sumB - sumA) << std::endl;
			A.push_back(sumB - sumA);
			std::vector<T> c_newB(amountB, 0);
			C.push_back(c_newB);
			addedC.first = true;
		}
	}
	else
	{
		if (full) std::cout << std::endl << "������ ��������, ������� ������� �����������, ��� ��� ��������� ������ ������������ � ����������� ����� (=" << sumA << ")" << std::endl;
	}

	if (full)
	{
		std::cout << std::endl << "������� �������:" << std::endl;
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

		std::cout << std::endl << "������� ������������ �������� (�):" << std::endl;
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

	minElement();												//��������� ������� ����
	if (full)
	{
		for (int i = 0; i < basisPerem_e.size(); i++)
		{
			L += basisPerem_e[i].first * C[basisPerem_e[i].second.first][basisPerem_e[i].second.second];
		}
		std::cout << std::endl << "�������� ������� ������� ��� ����� �������� �����: L = " << L << std::endl;
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
	if (full) std::cout << std::endl << std::endl << "���������� ������� ���� �������� �����������, ��� ��� ��� ���� ������ ����������� ������� `ai + `bj = cij" << std::endl;
	for (int i = 0; i < basisPerem_e.size(); i++)
	{
		L += basisPerem_e[i].first * C[basisPerem_e[i].second.first][basisPerem_e[i].second.second];
	}
	std::cout << std::endl << "����������� ������� ��������: L = " << L << std::endl;

	if (!full)
	{
		std::cout << std::endl << "����������� ���� ���������:" << std::endl;
		for (int i = 0; i < C.size(); i++)
		{
			std::cout << "�� " << i + 1 << "-�� ������ ������������ ���������� ������� ��������� � ";
			for (int j = 0; j < basisPerem_e.size(); j++)
			{
				if (basisPerem_e[j].second.first == i && C[basisPerem_e[j].second.first][basisPerem_e[j].second.second] != 0) std::cout << basisPerem_e[j].second.second + 1 << "-� ����� ����������� (" << basisPerem_e[j].first << " ������), ";
			}
			std::cout << std::endl;
		}
		for (int j = 0; j < basisPerem_e.size(); j++)
		{
			if (C[basisPerem_e[j].second.first][basisPerem_e[j].second.second] == 0) std::cout << "�� " << basisPerem_e[j].second.first + 1 << "-�� ������ ������� ���������������� ������� � ���������� " << basisPerem_e[j].first << " ������." << std::endl;
		}
	}
}

template <typename T>
bool Transportation<T>::potentialMethod()
{
	int sizeA = C.size();
	int sizeB = C[0].size();
	std::vector<std::pair<int, bool>> alpha, beta;		//������� ����������� � �� ���������� � ��������� �� ���
	int count = sizeA + sizeB - 1;

	alpha.resize(sizeA);
	beta.resize(sizeB);
	alpha[0].second = true;								//a1=0

	if (full) std::cout << std::endl << "�������� ������������� �������� �����. ������ ��������������� ���������� `Ai � `Bj �� ������� ������� �������, � ������� `Ai+`Bj=Cij, �������, ��� `A1=0" << std::endl;

	int max_iter = count;	//���� ������������� �� ����� ������-� ����-� ����������� -> ������ ��
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

		//��� �������� �� ������������� �������
		max_iter--;
		if (max_iter == 0)
		{
			if (full)
			{
				std::cout << "�� ������ ����� �������� ��������, ����� ��� ���������� ������� ������ �� �������� �� ������ �� �����������.";
				std::cout << "��� ��������� ������������� �������. ��� ��� ����������� � ���� �� ������ ����� ������ ������� ��������, ����� �������, ����� ������ ������ ������� �������.";
			}

			//�������� �� ��� �����/��������, � ������� ���� ����������, ������ � ���������� ���������� � ������� �
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
				std::cout << std::endl << "����� ������, � ������� ����� ���� ��������� ������� ��������, ���������� ����� ����� ������ " << min_cost << " (" << temp_i + 1 << ", " << temp_j + 1 << "). � ��� ��������� ������� �������� � ��� ���������� ������� �������.";
			}

			for (int i = 0; i < basisPerem_e.size(); i++)
			{
				if (basisPerem_e[i].first == 0)
				{
					if (full) std::cout << std::endl << "����� ������������ ���������� � ������ (" << basisPerem_e[i].second.first + 1 << ", " << basisPerem_e[i].second.second + 1 << ") �������." << std::endl;
					basisPerem_e.erase(basisPerem_e.begin() + i);
				}
			}
			basisPerem_e.push_back(std::make_pair(0, std::make_pair(temp_i, temp_j)));
			max_iter = count;
		}
	}

	//����� �����������:
	if (full)
	{
		std::cout << std::endl << "`a: ";
		for (int i = 0; i < sizeA; i++) std::cout << alpha[i].first << "  ";
		std::cout << std::endl << "`b: ";
		for (int i = 0; i < sizeB; i++) std::cout << beta[i].first << "  ";

	}

	bool flag = true;		//������������� �����
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
	if (full) std::cout << std::endl << "����������� ������� ��������: L = " << L << std::endl;

	if (!badC.empty() && full) std::cout << std::endl << "������� ���� �� ���������, ��� ��� ���������� ������, ��� ������� `ai + `bj > cij" << std::endl;

	return flag;
}

template <typename T>
void Transportation<T>::minElement()
{
	if (full) std::cout << "����� ���������� �������� ����� � ������� ������ ������������ ��������." << std::endl;
	if (full) std::cout << "���� ������ � ���, ��� �� ������� ���������� � ���������� ����������, � � ������, ��������������� ��, ���������� ������� �� ai � bj" << std::endl;

	T minCost, maxCost, cost;						//cost - ��������������� ������� ������ C~
	std::pair<int, int> minPosition;
	bool flagA = true;
	bool flagB = true;								//�����, ��� ��������� ������� �� ����

	if (addedC.first == true)						//�������� ����� �
	{
		minCost = C[0][0];
		maxCost = C[0][0];
		for (int k = 0; k < (A.size() + B.size() - 1); k++) //���-�� �������� = ������ ���-�� �������� ������
		{
			for (int i = 0; i < A.size(); i++) if (A[i] != 0) flagA = false;
			for (int j = 0; j < B.size(); j++) if (B[j] != 0) flagB = false;

			if (flagA && flagB)		//��� ������� ���������, ���� ��������
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

				if (full) std::cout << "������� �������: �(" << minPosition.first + 1 << ", " << minPosition.second + 1 << "), ��� ������ =" << A[minPosition.first] << ", ����������� =" << B[minPosition.second] << std::endl;
				minCost = maxCost;
				if (full) std::cout << "��������� ����������� �������� " << cost << ", �� �������� ���" << std::endl << std::endl;
				continue;
			}

			bool flag = true;
			for (int a = 0; a < A.size() - 1; a++)
			{
				if (A[a] != 0) flag = false;
			}

			if (flag && !flagB)		//���� ��� �������� � ���������, � � �� ���������
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

				cost = std::min(A[minPosition.first], B[minPosition.second]);		//����� min �������� ��:  !(b<a)?a:b
				basisPerem_e.push_back(std::make_pair(cost, minPosition));
				if (full) std::cout << "������� �������: �(" << minPosition.first + 1 << ", " << minPosition.second + 1 << "), ��� ������ =" << A[minPosition.first] << ", ����������� =" << B[minPosition.second] << std::endl;
				A[minPosition.first] = A[minPosition.first] - cost;
				B[minPosition.second] = B[minPosition.second] - cost;
				minCost = maxCost;
				if (full) std::cout << "��������� ����������� �������� " << cost << ", �� �������� ���" << std::endl << std::endl;
			}
			else			//���� ���� � � � �����������
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
				cost = std::min(A[minPosition.first], B[minPosition.second]);		//����� min �������� ��:  !(b<a)?a:b
				basisPerem_e.push_back(std::make_pair(cost, minPosition));
				if (full) std::cout << "������� �������: �(" << minPosition.first + 1 << ", " << minPosition.second + 1 << "), ��� ������ =" << A[minPosition.first] << ", ����������� =" << B[minPosition.second] << std::endl;
				A[minPosition.first] = A[minPosition.first] - cost;
				B[minPosition.second] = B[minPosition.second] - cost;
				minCost = maxCost;
				if (full) std::cout << "��������� ����������� �������� " << cost << ", �� �������� ���" << std::endl << std::endl;
			}
			flagA = true;
			flagB = true;
		}
	}

	else if (addedC.second == true)					// �������� ����� �
	{
		minCost = C[0][0];
		maxCost = C[0][0];
		for (int k = 0; k < (A.size() + B.size() - 1); k++) //���-�� �������� = ������ ���-�� �������� ������
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
				cost = std::min(A[minPosition.first], B[minPosition.second]);		//����� min �������� ��:  !(b<a)?a:b
				basisPerem_e.push_back(std::make_pair(cost, minPosition));
				if (full) std::cout << "������� �������: �(" << minPosition.first + 1 << ", " << minPosition.second + 1 << "), ��� ������ =" << A[minPosition.first] << ", ����������� =" << B[minPosition.second] << std::endl;
				A[minPosition.first] = A[minPosition.first] - cost;
				B[minPosition.second] = B[minPosition.second] - cost;
				minCost = maxCost;
				if (full) std::cout << "��������� ����������� �������� " << cost << ", �� �������� ���" << std::endl << std::endl;
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

				cost = std::min(A[minPosition.first], B[minPosition.second]);		//����� min �������� ��:  !(b<a)?a:b
				basisPerem_e.push_back(std::make_pair(cost, minPosition));
				if (full) std::cout << "������� �������: �(" << minPosition.first + 1 << ", " << minPosition.second + 1 << "), ��� ������ =" << A[minPosition.first] << ", ����������� =" << B[minPosition.second] << std::endl;
				A[minPosition.first] = A[minPosition.first] - cost;
				B[minPosition.second] = B[minPosition.second] - cost;
				minCost = maxCost;
				if (full) std::cout << "��������� ����������� �������� " << cost << ", �� �������� ���" << std::endl << std::endl;
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
				cost = std::min(A[minPosition.first], B[minPosition.second]);		//����� min �������� ��:  !(b<a)?a:b
				basisPerem_e.push_back(std::make_pair(cost, minPosition));
				if (full) std::cout << "������� �������: �(" << minPosition.first + 1 << ", " << minPosition.second + 1 << "), ��� ������ =" << A[minPosition.first] << ", ����������� =" << B[minPosition.second] << std::endl;
				A[minPosition.first] = A[minPosition.first] - cost;
				B[minPosition.second] = B[minPosition.second] - cost;
				minCost = maxCost;
				if (full) std::cout << "��������� ����������� �������� " << cost << ", �� �������� ���" << std::endl << std::endl;
			}
			flagA = true;
			flagB = true;
		}
	}

	else											// ������ ��������
	{
		minCost = C[0][0];
		maxCost = C[0][0];
		for (int k = 0; k < (A.size() + B.size() - 1); k++) //���-�� �������� = ������ ���-�� �������� ������
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
				cost = std::min(A[minPosition.first], B[minPosition.second]);		//����� min �������� ��:  !(b<a)?a:b
				basisPerem_e.push_back(std::make_pair(cost, minPosition));
				if (full) std::cout << "������� �������: �(" << minPosition.first + 1 << ", " << minPosition.second + 1 << "), ��� ������ =" << A[minPosition.first] << ", ����������� =" << B[minPosition.second] << std::endl;
				A[minPosition.first] = A[minPosition.first] - cost;
				B[minPosition.second] = B[minPosition.second] - cost;
				minCost = maxCost;
				if (full) std::cout << "��������� ����������� �������� " << cost << ", �� �������� ���" << std::endl << std::endl;
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
			cost = std::min(A[minPosition.first], B[minPosition.second]);		//����� min �������� ��:  !(b<a)?a:b
			basisPerem_e.push_back(std::make_pair(cost, minPosition));
			if (full) std::cout << "������� �������: �(" << minPosition.first + 1 << ", " << minPosition.second + 1 << "), ��� ������ =" << A[minPosition.first] << ", ����������� =" << B[minPosition.second] << std::endl;
			A[minPosition.first] = A[minPosition.first] - cost;
			B[minPosition.second] = B[minPosition.second] - cost;
			minCost = maxCost;
			if (full) std::cout << "��������� ����������� �������� " << cost << ", �� �������� ���" << std::endl << std::endl;
			flagA = true;
			flagB = true;
		}
	}

	//����� �������� �����
	if (full)
	{
		bool printed = false;
		std::cout << "�� �������� ��������� ������� ����: " << std::endl;
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

		std::cout << std::endl << "�������� ���������� ������ ���� m+n-1=" << C.size() + C[0].size() - 1 << ", �� " << basisPerem_e.size() << std::endl;
		if ((C.size() + C[0].size() - 1) == basisPerem_e.size()) std::cout << "�������������, ������� ���� �������� �������������" << std::endl;
		else
		{
			std::cout << "�������������, ������� ���� �������� �����������" << std::endl;
			//������ ��������� �� ������
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
	bool oddSign, looped;																//���� ���������� ��� ����������� + � - � �����: true=�����, false=����
	std::vector<std::vector<int>> loopMap(C.size(), std::vector <int>(C[0].size()));	//� loopMap ��-��: ��� ����, ����� =-1, ���� =1

	tmpMaxMin = C[badC[0].first][badC[0].second];
	positionLoopBegin = std::make_pair(badC[0].first, badC[0].second);

	if (badC.capacity() == 1)
	{
		if (full) std::cout << "������ ������: " << tmpMaxMin << " (" << positionLoopBegin.first + 1 << ", " << positionLoopBegin.second + 1 << "). ������ ����, ������� � ���" << std::endl;
	}
	else
	{
		if (full) std::cout << "��� ������: ";
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
		if (full) std::cout << std::endl << "�������� �� ��� ������������: " << tmpMaxMin << " (" << positionLoopBegin.first + 1 << ", " << positionLoopBegin.second + 1 << "). ������ ����, ������� � ���" << std::endl;
		tmpMaxMin = C[positionLoopBegin.first][positionLoopBegin.second];
	}

	loopMap[positionLoopBegin.first][positionLoopBegin.second] = 1;
	oddSign = true;
	basisPerem_e.push_back(std::make_pair(0, positionLoopBegin));
	std::vector<std::pair<int, int>> path;

	for (int i = 0; i < basisPerem_e.size() - 1; i++)
	{
		loopMap[basisPerem_e[i].second.first][basisPerem_e[i].second.second] = 8;	//�������� �� ����� ��� �������� ������ ���������� ������ 8, ����� ��� ���������� ������������ ����� (���� �� ������� ��������� ������)
	}
	loopMap[basisPerem_e.back().second.first][basisPerem_e.back().second.second] = 5;	//���������� ����� ��� ������ ������ � ����� - 5

	int maxIter = C.size()*C[0].size();
	path.clear();
	pathHorizontal(positionLoopBegin, path, loopMap, maxIter);

	//�������� ������ �����
	for (int p = 0; p < path.size(); p++)
	{
		if (!oddSign) loopMap[path[p].first][path[p].second] = -1;
		else loopMap[path[p].first][path[p].second] = 1;
		oddSign = !oddSign;
	}

	//������� ������, ��� �������� ����
	if (full)
	{
		std::cout << std::endl << "���������� ����:" << std::endl;
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

	//������� ����������� �� �������� ������ � �������
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
		std::cout << std::endl << "�� ������ � [-] �������� ����������, �� ���� " << tmpMaxMin << " (" << positionFreeCell.first + 1 << ", " << positionFreeCell.second + 1 << ")" << std::endl;
		std::cout << "���������� " << tmpMaxMin << " � ������� � [+] � �������� �� ������ � [-]" << std::endl;
	}

	//�������� �������� �����-� ������, �������� � ���� (+/-)
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

	//������� ������ �������������� �� ��������
	for (int b = 0; b < basisPerem_e.size(); b++)
		if (basisPerem_e[b].second.first == positionFreeCell.first && basisPerem_e[b].second.second == positionFreeCell.second)
		{
			basisPerem_e.erase(basisPerem_e.begin() + b);
			loopMap[positionFreeCell.first][positionFreeCell.second] = 0;
		}

	if (full)
	{
		bool printed = false;
		std::cout << std::endl << "� ���������� ������� ����� ������� ����:" << std::endl;
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
		if (loopMap[position.first][j] != 8) continue;		//���� ������ �� �������� ��������
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
		if (loopMap[i][position.second] == 5)		//���� ��������� � ������, � ������� ������ ������� ����
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