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
	dispOptions.dispNodeTags = true;
	dispOptions.dispEleTags = true;
	dispOptions.dispDeformedShape = false;
	dispOptions.dispUndeformedWire = true;
	dispOptions.tagSize = 20;
	dispOptions.nodeSize = 20;
	dispOptions.nodeSizeChanged = false;
	NDM = 0;
	NDOF = 0;
	wireColor = 254;
	elementColor = 7;
	eleDfrmdColor = 4;
	nodeColor = 7;
	nodeDfrmdColor = 30;
}

//-----------------------------------------------------------------------------
CDocData::CDocData (const CDocData &data) {
	*this = data;
}

//-----------------------------------------------------------------------------
CDocData::~CDocData () {
}
