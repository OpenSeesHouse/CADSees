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
//----- CSSJoint2dElement.cpp : Implementation of CSSJoint2dElement
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "CSSJoint2dElement.h"

//-----------------------------------------------------------------------------
Adesk::UInt32 CSSJoint2dElement::kCurrentVersionNumber =1 ;

//-----------------------------------------------------------------------------
ACRX_DXF_DEFINE_MEMBERS (
	CSSJoint2dElement, CSSElement,
	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent, 
	AcDbProxyEntity::kNoOperation, CSS_Joint2dElement,
CADSees
|Product Desc:     An OpenSees pre/post-processor
|Company:          Civil Soft Science
|WEB Address:      www.CivilSoftScience.com
)

//-----------------------------------------------------------------------------
CSSJoint2dElement::CSSJoint2dElement () : CSSElement () {
	m_iNod = 0;
	m_jNod = 0;
	m_kNod = 0;
	m_lNod = 0;
	m_length = 0;
	m_type = AcString(_T("Joint2d"));
}

CSSJoint2dElement::CSSJoint2dElement(int tag, std::vector<int> nodeTags) : CSSElement(tag, "Joint2d")
{
	m_length = 0;
	m_isNull = true;
	m_iNod = nodeTags[0];
	m_jNod = nodeTags[1];
	m_kNod = nodeTags[2];
	m_lNod = nodeTags[3];
	initiated = false;
}

CSSJoint2dElement::~CSSJoint2dElement () {
}

//-----------------------------------------------------------------------------
//----- AcDbObject protocols
//- Dwg Filing protocol
Acad::ErrorStatus CSSJoint2dElement::dwgOutFields (AcDbDwgFiler *pFiler) const {
	assertReadEnabled () ;
	//----- Save parent class information first.
	Acad::ErrorStatus es = CSSElement::dwgOutFields (pFiler) ;
	if ( es != Acad::eOk )
		return (es) ;
	//----- Object version number needs to be saved first
	if ( (es = pFiler->writeUInt32 (CSSJoint2dElement::kCurrentVersionNumber)) != Acad::eOk )
		return (es) ;
	//----- Output params
	if ( (es =pFiler->writeItem (m_iNod)) != Acad::eOk )
		return (es) ;
	if ( (es =pFiler->writeItem (m_jNod)) != Acad::eOk )
		return (es) ;
	if ( (es =pFiler->writeItem (m_kNod)) != Acad::eOk )
		return (es) ;
	if ( (es =pFiler->writeItem (m_lNod)) != Acad::eOk )
		return (es) ;
	return (pFiler->filerStatus ()) ;
}

Acad::ErrorStatus CSSJoint2dElement::dwgInFields (AcDbDwgFiler *pFiler) {
	assertWriteEnabled () ;
	//----- Read parent class information first.
	Acad::ErrorStatus es =CSSElement::dwgInFields (pFiler) ;
	if ( es != Acad::eOk )
		return (es) ;
	//----- Object version number needs to be read first
	Adesk::UInt32 version =0 ;
	if ( (es =pFiler->readUInt32 (&version)) != Acad::eOk )
		return (es) ;
	if ( version > CSSJoint2dElement::kCurrentVersionNumber )
		return (Acad::eMakeMeProxy) ;
	//- Uncomment the 2 following lines if your current object implementation cannot
	//- support previous version of that object.
	//if ( version < CSSJoint2dElement::kCurrentVersionNumber )
	//	return (Acad::eMakeMeProxy) ;
	//----- Read params
	if ( (es =pFiler->readItem (&m_iNod)) != Acad::eOk )
		return (es) ;
	if ( (es =pFiler->readItem (&m_jNod)) != Acad::eOk )
		return (es) ;
	if ( (es =pFiler->readItem (&m_kNod)) != Acad::eOk )
		return (es) ;
	if ( (es =pFiler->readItem (&m_lNod)) != Acad::eOk )
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
Adesk::Boolean CSSJoint2dElement::subWorldDraw (AcGiWorldDraw *mode) {
	assertReadEnabled () ;
	const Adesk::UInt32 faceListLength = 4;
	static const Adesk::Int32 faceList[] = { 4, 0, 1, 2, 3 };
	//pDeformedEntity->setColorIndex(DOCDATA.eleDfrmdColor);
	AcGiFaceData faceData;
	AcGiEdgeData edgeData;
	short colors[1];
	short edgeColors[4];
	if (ObjUtils::getShowDeformed())
	{
		colors[0] = DOCDATA.eleDfrmdColor;
		for (int i = 0; i < 4; i++)
			edgeColors[i] = DOCDATA.eleDfrmdColor;
		faceData.setColors(colors);
		edgeData.setColors(edgeColors);
		mode->geometry().shell(4, dfrmdVrtxList, faceListLength, faceList, &edgeData, &faceData);
		if (DISPOPTIONS.dispUndeformedWire)
		{
			//colors[0] = DOCDATA.wireColor;
			for (int i = 0; i < 4; i++)
				edgeColors[i] = DOCDATA.wireColor;
			faceData.setColors(colors);
			edgeData.setColors(edgeColors);
			mode->geometry().shell(4, vrtxList, faceListLength, faceList, &edgeData, &faceData);
		}
	}
	else
	{
		colors[0] = DOCDATA.wireColor;
		for (int i = 0; i < 4; i++)
			edgeColors[i] = DOCDATA.wireColor;
		faceData.setColors(colors);
		edgeData.setColors(edgeColors);
		mode->geometry().shell(4, vrtxList, faceListLength, faceList, &edgeData, &faceData);
	}

	if (DISPOPTIONS.dispEleTags)
	{
		AcGeVector3d vec(crds4-crds2);
		AcGeVector3d normal = ObjUtils::getNdm() == 2 ? AcGeVector3d(0, 0, 1) : AcGeVector3d(0, -1, 0);
		AcGeVector3d up = vec.perpVector();
		AcGePoint3d crds = crds1 + 0.5*vec + 0.5*vec2;
		AcString tagStr;
		tagStr.format(_T("%d"), m_tag);
		mode->geometry().text(crds, normal, AcGeVector3d(1, 0, 0), DISPOPTIONS.tagSize, 1., 0, tagStr.kACharPtr());
	}
	return (CSSElement::subWorldDraw (mode)) ;
}


void CSSJoint2dElement::subList() const
{
	CSSElement::subList();
	acutPrintf(_T("\n   nodeTags[0]:\t%d"), m_iNod);
	acutPrintf(_T("\n   nodeTags[1]:\t%d"), m_jNod);
	acutPrintf(_T("\n   kNode:\t%d"), m_kNod);
	acutPrintf(_T("\n   lNode:\t%d"), m_lNod);
}

double CSSJoint2dElement::getLength() const
{
	assertReadEnabled();
	return m_length;
}

bool CSSJoint2dElement::updateGeometry(bool useDeformedGeom)
{
	assertWriteEnabled(false, true);
	bool res = CSSElement::updateGeometry(useDeformedGeom);
	if (!res)
		return false;
	AcDbObjectId id;
	if (!ObjUtils::getNode(id, m_iNod))
	{
		acutPrintf(_T("CSSJoint2dElement:ERROR finding node object"));
		return false;
	}
	AcDbObject   *pObj = NULL;
	ErrorStatus es = acdbOpenObject(pObj, id, AcDb::kForRead);
    assert(pObj != NULL);
    piNode = CSSNode::cast(pObj);
    assert(piNode != NULL);

	if (!ObjUtils::getNode(id, m_jNod))
	{
		acutPrintf(_T("CSSJoint2dElement:ERROR finding node object"));
		piNode->close();
		return false;
	}
	pObj = NULL;
	es = acdbOpenObject(pObj, id, AcDb::kForRead);
    assert(pObj != NULL);
    pjNode = CSSNode::cast(pObj);
    assert(pjNode != NULL);
	
	if (!ObjUtils::getNode(id, m_kNod))
	{
		acutPrintf(_T("CSSJoint2dElement:ERROR finding node object"));
		piNode->close();
		return false;
	}
	pObj = NULL;
	es = acdbOpenObject(pObj, id, AcDb::kForRead);
    assert(pObj != NULL);
    pkNode = CSSNode::cast(pObj);
    assert(pjNode != NULL);
	
	if (!ObjUtils::getNode(id, m_lNod))
	{
		acutPrintf(_T("CSSJoint2dElement:ERROR finding node object"));
		piNode->close();
		return false;
	}
	pObj = NULL;
	es = acdbOpenObject(pObj, id, AcDb::kForRead);
    assert(pObj != NULL);
    plNode = CSSNode::cast(pObj);
    assert(pjNode != NULL);
	
	if (useDeformedGeom)
	{
		crds1 = piNode->getDeformedCrds();
		crds2 = pjNode->getDeformedCrds();
		crds3 = pkNode->getDeformedCrds();
		crds4 = plNode->getDeformedCrds();
	} else {
		crds1 = piNode->getCrds();
		crds2 = pjNode->getCrds();
		crds3 = pkNode->getCrds();
		crds4 = plNode->getCrds();
	}
	if (!initiated)
	{
		vec1 = (crds2 - crds4);
		vec2 = (crds3 - crds1);
		initiated = true;
	}
	m_length = 0.5*(vec1.length()+ vec2.length());
	AcGeVector3d v1(vec1.normal()), v2(vec2.normal()), v3(v1), v4(v2);
	if (useDeformedGeom)
	{
		v1.rotateBy(piNode->getRotation()[2], AcGeVector3d::kZAxis);
		v2.rotateBy(pjNode->getRotation()[2], AcGeVector3d::kZAxis);
		v3.rotateBy(pkNode->getRotation()[2], AcGeVector3d::kZAxis);
		v4.rotateBy(plNode->getRotation()[2], AcGeVector3d::kZAxis);
	}
	AcGeLine3d line1(crds1, v1);
	AcGeLine3d line2(crds2, v2);
	AcGeLine3d line3(crds3, v3);
	AcGeLine3d line4(crds4, v4);
	if (useDeformedGeom)
	{
		res = line1.intersectWith(line2, dfrmdVrtxList[0]);
		res = line2.intersectWith(line3, dfrmdVrtxList[1]);
		res = line3.intersectWith(line4, dfrmdVrtxList[2]);
		res = line4.intersectWith(line1, dfrmdVrtxList[3]);
	}
	else
	{
		res = line1.intersectWith(line2, vrtxList[0]);
		res = line2.intersectWith(line3, vrtxList[1]);
		res = line3.intersectWith(line4, vrtxList[2]);
		res = line4.intersectWith(line1, vrtxList[3]);
	}
	m_isNull = false;

	//piNode->setShiftVec(vec1);
	//pjNode->setShiftVec(-vec2);
	piNode->close();
	pjNode->close();
	pkNode->close();
	plNode->close();

	m_isNull = false;
	return true;
}