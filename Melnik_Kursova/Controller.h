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
		Array ^circles;//масив точок
		Array ^polyline;//масив точок ламаної
		Array ^theSmallest;//масив трьох точок, які найближче до ламаної
		Array ^theBiggest;//масив трьох точок, які найдалі від ламаної

		void createCirclesCoordFromFile(string path);//створити кола зі зчитаних з файлу точок
		void createPolylineCoordFromFile(string path);//створити точки ламаної зі зчитаних з файлу точок
		void findThreeTheSmallestAndTheBiggestCircles();//знайти три найбільше та найменше віддалених від ламаної точки
		void findParametersInLine(string line, int* parameters, const int paramSize);//знайти параметри (координати) в стрічці з файлу
		vector<int*> readFromFile(const int paramSize, string path);//зчитує точки з файлу
		bool contains(Array ^arr, CustomCircle ^nextCircle, int size);//чи є така точка в масиві
		void findDistanceFromCirclesCentersToPolyline();//знайти відстаньвід  усіх точок до ламаної
		void findCirclesIntersection();//знайти які кола перетинаються з ламаною
		void findTrianglesIntersection();//перевірити чи перетинаютсья трикутники
		bool isIntersectionOfTwoLines(Point^ A1, Point^ A2, Point^ B1, Point^ B2);//чи перетинаються два відрізки
		void findTheBiggestCircleIntersection();//знайти найбільше коло, яке перетинається з ламаною
				
	public:
		String^ messageAboutTrianglesIntersection;

		Controller(void);

		void drawTriangles(PaintEventArgs^ e);
		void drawCircles(PaintEventArgs^ e);
		void drawPolyline(PaintEventArgs^ e);
	};
}