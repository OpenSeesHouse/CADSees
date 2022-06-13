#pragma once
#ifdef CADSEESOBJECTS_MODULE
#define DLLIMPEXP __declspec(dllexport)
#else
#define DLLIMPEXP
#endif
#include "dbmain.h"
struct  comp {
	bool operator () (const AcGePoint3d& a, const AcGePoint3d& b) const {
		AcString str1, str2;
		str1.format(_T("%.4f_%.4f_%.4f"), a.x, a.y, a.z);
		str2.format(_T("%.4f_%.4f_%.4f"), b.x, b.y, b.z);
		return str1 < str2;
	}
};

class DLLIMPEXP CSSDocData : public AcDbObject
{
public:
	ACRX_DECLARE_MEMBERS(CSSDocData);
	static Adesk::UInt32 kCurrentVersionNumber;
	class DispOptions
	{
	public:
		DispOptions() {
			 dispNodeTags = false;
			 dispEleTags = false;
			 dispDeformedShape = false;
			 dispUndeformedWire = true;
			 nodeSize = 0.05;
			 tagSize = 0.15;
			 nodeSizeChanged = false;
		}
		bool dispNodeTags;
		bool dispEleTags;
		bool dispDeformedShape;
		bool dispUndeformedWire;
		double nodeSize;
		double tagSize;
		bool nodeSizeChanged;
	};
	CSSDocData();
	virtual Acad::ErrorStatus dwgOutFields(AcDbDwgFiler* pFiler) const;
	virtual Acad::ErrorStatus dwgInFields(AcDbDwgFiler* pFiler);
	void setNodeSize(double val);
	void setTagsSize(double val);
	void setNdm(int val);
	void setNdof(int val);
	void dispEleTags(bool val);
	void dispNodeTags(bool val);
	void dispDeformedShape(bool val, bool apply);
	void dispUndeformedWire(bool val);
	void save();
	void setDataId(AcDbObjectId id) {
		dataId = id;
	};
	inline DispOptions& getDispOptions() {
		return dispOptions;
	};
	inline int getNdm() const { return NDM; }
	inline int getNdof() const { return NDOF; }
	static DispOptions defaultDispOptions();
private:
	DispOptions dispOptions;
	int NDM;
	int NDOF;
	bool changed;
	AcDbObjectId dataId;
};

#ifdef CADSEESOBJECTS_MODULE
ACDB_REGISTER_OBJECT_ENTRY_AUTO(CSSDocData)
#endif
