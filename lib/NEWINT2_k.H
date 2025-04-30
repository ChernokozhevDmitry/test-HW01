#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#include <windows.h> // for _lcreate, _lopen, _lclose, _lread, _lwrite for windows
#else
#include <unistd.h> // for read, write for linux
#include <fcntl.h>  // for create, open, close for linux
#endif

// multithreading
int monlan4(char*, char*, char*, char*, char*,
	int, double, int, double, double,
	double, double, double, double, int, int,
	double, int,
	char*, double, PROGRESS, int);

// singlethreading
int monlan3(char*, char*, char*, char*, char*,
	int, double, int, double, double,
	double, double, double, double, int, int,
	double, int,
	char*, double, PROGRESS);

// make grd array for saving to file   
int surf2(int long, int long, char*, int);   

#ifdef __cplusplus
}
#endif