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
//----- CSSRecorder.h : Declaration of the CSSRecorder
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

#include <map>
#include "Matrix/Matrix.h"
#include "Matrix/Vector.h"
//-----------------------------------------------------------------------------
#include "dbmain.h"
#define NODENTRYKEY _T("CSSRecorders")

//-----------------------------------------------------------------------------
class DLLIMPEXP CSSRecorder : public AcDbObject {

public:
	ACRX_DECLARE_MEMBERS(CSSRecorder) ;

protected:
	static Adesk::UInt32 kCurrentVersionNumber ;

public:
	CSSRecorder () ;
	CSSRecorder (int objTag, int dof, std::string path, int dataCol, bool hasTime) ;
	virtual ~CSSRecorder () ;

	//----- AcDbObject protocols
	//- Dwg Filing protocol
	virtual Acad::ErrorStatus dwgOutFields (AcDbDwgFiler *pFiler) const ;
	virtual Acad::ErrorStatus dwgInFields (AcDbDwgFiler *pFiler) ;
protected:
	Adesk::UInt32 m_objTag;
	Adesk::UInt32 m_rcrdrTag;
	Adesk::UInt32 m_dof;
	AcString m_relFilePath;
	Adesk::UInt32 m_dataColId;			//zero-based number of data column
	Vector m_respVec;
	bool m_hasTime;
	static bool readFileData(AcString file, std::string folder, bool hasTime);
public:
	static std::map<AcString, Matrix*> m_fileMap;	//maps file names to collections of data columns (not data rows)
	static Vector timeVec;
	static int lastTag;
	//get functions
	AcString getRelFilePath() const;
	int getFileColId() const;
	int getRcrdrTag() const;
	int getDof() const;
	int getObjTag() const;
	bool getHasTime() const;			//used only to know if time data are provided in this recorder's file
	static Vector& getTimeVec();
	static int getLastTag();

	void setDof(int val);
	void setHasTime(bool val);
	void setPath(std::string path);
	void setDataCol(int dataCol);
	//response
	virtual bool applySelf(double t, double fac);
	virtual bool applySelf(int nStep, double fac);
	//to be implemented by derived classes
	bool recordResponse(std::string folder);
	//set functions
	//static void setTimeVec(VECTYPE& vec, int size);
	static void setLastTag(int val);
} ;

#ifdef CADSEESOBJECTS_MODULE
ACDB_REGISTER_OBJECT_ENTRY_AUTO(CSSRecorder)
#endif
