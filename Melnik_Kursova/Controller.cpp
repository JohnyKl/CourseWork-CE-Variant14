#include "stdafx.h"
#include "Controller.h"
#include "CustomCircle.h"

using namespace Melnik_Kursova;

Controller::Controller(void)
{
	messageAboutTrianglesIntersection = "Введено недостатньо точок для побудови трикутників";

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
		((CustomCircle^)circles->GetValue(i))->drawCircle(e);//намалювати кожне коло з масиву
	}
}

void Controller::drawPolyline(PaintEventArgs^ e)
{
	Pen ^pen = gcnew Pen(Color::Black, 3.0f);

	for( int i = 0; i < polyline->Length - 1; i++)
	{
		e->Graphics->DrawLine(pen, (Point)polyline->GetValue(i),(Point)polyline->GetValue(i + 1));//намалвати ламану, з'єднуючи дві послідовні точки з масиву лінією
	}
}

void Controller::drawTriangles(PaintEventArgs^ e)
{
	//оголосити масиви точок з центрів кіл для малювання трикутників
	Array^ littleTriangle = Array::CreateInstance(Point::typeid,3);
	Array^ bigTriangle = Array::CreateInstance(Point::typeid,3);

	Pen^ blackPen = gcnew Pen( Color::Black,3.0f);//створення олівця

	for(int i = 0; i < 3; i++)//ініціалізувати масиви точок центрами кіл
	{
		littleTriangle->SetValue(((CustomCircle^)theSmallest->GetValue(i))->getCenter(), i);
		bigTriangle->SetValue(((CustomCircle^)theBiggest->GetValue(i))->getCenter(), i);
	}
	//намалювати трикутники
	e->Graphics->DrawPolygon(blackPen, (array<Point>^)littleTriangle);
	e->Graphics->DrawPolygon(blackPen, (array<Point>^)bigTriangle);
}

vector<int*> Controller::readFromFile(const int paramSize, string path)
{
	std::ifstream ifs(path); //створення файлового потоку вводу

	std::vector<int*> coords;//створення структури вектору, у яку додаватимуться координати

	if(ifs.is_open())//якщо файловий опток відкрито
	{
		while (! ifs.eof() )//доки не досягнуто кінця файлу
		{
			string line = "";//оголошення та ініціалізація стрічки для зчитування з файлу
			getline(ifs,line);//отримання однієї стрічки з файлу

			int* coordsFromLine = new int[paramSize];//оголошення та ініціалізація масиву для збереження зчитаних значень координати
			findParametersInLine(line, coordsFromLine, paramSize);//знайти координати в стрічці

			bool isFound = true;

			for(int i = 0; i < paramSize && isFound; i++)
			{
				if(coordsFromLine[i] == -1) isFound = false;
			}

			if(isFound)//якщо координату знайдено
			{
				coords.push_back(coordsFromLine);//додати координату в вектор
			}		
		}
	}
	ifs.close();//закриття файлового потоку

	return coords;
}

void Controller::createCirclesCoordFromFile(string path)
{
	std::vector<int*> coords = readFromFile(3, path);//зчитати з файлу координати та радіус кіл

	int parametersNumber = coords.size();//отримати кількість кіл

	circles = Array::CreateInstance(CustomCircle::typeid, parametersNumber);//ініціалізація масиву з даними типу CustomCircle

	for(int i = 0; i < parametersNumber; i++)
	{
		circles->SetValue(gcnew CustomCircle(Point(coords[i][1], coords[i][2]), coords[i][0]), i);//заповнити створеними колами масив
	}
}

void Controller::findParametersInLine(string line, int* parameters, const int paramSize)//функція знаходить 2 (для координат відрізка) або 3 (для координат центру та радіусу кола) параметри
{
	size_t *indexes = new size_t[paramSize];//масив індексів на початок числа в стрічці
	size_t *counters = new size_t[paramSize];//масив індексів на символ після кінця числа в стрічці
	size_t previousIndex = 0;

	//знайти в стрічці всі необхідні індекси по розділювачам ("=" - для початку числа, "," - для кінця числа)
	for (int i = 0; i < paramSize - 1; i++)
	{
		previousIndex = line.find_first_of(",", previousIndex) + 1;
		counters[i] = previousIndex;
	}

	counters[paramSize - 1] = line.length();//останній розділювач на ",", а кінець стрічки

	previousIndex = 0;

	for (int i = 0; i < paramSize; i++)
	{
		previousIndex = line.find_first_of("=", previousIndex) + 1;
		indexes[i] = previousIndex;

		parameters[i] = -1;

		if(counters[i] > 0 && indexes[i] > 0)
		{
			int countSymbols = counters[i] - indexes[i];//знайти індекс першого входження символу "," та відняти від нього позицію першої цифри числа, щоб знайти кількість цифер числа

			parameters[i] = stoi(line.substr(indexes[i],countSymbols));//перетворити в число потрібну частину стрічки
		}
	}

	delete indexes;//звільнити пам'ять
	delete counters;
}

void Controller::createPolylineCoordFromFile(string path)
{
	std::vector<int*> coords = readFromFile(2, path);;//створення структури вектору, у яку додаватимуться координати

	int pointsNumber = coords.size();//кількість координат обчислюється як загальна кількість значень Х та У

	polyline = Array::CreateInstance(Point::typeid, pointsNumber);//ініціалізація масиву з даними типу Point

	for (int i = 0; i < pointsNumber; i++)
	{
		polyline->SetValue(Point(coords[i][0],coords[i][1]),i);//додати у масив точок нову створену точку з двох послідовних значень осей ординати та абсциси
	}
}

void Controller::findThreeTheSmallestAndTheBiggestCircles()
{
	theSmallest = Array::CreateInstance(CustomCircle::typeid, 3);//оголошує масив на 3 елемента для збереження найменш віддалених від ламаної кіл	
	theBiggest = Array::CreateInstance(CustomCircle::typeid, 3);//оголошує масив на 3 елемента для збереження набільш віддалених від ламаної кіл	
	
	for(int i = 0; i < 3; i++)
	{		
		float currentTheBiggestDistance = FLT_MIN;
		float currentTheSmallestDistance = FLT_MAX;

		for(int k = 0; k < circles->Length; k++)
		{
			CustomCircle ^currentCircle = (CustomCircle^)circles->GetValue(k);//отримує к-те коло з масиву кіл

			if(currentTheSmallestDistance > currentCircle->getDistanceToPolyline())//порівняти його радіус з найменшою відстанню у цьому проході, якщо менше
			{
				if(!contains(theSmallest,currentCircle, i))//перевірити, чи нема цього кола доданого в масив кіл, з яких буде устворено трикутник
				{
					currentTheSmallestDistance = currentCircle->getDistanceToPolyline();//оновити значення найменшої відстані
					theSmallest->SetValue(currentCircle, i);//оновити і-те значення в масиві
				}
			}
			//так само перевірити чи це не найбільш віддалене коло в поточному проході циклу
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

bool Controller::contains(Array ^ arr, CustomCircle^ nextCircle, int size)//перевіряє, чи поточне знайдене коло не було додане до масиву в попередніх проходах циклу
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
		((CustomCircle^)circles->GetValue(i))->findDistanceToPolyine(polyline);//для кожного кола в масиві шукає дистанцію від центра кола до ламаної
	}
}

void Controller::findCirclesIntersection()
{
	for(int i = 0; i < circles->Length; i++)
	{
		((CustomCircle^)circles->GetValue(i))->intersectionWithPolyine(polyline);//для кожного кола перевірити чи воно перетанається з ламаною
	}
}

void Controller::findTheBiggestCircleIntersection()//знаходить найбільше коло, яке перетинається з ламаною
{
	CustomCircle^ bCircle = (CustomCircle^)circles->GetValue(0);//ініціалізує початкове найбільше коло
	int bRadius = bCircle->getRadius();//ініціалізує початковий найбільший радіус

	for(int i = 0; i < circles->Length; i++)
	{
		CustomCircle^ currentCircle = (CustomCircle^)circles->GetValue(i);//отримує і-те коло

		if(currentCircle->getColor().Equals(Color::Red))//якщо колір встановлений червоний, тобто воно перетинається з ламаною
		{
			if(currentCircle->getRadius() > bRadius)//перевірити чи більший радіус цього кола за поточний
			{
				//оновити значення поточного найбільшого кола і радіусу
				bCircle = currentCircle;
				bRadius = currentCircle->getRadius();
			}
		}		
	}

	bCircle->setColor(Color::Blue);//встановити найбільшому колу інший колір
}

void Controller::findTrianglesIntersection()
{
	String ^intersection = "Трикутники перетинаються";
	String ^NotIntersection = "Трикутники не перетинаються";

	bool isNullPointers = false;
	bool isIntersection = false;

	for(int i = 0; i < 3 && !isIntersection && !isNullPointers; i++)//отримує пари вершин для одного трикутника
	{
		Point ^A1, ^A2;

		try
		{
			A1 = ((CustomCircle^)theBiggest->GetValue(i))->getCenter();
			
			if(i == 2) A2 = ((CustomCircle^)theBiggest->GetValue(0))->getCenter();
			else A2 = ((CustomCircle^)theBiggest->GetValue(i + 1))->getCenter();
		}
		catch(Exception ^e)//перехопити будь-яку помилку (це зроблено для того випадку, коли не ініціалізовано масив точок вершин трикутників по будь-яким причинам)
		{
			isNullPointers = true;//встановити прапор помилки нульового вказівника (найпоширеніша помилка у випадку, коли введено менше 3 точок)
		}

		for(int k = 0; k < 3 && !isIntersection && !isNullPointers; k++)//отримує попарно вершини іншого трикутника
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
				isIntersection = isIntersectionOfTwoLines(A1, A2, B1, B2);//перевіряє чи перетинаються відрізки (чи перетинаються дві сторони трикутників)
			}
		}
	}

	if(!isNullPointers)//відповідно до прапорця встановити правильний текст повідомлення
	{
		if(isIntersection) messageAboutTrianglesIntersection = intersection;
		else messageAboutTrianglesIntersection = NotIntersection;
	}
}

bool Controller::isIntersectionOfTwoLines(Point^ A1, Point^ A2, Point^ B1, Point^ B2)
{//функція за математичною формулою перевіряє чи перетинаються два відрізки
	float v1 = (B2->X - B1->X)*(A1->X - B1->Y) - (B2->Y - B1->Y)*(A1->X - B1->X);
	float v2 = (B2->X - B1->X)*(A2->Y - B1->Y) - (B2->Y - B1->Y)*(A2->X - B1->X);
	float v3 = (A2->X - A1->X)*(B1->X - A1->Y) - (A2->Y - A1->Y)*(B1->X - A1->X);
	float v4 = (A2->X - A1->X)*(B2->Y - A1->Y) - (A2->Y - A1->Y)*(B2->X - A1->X);

	return (v1*v2 < 0)&&(v3*v4 < 0);
}