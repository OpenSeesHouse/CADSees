#include "stdafx.h"
#include "Vector.h"

Vector::Vector()
{
	fromData = false;
	size = 0;
	data = 0;
}

Vector::Vector(int n)
{
	fromData = false;
	size = n;
	data = new double[n];
}

Vector::~Vector()
{
	size = 0;
	if (!fromData && data != 0)
	{
		delete[] data;
	}

}

Vector::Vector(double* d, int n)
{
	fromData = true;
	size = n;
	data = d;
}
