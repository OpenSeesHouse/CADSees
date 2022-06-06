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
//----- CSSCube.cpp : Implementation of CSSCube
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "CSSCube.h"

//-----------------------------------------------------------------------------
Adesk::UInt32 CSSCube::kCurrentVersionNumber =1 ;

//double CSSCube::douplShift = 0;
//-----------------------------------------------------------------------------
ACRX_DXF_DEFINE_MEMBERS (
	CSSCube, AcDbEntity,
	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent, 
	AcDbProxyEntity::kNoOperation, CSSCUBE,
CADSees
|Product Desc:     An OpenSees pre/post-processor
|Company:			 Civil Soft Science
|WEB Address: www.CivilSoftScience.com
)

//-----------------------------------------------------------------------------
CSSCube::CSSCube () : AcDbEntity () {
	m_crds = AcGePoint3d(0,0,0);
	pFaceList = 0;
	pVertexList = 0;
}

CSSCube::CSSCube(CSSCube*& pCube)
{
	m_crds = pCube->getCrds();
	pCube->getSize(&m_sizeX, &m_sizeY, &m_sizeZ);
	pVertexList = new AcGePoint3d[8];
	pFaceList = new Adesk::Int32[30];
	AcGePoint3d*& otherVert = pCube->getVertexList();
	for (int i = 0; i < 8; i++)
		pVertexList[i] = otherVert[i];
	Adesk::Int32*& otherfaces = pCube->getFaceList();
	for (int i = 0; i < 30; i++)
		pFaceList[i] = otherfaces[i];
}

CSSCube::CSSCube(AcGePoint3d crds, double size) : AcDbEntity()
{
	m_crds = crds;
	pFaceList = 0;
	pVertexList = 0;
	m_sizeX = size;
	m_sizeY = size;
	m_sizeZ = size;
	m_useVertex = false;
	initialize();
}

CSSCube::CSSCube(AcGePoint3d vertex, double sizeX, double sizeY, double sizeZ)
{
	 m_crds = vertex;
	 m_sizeX = sizeX;
	 m_sizeY = sizeY;
	 m_sizeZ = sizeZ;
	 pFaceList = 0;
	 pVertexList = 0;
	 m_useVertex = true;
	 initialize();
}

CSSCube::~CSSCube () {
	if (pVertexList != 0)
		delete[] pVertexList;
	if (pFaceList != 0)
		delete[] pFaceList;
}

//-----------------------------------------------------------------------------
//----- AcDbObject protocols
//- Dwg Filing protocol
Acad::ErrorStatus CSSCube::dwgOutFields (AcDbDwgFiler *pFiler) const {
	assertReadEnabled () ;
	//----- Save parent class information first.
	Acad::ErrorStatus es =AcDbEntity::dwgOutFields (pFiler) ;
	if ( es != Acad::eOk )
		return (es) ;
	//----- Object version number needs to be saved first
	if ( (es =pFiler->writeUInt32 (CSSCube::kCurrentVersionNumber)) != Acad::eOk )
		return (es) ;
	//----- Output params
	if ( (es =pFiler->writeItem (m_crds)) != Acad::eOk )
		return (es) ;
	if ( (es =pFiler->writeItem (m_sizeX)) != Acad::eOk )
		return (es) ;
	if ( (es =pFiler->writeItem (m_sizeY)) != Acad::eOk )
		return (es) ;
	if ( (es =pFiler->writeItem (m_sizeZ)) != Acad::eOk )
		return (es) ;
	return (pFiler->filerStatus ()) ;
}

Acad::ErrorStatus CSSCube::dwgInFields (AcDbDwgFiler *pFiler) {
	assertWriteEnabled () ;
	//----- Read parent class information first.
	Acad::ErrorStatus es =AcDbEntity::dwgInFields (pFiler) ;
	if ( es != Acad::eOk )
		return (es) ;
	//----- Object version number needs to be read first
	Adesk::UInt32 version =0 ;
	if ( (es =pFiler->readUInt32 (&version)) != Acad::eOk )
		return (es) ;
	if ( version > CSSCube::kCurrentVersionNumber )
		return (Acad::eMakeMeProxy) ;
	//- Uncomment the 2 following lines if your current object implementation cannot
	//- support previous version of that object.
	//if ( version < CSSCube::kCurrentVersionNumber )
	//	return (Acad::eMakeMeProxy) ;
	//----- Read params
	if ( (es =pFiler->readItem (&m_crds)) != Acad::eOk )
		return (es) ;
	if ( (es =pFiler->readItem (&m_sizeX)) != Acad::eOk )
		return (es) ;
	if ( (es =pFiler->readItem (&m_sizeY)) != Acad::eOk )
		return (es) ;
	if ( (es =pFiler->readItem (&m_sizeZ)) != Acad::eOk )
		return (es) ;
	initialize();
	return (pFiler->filerStatus ()) ;
}

//-----------------------------------------------------------------------------
//----- AcDbEntity protocols

Adesk::Boolean CSSCube::subWorldDraw (AcGiWorldDraw *mode) {
	assertReadEnabled () ;
	if (pVertexList == 0)
		initialize();
	mode->geometry().shell(8, pVertexList, 30, pFaceList);
	return (AcDbEntity::subWorldDraw(mode)) ;
}

Acad::ErrorStatus CSSCube::subTransformBy(const AcGeMatrix3d& xform)
{
	assertWriteEnabled (false, false) ;
	if (pVertexList == 0)
		return ErrorStatus::eOk;
	for (int i = 0; i < 8; i++)
		pVertexList[i].transformBy(xform);
	return ErrorStatus::eOk;
}

void CSSCube::initialize()
{
	assertWriteEnabled (false, false) ;
	AcGeMatrix3d trans;
	if (m_useVertex)
	{
		 AcGeVector3d vec = m_crds.asVector()+AcGeVector3d(m_sizeX/2,m_sizeY/2,m_sizeZ/2);
		 trans.setToTranslation(vec);
	}
	else
		 trans.setToTranslation((m_crds/*+m_shiftVec*douplShift*/).asVector());
	if (pVertexList == NULL)
		pVertexList = new AcGePoint3d[8];
	for (int i = 1; i <= 8; i++)
	{
		double x, y, z;
		switch(i)
		{
		case 1:
			x = -m_sizeX/2;
			y = -m_sizeY/2;
			z = -m_sizeZ/2;
			break;
		case 2:
			x =  m_sizeX / 2;
			y = -m_sizeY / 2;
			z = -m_sizeZ / 2;
			break;
		case 3:
			x = m_sizeX / 2;
			y = m_sizeY / 2;
			z = -m_sizeZ / 2;
			break;
		case 4:
			x = -m_sizeX / 2;
			y = m_sizeY / 2;
			z = -m_sizeZ / 2;
			break;
		case 5:
			x = -m_sizeX / 2;
			y = -m_sizeY / 2;
			z = m_sizeZ / 2;
			break;
		case 6:
			x = m_sizeX / 2;
			y = -m_sizeY / 2;
			z = m_sizeZ / 2;
			break;
		case 7:
			x = m_sizeX / 2;
			y = m_sizeY / 2;
			z = m_sizeZ / 2;
			break;
		case 8:
			x = -m_sizeX / 2;
			y = m_sizeY / 2;
			z = m_sizeZ / 2;
			break;
		}
		AcGePoint3d pnt(x, y, z);
		pnt.transformBy(trans);
		pVertexList[i-1] = pnt;
	}
	if (pFaceList == NULL)
	{
		pFaceList = new Adesk::Int32[30];
		int index = -1;
		int num, num2;
		num = 4;
		for (int i = 0; i < num; i++)
		{
			num2 = i + 1;
			if (i == num - 1)
				num2 = 0;
			pFaceList[++index] = 4;
			pFaceList[++index] = i;
			pFaceList[++index] = num2;
			pFaceList[++index] = num2 + num;
			pFaceList[++index] = i + num;
		}
		pFaceList[++index] = 4;
		pFaceList[++index] = 0;
		pFaceList[++index] = 1;
		pFaceList[++index] = 2;
		pFaceList[++index] = 3;

		pFaceList[++index] = 4;
		pFaceList[++index] = 4;
		pFaceList[++index] = 5;
		pFaceList[++index] = 6;
		pFaceList[++index] = 7;
	}
}

void CSSCube::setCrds(AcGePoint3d pnt)
{
	assertWriteEnabled (false, false) ;
	m_crds = pnt;
}

void CSSCube::setSize(double val)
{
	assertWriteEnabled (false, false) ;
	AcGeMatrix3d trans;
	trans.setToScaling(val/m_sizeX, m_crds);
	for (int i = 0; i < 8; i++)
		pVertexList[i].transformBy(trans);
	m_sizeX = val;
	m_sizeY = val;
	m_sizeZ = val;
}

AcGePoint3d CSSCube::getCrds() const
{
	assertReadEnabled();
	return m_crds/*+m_shiftVec*/;
}

void  CSSCube::getSize(double* x, double* y, double* z) const
{
	assertReadEnabled();
	*x = m_sizeX;
	*y = m_sizeY;
	*z = m_sizeZ;
}

AcGePoint3d*& CSSCube::getVertexList()
{
	assertReadEnabled();
	return pVertexList;
}

Adesk::Int32*& CSSCube::getFaceList()
{
	assertReadEnabled();
	return pFaceList;
}