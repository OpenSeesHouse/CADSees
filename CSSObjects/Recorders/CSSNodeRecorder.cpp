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
//----- CSSNodeRecorder.cpp : Implementation of CSSNodeRecorder
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "CSSNodeRecorder.h"
//-----------------------------------------------------------------------------
Adesk::UInt32 CSSNodeRecorder::kCurrentVersionNumber =1 ;

//-----------------------------------------------------------------------------
ACRX_DXF_DEFINE_MEMBERS (
	CSSNodeRecorder, CSSRecorder,
	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent, 
	AcDbProxyEntity::kNoOperation, CSSNODERECORDER,
CADSees
|Product Desc:     An OpenSees pre/post-processor
|Company:          Civil Soft Science
|WEB Address:      www.CivilSoftScience.com
)

//-----------------------------------------------------------------------------
CSSNodeRecorder::CSSNodeRecorder () : CSSRecorder () {

}

CSSNodeRecorder::CSSNodeRecorder(int objTag, int dof, std::string path, int dataCol, bool hasTime) : CSSRecorder (objTag, dof, path, dataCol, hasTime)
{
}

CSSNodeRecorder::~CSSNodeRecorder () {
}

//-----------------------------------------------------------------------------
//----- AcDbObject protocols
//- Dwg Filing protocol
Acad::ErrorStatus CSSNodeRecorder::dwgOutFields (AcDbDwgFiler *pFiler) const {
	assertReadEnabled () ;
	//----- Save parent class information first.
	Acad::ErrorStatus es =CSSRecorder::dwgOutFields (pFiler) ;
	if ( es != Acad::eOk )
		return (es) ;
	//----- Object version number needs to be saved first
	if ( (es =pFiler->writeUInt32 (CSSNodeRecorder::kCurrentVersionNumber)) != Acad::eOk )
		return (es) ;
	//----- Output params
	return (pFiler->filerStatus ()) ;
}

Acad::ErrorStatus CSSNodeRecorder::dwgInFields (AcDbDwgFiler *pFiler) {
	assertWriteEnabled () ;
	//----- Read parent class information first.
	Acad::ErrorStatus es =CSSRecorder::dwgInFields (pFiler) ;
	if ( es != Acad::eOk )
		return (es) ;
	//----- Object version number needs to be read first
	Adesk::UInt32 version =0 ;
	if ( (es =pFiler->readUInt32 (&version)) != Acad::eOk )
		return (es) ;
	if ( version > CSSNodeRecorder::kCurrentVersionNumber )
		return (Acad::eMakeMeProxy) ;
	//- Uncomment the 2 following lines if your current object implementation cannot
	//- support previous version of that object.
	//if ( version < CSSNodeRecorder::kCurrentVersionNumber )
	//	return (Acad::eMakeMeProxy) ;
	//----- Read params
	return (pFiler->filerStatus ()) ;
}

bool CSSNodeRecorder::applySelf(double t, double fac)
{
	assertReadEnabled();
	int i = 0;
	int timeSize = timeVec.Size();
	if (t == -1)
	{
		i = timeSize-1;
	}
	else
	{
		while(i < timeSize)
		{
			if (abs(timeVec(i)-t) < 1.e-6)
				break;
			i++;
		}
	}
	if(i == timeSize)
	{
		acutPrintf(L"CSSNodeRecorder::applySelf-Failed to find requested time in pTimeVec");
		return false;		
	}
	double value = m_respVec(i)*fac;

	AcDbObjectId id;
	if(!ObjUtils::getNode(id, m_objTag))
	{
		acutPrintf(_T("CSSNodeRecorder::applySelf-Failed to getNode() with tag %d"), m_objTag);
		return false;
	}
	AcDbObject* obj;
	ErrorStatus es = actrTransactionManager->getObject(obj, id, OpenMode::kForWrite);
	assert(obj != NULL);
	CSSNode* pNode = CSSNode::cast(obj);
	assert(pNode != NULL);
	pNode->setDeformationAt(m_dof, value);
	return true;
}

bool CSSNodeRecorder::applySelf(int nStep, double fac)
{
	if (nStep < 0)
		nStep = 0;
	double value;
	if (nStep >= m_respVec.Size())
		value = 0;
	else
		value = m_respVec(nStep)*fac;
	AcDbObjectId id;
	if(!ObjUtils::getNode(id, m_objTag))
	{
		acutPrintf(_T("CSSNodeRecorder::applySelf-Failed to getNode() with tag %d"), m_objTag);
		return false;
	}
	AcDbObject* obj;
	ErrorStatus es = actrTransactionManager->getObject(obj, id, OpenMode::kForWrite);
	assert(obj != NULL);
	CSSNode* pNode = CSSNode::cast(obj);
	assert(pNode != NULL);
	pNode->setDeformationAt(m_dof, value);
	return true;
}