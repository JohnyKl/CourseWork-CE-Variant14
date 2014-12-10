#pragma once

#include "stdafx.h"
#include "CustomCircle.h"

using namespace System;
using namespace System::Drawing;
using namespace std;
using namespace System::Windows::Forms;

namespace Melnik_Kursova 
{
	ref class Controller
	{
		Array ^circles;//����� �����
		Array ^polyline;//����� ����� ������
		Array ^theSmallest;//����� ����� �����, �� ��������� �� ������
		Array ^theBiggest;//����� ����� �����, �� ������ �� ������

		void createCirclesCoordFromFile(string path);//�������� ���� � �������� � ����� �����
		void createPolylineCoordFromFile(string path);//�������� ����� ������ � �������� � ����� �����
		void findThreeTheSmallestAndTheBiggestCircles();//������ ��� �������� �� �������� ��������� �� ������ �����
		void findParametersInLine(string line, int* parameters, const int paramSize);//������ ��������� (����������) � ������ � �����
		vector<int*> readFromFile(const int paramSize, string path);//����� ����� � �����
		bool contains(Array ^arr, CustomCircle ^nextCircle, int size);//�� � ���� ����� � �����
		void findDistanceFromCirclesCentersToPolyline();//������ ���������  ��� ����� �� ������
		void findCirclesIntersection();//������ �� ���� ������������� � �������
		void findTrianglesIntersection();//��������� �� ������������� ����������
		bool isIntersectionOfTwoLines(Point^ A1, Point^ A2, Point^ B1, Point^ B2);//�� ������������� ��� ������
		void findTheBiggestCircleIntersection();//������ �������� ����, ��� ������������ � �������
				
	public:
		String^ messageAboutTrianglesIntersection;

		Controller(void);

		void drawTriangles(PaintEventArgs^ e);
		void drawCircles(PaintEventArgs^ e);
		void drawPolyline(PaintEventArgs^ e);
	};
}