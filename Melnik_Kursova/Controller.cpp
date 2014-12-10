#include "stdafx.h"
#include "Controller.h"
#include "CustomCircle.h"

using namespace Melnik_Kursova;

Controller::Controller(void)
{
	messageAboutTrianglesIntersection = "������� ����������� ����� ��� �������� ����������";

	createCirclesCoordFromFile("centers_and_radiuses.txt");
	createPolylineCoordFromFile("polyline_coords.txt");

	findDistanceFromCirclesCentersToPolyline();
	findThreeTheSmallestAndTheBiggestCircles();
	findCirclesIntersection();
	findTrianglesIntersection();
	findTheBiggestCircleIntersection();
}

void Controller::drawCircles(PaintEventArgs^ e)
{
	for (int i = 0; i < circles->Length; i++)
	{
		((CustomCircle^)circles->GetValue(i))->drawCircle(e);//���������� ����� ���� � ������
	}
}

void Controller::drawPolyline(PaintEventArgs^ e)
{
	Pen ^pen = gcnew Pen(Color::Black, 3.0f);

	for( int i = 0; i < polyline->Length - 1; i++)
	{
		e->Graphics->DrawLine(pen, (Point)polyline->GetValue(i),(Point)polyline->GetValue(i + 1));//��������� ������, �'������� �� �������� ����� � ������ ���
	}
}

void Controller::drawTriangles(PaintEventArgs^ e)
{
	//��������� ������ ����� � ������ �� ��� ��������� ����������
	Array^ littleTriangle = Array::CreateInstance(Point::typeid,3);
	Array^ bigTriangle = Array::CreateInstance(Point::typeid,3);

	Pen^ blackPen = gcnew Pen( Color::Black,3.0f);//��������� �����

	for(int i = 0; i < 3; i++)//������������ ������ ����� �������� ��
	{
		littleTriangle->SetValue(((CustomCircle^)theSmallest->GetValue(i))->getCenter(), i);
		bigTriangle->SetValue(((CustomCircle^)theBiggest->GetValue(i))->getCenter(), i);
	}
	//���������� ����������
	e->Graphics->DrawPolygon(blackPen, (array<Point>^)littleTriangle);
	e->Graphics->DrawPolygon(blackPen, (array<Point>^)bigTriangle);
}

vector<int*> Controller::readFromFile(const int paramSize, string path)
{
	std::ifstream ifs(path); //��������� ��������� ������ �����

	std::vector<int*> coords;//��������� ��������� �������, � ��� �������������� ����������

	if(ifs.is_open())//���� �������� ����� �������
	{
		while (! ifs.eof() )//���� �� ��������� ���� �����
		{
			string line = "";//���������� �� ����������� ������ ��� ���������� � �����
			getline(ifs,line);//��������� ���� ������ � �����

			int* coordsFromLine = new int[paramSize];//���������� �� ����������� ������ ��� ���������� �������� ������� ����������
			findParametersInLine(line, coordsFromLine, paramSize);//������ ���������� � ������

			bool isFound = true;

			for(int i = 0; i < paramSize && isFound; i++)
			{
				if(coordsFromLine[i] == -1) isFound = false;
			}

			if(isFound)//���� ���������� ��������
			{
				coords.push_back(coordsFromLine);//������ ���������� � ������
			}		
		}
	}
	ifs.close();//�������� ��������� ������

	return coords;
}

void Controller::createCirclesCoordFromFile(string path)
{
	std::vector<int*> coords = readFromFile(3, path);//������� � ����� ���������� �� ����� ��

	int parametersNumber = coords.size();//�������� ������� ��

	circles = Array::CreateInstance(CustomCircle::typeid, parametersNumber);//����������� ������ � ������ ���� CustomCircle

	for(int i = 0; i < parametersNumber; i++)
	{
		circles->SetValue(gcnew CustomCircle(Point(coords[i][1], coords[i][2]), coords[i][0]), i);//��������� ���������� ������ �����
	}
}

void Controller::findParametersInLine(string line, int* parameters, const int paramSize)//������� ��������� 2 (��� ��������� ������) ��� 3 (��� ��������� ������ �� ������ ����) ���������
{
	size_t *indexes = new size_t[paramSize];//����� ������� �� ������� ����� � ������
	size_t *counters = new size_t[paramSize];//����� ������� �� ������ ���� ���� ����� � ������
	size_t previousIndex = 0;

	//������ � ������ �� �������� ������� �� ����������� ("=" - ��� ������� �����, "," - ��� ���� �����)
	for (int i = 0; i < paramSize - 1; i++)
	{
		previousIndex = line.find_first_of(",", previousIndex) + 1;
		counters[i] = previousIndex;
	}

	counters[paramSize - 1] = line.length();//������� ��������� �� ",", � ����� ������

	previousIndex = 0;

	for (int i = 0; i < paramSize; i++)
	{
		previousIndex = line.find_first_of("=", previousIndex) + 1;
		indexes[i] = previousIndex;

		parameters[i] = -1;

		if(counters[i] > 0 && indexes[i] > 0)
		{
			int countSymbols = counters[i] - indexes[i];//������ ������ ������� ��������� ������� "," �� ������ �� ����� ������� ����� ����� �����, ��� ������ ������� ����� �����

			parameters[i] = stoi(line.substr(indexes[i],countSymbols));//����������� � ����� ������� ������� ������
		}
	}

	delete indexes;//�������� ���'���
	delete counters;
}

void Controller::createPolylineCoordFromFile(string path)
{
	std::vector<int*> coords = readFromFile(2, path);;//��������� ��������� �������, � ��� �������������� ����������

	int pointsNumber = coords.size();//������� ��������� ������������ �� �������� ������� ������� � �� �

	polyline = Array::CreateInstance(Point::typeid, pointsNumber);//����������� ������ � ������ ���� Point

	for (int i = 0; i < pointsNumber; i++)
	{
		polyline->SetValue(Point(coords[i][0],coords[i][1]),i);//������ � ����� ����� ���� �������� ����� � ���� ���������� ������� ���� �������� �� �������
	}
}

void Controller::findThreeTheSmallestAndTheBiggestCircles()
{
	theSmallest = Array::CreateInstance(CustomCircle::typeid, 3);//������� ����� �� 3 �������� ��� ���������� ������� ��������� �� ������ ��	
	theBiggest = Array::CreateInstance(CustomCircle::typeid, 3);//������� ����� �� 3 �������� ��� ���������� ������ ��������� �� ������ ��	
	
	for(int i = 0; i < 3; i++)
	{		
		float currentTheBiggestDistance = FLT_MIN;
		float currentTheSmallestDistance = FLT_MAX;

		for(int k = 0; k < circles->Length; k++)
		{
			CustomCircle ^currentCircle = (CustomCircle^)circles->GetValue(k);//������ �-�� ���� � ������ ��

			if(currentTheSmallestDistance > currentCircle->getDistanceToPolyline())//�������� ���� ����� � ��������� �������� � ����� ������, ���� �����
			{
				if(!contains(theSmallest,currentCircle, i))//���������, �� ���� ����� ���� �������� � ����� ��, � ���� ���� ��������� ���������
				{
					currentTheSmallestDistance = currentCircle->getDistanceToPolyline();//������� �������� �������� ������
					theSmallest->SetValue(currentCircle, i);//������� �-�� �������� � �����
				}
			}
			//��� ���� ��������� �� �� �� ������� �������� ���� � ��������� ������ �����
			if(currentTheBiggestDistance < currentCircle->getDistanceToPolyline())
			{
				if(!contains(theBiggest,currentCircle, i))
				{
					currentTheBiggestDistance = currentCircle->getDistanceToPolyline();
					theBiggest->SetValue(currentCircle, i);
				}
			}
		}
	}
}

bool Controller::contains(Array ^ arr, CustomCircle^ nextCircle, int size)//��������, �� ������� �������� ���� �� ���� ������ �� ������ � ��������� �������� �����
{
	bool contain = false;

	for(int i = 0; i < size && !contain; i++)
	{
		contain = ((CustomCircle^) arr->GetValue(i))->isEquals(nextCircle);
	}

	return contain;
}

void Controller::findDistanceFromCirclesCentersToPolyline()
{
	for(int i = 0; i < circles->Length; i++)
	{
		((CustomCircle^)circles->GetValue(i))->findDistanceToPolyine(polyline);//��� ������� ���� � ����� ���� ��������� �� ������ ���� �� ������
	}
}

void Controller::findCirclesIntersection()
{
	for(int i = 0; i < circles->Length; i++)
	{
		((CustomCircle^)circles->GetValue(i))->intersectionWithPolyine(polyline);//��� ������� ���� ��������� �� ���� ������������ � �������
	}
}

void Controller::findTheBiggestCircleIntersection()//��������� �������� ����, ��� ������������ � �������
{
	CustomCircle^ bCircle = (CustomCircle^)circles->GetValue(0);//�������� ��������� �������� ����
	int bRadius = bCircle->getRadius();//�������� ���������� ��������� �����

	for(int i = 0; i < circles->Length; i++)
	{
		CustomCircle^ currentCircle = (CustomCircle^)circles->GetValue(i);//������ �-�� ����

		if(currentCircle->getColor().Equals(Color::Red))//���� ���� ������������ ��������, ����� ���� ������������ � �������
		{
			if(currentCircle->getRadius() > bRadius)//��������� �� ������ ����� ����� ���� �� ��������
			{
				//������� �������� ��������� ���������� ���� � ������
				bCircle = currentCircle;
				bRadius = currentCircle->getRadius();
			}
		}		
	}

	bCircle->setColor(Color::Blue);//���������� ���������� ���� ����� ����
}

void Controller::findTrianglesIntersection()
{
	String ^intersection = "���������� �������������";
	String ^NotIntersection = "���������� �� �������������";

	bool isNullPointers = false;
	bool isIntersection = false;

	for(int i = 0; i < 3 && !isIntersection && !isNullPointers; i++)//������ ���� ������ ��� ������ ����������
	{
		Point ^A1, ^A2;

		try
		{
			A1 = ((CustomCircle^)theBiggest->GetValue(i))->getCenter();
			
			if(i == 2) A2 = ((CustomCircle^)theBiggest->GetValue(0))->getCenter();
			else A2 = ((CustomCircle^)theBiggest->GetValue(i + 1))->getCenter();
		}
		catch(Exception ^e)//���������� ����-��� ������� (�� �������� ��� ���� �������, ���� �� ������������ ����� ����� ������ ���������� �� ����-���� ��������)
		{
			isNullPointers = true;//���������� ������ ������� ��������� ��������� (������������ ������� � �������, ���� ������� ����� 3 �����)
		}

		for(int k = 0; k < 3 && !isIntersection && !isNullPointers; k++)//������ ������� ������� ������ ����������
		{
			Point ^B1, ^B2;

			try
			{
				B1 = ((CustomCircle^)theSmallest->GetValue(k))->getCenter();
				
				if(k == 2) B2 = ((CustomCircle^)theSmallest->GetValue(0))->getCenter();
				else B2 = ((CustomCircle^)theSmallest->GetValue(k + 1))->getCenter();
			}
			catch(Exception ^e)
			{
				isNullPointers = true;
			}

			if(!isNullPointers)
			{
				isIntersection = isIntersectionOfTwoLines(A1, A2, B1, B2);//�������� �� ������������� ������ (�� ������������� �� ������� ����������)
			}
		}
	}

	if(!isNullPointers)//�������� �� �������� ���������� ���������� ����� �����������
	{
		if(isIntersection) messageAboutTrianglesIntersection = intersection;
		else messageAboutTrianglesIntersection = NotIntersection;
	}
}

bool Controller::isIntersectionOfTwoLines(Point^ A1, Point^ A2, Point^ B1, Point^ B2)
{//������� �� ������������ �������� �������� �� ������������� ��� ������
	float v1 = (B2->X - B1->X)*(A1->X - B1->Y) - (B2->Y - B1->Y)*(A1->X - B1->X);
	float v2 = (B2->X - B1->X)*(A2->Y - B1->Y) - (B2->Y - B1->Y)*(A2->X - B1->X);
	float v3 = (A2->X - A1->X)*(B1->X - A1->Y) - (A2->Y - A1->Y)*(B1->X - A1->X);
	float v4 = (A2->X - A1->X)*(B2->Y - A1->Y) - (A2->Y - A1->Y)*(B2->X - A1->X);

	return (v1*v2 < 0)&&(v3*v4 < 0);
}