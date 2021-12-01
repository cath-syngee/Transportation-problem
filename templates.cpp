//�������� ����, ��� ���������� ������ ������ ����������� ������� ������ Transportation
#include "Transportation.h"
#include "Transportation.cpp"				//�������� ������� �������� ���� � ����������������, �� ������ � ���� ����� (����� ������ ���������� ������ �� ��������������)

template class Transportation<int>;			//���� ������� ��������� ������� ������ Transportation<int>
template class Transportation<double>;		//���� ������� ��������� ������� ������ Transportation<double>



//������ ������������� ������ Transportation, ������� � ����� ����������� - ��� �������; �� ��� ���� �������� � �������
#include <vector>
#include <iostream>
int main() 
{
	setlocale(LC_ALL, "Russian");
	std::vector<double> A = { 94,85,36,11 };
	std::vector<double> B = { 23,17,15,19,40,14 };
	std::vector<std::vector<double>> C = { {6,5,6,4,2,3},{8,7,5,3,3,2}, {8,9,8,7,2,6}, {8,7,3,5,4,1} };
	double L;
	std::vector<std::pair<double, std::pair<int, int>>> basis;		//������ �������� ����������
																	//���������: (��������, <i, j>)
	Transportation<double> problem(A, B, C);
	L = problem.getL();
	std::cout << std::endl << "result = " << L << std::endl;		//441
	basis = problem.getBasis();
	C = problem.getC();												//���������� ������� �

	std::cout << "����������� ���� ���������:" << std::endl;
	for (int i = 0; i < C.size(); i++)
	{
		std::cout << "�� " << i + 1 << "-�� ������ ������������ ���������� ������� ��������� � ";
		for (int j = 0; j < basis.size(); j++)
		{
			if (basis[j].second.first == i && C[basis[j].second.first][basis[j].second.second] != 0) std::cout << basis[j].second.second + 1 << "-� ����� ����������� (" << basis[j].first << " ������), ";
		}
		std::cout << std::endl;
	}
	for (int j = 0; j < basis.size(); j++)
	{
		if (C[basis[j].second.first][basis[j].second.second] == 0) std::cout << "�� " << basis[j].second.first + 1 << "-�� ������ ������� ���������������� ������� � ���������� " << basis[j].first << " ������." << std::endl;
	}

	system("pause");
	return 0;
}