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
const int ssize = 4;//кол-во точек
double mistake = 0.5;
int chicken = 5;//вероятность ошибки
int distribution = 1;//тип ошибки
double cock_x, cock_y; // переменные для корректной отрисовки функции
int power_of_polynom = 3;//полином 3й степени, от 0 до 3 степени,иными словами размерность вектора
const double lambda = 0.005;


//double step =0.001;
double y;

inline double my_function1(double x, double cock_x,double cock_y) {
	return cos(2*Pi*x/cock_x)*cock_y;
}
inline double my_function2(double x, double cock_x,double cock_y) {//не подогнал
	auto cock =  ( pow(x/3, 3)*5.0/cock_x + pow(x, 2)/ cock_x + 5.0);
	return cock/ cock_y;
}
inline double my_function3(double x, double cock_x,double cock_y) {
	double cock = sin(2.0 * Pi*(x / cock_x))*x/cock_x;
	return cock*cock_y ;
}
//теперь функция, которая рисует полином этот
inline double polynom(double vec_sol[], int sizeOfMatrix, double cock_x, double x) {
	double polynom=0;
	for (int i = 0; i < sizeOfMatrix; i++) {
		polynom += vec_sol[i]*pow(x/3, i)/cock_x;
	}
	return polynom/cock_y;
}

double* Gauss(double** matrix, int n) {
	//Метод Гаусса
	//Прямой ход, приведение к верхнетреугольному виду
	int i, j, k;
	double tmp;
	double* vec_sol = new double[n];

	for (i = 0; i<n; i++)
	{
		tmp = matrix[i][i];//Элемент на главной диагонали
		for (j = n + 1; j >= i; j--)//приведение элемента главной диагонали к 1, деление элементов строки
			matrix[i][j] /= tmp;
		for (j = i + 1; j<n; j++)
		{
			tmp = matrix[j][i];//зануление столбцов под главной диагональю
			for (k = n; k >= i; k--)
				matrix[j][k] -= tmp * matrix[i][k];
		}
	}
	//имеем верхнетреугольную матрицу с правым столбиком
	/*обратный ход*/
	vec_sol[n - 1] = matrix[n - 1][n];
	for (i = n - 2; i >= 0; i--)
	{
		vec_sol[i] = matrix[i][n];
		for (j = i + 1; j<n; j++)
			vec_sol[i] -= matrix[i][j] * vec_sol[j];
	}
	return vec_sol;
}

void logging(int sizeOfMatrix, std::ofstream& loggi, double** matrix) {
	for (int i = 0; i < sizeOfMatrix; i++) {
		for (int j = 0; j < sizeOfMatrix + 1; j++) {
			loggi << matrix[i][j] << " ";
		}
		loggi << std::endl;
	}
	loggi << std::endl;
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
		MoveToEx(hdc, -cxClient, (int)my_function2(-cxClient, cock_x, cock_y), NULL);//устанавливаем начальную позицию
		for (int i = -cxClient; i < cxClient; i++)//рисуем график функции
		{
			double y = my_function2(i, cock_x, cock_y);
			LineTo(hdc, i, (int)y);
		}

		for (int i = 0; i < ssize; i++)//генерируем точки для метода
		{
			xx[i] = rand() % cxClient * 2 - cxClient;// x координата точки из выборки
			double mis = 0;
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
		//логирование
		logging(sizeOfMatrix, loggi, matrix);
		double* vec_sol = Gauss(matrix, sizeOfMatrix);
		logging(sizeOfMatrix,loggi,matrix);

		//регуляризация
		for (int i = 0; i < sizeOfMatrix; i++) {
			matrix[i][i] += lambda;
		}

		double* vec_sol_reg = new double[sizeOfMatrix];
		vec_sol_reg = Gauss(matrix, sizeOfMatrix);

		 //Выводим решения
		 for (int i = 0; i < sizeOfMatrix; i++) {
			 loggi << vec_sol[i] << " ";
		 }
		 loggi << std::endl;
		 for (int i = 0; i < sizeOfMatrix; i++) {
			 loggi << vec_sol_reg[i] << " ";
		 }
		 loggi.close();

		 hPen = CreatePen(PS_SOLID, 0, RGB(0, 255, 0));
		 hPenOld = (HPEN)SelectObject(hdc, hPen);
		 hBrush = CreateSolidBrush( RGB(0, 255, 0));
		 hBrushOld = (HBRUSH)SelectObject(hdc, hBrush);

		 MoveToEx(hdc, -cxClient, (int)polynom(vec_sol,sizeOfMatrix,cock_x,-cxClient), NULL);//устанавливаем начальную позицию
		 for (int i = -cxClient; i < cxClient; i++)//рисуем график функции
		 {
			 double y = polynom(vec_sol, sizeOfMatrix,cock_x,i);
			 LineTo(hdc, i, y);
		 }



		 hPen = CreatePen(PS_SOLID, 0, RGB(0, 0, 0));
		 hPenOld = (HPEN)SelectObject(hdc, hPen);
		 hBrush = CreateSolidBrush(RGB(0, 0, 0));
		 hBrushOld = (HBRUSH)SelectObject(hdc, hBrush);
		 MoveToEx(hdc, -cxClient, (int)polynom(vec_sol_reg, sizeOfMatrix, cock_x, -cxClient), NULL);//устанавливаем начальную позицию
		 for (int i = -cxClient; i < cxClient; i++)//рисуем график функции
		 {
			 double y = polynom(vec_sol_reg, sizeOfMatrix, cock_x, i);
			 LineTo(hdc, i, y);
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

