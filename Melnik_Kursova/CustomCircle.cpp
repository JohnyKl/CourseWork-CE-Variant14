#include "stdafx.h"
#include "CustomCircle.h"

using namespace Melnik_Kursova;

CustomCircle::CustomCircle(void)
{
	distanceToPolyline = FLT_MAX;
	radius = 0;
	center = Point();
	circleColor = Color::White;
	describedRectangle = Rectangle();
}

CustomCircle::CustomCircle(Point center, int radius)
{
	distanceToPolyline = FLT_MAX;
	this->radius = radius;
	this->center = center;
	this->circleColor = Color::Black;

	createDescribedRectangle();
}
void CustomCircle::createDescribedRectangle()
{
	int x = center.X - radius;
	int y = center.Y - radius;

	x = (x > 0) ? x:0;
	y = (y > 0) ? y:0;

	describedRectangle = Rectangle(x,y, radius*2, radius*2);
}


float CustomCircle::countVectorAbs(Point^ pA, Point^ pB)//підрахувати модуль вектора для знаходження його довжини
{
	float sqrX = (pB->X - pA->X)*(pB->X - pA->X);
	float sqrY = (pB->Y - pA->Y)*(pB->Y - pA->Y);

	return System::Math::Sqrt(sqrX + sqrY);
}

void CustomCircle::findDistanceToPolyine(Array ^polyline)
{
	float d = FLT_MAX;

	for(int k = 0; k < polyline->Length - 1; k++)
	{
		//отримати дві послідовні точки ламаної
		Point ^p1 = (Point^)polyline->GetValue(k);
		Point ^p2 = (Point^)polyline->GetValue(k + 1);

		if(isObtuseAngle(p1, p2))
			//якщо трикутник, утворений з центра кола та двох точок ламаної тупокутний, 
			//причому тупий кут прилягає до ламаної, то висота, проведена від центра кола
			//до прямої, яка проходить через ці дві точки, буде за межами відрізку.
			//в цьому випадку відстанню до ламаної буде менша відстань від центра до кожної з точок
		{
			float d1 = countVectorAbs(p1, gcnew Point(center.X, center.Y));
			float d2 = countVectorAbs(p2, gcnew Point(center.X, center.Y));

			if(d1 > d2) d = d2;
			else d = d1;
		}
		else//інакше за формулою знайти висоту від центра до відрізка
		{
			int A = p2->Y - p1->Y;
			int B = p1->X - p2->X;
			int C = p1->Y*p2->X - p1->X*p2->Y;

			d = Math::Abs(A*center.X + B*center.Y + C)/Math::Sqrt(A*A + B*B);
		}

		if(distanceToPolyline > d) distanceToPolyline = d;//якщо знайдена відстань уже менша за існуючу, оновити значення відстані
	}
}

bool CustomCircle::isObtuseAngle(Point^ A, Point^ B)//перевірити чи трикутник, утворений центром кола та вершинами тупокутний
{
	float sqrAB = countVectorAbs(A, B)*countVectorAbs(A, B);
	float sqrBC = countVectorAbs(B, gcnew Point(center.X, center.Y))*countVectorAbs(B, gcnew Point(center.X, center.Y));
	float sqrAC = countVectorAbs(A, gcnew Point(center.X, center.Y))*countVectorAbs(A, gcnew Point(center.X, center.Y));

	return sqrAC > (sqrAB + sqrBC) || sqrBC > (sqrAC + sqrAB);
}

void CustomCircle::intersectionWithPolyine(Array ^polyline)
{
	bool intersection = false;

	for(int k = 0; k < polyline->Length - 1 && !intersection; k++)
	{
		//для кожних двох послідовних точок
		Point ^p1 = (Point^)polyline->GetValue(k);
		Point ^p2 = (Point^)polyline->GetValue(k + 1);

		//за математичною формулою перевірити чи коло перетинається з відрізком
		int x1 = p1->X - center.X;
		int y1 = p1->Y - center.Y;
		int x2 = p2->X - center.X;
		int y2 = p2->Y - center.Y;
 
		double dx = x2 - x1;
		double dy = y2 - y1;
 
		double a = dx*dx + dy*dy;
		double b = 2.*(x1*dx + y1*dy);
		double c = x1*x1 + y1*y1 - radius*radius;
 
		if (-b < 0)
		{
			intersection = (c < 0);
		}
		else if (-b < (2*a))
		{
			intersection = ((4.*a*c - b*b) < 0);
		}
		
		else intersection = (a+b+c < 0);
	}

	if(intersection) circleColor = Color::Red;
}

void CustomCircle::drawCircle(System::Windows::Forms::PaintEventArgs^ e)
{
	Pen^ pen = gcnew Pen(circleColor, 3.0f);

	e->Graphics->DrawEllipse(pen, describedRectangle);
}

Point CustomCircle::getCenter()
{
	return center;
}
int CustomCircle::getRadius()
{
	return radius;
}
Color CustomCircle::getColor()
{
	return circleColor;
}

bool CustomCircle::isEquals(CustomCircle^ circle)
{
	return circle->getRadius() == radius && circle->getCenter().Equals(center) && circle->getColor().Equals(circleColor);
}

void CustomCircle::setColor(Color color)
{
	circleColor = color;
}

float CustomCircle::getDistanceToPolyline()
{
	return distanceToPolyline;
}