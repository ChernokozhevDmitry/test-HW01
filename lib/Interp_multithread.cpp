#include "global_variable.h"
#include "file_bin_write.h"
#include "Interp_multithread.h"
#include "NEWINT2_k.H"

// Так тоже работает, но лишнее вложение потоковой функции получается
// void monlan_threading(thread_data& data) {
// 	monlan4(data.thread_ima1, data.thread_ima2, data.thread_ima3, data.thread_ima4, data.thread_ima5,
// 		data.thread_teps, data.thread_eps, data.thread_otb, data.thread_rmin, data.thread_cont,
// 		data.thread_xsetndu, data.thread_xsetkdu, data.thread_ysetndu, data.thread_ysetkdu, data.thread_nxset, data.thread_nyset,
// 		data.thread_osred, data.thread_ITERAZ,
// 		data.thread_maska, data.thread_kodmask, data.thread_progr, data.thread_id);
// }

#ifdef _WIN32
__declspec(dllexport)
#endif
int monlan_thread_call(char *ima1, char* ima2, char* ima3, char* ima4, char* ima5,
	int teps, double eps, int otb, double rmin, double cont,
	double xsetndu, double xsetkdu, double ysetndu, double ysetkdu, int nxset, int nyset,
	double osred, int ITERAZ,
	char* maska, double kodmask, PROGRESS progr, int THREADCOUNT) {
	if (THREADCOUNT > 1) {												// если создаем потоки
		// для получения model.3d (ima3) без расчета grd (ima4)
		monlan3(ima1, ima2, ima3, nullptr, ima5,
				teps, eps, otb, rmin, cont,
				xsetndu, xsetkdu, ysetndu, ysetkdu, nxset, nyset,
				osred, ITERAZ,
				maska, kodmask, progr);
		// вектор потоков
		std::vector<std::thread> thread_data_vec(THREADCOUNT);
		//std::vector<std::future<void>> async_data_vec(THREADCOUNT);

		// массив структур для передачи в потоки
		std::vector<DataIOfiles> data_io_files(THREADCOUNT);
		std::vector<DataRarelyNeed> data_rarelyneed(THREADCOUNT);
		std::vector<DataGrid> data_grid(THREADCOUNT);
		std::vector<DataMaska> data_maska(THREADCOUNT);
		std::vector<DataSystem> data_system(THREADCOUNT);

		// определяем число точек по Y, в зависимости от числа потоков, суммарно точек будет чуть больше, чем в исходной сетке nyset
		int ThrRows = static_cast<int>(std::ceil(static_cast<double>(nyset) / static_cast<double>(THREADCOUNT)));
		// считаем шаг по Y
		double Ystep = (ysetkdu - ysetndu) / (static_cast<double>(nyset) - 1);

// БЫЛО		
//		V_global = (float*)malloc((nxset * (ThrRows * THREADCOUNT)) * sizeof(float));
//		if (V_global == NULL) return -1;
		V_global.resize(nxset * (ThrRows * THREADCOUNT));

		std::unique_ptr<char[]> maska_to_thread(new char[nxset * (ThrRows * THREADCOUNT)]);  

		// maska нужна для отрисовки только выделенной области
		if (maska) {
			for (size_t i = 0; i < static_cast<size_t>(nxset * nyset); ++i)
				maska_to_thread[i] = maska[i]; 
			for (size_t i = static_cast<size_t>(nxset * nyset); i < static_cast<size_t>(nxset * (ThrRows * THREADCOUNT)); ++i)
				maska_to_thread[i] = 0; // в данной логике геометрия прямоугольников в потоки сделана симметричной,
										// т.е. в последний поток "придет" чуть больше значений,
										// поэтому "добиваем" лишнюю маску нулями
		}
		else {
			maska_to_thread = nullptr;
		}

	// инициализируем структуры значениями
	for (int t = 0; t < THREADCOUNT; ++t) {
		double ThrYmin = ysetndu + t * ThrRows*Ystep; // минимальная Y координата для потока 
		double ThrYmax = ysetndu + ((t + 1)*ThrRows - 1)*Ystep; // максимальная Y координата для потока
		data_system[t].thread_id = t;
		data_io_files[t].thread_ima1 = nullptr; // в поток не нужен, если модель посчитана в monlan3 выше
		data_io_files[t].thread_ima2 = nullptr; // в поток не нужен, если модель посчитана в monlan3 выше
		data_io_files[t].thread_ima3 = ima3;
		data_io_files[t].thread_ima4 = ima4;
		data_io_files[t].thread_ima5 = ima5;
		data_rarelyneed[t].thread_teps = teps;
		data_rarelyneed[t].thread_eps = eps;
		data_rarelyneed[t].thread_otb = otb;
		data_rarelyneed[t].thread_rmin = rmin;
		data_rarelyneed[t].thread_cont = cont;
		data_grid[t].thread_xsetndu = xsetndu;
		data_grid[t].thread_xsetkdu = xsetkdu;
		data_grid[t].thread_ysetndu = ThrYmin; // передаем "свой" минимум Y для потока 
		data_grid[t].thread_ysetkdu = ThrYmax; // передаем "свой" максимум Y для потока
		data_grid[t].thread_nxset = nxset;
		data_grid[t].thread_nyset = ThrRows;  // передаем "свое" количество точек по Y для потока
		data_rarelyneed[t].thread_osred = osred;
		data_rarelyneed[t].thread_ITERAZ = ITERAZ;
		// >>
		maska_to_thread ?
		(data_maska[t].thread_maska = &maska_to_thread[t*nxset*ThrRows]) :
		(data_maska[t].thread_maska = nullptr);
		// <<
		data_maska[t].thread_kodmask = kodmask;
		// >>
		(t == static_cast<int>(THREADCOUNT / 2)) ? 
		(data_system[t].thread_progr = progr) :
		(data_system[t].thread_progr = nullptr); // для отклика progress в основном вызове берем изменения из среднего потока
		// <<
			thread_data_vec[t] = std::thread(monlan4,
							data_io_files[t].thread_ima1,
							data_io_files[t].thread_ima2,
							data_io_files[t].thread_ima3,
							data_io_files[t].thread_ima4,
							data_io_files[t].thread_ima5,
							data_rarelyneed[t].thread_teps,
							data_rarelyneed[t].thread_eps,
							data_rarelyneed[t].thread_otb,
							data_rarelyneed[t].thread_rmin,
							data_rarelyneed[t].thread_cont,
							data_grid[t].thread_xsetndu,
							data_grid[t].thread_xsetkdu,
							data_grid[t].thread_ysetndu,
							data_grid[t].thread_ysetkdu,
							data_grid[t].thread_nxset,
							data_grid[t].thread_nyset,
							data_rarelyneed[t].thread_osred,
							data_rarelyneed[t].thread_ITERAZ,
							std::ref(data_maska[t].thread_maska),
							data_maska[t].thread_kodmask,
							std::ref(data_system[t].thread_progr),
							data_system[t].thread_id);

//		async_data_vec.emplace_back(std::async(std::launch::async, monlan_threading, std::ref(data[t])));
}

// завершение всех потоков
	for (auto& thr : thread_data_vec) {
		if (thr.joinable()) {
			thr.join();
		}
	}
	//for (auto& asnc : async_data_vec) {
	//	asnc.get();
	//}

	// запись всех сопутствующих данных и рассчитанного массива V_global в файл 
	FileBinWrite file_bin_write(ima4);
	file_bin_write.writeChar("DSBB", 4);
	file_bin_write.write(nxset);
	file_bin_write.write(nyset);
	file_bin_write.write(xsetndu);
	file_bin_write.write(xsetkdu);
	file_bin_write.write(ysetndu);
	file_bin_write.write(ysetkdu);
	file_bin_write.write(fimin_global);
	file_bin_write.write(fimax_global);
	V_global.resize(nxset * nyset);
	file_bin_write.writeOwnArray(V_global);
}
else // если не нужны потоки, считаем просто monlan3 без расчета model.3d (ima3)
{
	monlan3(ima1, ima2, nullptr, ima4, ima5,
		teps, eps, otb, rmin, cont,
		xsetndu, xsetkdu, ysetndu, ysetkdu, nxset, nyset,
		osred, ITERAZ,
		maska, kodmask, progr);
}
	return 0;
}
