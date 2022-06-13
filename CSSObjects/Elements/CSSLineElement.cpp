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
}

CSSLineElement::CSSLineElement(int tag, std::vector<int> nodeTags, std::string type) : CSSElement(tag, nodeTags, type)
{
}

CSSLineElement::~CSSLineElement () {
}


//-----------------------------------------------------------------------------
//----- AcDbEntity protocols
Adesk::Boolean CSSLineElement::subWorldDraw (AcGiWorldDraw *mode) {
	assertReadEnabled () ;
	if (DISPOPTIONS.dispEleTags)
	{
		AcGePoint3d crd = m_crds[0];
		double l = vec.length();
		AcGeVector3d normal = DOCDATA->getNdm() == 2 ? AcGeVector3d(0, 0, 1) : AcGeVector3d(0, -1, 0);
		AcGeVector3d up = vec.perpVector();
		crd += 0.5*(l- DISPOPTIONS.tagSize)*vec.normal() + 0.03*vec.length()*up;
		AcString tagStr;
		tagStr.format(_T("%d"), m_tag);
		mode->geometry().text(crd, normal, AcGeVector3d(1, 0, 0), DISPOPTIONS.tagSize, 1., 0, tagStr.kACharPtr());
	}
	return (CSSElement::subWorldDraw (mode)) ;
}


bool CSSLineElement::updateGeometry(bool useDeformedGeom)
{
	 assertWriteEnabled(false, true);
	 bool res = CSSElement::updateGeometry(useDeformedGeom);
	 if (!res)
		  return false;

	 if (useDeformedGeom)
	 {
		  m_crds[0] = m_nodePtrs[0]->getDeformedCrds();
		  m_crds[1] = m_nodePtrs[1]->getDeformedCrds();
	 }
	 else {
		  m_crds[0] = m_nodePtrs[0]->getCrds();
		  m_crds[1] = m_nodePtrs[1]->getCrds();
	 }
	 vec = m_crds[1] - m_crds[0];
	 return true;
}