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
//----- CSSElasticBeamColumn.cpp : Implementation of CSSElasticBeamColumn
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "CSSElasticBeamColumn.h"

//-----------------------------------------------------------------------------
Adesk::UInt32 CSSElasticBeamColumn::kCurrentVersionNumber =1 ;

//-----------------------------------------------------------------------------
ACRX_DXF_DEFINE_MEMBERS (
	CSSElasticBeamColumn, CSSBeamElement,
	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent, 
	AcDbProxyEntity::kNoOperation, CSS_elasticBeamColumn,
CADSees
|Product Desc:     An OpenSees pre/post-processor
|Company:          Civil Soft Science
|WEB Address:      www.CivilSoftScience.com
)

//-----------------------------------------------------------------------------
CSSElasticBeamColumn::CSSElasticBeamColumn () : CSSBeamElement () {
	m_type = AcString(_T("elasticBeamColumn"));
}

CSSElasticBeamColumn::CSSElasticBeamColumn(int tag, std::vector<int> nodeTags, int nIntegPnts): CSSBeamElement (tag, nodeTags[0], nodeTags[1], nIntegPnts, "elasticBeamColumn")
{
}

CSSElasticBeamColumn::~CSSElasticBeamColumn () {
}

//-----------------------------------------------------------------------------
//----- AcDbObject protocols
//- Dwg Filing protocol
Acad::ErrorStatus CSSElasticBeamColumn::dwgOutFields (AcDbDwgFiler *pFiler) const {
	assertReadEnabled () ;
	//----- Save parent class information first.
	Acad::ErrorStatus es =CSSBeamElement::dwgOutFields (pFiler) ;
	if ( es != Acad::eOk )
		return (es) ;
	//----- Object version number needs to be saved first
	if ( (es =pFiler->writeUInt32 (CSSElasticBeamColumn::kCurrentVersionNumber)) != Acad::eOk )
		return (es) ;
	//----- Output params

	return (pFiler->filerStatus ()) ;
}

Acad::ErrorStatus CSSElasticBeamColumn::dwgInFields (AcDbDwgFiler *pFiler) {
	assertWriteEnabled () ;
	//----- Read parent class information first.
	Acad::ErrorStatus es =CSSBeamElement::dwgInFields (pFiler) ;
	if ( es != Acad::eOk )
		return (es) ;
	//----- Object version number needs to be read first
	Adesk::UInt32 version =0 ;
	if ( (es =pFiler->readUInt32 (&version)) != Acad::eOk )
		return (es) ;
	if ( version > CSSElasticBeamColumn::kCurrentVersionNumber )
		return (Acad::eMakeMeProxy) ;
	//- Uncomment the 2 following lines if your current object implementation cannot
	//- support previous version of that object.
	//if ( version < CSSElasticBeamColumn::kCurrentVersionNumber )
	//	return (Acad::eMakeMeProxy) ;
	//----- Read params
	return (pFiler->filerStatus ()) ;
}