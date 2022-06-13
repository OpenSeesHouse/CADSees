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
//----- CSSNode.cpp : Implementation of CSSNode
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "CSSNode.h"

//-----------------------------------------------------------------------------
Adesk::UInt32 CSSNode::kCurrentVersionNumber =1 ;
//double CSSNode::douplShift = 0;
//-----------------------------------------------------------------------------
ACRX_DXF_DEFINE_MEMBERS (
	CSSNode, AcDbEntity,
	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent, 
	AcDbProxyEntity::kNoOperation, CSSNODE,
CADSees
|Product Desc:     An OpenSees pre/post-processor
|Company:          Civil Soft Science
|WEB Address:      www.CivilSoftScience.com
)

//-----------------------------------------------------------------------------
CSSNode::CSSNode () : AcDbEntity () {
	m_crds = AcGePoint3d(0,0,0);
	m_tag = 0;
	m_translation = AcGeVector3d(0, 0, 0);
	m_rotation = AcGeVector3d(0, 0, 0);
	m_NDof = 3;
	pUndeformedCube = 0;
	pDeformedCube = 0;
	m_numDouplPos = 0;
	//m_shiftVec.set(0., 0., 0);
}

CSSNode::CSSNode(int tag, int ndof, AcGePoint3d crds, int numDouplPos) : AcDbEntity()
{
	m_crds = crds;
	m_translation = AcGeVector3d(0, 0, 0);
	m_rotation = AcGeVector3d(0, 0, 0);
	m_tag = tag;
	m_NDof = ndof;
	pUndeformedCube = 0;
	pDeformedCube = 0;
	m_numDouplPos = numDouplPos;
	//m_shiftVec.set(0., 0., 0);
}

CSSNode::~CSSNode () {
	if (pUndeformedCube != 0)
		delete pUndeformedCube;
	if (pDeformedCube != 0)
		delete pDeformedCube;
}

//-----------------------------------------------------------------------------
//----- AcDbObject protocols
//- Dwg Filing protocol
Acad::ErrorStatus CSSNode::dwgOutFields (AcDbDwgFiler *pFiler) const {
	assertReadEnabled () ;
	//----- Save parent class information first.
	Acad::ErrorStatus es =AcDbEntity::dwgOutFields (pFiler) ;
	if ( es != Acad::eOk )
		return (es) ;
	//----- Object version number needs to be saved first
	if ( (es =pFiler->writeUInt32 (CSSNode::kCurrentVersionNumber)) != Acad::eOk )
		return (es) ;
	//----- Output params
	if ( (es =pFiler->writeItem (m_tag)) != Acad::eOk )
		return (es) ;
	if ( (es =pFiler->writeItem (m_crds)) != Acad::eOk )
		return (es) ;
	if ( (es =pFiler->writeItem (m_NDof)) != Acad::eOk )
		return (es) ;
	if ( (es =pFiler->writeItem (m_translation)) != Acad::eOk )
		return (es) ;
	if ( (es =pFiler->writeItem (m_rotation)) != Acad::eOk )
		return (es) ;
	if ( (es =pFiler->writeItem (m_numDouplPos)) != Acad::eOk )
		return (es) ;
	es = pFiler->filerStatus();
	return es;
}

Acad::ErrorStatus CSSNode::dwgInFields (AcDbDwgFiler *pFiler) {
	assertWriteEnabled () ;
	//----- Read parent class information first.
	Acad::ErrorStatus es =AcDbEntity::dwgInFields (pFiler) ;
	if ( es != Acad::eOk )
		return (es) ;
	//----- Object version number needs to be read first
	Adesk::UInt32 version =0 ;
	if ( (es =pFiler->readUInt32 (&version)) != Acad::eOk )
		return (es) ;
	if ( version > CSSNode::kCurrentVersionNumber )
		return (Acad::eMakeMeProxy) ;
	//- Uncomment the 2 following lines if your current object implementation cannot
	//- support previous version of that object.
	//if ( version < CSSNode::kCurrentVersionNumber )
	//	return (Acad::eMakeMeProxy) ;
	//----- Read params
	if ( (es =pFiler->readItem (&m_tag)) != Acad::eOk )
		return (es) ;
	if ( (es =pFiler->readItem (&m_crds)) != Acad::eOk )
		return (es) ;
	if ( (es =pFiler->readItem (&m_NDof)) != Acad::eOk )
		return (es) ;
	if ( (es =pFiler->readItem (&m_translation)) != Acad::eOk )
		return (es) ;
	if ( (es =pFiler->readItem (&m_rotation)) != Acad::eOk )
		return (es) ;
	if ( (es =pFiler->readItem (&m_numDouplPos)) != Acad::eOk )
		return (es) ;
	auto it = NODEATTAGMAP.find(m_tag);
	if (it == NODEATTAGMAP.end())
	{
		 NODEATTAGMAP.insert(std::pair<int, AcDbObjectId>(m_tag, this->objectId()));
	}
	auto it2 = NODEATCRDSMAP.find(m_crds);
	if (it2 == NODEATCRDSMAP.end() || it2->second != m_tag)
	{
		 NODEATCRDSMAP.insert(std::pair<AcGePoint3d,int>(m_crds,m_tag));
	}
	es = pFiler->filerStatus();
	return es ;
}

//-----------------------------------------------------------------------------
//----- AcDbEntity protocols
Adesk::Boolean CSSNode::subWorldDraw (AcGiWorldDraw *mode) {
	assertReadEnabled () ;
	if (pUndeformedCube == 0)
		return (AcDbEntity::subWorldDraw(mode));
	if (ObjUtils::getShowDeformed())
	{
		mode->geometry().draw(pDeformedCube);
		if (DISPOPTIONS.dispUndeformedWire)
		{
			pUndeformedCube->setColorIndex(wireColor);
			mode->geometry().draw(pUndeformedCube);
		}
	}
	else
	{
		pUndeformedCube->setColorIndex(nodeColor);
		mode->geometry().draw(pUndeformedCube);
	}
	if (DISPOPTIONS.dispNodeTags)
	{
		AcGePoint3d cntr = ObjUtils::getShowDeformed() ? getDeformedCrds() : m_crds;
		AcGeVector3d vec(1., 0., 0.);
		vec *= 1.2*DISPOPTIONS.nodeSize;
		if (m_numDouplPos > 0)
		{
			AcGeVector3d perp = vec.perpVector().normal();
			cntr += perp*DISPOPTIONS.tagSize*m_numDouplPos;
		}
		AcGeVector3d normal = DOCDATA->getNdm() == 2 ? AcGeVector3d(0, 0, 1) : AcGeVector3d(0, -1, 0);
		AcString tagStr;
		tagStr.format(_T("%d"), m_tag);
		mode->geometry().text(cntr+vec, normal, vec, DISPOPTIONS.tagSize, 1, 0, tagStr.kACharPtr());
		//mode->geometry().polygon(4, )
	}

	return (AcDbEntity::subWorldDraw(mode)) ;
}


Adesk::UInt32 CSSNode::subSetAttributes (AcGiDrawableTraits *traits) {
	assertReadEnabled () ;
	return (AcDbEntity::subSetAttributes (traits)) ;
}

	//- Osnap points protocol
Acad::ErrorStatus CSSNode::subGetOsnapPoints (
	AcDb::OsnapMode osnapMode,
	Adesk::GsMarker gsSelectionMark,
	const AcGePoint3d &pickPoint,
	const AcGePoint3d &lastPoint,
	const AcGeMatrix3d &viewXform,
	AcGePoint3dArray &snapPoints,
	AcDbIntArray &geomIds) const
{
	assertReadEnabled () ;
	return (AcDbEntity::subGetOsnapPoints (osnapMode, gsSelectionMark, pickPoint, lastPoint, viewXform, snapPoints, geomIds)) ;
}

Acad::ErrorStatus CSSNode::subGetOsnapPoints (
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
	return (AcDbEntity::subGetOsnapPoints (osnapMode, gsSelectionMark, pickPoint, lastPoint, viewXform, snapPoints, geomIds, insertionMat)) ;
}

//- Grip points protocol
Acad::ErrorStatus CSSNode::subGetGripPoints (
	AcGePoint3dArray &gripPoints, AcDbIntArray &osnapModes, AcDbIntArray &geomIds
) const {
	assertReadEnabled () ;
	//----- This method is never called unless you return eNotImplemented 
	//----- from the new getGripPoints() method below (which is the default implementation)

	return (AcDbEntity::subGetGripPoints (gripPoints, osnapModes, geomIds)) ;
}

Acad::ErrorStatus CSSNode::subMoveGripPointsAt (const AcDbIntArray &indices, const AcGeVector3d &offset) {
	assertWriteEnabled () ;
	//----- This method is never called unless you return eNotImplemented 
	//----- from the new moveGripPointsAt() method below (which is the default implementation)

	return (AcDbEntity::subMoveGripPointsAt (indices, offset)) ;
}

Acad::ErrorStatus CSSNode::subGetGripPoints (
	AcDbGripDataPtrArray &grips, const double curViewUnitSize, const int gripSize, 
	const AcGeVector3d &curViewDir, const int bitflags
) const {
	assertReadEnabled () ;

	//----- If you return eNotImplemented here, that will force AutoCAD to call
	//----- the older getGripPoints() implementation. The call below may return
	//----- eNotImplemented depending of your base class.
	return (AcDbEntity::subGetGripPoints (grips, curViewUnitSize, gripSize, curViewDir, bitflags)) ;
}

Acad::ErrorStatus CSSNode::subMoveGripPointsAt (
	const AcDbVoidPtrArray &gripAppData, const AcGeVector3d &offset,
	const int bitflags
) {
	assertWriteEnabled () ;

	//----- If you return eNotImplemented here, that will force AutoCAD to call
	//----- the older getGripPoints() implementation. The call below may return
	//----- eNotImplemented depending of your base class.
	return (AcDbEntity::subMoveGripPointsAt (gripAppData, offset, bitflags)) ;
}

void CSSNode::subList() const
{
	acutPrintf(_T("\n***CADSees***"));
	acutPrintf(_T("\nCSS_Node:"));
	acutPrintf(_T("\n   tag:\t\t\t%d"), m_tag);
	acutPrintf(_T("\n   ndof:\t\t\t%d"), m_NDof);
	acutPrintf(_T("\n   undeformed coordinates(x,y,z):\t%.2f, %.2f, %.2f"), m_crds.x, m_crds.y, m_crds.z);
	acutPrintf(_T("\n   translational deformations(x,y,z):\t%.2f, %.2f, %.2f"), m_translation.x, m_translation.y, m_translation.z);
	acutPrintf(_T("\n   rotational deformations(x,y,z):\t%.2f, %.2f, %.2f"), m_rotation.x, m_rotation.y, m_rotation.z);
}

Acad::ErrorStatus CSSNode::subErase(Adesk::Boolean pErasing)
{
	 auto it = NODEATTAGMAP.find(m_tag);
	 if (it != NODEATTAGMAP.end())
	 {
		  NODEATTAGMAP.erase(it);
	 }
	 auto it2 = NODEATCRDSMAP.find(m_crds);
	 if (it2 != NODEATCRDSMAP.end() && it2->second == m_tag)
	 {
		 NODEATCRDSMAP.erase(it2);
	 }

	 return AcDbEntity::subErase(pErasing);
}

void CSSNode::setDeformationAt(int dof, double value)
{
	assertWriteEnabled(false, true);
	switch (dof)
	{
	case 1:
		m_translation.x = value;
		break;
	case 2:
		m_translation.y = value;
		break;
	case 3:
		if (DOCDATA->getNdm() == 3)
			m_translation.z = value;
		else
			m_rotation.z = value;
		break;
	case 4:
		m_rotation.x = value;
		break;
	case 5:
		m_rotation.y = value;
		break;
	case 6:
		m_rotation.z = value;
		break;
	default:
		acutPrintf(_T("PSCNode::setDeformationAt - Unrecognized dof: %d"), dof);
	}
}

void CSSNode::initialize()
{
	if (pUndeformedCube == NULL)
	{
		double ndSz = DISPOPTIONS.nodeSize;
		pUndeformedCube = new CSSCube(m_crds, ndSz);
		pDeformedCube = new CSSCube(pUndeformedCube);
		pDeformedCube->setColorIndex(nodeDfrmdColor);
	} else
	{
		pUndeformedCube->setSize(DISPOPTIONS.nodeSize);
		pDeformedCube->setSize(DISPOPTIONS.nodeSize);
	}

}

void CSSNode::updateDeformedGeometry()
{
	assertWriteEnabled(false, true);
	if (pUndeformedCube == 0 || DISPOPTIONS.nodeSizeChanged)
	{
		initialize();
	}
	AcGeMatrix3d trans;
	trans.setToTranslation(m_translation);
	if (m_NDof >= 3)
	{
		AcGeMatrix3d trans2;
		trans2.setToRotation(m_rotation.z, AcGeVector3d::kZAxis, m_crds);
		trans *= trans2;
		if (m_NDof > 3)
		{
			trans2.setToRotation(m_rotation.x, AcGeVector3d::kXAxis, m_crds);
			trans *= trans2;
			trans2.setToRotation(m_rotation.y, AcGeVector3d::kYAxis, m_crds);
			trans *= trans2;
		}
	}
	pDeformedCube->initialize();
	pDeformedCube->transformBy(trans);
}


int CSSNode::getTag() const
{
	assertReadEnabled();
	return m_tag;
}

int CSSNode::getNDof() const
{
	assertReadEnabled();
	return m_NDof;
}

void CSSNode::setNDof(int val)
{
	assertWriteEnabled(false, true);
	m_NDof = val;
}

//void CSSNode::setShiftVec(AcGeVector3d vec)
//{
//	assertWriteEnabled(false, true);
//	m_shiftVec = vec.normal();
//	if (m_isDouplPosition)
//		initialize();
//}

AcGePoint3d CSSNode::getCrds() const
{
	assertReadEnabled();
	return m_crds/*+m_shiftVec*/;
}

AcGePoint3d CSSNode::getDeformedCrds() const
{
	assertReadEnabled();
	return (m_crds/*+m_shiftVec*/ + m_translation);
}

AcGeVector3d CSSNode::getRotation() const
{
	assertReadEnabled();
	return m_rotation;
}

//bool CSSNode::getMustShift() const
//{
//	assertReadEnabled();
//	return m_isDouplPosition;
//}