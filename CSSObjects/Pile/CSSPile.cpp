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
//----- CSSPile.cpp : Implementation of CSSPile
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "CSSPile.h"

//-----------------------------------------------------------------------------
Adesk::UInt32 CSSPile::kCurrentVersionNumber =1 ;

//double CSSPile::douplShift = 0;
//-----------------------------------------------------------------------------
ACRX_DXF_DEFINE_MEMBERS (
	CSSPile, AcDbEntity,
	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent, 
	AcDbProxyEntity::kNoOperation, CSSPILE,
CADSees
|Product Desc:     An OpenSees pre/post-processor
|Company:          Civil Soft Science
|WEB Address:      www.CivilSoftScience.com
)

//-----------------------------------------------------------------------------
CSSPile::CSSPile () : AcDbEntity () {
	m_crds1 = AcGePoint3d(0,0,0);
	m_crds2 = AcGePoint3d(0,0,0);
	pFaceList = 0;
	pVertexList = 0;
}

CSSPile::CSSPile(CSSPile*& pCube)
{
	m_crds1 = pCube->getCrds1();
	m_crds2 = pCube->getCrds2();
}

CSSPile::CSSPile(AcGePoint3d crds1, AcGePoint3d crds2) : AcDbEntity()
{
	m_crds1 = crds1;
	m_crds2 = crds2;
	pFaceList = 0;
	pVertexList = 0;
	initialize();
}

CSSPile::~CSSPile () {
	if (pVertexList != 0)
		delete[] pVertexList;
	if (pFaceList != 0)
		delete[] pFaceList;
}

//-----------------------------------------------------------------------------
//----- AcDbObject protocols
//- Dwg Filing protocol
Acad::ErrorStatus CSSPile::dwgOutFields (AcDbDwgFiler *pFiler) const {
	assertReadEnabled () ;
	//----- Save parent class information first.
	Acad::ErrorStatus es =AcDbEntity::dwgOutFields (pFiler) ;
	if ( es != Acad::eOk )
		return (es) ;
	//----- Object version number needs to be saved first
	if ( (es =pFiler->writeUInt32 (CSSPile::kCurrentVersionNumber)) != Acad::eOk )
		return (es) ;
	//----- Output params
	return (pFiler->filerStatus ()) ;
}

Acad::ErrorStatus CSSPile::dwgInFields (AcDbDwgFiler *pFiler) {
	assertWriteEnabled () ;
	//----- Read parent class information first.
	Acad::ErrorStatus es =AcDbEntity::dwgInFields (pFiler) ;
	if ( es != Acad::eOk )
		return (es) ;
	//----- Object version number needs to be read first
	Adesk::UInt32 version =0 ;
	if ( (es =pFiler->readUInt32 (&version)) != Acad::eOk )
		return (es) ;
	if ( version > CSSPile::kCurrentVersionNumber )
		return (Acad::eMakeMeProxy) ;
	//- Uncomment the 2 following lines if your current object implementation cannot
	//- support previous version of that object.
	//if ( version < CSSPile::kCurrentVersionNumber )
	//	return (Acad::eMakeMeProxy) ;
	//----- Read params
	initialize();
	return (pFiler->filerStatus ()) ;
}

//-----------------------------------------------------------------------------
//----- AcDbEntity protocols

Adesk::Boolean CSSPile::subWorldDraw (AcGiWorldDraw *mode) {
	assertReadEnabled () ;
	AcGePoint3d pnts[2];
	pnts[0] = m_crds1;
	pnts[1] = m_crds2;
	mode->geometry().worldLine(pnts);
	return (AcDbEntity::subWorldDraw(mode)) ;
}

Acad::ErrorStatus CSSPile::subTransformBy(const AcGeMatrix3d& xform)
{
	assertWriteEnabled (false, false) ;
	m_crds1.transformBy(xform);
	m_crds2.transformBy(xform);
	return ErrorStatus::eOk;
}

void CSSPile::initialize()
{
}


AcGePoint3d CSSPile::getCrds1() const
{
	assertReadEnabled();
	return m_crds1/*+m_shiftVec*/;
}

AcGePoint3d CSSPile::getCrds2() const
{
	assertReadEnabled();
	return m_crds2/*+m_shiftVec*/;
}

