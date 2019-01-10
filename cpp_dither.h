#define DLL_EXPORT __declspec(dllexport)

//DLL_EXPORT double threshIt(double pix, double colors);
extern "C"
{
	//DLL_EXPORT double get2DArrayMean(vector<vector<double>> pixels);
	DLL_EXPORT double get2DArrayMean(double** pixels, int rows, int cols);
	//DLL_EXPORT void getAArray(vector<vector<double>> pixels, vector<vector<double>> &a_array);
	DLL_EXPORT void getAArray(double** pixels, double** a_array, int rows, int cols);
	DLL_EXPORT double threshIt(double pix, double colors);
	DLL_EXPORT int getDoubleIntensity_asInt_andClip(double apix);
	DLL_EXPORT double GetRandomNumber(uniform_real_distribution<double> unif, default_random_engine &re);
	//DLL_EXPORT void dither_VarcoBreak(vector<vector<double>> &pixels, double numcolors);
	DLL_EXPORT void dither_VarcoBreak(double** pixels, int rows, int cols, double numcolors);
	//DLL_EXPORT void dither_VarcoBlue(vector<vector<double>> &pixels, double numcolors, bool serpentine);
	DLL_EXPORT void dither_VarcoBlue(double** pixels, int rows, int cols, double numcolors, bool serpentine);
}