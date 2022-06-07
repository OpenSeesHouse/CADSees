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
//----- CSSModElasticBeamColumn.cpp : Implementation of CSSModElasticBeamColumn
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "CSSModElasticBeamColumn.h"

//-----------------------------------------------------------------------------
Adesk::UInt32 CSSModElasticBeamColumn::kCurrentVersionNumber =1 ;

//-----------------------------------------------------------------------------
ACRX_DXF_DEFINE_MEMBERS (
	CSSModElasticBeamColumn, CSSBeamElement,
	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent, 
	AcDbProxyEntity::kNoOperation, CSS_modElasticBeamColumn,
CADSees
|Product Desc:     An OpenSees pre/post-processor
|Company:          Civil Soft Science
|WEB Address:      www.CivilSoftScience.com
)

//-----------------------------------------------------------------------------
CSSModElasticBeamColumn::CSSModElasticBeamColumn () : CSSBeamElement () {
	m_type = AcString(_T("modElasticBeamColumn"));
}

CSSModElasticBeamColumn::CSSModElasticBeamColumn(int tag, std::vector<int> nodeTags): CSSBeamElement (tag, nodeTags, "modElasticBeamColumn")
{
}

CSSModElasticBeamColumn::~CSSModElasticBeamColumn () {
}