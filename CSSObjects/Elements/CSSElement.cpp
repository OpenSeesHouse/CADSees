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
}

CSSElement::CSSElement(int tag, std::string type): AcDbEntity ()
{
	m_tag = tag;
	m_type = AcString(type.c_str(), AcString::Encoding::Utf8);
	m_isNull = false;
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
	//- Uncomment the 2 following lines if your current object implementation cannot
	//- support previous version of that object.
	//if ( version < CSSElement::kCurrentVersionNumber )
	//	return (Acad::eMakeMeProxy) ;
	//----- Read params
	if ( (es =pFiler->readItem (&m_tag)) != Acad::eOk )
		return (es) ;
	if ( (es =pFiler->readString (m_type)) != Acad::eOk )
		return (es) ;
	m_isNull = false;
	return (pFiler->filerStatus ()) ;
}

//-----------------------------------------------------------------------------
//----- AcDbEntity protocols

void CSSElement::subList() const
{
	acutPrintf(_T("\n***CADSees***"));
	acutPrintf(_T("\n	Element type: %s"), m_type.kACharPtr());
	acutPrintf(_T("\n   tag:\t\t%d"), m_tag);
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


void CSSElement::updateDeformedGeometry()
{
}
