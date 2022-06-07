// (C) Copyright 2002-2007 by Autodesk, Inc. 
//
// Permission to use, copy, modify, and distribute this software in
// object code form for any purpose and without fee is hereby granted, 
// provided that the above copyright notice appears in all copies and 
// that both that copyright notice and the limited warranty and
// restricted rights notice below appear in all supporting 
// documentation.
//
// AUTODESK PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF
// MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK, INC. 
// DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//
// Use, duplication, or disclosure by the U.S. Government is subject to 
// restrictions set forth in FAR 52.227-19 (Commercial Computer
// Software - Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
// (Rights in Technical Data and Computer Software), as applicable.
//

//-----------------------------------------------------------------------------
//----- CSSElement.h : Declaration of the CSSElement
//-----------------------------------------------------------------------------
#pragma once

#ifdef CADSEESOBJECTS_MODULE
#define DLLIMPEXP __declspec(dllexport)
#else
//----- Note: we don't use __declspec(dllimport) here, because of the
//----- "local vtable" problem with msvc. If you use __declspec(dllimport),
//----- then, when a client dll does a new on the class, the object's
//----- vtable pointer points to a vtable allocated in that client
//----- dll. If the client dll then passes the object to another dll,
//----- and the client dll is then unloaded, the vtable becomes invalid
//----- and any virtual calls on the object will access invalid memory.
//-----
//----- By not using __declspec(dllimport), we guarantee that the
//----- vtable is allocated in the server dll during the ctor and the
//----- client dll does not overwrite the vtable pointer after calling
//----- the ctor. And, since we expect the server dll to remain in
//----- memory indefinitely, there is no problem with vtables unexpectedly
//----- going away.
#define DLLIMPEXP
#endif

//-----------------------------------------------------------------------------
#include "dbspline.h"
#define NODSIZERAT 0.03
class CSSNode;

//-----------------------------------------------------------------------------
class DLLIMPEXP CSSElement : public AcDbEntity {

public:
	ACRX_DECLARE_MEMBERS(CSSElement);
	static std::vector<std::string> getSupportedEleList(std::vector<int>& eleNumNodes)
	{
		std::vector<std::string> res;
		res.push_back("truss");
		res.push_back("corotTruss");
		res.push_back("elasticBeamColumn");
		res.push_back("twoNodeLink");
		res.push_back("dispBeamColumn");
		res.push_back("forceBeamColumn");
		res.push_back("nonlinearBeamColumn");
		res.push_back("zeroLength");
		res.push_back("ModElasticBeam2d");
		res.push_back("ModElasticBeam3d");
		res.push_back("Joint2D");
		res.push_back("20_8_BrickUP");
		res.push_back("SSPbrickUP");
		res.push_back("SSPbrick");
		res.push_back("stdBrick");
		res.push_back("bbarBrick");
		res.push_back("bbarBrickUP");

		eleNumNodes.push_back(2);
		eleNumNodes.push_back(2);
		eleNumNodes.push_back(2);
		eleNumNodes.push_back(2);
		eleNumNodes.push_back(2);
		eleNumNodes.push_back(2);
		eleNumNodes.push_back(2);
		eleNumNodes.push_back(2);
		eleNumNodes.push_back(2);
		eleNumNodes.push_back(2);
		eleNumNodes.push_back(4);
		eleNumNodes.push_back(20);
		eleNumNodes.push_back(8);
		eleNumNodes.push_back(8);
		eleNumNodes.push_back(8);
		eleNumNodes.push_back(8);
		eleNumNodes.push_back(8);
		return res;
	}
protected:
	AcDbEntity* getDeformedEntity();
	AcDbEntity* getUndeformedEntity();
	virtual Adesk::Boolean subWorldDraw(AcGiWorldDraw* mode);
public:
	CSSElement();
	CSSElement(int tag, std::vector<int> nodeTags, std::string type);
	virtual ~CSSElement();

	//----- AcDbObject protocols
	//- Dwg Filing protocol
	virtual Acad::ErrorStatus dwgOutFields(AcDbDwgFiler* pFiler) const;
	virtual Acad::ErrorStatus dwgInFields(AcDbDwgFiler* pFiler);
public:

	//- Grip points protocol
	virtual void subList() const override;
	virtual Acad::ErrorStatus subErase(Adesk::Boolean pErasing);

protected:
	AcDbEntity* pDeformedEntity;
	AcDbEntity* pUndeformedEntity;
	static Adesk::UInt32 kCurrentVersionNumber;
	Adesk::Boolean m_isNull;
	Adesk::UInt32 m_tag;
	AcString m_type;
	std::vector<int> m_nodes;
	std::vector <AcGePoint3d> m_crds;
	std::vector<CSSNode*> m_nodePtrs;

public:
	int getTag() const;
	int getIsNull() const;
	virtual bool updateGeometry(bool useDeformedGeom);
};

#ifdef CADSEESOBJECTS_MODULE
ACDB_REGISTER_OBJECT_ENTRY_AUTO(CSSElement)
#endif
