// main.cpp
#include <iostream>
#include <chrono>
#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif
#include <string>
#include <cstdio>

using PROGRESS = int(*)(int percent, char* msg); 

#ifdef _WIN32
void moveCursorUp(int lines)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord;
    CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
    GetConsoleScreenBufferInfo(hConsole, &bufferInfo);
    coord.X = bufferInfo.dwCursorPosition.X;
    coord.Y = bufferInfo.dwCursorPosition.Y - lines;
    SetConsoleCursorPosition(hConsole, coord);
}
#endif

void ErrorCall()
{
    #ifdef _WIN32
        GetLastError();
    #else
        dlerror();
    #endif
}

int progress(int percent, char* msg)
{
    #ifdef _WIN32
    if (percent < 100) {moveCursorUp(1); std::cout << "percent = " << percent << std::endl;}
    #else
        // printf("percent = %d; msg = %s\n", percent, msg);
        // printf("\033[A");
        // printf("\033[K");
        #endif
    return 1;
}

using FunctionType = int(*)(char *ima1, char* ima2, char* ima3, char* ima4, char* ima5,
	int teps, double eps, int otb, double rmin, double cont,
	double xsetndu, double xsetkdu, double ysetndu, double ysetkdu, int nxset, int nyset,
	double osred, int ITERAZ,
	char* maska, double kodmask, PROGRESS progr, int THREADCOUNT);

int main()
{
//    constexpr char* LibPath = 
    #ifdef _WIN32
        constexpr char* LibPath = "Interp_new.dll";
        HMODULE hModule = LoadLibrary(LibPath);
    #else 
        const char* LibPath = "libInterp_new.so";
        void* hModule = dlopen(LibPath, RTLD_LAZY);
    #endif
    if (!hModule){
        std::cerr << "Error loading DLL: " << ErrorCall << std::endl;
        return 1;
    }

    #ifdef _WIN32
        FunctionType func = (FunctionType)GetProcAddress(hModule, "monlan_thread_call");
    #else
        FunctionType func = (FunctionType)dlsym(hModule, "monlan_thread_call");
    #endif
    if (!func) {
        std::cerr << "Error getting function address: " << ErrorCall << std::endl;
        #ifdef _WIN32
            FreeLibrary(hModule);
        #else
            dlclose(hModule);
        #endif

        return 1;
    }

    int ColCount = 1420;
    int RowCount = 2171;
    double Ymin = 28000.0;
    double Ymax = 245000.0;
    double Xmin = 8000.0;
    double Xmax = 149900.0;

    char points_name[20] = "points.dat";
    char faults_name[20] = "faults.bln";
    char model_name[20] = "model.3d"; 

    for (int ThreadCount = 8; ThreadCount > 0; --ThreadCount) {
        auto start = std::chrono::high_resolution_clock::now();
        std::cout << "----------------------------------------" << std::endl;
        std::cout << "Calculating with " << ThreadCount << " threads..." << std::endl; // << std::endl;
        
        char grid_name[20] = "";
        snprintf(grid_name, sizeof(grid_name), "grid-%d.grd", ThreadCount);
            func(points_name, faults_name, model_name, grid_name, nullptr,
                1, 0.001, 0, 0, -100000,
              Xmin, Xmax, Ymin, Ymax, ColCount, RowCount,
              0, 300, nullptr, /*1.70141e+38*/0, &progress, ThreadCount);
        
              auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = end - start; // in ms
        #ifdef _WIN32
        moveCursorUp(2);                    
        #endif
        std::cout << "Calculation time with " << ThreadCount << " threads : " <<  duration.count() << " ms" << std::endl;
    }

    #ifdef _WIN32
        FreeLibrary(hModule);
    #else
        dlclose(hModule);
    #endif

    return 0;
}