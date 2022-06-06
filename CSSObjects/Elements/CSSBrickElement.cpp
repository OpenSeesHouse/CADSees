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
//----- CSSBrickElement.cpp : Implementation of CSSBrickElement
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "CSSBrickElement.h"
#include <math.h> 
//-----------------------------------------------------------------------------
Adesk::UInt32 CSSBrickElement::kCurrentVersionNumber =1 ;

//double CSSBrickElement::douplShift = 0;
//-----------------------------------------------------------------------------
ACRX_DXF_DEFINE_MEMBERS (
	CSSBrickElement, CSSElement,
	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent, 
	AcDbProxyEntity::kNoOperation, CSSBrickElement,
CADSees
| Product Desc : An OpenSees pre / post - processor
| Company : Civil Soft Science
| WEB Address : www.CivilSoftScience.com
)

//-----------------------------------------------------------------------------
CSSBrickElement::CSSBrickElement() : CSSElement() {
	 m_type = "NotInitiated";
	 pFaceList = 0;
	 pVertexList = 0;
}


CSSBrickElement::CSSBrickElement(int tag, std::vector<int> nodeTags, std::string type) : CSSElement(tag, type)
{
	 m_type = type;
	 m_nodeTags = nodeTags;
	 for (int i = 1; i <= nodeTags.size(); i++)
		  m_nodes.push_back(NULL);
	 m_cornerNodes = { 1, 2, 3, 4, 5, 6, 7, 8 };
	 pFaceList = NULL;
	 pVertexList = NULL;
}

CSSBrickElement::~CSSBrickElement () {
	if (pVertexList != 0)
		delete[] pVertexList;
	if (pFaceList != 0)
		delete[] pFaceList;
}

//-----------------------------------------------------------------------------
//----- AcDbObject protocols
//- Dwg Filing protocol
Acad::ErrorStatus CSSBrickElement::dwgOutFields (AcDbDwgFiler *pFiler) const {
	assertReadEnabled () ;
	//----- Save parent class information first.
	Acad::ErrorStatus es =AcDbEntity::dwgOutFields (pFiler) ;
	if ( es != Acad::eOk )
		return (es) ;
	//----- Object version number needs to be saved first
	if ( (es =pFiler->writeUInt32 (CSSBrickElement::kCurrentVersionNumber)) != Acad::eOk )
		return (es) ;
	//----- Output params
	return (pFiler->filerStatus ()) ;
}

Acad::ErrorStatus CSSBrickElement::dwgInFields (AcDbDwgFiler *pFiler) {
	assertWriteEnabled () ;
	//----- Read parent class information first.
	Acad::ErrorStatus es =AcDbEntity::dwgInFields (pFiler) ;
	if ( es != Acad::eOk )
		return (es) ;
	//----- Object version number needs to be read first
	Adesk::UInt32 version =0 ;
	if ( (es =pFiler->readUInt32 (&version)) != Acad::eOk )
		return (es) ;
	if ( version > CSSBrickElement::kCurrentVersionNumber )
		return (Acad::eMakeMeProxy) ;
	//- Uncomment the 2 following lines if your current object implementation cannot
	//- support previous version of that object.
	//if ( version < CSSBrickElement::kCurrentVersionNumber )
	//	return (Acad::eMakeMeProxy) ;
	//----- Read params

	return (pFiler->filerStatus ()) ;
}

//-----------------------------------------------------------------------------
//----- AcDbEntity protocols

Adesk::Boolean CSSBrickElement::subWorldDraw (AcGiWorldDraw *mode) {
	assertReadEnabled () ;
	if (pVertexList == NULL)
	{
		 upgradeOpen();
		 updateGeometry(false);
	}
	if (DISPOPTIONS.dispEleTags)
	{
		 AcGeVector3d vec(pVertexList[1] - pVertexList[0]);
		 AcGeVector3d normal = ObjUtils::getNdm() == 2 ? AcGeVector3d(0, 0, 1) : AcGeVector3d(0, -1, 0);
		 AcGeVector3d up = vec.perpVector();
		 AcGePoint3d crds = pVertexList[0] + 0.5 * vec + 0.03 * getLength() * up;
		 AcString tagStr;
		 tagStr.format(_T("%d"), m_tag);
		 mode->geometry().text(crds, normal, AcGeVector3d(1, 0, 0), DISPOPTIONS.tagSize, 1., 0, tagStr.kACharPtr());
	}
	mode->geometry().shell(8, pVertexList, 30, pFaceList);
	return (CSSElement::subWorldDraw(mode)) ;
}

Acad::ErrorStatus CSSBrickElement::subTransformBy(const AcGeMatrix3d& xform)
{
	assertWriteEnabled (false, false) ;
	if (pVertexList == 0)
		return ErrorStatus::eOk;
	for (int i = 0; i < 8; i++)
		pVertexList[i].transformBy(xform);
	return ErrorStatus::eOk;
}

double CSSBrickElement::getLength() const
{
	 assertReadEnabled();
	 double m_length = 0;
	 for (int i = 0; i < 3; i++)
		  m_length += pow(m_size[i],2);
	 m_length = pow(m_length,0.5);
	 return m_length;
}
void CSSBrickElement::subList() const
{
	 CSSElement::subList();
}

bool CSSBrickElement::updateGeometry(bool useDeformedGeom)
{
	 assertWriteEnabled(false, true);
	 bool res = CSSElement::updateGeometry(useDeformedGeom);
	 if (!res)
		  return false;
	 if (pVertexList == NULL)
	 {
		  pVertexList = new AcGePoint3d[8];
		  AcDbObjectId id;
		  for (int i : m_cornerNodes)
		  {
				if (!ObjUtils::getNode(id, m_nodeTags[i - 1]))
				{
					 acutPrintf(_T("CSSBrickElement:Node with tag %d was not found in the dataBase"), m_nodeTags[i - 1]);
					 return false;
				}
				AcDbObject* pObj = NULL;
				ErrorStatus es = acdbOpenObject(pObj, id, AcDb::kForRead);
				assert(pObj != NULL);
				m_nodes[i - 1] = CSSNode::cast(pObj);
				assert(m_nodes[i-1] != NULL);
				if (useDeformedGeom)
					 pVertexList[i-1] = m_nodes[i - 1]->getDeformedCrds();
				else
					 pVertexList[i-1] = m_nodes[i - 1]->getCrds();
				m_nodes[i - 1]->close();
		  }

		  pFaceList = new Adesk::Int32[30];
		  int index = -1;
		  int num, num2;
		  num = 4;
		  for (int i = 0; i < num; i++)
		  {
				num2 = i + 1;
				if (i == num - 1)
					 num2 = 0;
				pFaceList[++index] = 4;
				pFaceList[++index] = i;
				pFaceList[++index] = num2;
				pFaceList[++index] = num2 + num;
				pFaceList[++index] = i + num;
		  }
		  pFaceList[++index] = 4;
		  pFaceList[++index] = 0;
		  pFaceList[++index] = 1;
		  pFaceList[++index] = 2;
		  pFaceList[++index] = 3;

		  pFaceList[++index] = 4;
		  pFaceList[++index] = 4;
		  pFaceList[++index] = 5;
		  pFaceList[++index] = 6;
		  pFaceList[++index] = 7;
	 }

	 m_size[0] = (pVertexList[1] - pVertexList[0]).length();
	 m_size[1] = (pVertexList[7] - pVertexList[3]).length();
	 m_size[2] = (pVertexList[0] - pVertexList[3]).length();
	 m_isNull = false;
	 return true;
}