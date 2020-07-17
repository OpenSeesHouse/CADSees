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
//----- CSSBCElement.cpp : Implementation of CSSLineElement
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
	m_iNod = 0;
	m_jNod = 0;
	m_numIntegPnts = 0;
	m_pIntegPoints = NULL;
	pDeformedSpLine = 0;
	pUndeformedRep = 0;
	m_length = 0;
}

CSSLineElement::CSSLineElement(int tag, int inode, int jnode, int nIntegPnts, std::string type): CSSElement (tag, type)
{
	m_pIntegPoints = NULL;
	m_iNod = inode;
	m_jNod = jnode;
	m_numIntegPnts = nIntegPnts;
	AcGePoint3dArray pntArr;
	AcGeVector3d vec1, vec2;
	if (! initiatePnts(pntArr, vec1, vec2, false))
	{
		m_isNull = true;
		m_tag = 0;
		m_iNod = 0;
		m_jNod = 0;
		m_numIntegPnts = 0;
		return;
	}
	m_length = vec1.length();
	pDeformedSpLine = new AcDbSpline(pntArr, vec1, vec2, 3, 0);
	pUndeformedRep = new AcDbLine(pntArr.first(), pntArr.last());
	m_isNull = false;
}

CSSLineElement::~CSSLineElement () {
	if (m_pIntegPoints != NULL)
		delete[] m_pIntegPoints;
	if (pDeformedSpLine != 0)
		delete pDeformedSpLine;
	if (pUndeformedRep != 0)
		delete pUndeformedRep;
}

//-----------------------------------------------------------------------------
//----- AcDbObject protocols
//- Dwg Filing protocol
Acad::ErrorStatus CSSLineElement::dwgOutFields (AcDbDwgFiler *pFiler) const {
	assertReadEnabled () ;
	//----- Save parent class information first.
	Acad::ErrorStatus es =CSSElement::dwgOutFields (pFiler) ;
	if ( es != Acad::eOk )
		return (es) ;
	//----- Object version number needs to be saved first
	if ( (es =pFiler->writeUInt32 (CSSLineElement::kCurrentVersionNumber)) != Acad::eOk )
		return (es) ;
	//----- Output params
	if ( (es =pFiler->writeItem (m_iNod)) != Acad::eOk )
		return (es) ;
	if ( (es =pFiler->writeItem (m_jNod)) != Acad::eOk )
		return (es) ;
	if ( (es =pFiler->writeItem (m_numIntegPnts)) != Acad::eOk )
		return (es) ;

	return (pFiler->filerStatus ()) ;
}

Acad::ErrorStatus CSSLineElement::dwgInFields (AcDbDwgFiler *pFiler) {
	assertWriteEnabled () ;
	//----- Read parent class information first.
	Acad::ErrorStatus es =CSSElement::dwgInFields (pFiler) ;
	if ( es != Acad::eOk )
		return (es) ;
	//----- Object version number needs to be read first
	Adesk::UInt32 version =0 ;
	if ( (es =pFiler->readUInt32 (&version)) != Acad::eOk )
		return (es) ;
	if ( version > CSSLineElement::kCurrentVersionNumber )
		return (Acad::eMakeMeProxy) ;
	//- Uncomment the 2 following lines if your current object implementation cannot
	//- support previous version of that object.
	//if ( version < CSSLineElement::kCurrentVersionNumber )
	//	return (Acad::eMakeMeProxy) ;
	//----- Read params
	if ( (es =pFiler->readItem (&m_iNod)) != Acad::eOk )
		return (es) ;
	if ( (es =pFiler->readItem (&m_jNod)) != Acad::eOk )
		return (es) ;
	if ( (es =pFiler->readItem (&m_numIntegPnts)) != Acad::eOk )
		return (es) ;
	AcGePoint3dArray pntArr;
	AcGeVector3d vec1, vec2;
	if (!initiatePnts(pntArr, vec1, vec2, false))
	{
		m_isNull = true;
		return ErrorStatus::eAmbiguousInput;
	}
	m_length = vec1.length();
	if(ObjUtils::getNodeSize() == 0)
	{
		ObjUtils::setNodesSize(m_length*NODSIZERAT);
		ObjUtils::setTagsSize(m_length*NODSIZERAT);
		ObjUtils::RedrawNodeGraphics(true);
	}
	if (pDeformedSpLine != 0)
		delete pDeformedSpLine;
	if (pUndeformedRep != 0)
		delete pUndeformedRep;
	pDeformedSpLine = new AcDbSpline(pntArr, vec1, vec2, 3, 0);
	pUndeformedRep = new AcDbLine(pntArr.first(), pntArr.last());
	m_isNull = false;
	return (pFiler->filerStatus ()) ;
}

//-----------------------------------------------------------------------------
//----- AcDbEntity protocols
Adesk::Boolean CSSLineElement::subWorldDraw (AcGiWorldDraw *mode) {
	assertReadEnabled () ;
	/*for(int i = 0; i < m_numIntegPnts; i++)
		mode->geometry().draw(m_pIntegPoints[i]);*/
	if (ObjUtils::getShowDeformed())
	{
		pDeformedSpLine->setColorIndex(DOCDATA.elementColor);
		mode->geometry().draw(pDeformedSpLine);
		if (DISPOPTIONS.dispUndeformedWire)
		{
			pUndeformedRep->setColorIndex(DOCDATA.wireColor);
			mode->geometry().draw(pUndeformedRep);
		}
	}
	else
	{
		pUndeformedRep->setColorIndex(DOCDATA.elementColor);
		mode->geometry().draw(pUndeformedRep);
	}
	AcGePoint3d pnt1, pnt2;
	if (ObjUtils::getShowDeformed())
	{
		pDeformedSpLine->getFitPointAt(0, pnt1);
		pDeformedSpLine->getFitPointAt(1, pnt2);
	}
	else
	{
		pUndeformedRep->getStartPoint(pnt1);
		pUndeformedRep->getEndPoint(pnt2);
	}
	if (DISPOPTIONS.dispEleTags)
	{
		AcGeVector3d vec(pnt2-pnt1);
		AcGeVector3d normal = ObjUtils::getNdm() == 2 ? AcGeVector3d(0, 0, 1) : AcGeVector3d(0, -1, 0);
		AcGeVector3d up = vec.perpVector();
		pnt1 += 0.5*vec + 0.03*m_length*up;
		AcString tagStr;
		tagStr.format(_T("%d"), m_tag);
		mode->geometry().text(pnt1, normal, AcGeVector3d(1, 0, 0), DISPOPTIONS.tagSize, 1., 0, tagStr.kACharPtr());
	}
	return (AcDbEntity::subWorldDraw (mode)) ;
}


Adesk::UInt32 CSSLineElement::subSetAttributes (AcGiDrawableTraits *traits) {
	assertReadEnabled () ;
	return (AcDbEntity::subSetAttributes (traits)) ;
}

	//- Osnap points protocol
Acad::ErrorStatus CSSLineElement::subGetOsnapPoints (
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

Acad::ErrorStatus CSSLineElement::subGetOsnapPoints (
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
Acad::ErrorStatus CSSLineElement::subGetGripPoints (
	AcGePoint3dArray &gripPoints, AcDbIntArray &osnapModes, AcDbIntArray &geomIds
) const {
	assertReadEnabled () ;
	//----- This method is never called unless you return eNotImplemented 
	//----- from the new getGripPoints() method below (which is the default implementation)

	return (AcDbEntity::subGetGripPoints (gripPoints, osnapModes, geomIds)) ;
}

Acad::ErrorStatus CSSLineElement::subMoveGripPointsAt (const AcDbIntArray &indices, const AcGeVector3d &offset) {
	assertWriteEnabled () ;
	//----- This method is never called unless you return eNotImplemented 
	//----- from the new moveGripPointsAt() method below (which is the default implementation)

	return (AcDbEntity::subMoveGripPointsAt (indices, offset)) ;
}

Acad::ErrorStatus CSSLineElement::subGetGripPoints (
	AcDbGripDataPtrArray &grips, const double curViewUnitSize, const int gripSize, 
	const AcGeVector3d &curViewDir, const int bitflags
) const {
	assertReadEnabled () ;

	//----- If you return eNotImplemented here, that will force AutoCAD to call
	//----- the older getGripPoints() implementation. The call below may return
	//----- eNotImplemented depending of your base class.
	return (AcDbEntity::subGetGripPoints (grips, curViewUnitSize, gripSize, curViewDir, bitflags)) ;
}

Acad::ErrorStatus CSSLineElement::subMoveGripPointsAt (
	const AcDbVoidPtrArray &gripAppData, const AcGeVector3d &offset,
	const int bitflags
) {
	assertWriteEnabled () ;

	//----- If you return eNotImplemented here, that will force AutoCAD to call
	//----- the older getGripPoints() implementation. The call below may return
	//----- eNotImplemented depending of your base class.
	return (AcDbEntity::subMoveGripPointsAt (gripAppData, offset, bitflags)) ;
}

void CSSLineElement::subList() const
{
	CSSElement::subList();
	acutPrintf(_T("\n   iNode:\t%d"), m_iNod);
	acutPrintf(_T("\n   jNode:\t%d"), m_jNod);
	if (m_numIntegPnts != 0)
	{
		acutPrintf(_T("\n   numIntegPnts: %d"), m_numIntegPnts);		
	}
}

double CSSLineElement::getLength() const
{
	assertReadEnabled();
	return m_length;
}

bool CSSLineElement::initiatePnts(AcGePoint3dArray& pntArr, AcGeVector3d& vec1, AcGeVector3d& vec2, bool useDeformedGeom)
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
	crds1 = piNode->getCrds();
	crds2 = pjNode->getCrds();
	vec1 = vec2 = crds2-crds1;
	//piNode->setShiftVec(vec1);
	//pjNode->setShiftVec(-vec2);
	if (useDeformedGeom)
	{
		int ndof = piNode->getNDof();
		crds1 = piNode->getDeformedCrds();
		crds2 = pjNode->getDeformedCrds();
		AcGeVector3d rotation = piNode->getRotation();
		AcGeMatrix3d trans;
		trans.setToIdentity();
		if (ndof >= 3)
		{
			trans.setToRotation(rotation.z, AcGeVector3d::kZAxis, AcGePoint3d::kOrigin);
			if (ndof > 3)
			{
				AcGeMatrix3d trans2;
				trans2.setToRotation(rotation.x, AcGeVector3d::kXAxis, AcGePoint3d::kOrigin);
				trans *= trans2;
				trans2.setToRotation(rotation.y, AcGeVector3d::kYAxis, AcGePoint3d::kOrigin);
				trans *= trans2;
			}
		}
		vec1.transformBy(trans);

		rotation = pjNode->getRotation();
		trans.setToIdentity();
		if (ndof >= 3)
		{
			trans.setToRotation(rotation.z, AcGeVector3d::kZAxis, AcGePoint3d::kOrigin);
			if (ndof > 3)
			{
				AcGeMatrix3d trans2;
				trans2.setToRotation(rotation.x, AcGeVector3d::kXAxis, AcGePoint3d::kOrigin);
				trans *= trans2;
				trans2.setToRotation(rotation.y, AcGeVector3d::kYAxis, AcGePoint3d::kOrigin);
				trans *= trans2;
			}
		}
		vec2.transformBy(trans);
	}
	piNode->close();
	pjNode->close();

	pntArr.append(crds1);
	//AcGeVector3d vec(crds2-crds1);
	//double l = vec.length();
	//vec.normalize();
	/*if (m_numIntegPnts > 0)
	{
		m_pIntegPoints = new CSSNode*[m_numIntegPnts];
		double* xi = getSectionLocations(m_numIntegPnts);
		for (int i = 0; i < m_numIntegPnts; i++)
		{
			AcGePoint3d pnt(crds1+vec*xi[i]*l);
			m_pIntegPoints[i] = new CSSNode(0, pnt);
			pntArr.append(pnt);
		}
		delete[] xi;
	}*/
	pntArr.append(crds2);
	return true;
}

double * CSSLineElement::getSectionLocations(int numSections)
{
	double* xi = new double[numSections];
  switch(numSections) {
    
  case 1:
    xi[0] = 0.0;
    break;

  case 2:
    xi[0] = -0.577350269189626;
    xi[1] =  0.577350269189626;
    break;
    
  case 3:
    xi[0] = -0.774596669241483;
    xi[1] =  0.0;
    xi[2] =  0.774596669241483;
    break;
    
  case 4:
    xi[0] = -0.861136311594053;
    xi[1] = -0.339981043584856;
    xi[2] =  0.339981043584856;
    xi[3] =  0.861136311594053;
    break;
    
  case 5:
    xi[0] = -0.906179845938664;
    xi[1] = -0.538469310105683;
    xi[2] =  0.0;
    xi[3] =  0.538469310105683;
    xi[4] =  0.906179845938664;
    break;
    
  case 6:
    xi[0] = -0.932469514203152;
    xi[1] = -0.661209386466265;
    xi[2] = -0.238619186083197;
    xi[3] =  0.238619186083197;
    xi[4] =  0.661209386466265;
    xi[5] =  0.932469514203152;
    break;
    
  case 7:
    xi[0] = -0.949107912342759;
    xi[1] = -0.741531185599394;
    xi[2] = -0.405845151377397;
    xi[3] =  0.0;
    xi[4] =  0.405845151377397;
    xi[5] =  0.741531185599394;
    xi[6] =  0.949107912342759;
    break;

  case 8:
    xi[0] = -0.960289856497536;
    xi[1] = -0.796666477413627;
    xi[2] = -0.525532409916329;
    xi[3] = -0.183434642495650;
    xi[4] =  0.183434642495650;
    xi[5] =  0.525532409916329;
    xi[6] =  0.796666477413627;
    xi[7] =  0.960289856497536;
    break;
    
  case 9:
    xi[0] = -0.968160239507626;
    xi[1] = -0.836031107326636;
    xi[2] = -0.613371432700590;
    xi[3] = -0.324253423403809;
    xi[4] =  0.0;
    xi[5] =  0.324253423403809;
    xi[6] =  0.613371432700590;
    xi[7] =  0.836031107326636;
    xi[8] =  0.968160239507626;
    break;

  case 10:
    xi[0] = -0.973906528517172;
    xi[1] = -0.865063366688985;
    xi[2] = -0.679409568299024;
    xi[3] = -0.433395394129247;
    xi[4] = -0.148874338981631;
    xi[5] =  0.148874338981631;
    xi[6] =  0.433395394129247;
    xi[7] =  0.679409568299024;
    xi[8] =  0.865063366688985;
    xi[9] =  0.973906528517172;
    break;
  }

  for (int i = 0; i < numSections; i++)
    xi[i]  = 0.5*(xi[i] + 1.0);
  return xi;
}

void CSSLineElement::updateDeformedGeometry()
{
	assertWriteEnabled(false, true);
	AcGePoint3dArray pntArr;
	AcGeVector3d vec1, vec2;
	if (!initiatePnts(pntArr, vec1, vec2, true))
	{
		m_isNull = true;
	}
	pDeformedSpLine->setFitData(pntArr, 3, 0, vec1, vec2);
	//AcDbSpline::setFitData(pntArr, 3, 0, vec1, vec2);
}
