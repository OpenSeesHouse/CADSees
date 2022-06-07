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
//----- CSSForceBeamColumn.cpp : Implementation of CSSForceBeamColumn
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "CSSForceBeamColumn.h"

//-----------------------------------------------------------------------------
Adesk::UInt32 CSSForceBeamColumn::kCurrentVersionNumber =1 ;

//-----------------------------------------------------------------------------
ACRX_DXF_DEFINE_MEMBERS (
	CSSForceBeamColumn, CSSBeamElement,
	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent, 
	AcDbProxyEntity::kNoOperation, CSS_forceBeamColumn,
CADSees
|Product Desc:     An OpenSees pre/post-processor
|Company:          Civil Soft Science
|WEB Address:      www.CivilSoftScience.com
)

//-----------------------------------------------------------------------------
CSSForceBeamColumn::CSSForceBeamColumn () : CSSBeamElement () {
	m_type = AcString(_T("forceBeamColumn"));
}

CSSForceBeamColumn::CSSForceBeamColumn(int tag, std::vector<int> nodeTags): CSSBeamElement (tag, nodeTags, "forceBeamColumn")
{
}

CSSForceBeamColumn::~CSSForceBeamColumn () {
}