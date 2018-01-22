#include <windows.h>
#include <math.h>
#include <random>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <ctime>

const double Pi = 3.14;
const int ssize = 2;//кол-во точек
double mistake = 0.5;
int chicken = 20;//вероятность ошибки
int distribution = 1;//тип ошибки
double cock_x, cock_y; // переменные для корректной отрисовки функции


double step =0.001;
double y;



inline double my_function1(int x, double cock_x,double cock_y) {
	return cos(2*Pi*x/cock_x)*cock_y;
}
inline double my_function2(int x, double cock_x,double cock_y) {//не подогнал
	auto cock =  ( pow(x, 3)*5 + pow(x, 2)+ 5) /cock_x ;
	return cock/ cock_y;
}
inline double my_function3(int x, double cock_x,double cock_y) {
	auto cock = sin(2.0 * Pi*(x / cock_x))*x/cock_x;
	return cock*cock_y ;
}
//теперь функция, которая рисует полином этот
inline double polynom(double vec_sol[], int sizeOfMatrix, double cock_x) {
	double polynom=0;
	for (int i = 0; i < sizeOfMatrix; i++) {
		polynom += pow(vec_sol[i], sizeOfMatrix - i)/cock_x;
	}
	return polynom*cock_y;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	static int cxClient, cyClient;
	HDC hdc;
	PAINTSTRUCT ps;
	HPEN hPen, hPenOld;
	HBRUSH hBrush, hBrushOld;
	//std::random_device rd();
	std::mt19937 gen(547);
	std::uniform_real_distribution<> distr(-mistake, mistake);

	switch (iMsg)
	{

	case WM_CREATE:
		break;

	case WM_SIZE:
		cxClient = LOWORD(lParam);//ширина
		cyClient = HIWORD(lParam);//высота
		break;

	case WM_PAINT:
	{
		std::ofstream loggi;// не можем нормально работать с консолью, ходим логгировать матрицы
		loggi.open("loggi.txt");

		double *xx = new double[ssize];
		double *yy = new double[ssize];

		hdc = BeginPaint(hwnd, &ps);//начало рисования

		SetMapMode(hdc, MM_ISOTROPIC);// режим рисования с единообразными осями
		SetWindowExtEx(hdc, cxClient, cyClient, 0);//установили размеры
		SetViewportExtEx(hdc, cxClient / 2, -cyClient / 2, 0);
		SetViewportOrgEx(hdc, cxClient / 2, cyClient / 2, 0);

		MoveToEx(hdc, -cxClient, 0, NULL);//переместили курсор
		LineTo(hdc, cxClient, 0);//нарисовали одну ось
		MoveToEx(hdc, 0, -cyClient, NULL);
		LineTo(hdc, 0, cyClient);//нарисовали другую


		hPen = CreatePen(PS_SOLID, 0, RGB(255, 0, 0));
		SelectObject(hdc, hPen);
		cock_x = cxClient;
		cock_y = cyClient / 2;
		MoveToEx(hdc, -cxClient, my_function2(-cxClient, cock_x, cock_y), NULL);//устанавливаем начальную позицию
		for (int i = -cxClient; i < cxClient; i++)//рисуем график функции
		{
			int y = my_function2(i, cock_x, cock_y);
			LineTo(hdc, i, (int)y);
		}

		for (int i = 0; i < ssize; i++)//генерируем точки для метода
		{
			xx[i] = rand() % cxClient * 2 - cxClient;// x координата точки из выборки
			int mis = 0;
			if (rand() % 100 < chicken)//если случайно число меньше наперёд заданного
			{
				if (distribution == 1)//равномерная ошибка
				{
					mis = distr(gen);
				}
				if (distribution == 2)//нормальная ошибка
				{
					for (int j = 0; j < 12; j++)
						mis += distr(gen);
					mis /= (2.0*mistake);
				}

			}
			yy[i] = my_function2(xx[i], cock_x, cock_y) + cock_y * mis;//y координата этой точки + ошибка для отрисовки в нужном маштабе

			hPen = CreatePen(PS_SOLID, 0, RGB(0, 0, 255));
			hPenOld = (HPEN)SelectObject(hdc, hPen);
			hBrush = CreateSolidBrush(RGB(0, 0, 255));
			hBrushOld = (HBRUSH)SelectObject(hdc, hBrush);

			Ellipse(hdc, xx[i] - 5, yy[i] - 5, xx[i] + 5, yy[i] + 5);

			xx[i] /= cock_x;
			yy[i] /= cock_y;

			SelectObject(hdc, hBrushOld);
			DeleteObject(hBrush);

			SelectObject(hdc, hPenOld);
			DeleteObject(hPen);
		}

		int power_of_polynom = 3;//полином 3й степени, от 0 до 3 степени
		const int sizeOfMatrix = power_of_polynom + 1;
		double** matrix = new double*[sizeOfMatrix];
		for (int i = 0; i < sizeOfMatrix; i++) {
			matrix[i] = new double[sizeOfMatrix + 1];
		}

		//слау такого-то размера подготавливаем к работе
		for (int i = 0; i < sizeOfMatrix; i++) {
			for (int j = 0; j < sizeOfMatrix + 1; j++)
				matrix[i][j] = 0;
		}
		//вычисление всей матрицы, кроме последнего столбца 
		for (int i = 0; i < sizeOfMatrix; i++) {
			for (int j = 0; j < sizeOfMatrix; j++) {
				for (int k = 0; k < ssize; k++)
					matrix[i][j] += pow(xx[k], i + j);
			}
		}

		//вычисление последнего столбца (правых частей) 
		for (int i = 0; i < sizeOfMatrix; i++) {
			for (int k = 0; k < ssize; k++)
				matrix[i][sizeOfMatrix] += yy[k] * pow(xx[k], i);
		}

		 //Метод Гаусса
		 //Прямой ход, приведение к верхнетреугольному виду
		int i,j,k;
		double tmp;
		double* vec_sol = new double[sizeOfMatrix];

		for (i = 0; i<sizeOfMatrix; i++)
		{
			 tmp = matrix[i][i];//Элемент на главной диагонали
			 for (j = sizeOfMatrix +1; j >= i; j--)//приведение элемента главной диагонали к 1, деление элементов строки
				 matrix[i][j] /= tmp; 
			 for (j = i + 1; j<sizeOfMatrix; j++)
			 {
				 tmp = matrix[j][i];//зануление столбцов под главной диагональю
				 for (k = sizeOfMatrix; k >= i; k--)
					 matrix[j][k] -= tmp * matrix[i][k];
			 }
		 }
		 //имеем верхнетреугольную матрицу с правым столбиком
		 /*обратный ход*/
		 vec_sol[sizeOfMatrix - 1] = matrix[sizeOfMatrix-1][sizeOfMatrix];
		 for (i = sizeOfMatrix - 2; i >= 0; i--)
		 {
			 vec_sol[i] = matrix[i][sizeOfMatrix];
			 for (j = i + 1; j<sizeOfMatrix; j++)
				 vec_sol[i] -= matrix[i][j] * vec_sol[j];
		 }
		 
		 for (int i = 0; i < sizeOfMatrix; i++) {
			 for (int j = 0; j < sizeOfMatrix + 1; j++) {
				 loggi << matrix[i][j] << " ";
			 }
			 loggi << std::endl;
		 }
		 loggi << std::endl;
		 //Выводим решения
		 for (i = 0; i < sizeOfMatrix; i++) {
			 loggi << vec_sol[i] << " ";
		 }
		 loggi.close();

		 hBrush = CreateSolidBrush( RGB(50, 255, 50));
		 MoveToEx(hdc, -cxClient, polynom(vec_sol,sizeOfMatrix,cock_x), NULL);//устанавливаем начальную позицию
		 for (int i = -cxClient; i < cxClient; i++)//рисуем график функции
		 {
			 int y = polynom(vec_sol, sizeOfMatrix,cock_x);
			 LineTo(hdc, i, (int)y);
		 }


		 /*for (int i = 0; i < sizeOfMatrix; i++) {
			 delete[] matrix[i];
		 }*/
		 delete[] matrix;

		///////////////////////////////////////////////////////////////////////////
		
		return 0;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT("SineWave");
	HWND hwnd;
	MSG msg;

	WNDCLASSEX wndclass;
	wndclass.cbSize = sizeof(wndclass);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = TEXT("MYMENU");
	wndclass.lpszClassName = (szAppName);
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassEx(&wndclass);
	hwnd = CreateWindow(szAppName, TEXT("Метод наименьших квадратов"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		1000, 750,
		NULL, NULL, hInstance, NULL);
	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

