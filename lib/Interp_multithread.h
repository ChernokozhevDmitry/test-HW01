#include <iostream>
#include <thread>
#include <future>
#include <vector>
#include <string>
#include <chrono>
#include <fstream>

#include <cmath>

// Структуры для передачи аргументов потоку в monlan4
struct DataIOfiles {
//	int thread_id;
	char* thread_ima1; // входной файл с координатами и значениями *.dat
	char* thread_ima2; // входной файл с разломами *.bln
	char* thread_ima3; // расчетный модельный файл model
	char* thread_ima4; // выходной файл грида *.grd
	char* thread_ima5; // выходной файл (вспомогательный - расчет в точках)
};

struct DataRarelyNeed { // 
	int thread_teps;
	double thread_eps;
	int thread_otb;
	double thread_rmin;
	double thread_cont;
	double thread_osred;
	int thread_ITERAZ;
};
struct DataGrid {
	double thread_xsetndu; // координата X min
	double thread_xsetkdu; // координаат X max
	double thread_ysetndu; // координата Y min
	double thread_ysetkdu; // координата Y max
	int thread_nxset; // число точек по X (растр)
	int thread_nyset; // число точек по Y (растр)
};

struct DataMaska {
	char* thread_maska; // 0, 1 - не считать/считать значение в точке
	double thread_kodmask; // какое значение подставлять, если maska 0
};

struct DataSystem{
		int thread_id; // номер потока
		PROGRESS thread_progr; // обратная связь прогресса выполнения
};

//void monlan_threading(thread_data& data);

extern "C" {
	#ifdef _WIN32
	__declspec(dllexport)
	#endif
		int monlan_thread_call(char *ima1, char* ima2, char* ima3, char* ima4, char* ima5,
		int teps, double eps, int otb, double rmin, double cont,
		double xsetndu, double xsetkdu, double ysetndu, double ysetkdu, int nxset, int nyset,
		double osred, int ITERAZ,
		char* maska, double kodmask, PROGRESS progr, int THREADCOUNT);
}