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
//----- CSSTruss.cpp : Implementation of CSSTruss
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "CSSTruss.h"

//-----------------------------------------------------------------------------
Adesk::UInt32 CSSTruss::kCurrentVersionNumber =1 ;

//-----------------------------------------------------------------------------
ACRX_DXF_DEFINE_MEMBERS (
	CSSTruss, CSSLineElement,
	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent, 
	AcDbProxyEntity::kNoOperation, CSS_truss,
CADSees
|Product Desc:     An OpenSees pre/post-processor
|Company:          Civil Soft Science
|WEB Address:      www.CivilSoftScience.com
)

//-----------------------------------------------------------------------------
CSSTruss::CSSTruss () : CSSLineElement () {
	m_type = AcString(_T("truss"));
}

CSSTruss::CSSTruss(int tag, std::vector<int> nodeTags): CSSLineElement (tag, nodeTags, "truss")
{
}

CSSTruss::~CSSTruss () {
	if (pDeformedEntity != 0)
		delete pDeformedEntity;
	if (pUndeformedEntity != 0)
		delete pUndeformedEntity;
}


bool CSSTruss::updateGeometry(bool useDeformedGeom)
{
	assertWriteEnabled(false, true);
	bool res = CSSLineElement::updateGeometry(useDeformedGeom);
	if (!res)
		return false;

	if (pDeformedEntity == nullptr)
	{
		pDeformedEntity = new AcDbLine(m_crds[0], m_crds[1]);
		pUndeformedEntity = new AcDbLine(m_crds[0], m_crds[1]);
	}
	else if (useDeformedGeom)
	{
		AcDbLine* pLine = (AcDbLine*)pDeformedEntity;
		pLine->setStartPoint(m_crds[0]);
		pLine->setEndPoint(m_crds[1]);
	}
	m_isNull = false;
	m_nodePtrs[0]->close();
	m_nodePtrs[1]->close();
	return true;
}