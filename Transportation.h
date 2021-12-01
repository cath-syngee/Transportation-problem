#pragma once
#include <vector>

template <class T> //��� ������ ����� ������� �������� (���� ����� � ����������� ����)
class Transportation
{
private:
	std::vector<T> A, B;						//������ ������-�� � ������-� � �������
	T sumA;										//��������� ������ �����-�� � ������-�
	T sumB;
	std::vector<std::vector<T>> C;				//������� ������������ ��������
	T L;										//�������� ������� �������
	int amountA, amountB;
	bool optPlan;								//��� �������� �� �������������
	std::pair <bool, bool> addedC;				//��� �����������, ����� ������ ���������� � �
	bool full;									//full - ����� ������� �������

	std::vector<std::pair<T, std::pair<int, int>>> basisPerem_e;	//������ �����-� �����-�, 1- ����-��� �����-�, 2- ������� � ������� �
	std::vector<std::pair<int, int>> badC;							//������ ��������� ������������� ������
	std::vector<T> badC_Value;

	//��������� �������
	void solve();								//����� ������� ������					��� ����������? � ��� ��������? � ����� ���� ������ (������� �� int'�� ��� ���)?
	bool potentialMethod();						//���������� ������������ ����� ������� �����������
	void minElement();							//���������� ���������� �������� ����� ������� ������������ ��������
	bool notInBasis(int i, int j);				//�������� �� ������� [i,j] �������� ����������
	void loopBuild();							//���������� �����
	bool pathHorizontal(std::pair<int, int>, std::vector<std::pair<int, int>>&, std::vector<std::vector<int>>, int);
	bool pathVertical(std::pair<int, int>, std::vector<std::pair<int, int>>&, std::vector<std::vector<int>>, int);

public:
	Transportation(std::vector<T> A, std::vector<T> B, std::vector<std::vector<T>> C, bool full_out = false);
	T getL();
	std::vector<std::pair<T, std::pair<int, int>>> getBasis();
	std::vector<std::vector<T>> getC();
};