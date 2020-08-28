#include "stdafx.h"
#include "Matrix.h"

Matrix::Matrix()
{
	nCols = 0;
	nRows = 0;
	data = 0;

}
Matrix::Matrix(int nrows, int ncols)
{
	nCols = ncols;
	nRows = nrows;
	data = new double* [ncols];
	for (int i = 0; i < ncols; i++)
		data[i] = new double[nrows];
}

Matrix::~Matrix()
{
	if (data == 0)
		return;
	for (int i = 0; i < nCols; i++)
		delete[] data[i];
	delete[] data;
}

 Vector Matrix::operator[] (int col) {
	try {
		return Vector(data[col], nRows); 
	}
	catch (const std::exception & e) {
		throw e;
	}
}
