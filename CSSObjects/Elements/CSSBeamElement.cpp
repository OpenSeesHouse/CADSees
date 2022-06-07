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
//----- CSSBeamElement.cpp : Implementation of CSSBeamElement
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "CSSBeamElement.h"

//-----------------------------------------------------------------------------
Adesk::UInt32 CSSBeamElement::kCurrentVersionNumber = 1 ;

//-----------------------------------------------------------------------------
ACRX_DXF_DEFINE_MEMBERS (
	CSSBeamElement, CSSLineElement,
	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent, 
	AcDbProxyEntity::kNoOperation, CSSBeamElement,
CADSees
|Product Desc:     An OpenSees pre/post-processor
|Company:          Civil Soft Science
|WEB Address:      www.CivilSoftScience.com
)

//-----------------------------------------------------------------------------
CSSBeamElement::CSSBeamElement () : CSSLineElement() {
}

CSSBeamElement::CSSBeamElement(int tag, std::vector<int> nodeTags, std::string type): CSSLineElement (tag, nodeTags, type)
{
}

CSSBeamElement::~CSSBeamElement () {
	// if (m_pIntegPoints != NULL)
		// delete[] m_pIntegPoints;
	if (pDeformedEntity != 0)
		delete pDeformedEntity;
	if (pUndeformedEntity != 0)
		delete pUndeformedEntity;
}

bool CSSBeamElement::updateGeometry(bool useDeformedGeom)
{
	assertWriteEnabled(false, true);
	bool res = CSSLineElement::updateGeometry(useDeformedGeom);
	if (!res)
		return false;
	static AcGeVector3d vec2;
	static AcGePoint3dArray pntArr;
	pntArr.removeAll();
	if (useDeformedGeom)
	{
		int ndof = m_nodePtrs[0]->getNDof();
		AcGeVector3d rotation = m_nodePtrs[0]->getRotation();
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
		vec2 = vec; //vec is set in CSSLineElement
		vec.transformBy(trans);

		rotation = m_nodePtrs[1]->getRotation();
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

	pntArr.append(m_crds[0]);
	pntArr.append(m_crds[1]);
	m_nodePtrs[0]->close();
	m_nodePtrs[1]->close();
	if (pDeformedEntity == nullptr)
	{
		pDeformedEntity = new AcDbSpline(pntArr, vec, vec2, 3, 0);
		pUndeformedEntity = new AcDbLine(pntArr.first(), pntArr.last());
	}
	else if (useDeformedGeom)
	{
		AcDbSpline* pSp = (AcDbSpline*)pDeformedEntity;
		pSp->setFitData(pntArr, 3, 0, vec, vec2);
	}
	m_isNull = false;
	return true;
}

/*double* CSSBeamElement::getSectionLocations(int numSections)
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
}*/
