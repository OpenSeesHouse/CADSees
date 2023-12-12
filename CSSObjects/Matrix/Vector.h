#pragma once
#ifdef CADSEESOBJECTS_MODULE
#define DLLIMPEXP __declspec(dllexport)
#else
#define DLLIMPEXP
#endif

class DLLIMPEXP Vector
{
public:
	Vector();
	~Vector();
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

