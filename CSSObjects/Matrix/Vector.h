#pragma once
class Vector
{
public:
	Vector();
	Vector(int n);
	Vector(double* d, int n);
	int Size() const {
		return size;
	}
	inline double& operator() (int i)
	{
		try {
			return data[i];
		}
		catch (const std::exception & e) {
			throw e;
		}
	}

private:
	int size;
	bool fromData;
	double* data;
};

