#pragma once

#include "stdafx.h"

using namespace System;
using namespace System::Drawing;

namespace Melnik_Kursova 
{
	ref class CustomCircle
	{
	private:
		Rectangle describedRectangle;
		int radius;
		Point center;
		Color circleColor;
		float distanceToPolyline;

		
		bool isObtuseAngle(Point^ A, Point^ B);

		float countVectorAbs(Point^ pA, Point^ pB);
		void createDescribedRectangle();
	public:

		CustomCircle(void);
		CustomCircle(Point center, int radius);

		void findDistanceToPolyine(Array ^polyline);
		void intersectionWithPolyine(Array ^polyline);
		void drawCircle(System::Windows::Forms::PaintEventArgs^ e);

		bool isEquals(CustomCircle^ circle);
		Point getCenter();
		int getRadius();
		Color getColor();
		void setColor(Color color);
		float getDistanceToPolyline();
	};
}
