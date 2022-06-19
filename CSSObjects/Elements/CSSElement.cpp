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
//----- CSSBCElement.cpp : Implementation of CSSElement
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "CSSElement.h"

//-----------------------------------------------------------------------------
Adesk::UInt32 CSSElement::kCurrentVersionNumber =1 ;

//-----------------------------------------------------------------------------
ACRX_DXF_DEFINE_MEMBERS (
	CSSElement, AcDbEntity,
	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent, 
	AcDbProxyEntity::kNoOperation, CSSELEMENT,
CADSees
|Product Desc:     An OpenSees pre/post-processor
|Company:          Civil Soft Science
|WEB Address:      www.CivilSoftScience.com
)


//-----------------------------------------------------------------------------
CSSElement::CSSElement () : AcDbEntity () {
	m_tag = 0;
	m_isNull = true;
	pDeformedEntity = 0;
	pUndeformedEntity = 0;
}

CSSElement::CSSElement(int tag, std::vector<int> nodeTags, std::string type): AcDbEntity ()
{
	m_tag = tag;
	m_type = AcString(type.c_str(), AcString::Encoding::Utf8);
	m_isNull = false;
	m_nodes = nodeTags;
	m_crds.reserve(m_nodes.size());
	m_nodePtrs.reserve(m_nodes.size());
	for (int nd : m_nodes)
	{
		AcGePoint3d pCrd;
		if(ObjUtils::getNodeCrds(nd, &pCrd))
			m_crds.push_back(pCrd);
		else
			m_crds.push_back(AcGePoint3d(0, 0, 0));
		m_nodePtrs.push_back(0);
	}
	pDeformedEntity = 0;
	pUndeformedEntity = 0;
}

CSSElement::~CSSElement () {
}

//-----------------------------------------------------------------------------
//----- AcDbObject protocols
//- Dwg Filing protocol
Acad::ErrorStatus CSSElement::dwgOutFields (AcDbDwgFiler *pFiler) const {
	assertReadEnabled () ;
	//----- Save parent class information first.
	Acad::ErrorStatus es =AcDbEntity::dwgOutFields (pFiler) ;
	if ( es != Acad::eOk )
		return (es) ;
	//----- Object version number needs to be saved first
	if ( (es =pFiler->writeUInt32 (CSSElement::kCurrentVersionNumber)) != Acad::eOk )
		return (es) ;
	//----- Output params
	if ( (es =pFiler->writeItem (m_tag)) != Acad::eOk )
		return (es) ;
	if ( (es =pFiler->writeItem (m_type)) != Acad::eOk )
		return (es) ;
	if ( (es =pFiler->writeItem ((int)m_nodes.size())) != Acad::eOk )
		return (es) ;
	for (const int& nd : m_nodes)
	{
		if ((es = pFiler->writeItem(nd)) != Acad::eOk)
			return (es);
	}
	for (const AcGePoint3d& pnt : m_crds)
	{
		if ((es = pFiler->writeItem(pnt)) != Acad::eOk)
			return (es);
	}

	return (pFiler->filerStatus ()) ;
}

Acad::ErrorStatus CSSElement::dwgInFields (AcDbDwgFiler *pFiler) {
	assertWriteEnabled () ;
	//----- Read parent class information first.
	Acad::ErrorStatus es =AcDbEntity::dwgInFields (pFiler) ;
	if ( es != Acad::eOk )
		return (es) ;
	//----- Object version number needs to be read first
	Adesk::UInt32 version =0 ;
	if ( (es =pFiler->readUInt32 (&version)) != Acad::eOk )
		return (es) ;
	if ( version > CSSElement::kCurrentVersionNumber )
		return (Acad::eMakeMeProxy) ;
	if ( (es =pFiler->readItem (&m_tag)) != Acad::eOk )
		return (es) ;
	if ( (es =pFiler->readString (m_type)) != Acad::eOk )
		return (es) ;
	int numNodes = 0;
	if ((es = pFiler->readItem(&numNodes)) != Acad::eOk)
		return (es);
	m_nodes.reserve(numNodes);
	m_crds.reserve(numNodes);
	m_nodePtrs.reserve(numNodes);
	for (int i = 0; i < numNodes; i++)
	{
		m_nodes.push_back(0);
		if ((es = pFiler->readItem(&m_nodes[i])) != Acad::eOk)
			return (es);
	}
	for (int i = 0; i < numNodes; i++)
	{
		m_crds.push_back(AcGePoint3d());
		if ((es = pFiler->readItem(&m_crds[i])) != Acad::eOk)
			return (es);
		m_nodePtrs.push_back(0);
	}
	auto it = ELEMATTAGMAP.find(m_tag);
	if (it == ELEMATTAGMAP.end())
	{
		 ELEMATTAGMAP.insert(std::pair<int, AcDbObjectId>(m_tag, this->objectId()));
	}
	m_isNull = true;
	return (pFiler->filerStatus ()) ;
}

//-----------------------------------------------------------------------------
//----- AcDbEntity protocols

void CSSElement::subList() const
{
	acutPrintf(_T("\n	CADSees Element (%s):"), m_type.kACharPtr());
	acutPrintf(_T("\n   tag:\t\t%d"), m_tag);
	for (int i = 0; i < m_nodes.size(); i++)
		acutPrintf(_T("\n   Node[%d]:\t\t%d"), i+1, m_nodes[i]);
}

Acad::ErrorStatus CSSElement::subErase(Adesk::Boolean pErasing)
{
	 std::map<int, AcDbObjectId>::iterator it = ELEMATTAGMAP.find(m_tag);
	 if (it != ELEMATTAGMAP.end())
	 {
		  ELEMATTAGMAP.erase(it);
	 }

	 return AcDbEntity::subErase(pErasing);
}

int CSSElement::getTag() const
{
	assertReadEnabled();
	return m_tag;
}

int CSSElement::getIsNull() const
{
	assertReadEnabled();
	return m_isNull;
}


bool CSSElement::updateGeometry(bool useDeformedGeom)
{
	assertWriteEnabled(false, true);
	AcDbObjectId id;
	int i = 0;
	ErrorStatus es;
	AcDbObject* pObj;
	for (int nd : m_nodes)
	{
		if (!ObjUtils::getNode(id, nd))
		{
			acutPrintf(_T("CSSElement:ERROR finding node object"));
			return false;
		}
		pObj = NULL;
		es = acdbOpenObject(pObj, id, AcDb::kForRead);
		if (pObj == NULL)
		{
			acutPrintf(_T("CSSElement::updateGeometry: failed to open node object for read; es = %s\n"), es);
			return false;
		}
		m_nodePtrs[i] = CSSNode::cast(pObj);
		assert(m_nodePtrs[i] != NULL);
		i++;
	}
	return true;
}


AcDbEntity* CSSElement::getDeformedEntity()
{
	return pDeformedEntity;
}
AcDbEntity* CSSElement::getUndeformedEntity()
{
	return pUndeformedEntity;
}
Adesk::Boolean CSSElement::subWorldDraw (AcGiWorldDraw *mode) {
	assertReadEnabled () ;
	if (pUndeformedEntity == NULL || pDeformedEntity == NULL)
		return (AcDbEntity::subWorldDraw(mode));

	pDeformedEntity->setColorIndex(eleDfrmdColor);
	if (ObjUtils::getShowDeformed())
	{
		mode->geometry().draw(pDeformedEntity);
		if (DISPOPTIONS.dispUndeformedWire)
		{
			pUndeformedEntity->setColorIndex(wireColor);
			mode->geometry().draw(pUndeformedEntity);
		}
	}
	else
	{
		pUndeformedEntity->setColorIndex(elementColor);
		mode->geometry().draw(pUndeformedEntity);
	}
	return (AcDbEntity::subWorldDraw (mode)) ;
}


