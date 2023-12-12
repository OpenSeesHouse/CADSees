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
//----- CSSNode.h : Declaration of the CSSNode
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
//#include <vector>

//-----------------------------------------------------------------------------
class CSSNode;
class CSSLineElement;
class AcDbBlockTableRecord;
class CSSDocData;
//-----------------------------------------------------------------------------
class DLLIMPEXP ObjUtils {
public:
	static std::vector<std::string> pars(std::string str, const char* delimiter);
	static AcDbObjectId getModelSpaceId();
	static AcDbBlockTableRecord* getModelSpace(OpenMode mode);
	static void setDocData(CSSDocData*& data);
	//static void setRlsData(bool rls) {DocVars.docData(curDoc()).relsData = rls;}
	static CSSDocData* getDocData();
	/*static int getNdm();
	static int getNdof();
	static double getNodeSize();
	static void setNdm(int val);
	static void setNdof(int val);
	static void setShowDeformed(bool deformed);
	static void setShowUndeformedWire(bool val);
	static void setShowNodeTags(bool val);
	static void setShowEleTags(bool val);
	static void setTagsSize(double val);
	static void setNodesSize(double val);*/

	static bool getNode(AcDbObjectId& resId, int tag);
	static bool getNodeCrds(int tag, AcGePoint3d*);
	static bool getElement(AcDbObjectId* pResId, int tag);
	static bool getRecorder(AcDbObjectId* pId, int obj_tag, int dof, const char* respType);
	static void GetAllRecorders(std::vector<AcDbObjectId> &resIds);
	static int getNumNodesAtCrd(AcGePoint3d pnt);
	static void GetAllNodes(std::vector<AcDbObjectId>& resIds);
	static void GetAllElements(std::vector<AcDbObjectId>& resids);
	//static AcDbObjectId GetAnyNode();

	static void addPile(AcGePoint3d vertex, AcGePoint3d vertex2);
	static void addCube(AcGePoint3d vertex, double sizeX, double sizeY, double sizeZ);
	static void addNode(int tag, AcGePoint3d pnt);
	static void addElement(std::string type, int tag, std::vector<int> nodeTags, std::vector<std::string> line);
	static void addNodeRecorder(int* objTags, int* dofs, int num, std::string path, int* dataCols, bool hasTime);
	static void RedrawGraphics(bool redrawBody = false);
	static void RedrawNodeGraphics(bool redrawBody = false);
	static void RedrawElementsGraphics(bool redrawBody = false);
	static bool getShowDeformed();
} ;

