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
//----- CSSLineElement.cpp : Implementation of CSSLineElement
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "CSSLineElement.h"

//-----------------------------------------------------------------------------
Adesk::UInt32 CSSLineElement::kCurrentVersionNumber =1 ;

//-----------------------------------------------------------------------------
ACRX_DXF_DEFINE_MEMBERS (
	CSSLineElement, CSSElement,
	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent, 
	AcDbProxyEntity::kNoOperation, CSSLINEELEMENT,
CADSees
|Product Desc:     An OpenSees pre/post-processor
|Company:          Civil Soft Science
|WEB Address:      www.CivilSoftScience.com
)

//-----------------------------------------------------------------------------
CSSLineElement::CSSLineElement () : CSSElement () {
	m_iNod = 0;
	m_jNod = 0;
	m_length = 0;
}

CSSLineElement::CSSLineElement(int tag, int inode, int jnode, std::string type) : CSSElement(tag, type)
{
	m_length = 0;
	m_isNull = true;
	m_iNod = inode;
	m_jNod = jnode;
}

CSSLineElement::~CSSLineElement () {
}

//-----------------------------------------------------------------------------
//----- AcDbObject protocols
//- Dwg Filing protocol
Acad::ErrorStatus CSSLineElement::dwgOutFields (AcDbDwgFiler *pFiler) const {
	assertReadEnabled () ;
	//----- Save parent class information first.
	Acad::ErrorStatus es = CSSElement::dwgOutFields (pFiler) ;
	if ( es != Acad::eOk )
		return (es) ;
	//----- Object version number needs to be saved first
	if ( (es = pFiler->writeUInt32 (CSSLineElement::kCurrentVersionNumber)) != Acad::eOk )
		return (es) ;
	//----- Output params
	if ( (es =pFiler->writeItem (m_iNod)) != Acad::eOk )
		return (es) ;
	if ( (es =pFiler->writeItem (m_jNod)) != Acad::eOk )
		return (es) ;
	return (pFiler->filerStatus ()) ;
}

Acad::ErrorStatus CSSLineElement::dwgInFields (AcDbDwgFiler *pFiler) {
	assertWriteEnabled () ;
	//----- Read parent class information first.
	Acad::ErrorStatus es =CSSElement::dwgInFields (pFiler) ;
	if ( es != Acad::eOk )
		return (es) ;
	//----- Object version number needs to be read first
	Adesk::UInt32 version =0 ;
	if ( (es =pFiler->readUInt32 (&version)) != Acad::eOk )
		return (es) ;
	if ( version > CSSLineElement::kCurrentVersionNumber )
		return (Acad::eMakeMeProxy) ;
	//- Uncomment the 2 following lines if your current object implementation cannot
	//- support previous version of that object.
	//if ( version < CSSLineElement::kCurrentVersionNumber )
	//	return (Acad::eMakeMeProxy) ;
	//----- Read params
	if ( (es =pFiler->readItem (&m_iNod)) != Acad::eOk )
		return (es) ;
	if ( (es =pFiler->readItem (&m_jNod)) != Acad::eOk )
		return (es) ;
	if(ObjUtils::getNodeSize() == 0)
	{
		ObjUtils::setNodesSize(m_length*NODSIZERAT);
		ObjUtils::setTagsSize(m_length*NODSIZERAT);
		ObjUtils::RedrawNodeGraphics(true);
	}
	return (pFiler->filerStatus ()) ;
}

//-----------------------------------------------------------------------------
//----- AcDbEntity protocols
Adesk::Boolean CSSLineElement::subWorldDraw (AcGiWorldDraw *mode) {
	assertReadEnabled () ;
	if (DISPOPTIONS.dispEleTags)
	{
		AcGeVector3d vec(crds2-crds1);
		AcGeVector3d normal = ObjUtils::getNdm() == 2 ? AcGeVector3d(0, 0, 1) : AcGeVector3d(0, -1, 0);
		AcGeVector3d up = vec.perpVector();
		crds1 += 0.5*vec + 0.03*m_length*up;
		AcString tagStr;
		tagStr.format(_T("%d"), m_tag);
		mode->geometry().text(crds1, normal, AcGeVector3d(1, 0, 0), DISPOPTIONS.tagSize, 1., 0, tagStr.kACharPtr());
	}
	return (CSSElement::subWorldDraw (mode)) ;
}


void CSSLineElement::subList() const
{
	CSSElement::subList();
	acutPrintf(_T("\n   iNode:\t%d"), m_iNod);
	acutPrintf(_T("\n   jNode:\t%d"), m_jNod);
}

double CSSLineElement::getLength() const
{
	assertReadEnabled();
	return m_length;
}

bool CSSLineElement::updateGeometry(bool useDeformedGeom)
{
	 assertWriteEnabled(false, true);
	 bool res = CSSElement::updateGeometry(useDeformedGeom);
	 if (!res)
		  return false;
	 AcDbObjectId id;
	 if (!ObjUtils::getNode(id, m_iNod))
	 {
		  acutPrintf(_T("CSSLineElement:ERROR finding node object"));
		  return false;
	 }
	 AcDbObject* pObj = NULL;
	 ErrorStatus es = acdbOpenObject(pObj, id, AcDb::kForRead);
	 assert(pObj != NULL);
	 piNode = CSSNode::cast(pObj);
	 assert(piNode != NULL);

	 if (!ObjUtils::getNode(id, m_jNod))
	 {
		  acutPrintf(_T("CSSLineElement:ERROR finding node object"));
		  piNode->close();
		  return false;
	 }
	 pObj = NULL;
	 es = acdbOpenObject(pObj, id, AcDb::kForRead);
	 assert(pObj != NULL);
	 pjNode = CSSNode::cast(pObj);
	 assert(pjNode != NULL);
	 if (useDeformedGeom)
	 {
		  crds1 = piNode->getDeformedCrds();
		  crds2 = pjNode->getDeformedCrds();
	 }
	 else {
		  crds1 = piNode->getCrds();
		  crds2 = pjNode->getCrds();
	 }
	 vec = crds2 - crds1;
	 m_length = vec.length();
	 m_isNull = false;

	 //piNode->setShiftVec(vec1);
	 //pjNode->setShiftVec(-vec2);
	 piNode->close();
	 pjNode->close();

	 return true;
}