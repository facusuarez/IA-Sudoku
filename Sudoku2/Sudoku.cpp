// Sudoku2.cpp: define el punto de entrada de la aplicación de consola.
//

#include "stdafx.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core_c.h"
#include "opencv2/core/core.hpp"
#include "opencv2/flann/miniflann.hpp"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/video.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/ml/ml.hpp"
#include "opencv2/highgui/highgui_c.h"
#include "opencv2/highgui/highgui.hpp"
#include "stdafx.h"
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "iostream"
#include <dirent.h>
using namespace cv;
using namespace std;

//-----------DECLARACION DE VARIABLES-------------
namespace DeclaracionDeVariables {
	// UNASSIGNED es usado para las columnas vacias en el sudoku
#define UNASSIGNED 0

	//Tamaño del sudoku
#define N 9

	//----------DECLARACION DE VARIABLES------------

	//CANTIDAD DE IMAGENES DE ENTRENAMIENTO, SI NO SE ACTUALIZA SE ROMPE
	int y = 1886;

	Mat src;//Foto original
	Mat srcb;//Foto en blanco y negro
	Mat smooth;//Foto sin ruido
	Mat thresholded;//Imagen binarizada
	Mat thresholded2;//Copia de la anterior
	vector< vector < Point > >contours;
	vector <Vec4i> heirarchy;
	int p;//Index del contorno del sudoku
	Point2f in[4];//imagen de entrada para puntas
	Point2f out[4];//imagen de salida para puntas
	Mat wrap; //Imagen cortada
	Mat mat;
	Mat ch;
	Mat thresholded31;// Imagen cortada binarizada
	int size = 16 * 16;
	Ptr<cv::ml::KNearest> knearest = cv::ml::KNearest::create();
	vector <Mat>;
	vector <Mat> smallt;
	int z[9][9];
	int grid[N][N];
	bool entr = false;
	
}

//---------------METODOS AUXILIARES----------
namespace MetodosSecundarios{

	using namespace DeclaracionDeVariables;
	//Metodo que resuelve
	bool FindUnassignedLocation(int grid[N][N], int &row, int &col)//here i have used call y ref s

	{
		for (row = 0; row < N; row++)
			for (col = 0; col < N; col++)
				if (grid[row][col] == UNASSIGNED)
					return true;
		return false;
	}

	//heckea si el numero ya esta en alguna fila o columna
	bool UsedInRow(int grid[N][N], int row, int num)
	{
		for (int col = 0; col < N; col++)
			if (grid[row][col] == num)
				return true;
		return false;
	}

	bool UsedInCol(int grid[N][N], int col, int num)
	{
		for (int row = 0; row < N; row++)
			if (grid[row][col] == num)
				return true;
		return false;
	}

	bool UsedInBox(int grid[N][N], int boxStartRow, int boxStartCol, int num)
	{
		for (int row = 0; row < 3; row++)
			for (int col = 0; col < 3; col++)
				if (grid[row + boxStartRow][col + boxStartCol] == num)
					return true;
		return false;
	}

	// Checkea que sea legal poner un numero en la fila o columna
	bool isSafe(int grid[N][N], int row, int col, int num)
	{
		// Checkea si no esta en la misma fila, columna o cuadro
		return !UsedInRow(grid, row, num) &&
			!UsedInCol(grid, col, num) &&
			!UsedInBox(grid, row - row % 3, col - col % 3, num);
	}
	bool SolveSudoku(int grid[N][N])
	{
		int row, col;

		// Si no hay filas con 0 el Sudoku esta resuelto
		if (!FindUnassignedLocation(grid, row, col)){

			return true;
	}

		//Testeo los numeros del 1 al 9
		for (int num = 1; num <= 9; num++)
		{

			if (isSafe(grid, row, col, num))
			{
				//Pongo el primer numero legal posible
				grid[row][col] = num;
				if (SolveSudoku(grid)) 
					return true;
				grid[row][col] = UNASSIGNED;
			}
		}
		
		return false;		
	}
	//Busca cuadros que no han sido resueltos (Tienen 0)
	

	void printGrid(int grid[N][N])
	{
		for (int row = 0; row < N; row++)
		{
			for (int col = 0; col < N; col++)
				printf("%2d", grid[row][col]);
			printf("\n");
		}
	}
	void pregentrenar() {
		printf("Desea entrenar el SudokuSolver? (Y/N): \n");
		CHAR resp;
		cin >> resp;
		if (resp == 'Y' || resp == 'y') {
			entr = true;

		}
		else
		{
			entr = false;
		}

	}
	void complete() {
		Mat sol = mat;
		int cellSideLength = ceil(static_cast<double>(sol.size().width) / 9.0);
		for (int row = 0; row<9; ++row) {
			for (int col = 0; col<9; ++col) {
				if (z[row][col] != 0) {
					continue;
				}
				ostringstream digit;
				digit << grid[row][col];

				int baseline;
				Size textSize = getTextSize(digit.str(), CV_FONT_HERSHEY_PLAIN, 2.0f, 3, &baseline);

				int startX = col*cellSideLength + cellSideLength / 2 - textSize.width / 2;
				int startY = row*cellSideLength + cellSideLength / 2 + textSize.height / 2;

				putText(sol, digit.str(), Point(startX, startY),
					CV_FONT_HERSHEY_SIMPLEX, 1.0f, CV_RGB(255, 0, 0), 2);
				imshow("Sudoku Resuelto", sol);
				/*imwrite(SUDOKU_SOLUTION_FILE, solution);*/
			}
		}//*/
	}

}

//---------------METODOS PRINCIPALES----------
namespace MetodosPrincipales {
	using namespace DeclaracionDeVariables;
	using namespace MetodosSecundarios;
	void webcam() {
		int ct = 0;
		char tipka;
		char filename[100]; // For filename
		int  c = 1; // For filename
					// A Simple Camera Capture Framework

		Mat frame;
		//--- INITIALIZE VIDEOCAPTURE
		VideoCapture cap;
		// open the default camera using default API
		cap.open(0);
		// OR advance usage: select any API backend
		int deviceID = 0;             // 0 = open default camera   
		int apiID = cv::CAP_ANY;      // 0 = autodetect default API
									  // open selected camera using selected API
		cap.open(deviceID + apiID);
		// check if we succeeded
		if (!cap.isOpened()) {
			cerr << "ERROR! Unable to open camera\n";
		}
		//--- GRAB AND WRITE LOOP

		for (;;)
		{
			// wait for a new frame from camera and store it into 'frame'
			cap.read(frame);

			if (frame.empty()) {
				cerr << "ERROR! blank frame grabbed\n";
				break;
			}


			Sleep(5); // Sleep is mandatory - for no leg!

			transpose(frame, frame);
			flip(frame, frame, 1);

					  // show live and wait for a key with timeout long enough to show images
			imshow("CÁMARA - Presione Q para tomar la foto", frame);  // Window name


			tipka = cv::waitKey(30);


			if (tipka == 'q') {

				sprintf_s(filename, "./SudokuFotos/NuevoSudoku.jpg", c); // select your folder - filename is "Frame_n"
				cv::waitKey(10);

				/*imshow("CAMERA 1", frame);*/
				imwrite(filename, frame);
				c++;
				Sleep(2000);
				break;
			}


			/*if (tipka == 'a') {
			cout << "Terminating..." << endl;

			}*/


		}
	}
	string seleccionarFoto() {
		
		OPENFILENAMEA    ofn;
		char filename[256] = { 0 };

		ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
		ofn.lStructSize = sizeof(OPENFILENAMEA);
		ofn.lpstrFilter = "*.jpg;*.jpeg;*\0\0";
		ofn.lpstrFile = filename;
		ofn.nMaxFile = 256;
		ofn.lpstrTitle = "Browse";
		ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
		string path;
		if (GetOpenFileNameA(&ofn)) {
			path = ofn.lpstrFile;
			std::replace(path.begin(), path.end(), '\\', '/');
			return path;			
		}		
	}
	int cargarImagen() {
		// Read original image 
		string p;
		printf("Desea utilizar su webcam? (Y/N)\n");
		CHAR resp;
		cin >> resp;
		if (resp == 'Y' || resp == 'y') {
			webcam();
			p = "./SudokuFotos/NuevoSudoku.jpg";
		}
		else
		{
			p = seleccionarFoto();
		}
		
		src = imread(p, CV_LOAD_IMAGE_UNCHANGED);
		
		resize(src, src, Size(540, 540), 0, 0, INTER_NEAREST);

		//if fail to read the image
		if (!src.data)
		{
			cout << "Error al cargar la Imagen" << endl;
			return -1;
		}
		cvtColor(src, srcb, COLOR_BGR2GRAY);
		pregentrenar();
		imshow("Imagen Original", src);
	}
	void binarizacion() {
		GaussianBlur(srcb, smooth, Size(11, 11), 0, 0); //Elimino ruido

		adaptiveThreshold(smooth, thresholded, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 15, 5);//Binarizo la imagen

		thresholded2 = thresholded.clone();//Creo una copia

		/*imshow("Imagen Suavizada", thresholded);*/
	}
	void buscarContorno() {
		findContours(thresholded2, contours, heirarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);//Busco el contrno

		//Busco el contorno mas grande	que va a hacer el area de mi sudoku																			  
		double area; double maxarea = 0;
		for (int i = 0; i < contours.size(); i++)
		{
			area = contourArea(contours[i], false);
			if (area > 16)
			{
				if (area > maxarea)
				{
					maxarea = area;
					p = i;
				}
			}
		}

		double perimeter = arcLength(contours[p], true);

		approxPolyDP(contours[p], contours[p], 0.01*perimeter, true);

		drawContours(src, contours, p, Scalar(255, 0, 0), 1, 8);

		/*imshow("Imagen del Contorno del sudoku", src);*/
	}
	void ordenarPuntas() {
		double sum = 0; double prevsum = 0; double diff1; double diff2;  double diffprev2 = 0; double diffprev = 0; double prevsum2 = contours[p][0].x + contours[p][0].y;
		int a = 2;
		int b = 0;
		int c = 3;
		int d = 4;
		for (int i = 0; i < 4; i++)
		{
			sum = contours[p][i].x + contours[p][i].y;
			diff1 = contours[p][i].x - contours[p][i].y;
			diff2 = contours[p][i].y - contours[p][i].x;
			if (diff1 > diffprev)
			{
				diffprev = diff1;
				c = i;
			}
			if (diff2 > diffprev2)
			{
				diffprev2 = diff2;
				d = i;
			}

			if (sum > prevsum)
			{
				prevsum = sum; a = i;
			}

			if (sum < prevsum2)
			{
				prevsum2 = sum;
				b = i;
			}
		}

		in[0] = contours[p][a];
		in[1] = contours[p][b];
		in[2] = contours[p][c];
		in[3] = contours[p][d];

		out[0] = Point2f(450, 450);
		out[1] = Point2f(0, 0);
		out[2] = Point2f(450, 0);
		out[3] = Point(0, 450);
	}
	void cortarFoto() {
		mat = Mat::zeros(src.size(), src.type());

		wrap = getPerspectiveTransform(in, out);

		warpPerspective(src, mat, wrap, Size(450, 450));

		/*imshow("Imagen recortada", mat);*/
	}
	void binarizacionSudoku() {
		cvtColor(mat, ch, CV_BGR2GRAY);

		GaussianBlur(ch, ch, Size(11, 11), 0, 0);

		adaptiveThreshold(ch, thresholded31, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 5, 2);
		bitwise_not(thresholded31, thresholded31);

		Mat kernel = (Mat_<uchar>(3, 3) << 0, 1, 0, 1, 1, 1, 0, 1, 0);
		dilate(thresholded31, thresholded31, kernel, Point(-1, -1), 1);

		erode(thresholded31, thresholded31, 2);

		int p2 = 0; int p3 = 0;

		while (p3 < 450)
		{
			for (int i = p3; i < p3 + 10; i++)
			{
				for (int j = 0; j < 450; j++)
				{
					thresholded31.at<uchar>(j, i) = 0;

				}
			}
			p3 = p3 + 50;
		}

		while (p2 < 450)
		{
			for (int i = 0; i < 450; i++)
			{
				for (int j = p2; j < p2 + 10; j++)
				{
					thresholded31.at<uchar>(j, i) = 0;

				}
			}
			p2 = p2 + 50;
		}

		for (int i = 440; i < 450; i++)
		{
			for (int j = 0; j < 450; j++)
			{
				thresholded31.at<uchar>(j, i) = 0;
			}
		}

		for (int i = 0; i < 450; i++)
		{
			for (int j = 440; j < 450; j++)
			{
				thresholded31.at<uchar>(j, i) = 0;
			}
		}

		for (int i = 0; i < 450; i++)
		{
			for (int j = 150; j < 160; j++)
			{
				thresholded31.at<uchar>(j, i) = 0;
			}

		}

		/*imshow("Nueva Imagen Binarizada", thresholded31);*/
	}
	void entrenar() {
		int num = y + 2;
		int tam = 16 * 16;
		Mat trainData = Mat(Size(tam, num), CV_32FC1);
		Mat responces = Mat(Size(1, num), CV_32FC1);

		int counter = 0;
		for (int i = 0; i <= 9; i++)
		{
			DIR *dir;
			struct dirent *ent;
			char pathToImages[] = "./digits3";
			char path[255];
			sprintf_s(path, "%s/%d", pathToImages, i);
			if ((dir = opendir(path)) != NULL)
			{

				while ((ent = readdir(dir)) != NULL)
				{

					if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0)
					{

						char text[255];
						sprintf_s(text, "/%s", ent->d_name);
						string digit(text);
						digit = path + digit;

						Mat mat = imread(digit, 1);

						cvtColor(mat, mat, CV_BGR2GRAY);

						threshold(mat, mat, 200, 255, THRESH_OTSU);

						mat.convertTo(mat, CV_32FC1, 1.0 / 255.0);

						resize(mat, mat, Size(16, 16), 0, 0, INTER_NEAREST);

						mat.reshape(1, 1);





						for (int k = 0; k < tam; k++)
						{
							trainData.at<float>(counter*tam + k) = mat.at<float>(k);
						}
						responces.at<float>(counter) = i;
						counter++;

					}

				}

			}
			closedir(dir);
		}

		//Ptr<ml::TrainData> TrainingData = ml::TrainData::create();
		/*KNearest knearest(trainData, responces);*/

		/*ROW_SAMPLE
		each training sample is a row of samples*/
	
		knearest->train(trainData, ml::ROW_SAMPLE, responces);
	}
	void encontrar() {
		int size = 16 * 16;
		int m = 0, n = 0; Mat smallimage; Mat smallimage2;
		for (; m < 450; m = m + 50)
		{
			for (n = 0; n < 450; n = n + 50)
			{

				smallimage = Mat(thresholded31, cv::Rect(n, m, 50, 50));


				smallt.push_back(smallimage);
			}
		}


		for (size_t i = 0; i < smallt.size(); i++)
		{


			Mat img123 = Mat(Size(size, 1), CV_32FC1);



			if (countNonZero(smallt[i]) > 200)
			{

				Mat thresholded32; Mat regionOfInterest; Mat img12;


				thresholded32 = smallt[i].clone();

				vector < vector <Point> >contours2;

				findContours(thresholded32, contours2, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

				Rect prevb; double areaprev = 0; double area2; int q;

				for (int j = 0; j < contours2.size(); j++)
				{


					Rect bnd = boundingRect(contours2[j]);

					area2 = bnd.height*bnd.width;

					if (area2 > areaprev)
					{
						prevb = bnd;
						areaprev = area2;

						q = j;
					}
				}






				Rect rec = prevb;

				regionOfInterest = smallt[i](rec);

				resize(regionOfInterest, img12, Size(16, 16), 0, 0, INTER_NEAREST);

				img12.convertTo(img12, CV_32FC1, 1.0 / 255.0);
				img12.reshape(1, 1);

				Mat output;
				if (countNonZero(img12) > 50)
				{

					/*imshow("Numero del Sudoku", img12);*/
										
					

					if (entr == true) {
						stringstream ss;
						string name = "./digits3/";
						string type = ".jpg";
						ss << name << (y + 1) << type;
						string filename = ss.str();
						ss.str("");
						y++;
						std::vector<int> qualityType;
						qualityType.push_back(CV_IMWRITE_JPEG_QUALITY);
						qualityType.push_back(90);
						Mat imageF_8UC3;
						img12.convertTo(imageF_8UC3, CV_8UC3, 255);
						imwrite(filename, imageF_8UC3, qualityType);
					}

					for (int k = 0; k < size; k++)
					{
						img123.at<float>(k) = img12.at<float>(k);
					}

					Mat results;
					Mat neigh;
					Mat dist;

					float p = knearest->findNearest(img123.reshape(1, 1), 1, results, neigh, dist);
					/*cout << p;
					cout << results;
					cout << neigh;
					cout << dist << "\n";*/
					/*if (dist.at<float>(0, 0) >=40)
					{
						p = 0;
					}*/
					/*cout << p;
					waitKey(0);*/
					z[i / 9][i % 9] = p;
				}
				else

					z[i / 9][i % 9] = 0;
			}
			else z[i / 9][i % 9] = 0;
		}
	}
	void printSudokuTomado() {
		printf(" -------------------------\n");
		printf("Sudoku tomado: \n");
		printf(" -------------------------\n");

		printGrid(z);

		for (int i = 0; i < 9; i++)
		{
			for (int j = 0; j < 9; j++)
			{
				grid[i][j] = z[i][j];
			}


		}
	}
	int	printSudokuResuelto() {
		if (SolveSudoku(grid) == true) {
			printf(" -------------------------\n");
			printf("Sudoku resuelto: \n");
			printf(" -------------------------\n");
			printGrid(grid);
			complete();
			waitKey(0);
		}

		else {
		printf("SUDOKU MAL TOMADO");
		//cout << "Problem en la fila: " << row << " y en la columna: " << col;
		waitKey(0);
		return 0;
	}
	}
	void consola() {
		HWND hCurWnd = ::GetForegroundWindow();
		DWORD dwMyID = ::GetCurrentThreadId();
		DWORD dwCurID = ::GetWindowThreadProcessId(hCurWnd, NULL);
		HWND m_hWnd = ::GetConsoleWindow();
		::SetForegroundWindow(m_hWnd);
		::AttachThreadInput(dwCurID, dwMyID, FALSE);
		::SetFocus(m_hWnd);
		::SetActiveWindow(m_hWnd);
		::SetForegroundWindow(m_hWnd);
		::AttachThreadInput(dwCurID, dwMyID, TRUE);
		SetWindowPos(
			m_hWnd, // window handle
			HWND_TOPMOST, // "handle to the window to precede
						  // the positioned window in the Z order
						  // OR one of the following:"
						  // HWND_BOTTOM or HWND_NOTOPMOST or HWND_TOP or HWND_TOPMOST
			0, 0, // X, Y position of the window (in client coordinates)
			0, 0, // cx, cy => width & height of the window in pixels
			SWP_DRAWFRAME | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW // The window sizing and positioning flags.
		);
		// OPTIONAL ! - SET WINDOW'S "SHOW STATE"
		ShowWindow(
			m_hWnd, // window handle
			SW_NORMAL // how the window is to be shown
					  // SW_NORMAL => "Activates and displays a window.
					  // If the window is minimized or maximized,
					  // the system restores it to its original size and position.
					  // An application should specify this flag
					  // when displaying the window for the first time."
		);
	}
	
}

//---------------METODO MAIN-------------------
int main(int argc, char** argv)
{
	using namespace MetodosPrincipales;
	using namespace cv;

	
	//Metodo que carga la imagen, sino se carga, sale de la aplicacion y retorna -1
	if (cargarImagen() == -1) {
		return -1;
	}
	//Pone a la consola siempre en foreground
	/*consola();*/
	
	//Metodo que binariza la imagen
	binarizacion();

	//Busco contornos
	buscarContorno();

	//Ordeno las puntas de la imagen del recuadro del sudoku
	ordenarPuntas();

	//Corto el pedacito del Sudoku
	cortarFoto();

	//Binarizo la parte cortada
	binarizacionSudoku();

	//Entrenar OCR
	entrenar();

	//Encontrar el mas cercano
	encontrar();

	//Print Sudoku Tomado
	printSudokuTomado();
	
	//Print Sudoku Tomado
	if (printSudokuResuelto() == 0) {
		//return 0;
	}
	
	
}