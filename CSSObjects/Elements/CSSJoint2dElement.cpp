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
Adesk::UInt32 CSSJoint2dElement::kCurrentVersionNumber = 1;

//-----------------------------------------------------------------------------
ACRX_DXF_DEFINE_MEMBERS(
	CSSJoint2dElement, CSSElement,
	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
	AcDbProxyEntity::kNoOperation, CSS_Joint2dElement,
	CADSees
	| Product Desc : An OpenSees pre / post - processor
	| Company : Civil Soft Science
	| WEB Address : www.CivilSoftScience.com
)

//-----------------------------------------------------------------------------
CSSJoint2dElement::CSSJoint2dElement() : CSSElement() {
	m_type = AcString(_T("Joint2d"));
}

CSSJoint2dElement::CSSJoint2dElement(int tag, std::vector<int> nodeTags)
	: CSSElement(tag, nodeTags, "Joint2d")
{
}

CSSJoint2dElement::~CSSJoint2dElement() {
}


//-----------------------------------------------------------------------------
//----- AcDbEntity protocols
Adesk::Boolean CSSJoint2dElement::subWorldDraw(AcGiWorldDraw* mode) {
	assertReadEnabled();
	const Adesk::UInt32 faceListLength = 4;
	static const Adesk::Int32 faceList[] = { 4, 0, 1, 2, 3 };
	//pDeformedEntity->setColorIndex(DOCDATA->eleDfrmdColor);
	AcGiFaceData faceData;
	AcGiEdgeData edgeData;
	short colors[1];
	short edgeColors[4];
	if (ObjUtils::getShowDeformed())
	{
		colors[0] = eleDfrmdColor;
		for (int i = 0; i < 4; i++)
			edgeColors[i] = eleDfrmdColor;
		faceData.setColors(colors);
		edgeData.setColors(edgeColors);
		mode->geometry().shell(4, dfrmdVrtxList, faceListLength, faceList, &edgeData, &faceData);
		if (DISPOPTIONS.dispUndeformedWire)
		{
			//colors[0] = DOCDATA->wireColor;
			for (int i = 0; i < 4; i++)
				edgeColors[i] = wireColor;
			faceData.setColors(colors);
			edgeData.setColors(edgeColors);
			mode->geometry().shell(4, vrtxList, faceListLength, faceList, &edgeData, &faceData);
		}
	}
	else
	{
		colors[0] = wireColor;
		for (int i = 0; i < 4; i++)
			edgeColors[i] = wireColor;
		faceData.setColors(colors);
		edgeData.setColors(edgeColors);
		mode->geometry().shell(4, vrtxList, faceListLength, faceList, &edgeData, &faceData);
	}

	if (DISPOPTIONS.dispEleTags)
	{
		static AcGeVector3d vec, vec2, normal(0, 0, 1);
		vec = m_crds[3] - m_crds[1];
		vec2 = m_crds[2] - m_crds[0];
		AcGeVector3d up = vec.perpVector();
		AcGePoint3d crds = m_crds[0] + 0.5 * vec + 0.5 * vec2;
		AcString tagStr;
		tagStr.format(_T("%d"), m_tag);
		mode->geometry().text(crds, normal, AcGeVector3d(1, 0, 0), DISPOPTIONS.tagSize, 1., 0, tagStr.kACharPtr());
	}
	return (AcDbEntity::subWorldDraw(mode));
}


void CSSJoint2dElement::subList() const
{
	CSSElement::subList();
}

bool CSSJoint2dElement::updateGeometry(bool useDeformedGeom)
{
	assertWriteEnabled(false, true);
	bool res = CSSElement::updateGeometry(useDeformedGeom);
	if (!res)
		return false;
	static AcGeVector3d vec1, vec2;

	if (useDeformedGeom)
	{
		m_crds[0] = m_nodePtrs[0]->getDeformedCrds();
		m_crds[1] = m_nodePtrs[1]->getDeformedCrds();
		m_crds[2] = m_nodePtrs[2]->getDeformedCrds();
		m_crds[3] = m_nodePtrs[3]->getDeformedCrds();
	}
	else {
		m_crds[0] = m_nodePtrs[0]->getCrds();
		m_crds[1] = m_nodePtrs[1]->getCrds();
		m_crds[2] = m_nodePtrs[2]->getCrds();
		m_crds[3] = m_nodePtrs[3]->getCrds();
	}
	static AcGeVector3d v1, v2, v3, v4;
	v1 = (m_nodePtrs[1]->getCrds() - m_nodePtrs[3]->getCrds()).normal();
	v2 = (m_nodePtrs[2]->getCrds() - m_nodePtrs[0]->getCrds()).normal();
	v3 = v1;
	v4 = v2;;
	if (useDeformedGeom)
	{
		v1.rotateBy(m_nodePtrs[0]->getRotation()[2], AcGeVector3d::kZAxis);
		v2.rotateBy(m_nodePtrs[1]->getRotation()[2], AcGeVector3d::kZAxis);
		v3.rotateBy(m_nodePtrs[2]->getRotation()[2], AcGeVector3d::kZAxis);
		v4.rotateBy(m_nodePtrs[3]->getRotation()[2], AcGeVector3d::kZAxis);
	}
	AcGeLine3d line1(m_crds[0], v1);
	AcGeLine3d line2(m_crds[1], v2);
	AcGeLine3d line3(m_crds[2], v3);
	AcGeLine3d line4(m_crds[3], v4);
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

	//m_nodePtrs[0]->setShiftVec(vec1);
	//m_nodePtrs[1]->setShiftVec(-vec2);
	m_nodePtrs[0]->close();
	m_nodePtrs[1]->close();
	m_nodePtrs[2]->close();
	m_nodePtrs[3]->close();

	m_isNull = false;
	return true;
}