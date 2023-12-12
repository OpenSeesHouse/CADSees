#pragma once
#ifdef CADSEESOBJECTS_MODULE
#define DLLIMPEXP __declspec(dllexport)
#else
#define DLLIMPEXP
#endif
#include "Vector.h"
class Matrix
{
public:
	Matrix();
	Matrix(int nrows, int ncols);
	~Matrix();
	int NumCols() const {
		return nCols;
	}
	int NumRows() const {
		return nRows;
	}
	Vector operator[] (int col);
	inline double*& operator() (int col) {
		try {
			return data[col];
		}
		catch (const std::exception & e) {
			throw e;
		}
	}
	inline double& operator() (int col, int row) {
		try {
			return data[col][row];
		}
		catch (const std::exception & e) {
			throw e;
		}
	}
private:
	int nCols, nRows;
	double** data;
};

