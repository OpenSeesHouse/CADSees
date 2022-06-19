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
//----- CSSZeroLength.cpp : Implementation of CSSZeroLength
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "CSSZeroLength.h"

//-----------------------------------------------------------------------------
Adesk::UInt32 CSSZeroLength::kCurrentVersionNumber =1 ;

//-----------------------------------------------------------------------------
ACRX_DXF_DEFINE_MEMBERS (
	CSSZeroLength, CSSElement,
	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent, 
	AcDbProxyEntity::kNoOperation, CSS_zeroLength,
CADSees
|Product Desc:     An OpenSees pre/post-processor
|Company:          Civil Soft Science
|WEB Address:      www.CivilSoftScience.com
)

//-----------------------------------------------------------------------------
CSSZeroLength::CSSZeroLength () : CSSElement () {
	m_type = AcString(_T("zeroLength"));
	pVertList = 0;
}

CSSZeroLength::CSSZeroLength(int tag, std::vector<int> nodeTags): CSSElement (tag, nodeTags, "zeroLength")
{
	pVertList = 0;
}

CSSZeroLength::~CSSZeroLength () {
	if (pVertList != 0)
		delete[] pVertList;
	pVertList = 0;
}


//-----------------------------------------------------------------------------
//----- CSSElement protocols
Adesk::Boolean CSSZeroLength::subWorldDraw (AcGiWorldDraw *mode) {
	assertReadEnabled ();
	if (pVertList != 0)
		mode->geometry().polygon(4, pVertList);
	return (CSSElement::subWorldDraw (mode)) ;
}



bool CSSZeroLength::updateGeometry(bool useDeformedGeom)
{
	CSSElement::updateGeometry(useDeformedGeom);
	if (useDeformedGeom)
	{
		m_crds[0] = m_nodePtrs[0]->getDeformedCrds();
		m_crds[1] = m_nodePtrs[1]->getDeformedCrds();
	}
	else
	{
		m_crds[0] = m_nodePtrs[0]->getCrds();
		m_crds[1] = m_nodePtrs[1]->getCrds();
	}
	AcGePoint3d crds1 = m_crds[0], crds2 = m_crds[1];
	m_nodePtrs[0]->close();
	m_nodePtrs[1]->close();
	AcGeVector3d vec(m_crds[1] - m_crds[0]);
	if (vec.length() < 1.e-5)
		vec.set(1, 0, 0);
	double Ln = vec.length() * ZLHTOLRAT;
	AcGeVector3d normal = DOCDATA->getNdm() == 2 ? AcGeVector3d(0, 0, 1) : AcGeVector3d(0, -1, 0);
	normal = -vec.crossProduct(normal);
	normal.normalize();
	if (pVertList == 0)
		pVertList = new AcGePoint3d[4];
	pVertList[0] = crds1;
	crds1 += 0.5 * vec + 0.5 * Ln * normal;
	pVertList[1] = crds1;
	crds1 -= normal;
	pVertList[2] = crds1;
	pVertList[3] = crds2;
	return true;
}
