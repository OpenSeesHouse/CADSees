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
	AcDbProxyEntity::kNoOperation, CSSTRUSS,
CADSees
|Product Desc:     An OpenSees pre/post-processor
|Company:          Civil Soft Science
|WEB Address:      www.CivilSoftScience.com
)

//-----------------------------------------------------------------------------
CSSTruss::CSSTruss () : CSSLineElement () {
	m_type = AcString(_T("truss"));
}

CSSTruss::CSSTruss(int tag, int inode, int jnode, int nIntegPnts): CSSLineElement (tag, inode, jnode, nIntegPnts, "truss")
{
}

CSSTruss::~CSSTruss () {
}

//-----------------------------------------------------------------------------
//----- AcDbObject protocols
//- Dwg Filing protocol
Acad::ErrorStatus CSSTruss::dwgOutFields (AcDbDwgFiler *pFiler) const {
	assertReadEnabled () ;
	//----- Save parent class information first.
	Acad::ErrorStatus es =CSSLineElement::dwgOutFields (pFiler) ;
	if ( es != Acad::eOk )
		return (es) ;
	//----- Object version number needs to be saved first
	if ( (es =pFiler->writeUInt32 (CSSTruss::kCurrentVersionNumber)) != Acad::eOk )
		return (es) ;
	//----- Output params

	return (pFiler->filerStatus ()) ;
}

Acad::ErrorStatus CSSTruss::dwgInFields (AcDbDwgFiler *pFiler) {
	assertWriteEnabled () ;
	//----- Read parent class information first.
	Acad::ErrorStatus es =CSSLineElement::dwgInFields (pFiler) ;
	if ( es != Acad::eOk )
		return (es) ;
	//----- Object version number needs to be read first
	Adesk::UInt32 version =0 ;
	if ( (es =pFiler->readUInt32 (&version)) != Acad::eOk )
		return (es) ;
	if ( version > CSSTruss::kCurrentVersionNumber )
		return (Acad::eMakeMeProxy) ;
	//- Uncomment the 2 following lines if your current object implementation cannot
	//- support previous version of that object.
	//if ( version < CSSTruss::kCurrentVersionNumber )
	//	return (Acad::eMakeMeProxy) ;
	//----- Read params
	return (pFiler->filerStatus ()) ;
}

//-----------------------------------------------------------------------------
//----- CSSLineElement protocols
Adesk::Boolean CSSTruss::subWorldDraw (AcGiWorldDraw *mode) {
	assertReadEnabled () ;
	return (CSSLineElement::subWorldDraw (mode)) ;
}


Adesk::UInt32 CSSTruss::subSetAttributes (AcGiDrawableTraits *traits) {
	assertReadEnabled () ;
	return (CSSLineElement::subSetAttributes (traits)) ;
}

	//- Osnap points protocol
Acad::ErrorStatus CSSTruss::subGetOsnapPoints (
	AcDb::OsnapMode osnapMode,
	Adesk::GsMarker gsSelectionMark,
	const AcGePoint3d &pickPoint,
	const AcGePoint3d &lastPoint,
	const AcGeMatrix3d &viewXform,
	AcGePoint3dArray &snapPoints,
	AcDbIntArray &geomIds) const
{
	assertReadEnabled () ;
	return (CSSLineElement::subGetOsnapPoints (osnapMode, gsSelectionMark, pickPoint, lastPoint, viewXform, snapPoints, geomIds)) ;
}

Acad::ErrorStatus CSSTruss::subGetOsnapPoints (
	AcDb::OsnapMode osnapMode,
	Adesk::GsMarker gsSelectionMark,
	const AcGePoint3d &pickPoint,
	const AcGePoint3d &lastPoint,
	const AcGeMatrix3d &viewXform,
	AcGePoint3dArray &snapPoints,
	AcDbIntArray &geomIds,
	const AcGeMatrix3d &insertionMat) const
{
	assertReadEnabled () ;
	return (CSSLineElement::subGetOsnapPoints (osnapMode, gsSelectionMark, pickPoint, lastPoint, viewXform, snapPoints, geomIds, insertionMat)) ;
}

//- Grip points protocol
Acad::ErrorStatus CSSTruss::subGetGripPoints (
	AcGePoint3dArray &gripPoints, AcDbIntArray &osnapModes, AcDbIntArray &geomIds
) const {
	assertReadEnabled () ;
	//----- This method is never called unless you return eNotImplemented 
	//----- from the new getGripPoints() method below (which is the default implementation)

	return (CSSLineElement::subGetGripPoints (gripPoints, osnapModes, geomIds)) ;
}

Acad::ErrorStatus CSSTruss::subMoveGripPointsAt (const AcDbIntArray &indices, const AcGeVector3d &offset) {
	assertWriteEnabled () ;
	//----- This method is never called unless you return eNotImplemented 
	//----- from the new moveGripPointsAt() method below (which is the default implementation)

	return (CSSLineElement::subMoveGripPointsAt (indices, offset)) ;
}

Acad::ErrorStatus CSSTruss::subGetGripPoints (
	AcDbGripDataPtrArray &grips, const double curViewUnitSize, const int gripSize, 
	const AcGeVector3d &curViewDir, const int bitflags
) const {
	assertReadEnabled () ;

	//----- If you return eNotImplemented here, that will force AutoCAD to call
	//----- the older getGripPoints() implementation. The call below may return
	//----- eNotImplemented depending of your base class.
	return (CSSLineElement::subGetGripPoints (grips, curViewUnitSize, gripSize, curViewDir, bitflags)) ;
}

Acad::ErrorStatus CSSTruss::subMoveGripPointsAt (
	const AcDbVoidPtrArray &gripAppData, const AcGeVector3d &offset,
	const int bitflags
) {
	assertWriteEnabled () ;

	//----- If you return eNotImplemented here, that will force AutoCAD to call
	//----- the older getGripPoints() implementation. The call below may return
	//----- eNotImplemented depending of your base class.
	return (CSSLineElement::subMoveGripPointsAt (gripAppData, offset, bitflags)) ;
}

bool CSSTruss::initiatePnts(AcGePoint3dArray& pntArr, AcGeVector3d& vec1, AcGeVector3d& vec2, bool useDeformedGeom)
{
	AcDbObjectId id;
	if (!ObjUtils::getNode(&id, m_iNod))
	{
		acutPrintf(_T("CSSLineElement:ERROR finding node object"));
		return false;
	}
	AcDbObject   *pObj = NULL;
	CSSNode *piNode;
	ErrorStatus es = acdbOpenObject(pObj, id, AcDb::kForRead);
    assert(pObj != NULL);
    piNode = CSSNode::cast(pObj);
    assert(piNode != NULL);

	if (!ObjUtils::getNode(&id, m_jNod))
	{
		acutPrintf(_T("CSSLineElement:ERROR finding node object"));
		piNode->close();
		return false;
	}
	pObj = NULL;
	CSSNode *pjNode;
	es = acdbOpenObject(pObj, id, AcDb::kForRead);
    assert(pObj != NULL);
    pjNode = CSSNode::cast(pObj);
    assert(pjNode != NULL);
	AcGePoint3d crds1, crds2;
	if (useDeformedGeom)
	{
		crds1 = piNode->getDeformedCrds();
		crds2 = pjNode->getDeformedCrds();
	} else
	{
		crds1 = piNode->getCrds();
		crds2 = pjNode->getCrds();
		
	}
	piNode->close();
	pjNode->close();
	vec1 = vec2 = crds2-crds1;
	pntArr.append(crds1);
	pntArr.append(crds2);
	return true;
}
