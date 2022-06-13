// (C) Copyright 2002-2012 by Autodesk, Inc. 
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
//----- DocData.cpp : Implementation file
//-----------------------------------------------------------------------------
#include "StdAfx.h"

//-----------------------------------------------------------------------------
//----- The one and only document manager object. You can use the DocVars object to retrieve
//----- document specific data throughout your application
AcApDataManager<CDocData> DocVars ;

//-----------------------------------------------------------------------------
//----- Implementation of the document data class.
CDocData::CDocData () {
	//pData = new CSSDocData();
	pDocReactor = NULL;
	pData = NULL;
	lengthUnit = LengthUnit::m;
	lengthFac = 1.0;
}

//-----------------------------------------------------------------------------
CDocData::CDocData (const CDocData &data) {
	*this = data;
}

//-----------------------------------------------------------------------------
CDocData::~CDocData () {
}

CSSDocData* CDocData::getData()
{
	if (pData != NULL)
		return pData;
	ErrorStatus es;
	AcDbObjectId dataId;
	AcDbDictionary* pNamedObj;
	AcDbDictionary* pDict;
	AcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase();
	if (pDb == NULL)
		return NULL;
	es = pDb->getNamedObjectsDictionary(pNamedObj, AcDb::kForRead);
	es = pNamedObj->getAt(_T("CSSData"), (AcDbObject*&)pDict, AcDb::kForRead);
	if (es == eOk)
	{
		es = pDict->getAt(_T("settings"), dataId);
		if (es == eOk)
		{
			es = acdbOpenObject(pData, dataId, kForRead);
			if (es != eOk)
				acutPrintf(_T("CSSDocData::error opening object\n"));
		}
	}
	if (pData == NULL)
	{
		pDict = new AcDbDictionary;
		AcDbObjectId DictId;
		es = pNamedObj->upgradeOpen();
		es = pNamedObj->setAt(_T("CSSData"), pDict, DictId);
		pData = new CSSDocData();
		es = pDict->setAt(_T("settings"), pData, dataId);

	}
	pDict->close();
	pNamedObj->close();
	pData->setDataId(dataId);
	pData->close();
	inputFile = "";
	ObjUtils::setDocData(pData);
	return pData;
}