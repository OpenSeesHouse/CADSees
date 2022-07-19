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
//- StdAfx.h : include file for standard system include files,
//-      or project specific include files that are used frequently,
//-      but are changed infrequently
//-----------------------------------------------------------------------------
#pragma once
#define CADSEESOBJECTS_MODULE

#pragma pack (push, 8)
#pragma warning(disable: 4786 4996)
//#pragma warning(disable: 4098)

//-----------------------------------------------------------------------------
#include <windows.h>

//- ObjectARX and OMF headers needs this
#include <map>
#include <string>
#include <fstream>
#define _T(x)      L ## x

//-----------------------------------------------------------------------------
//- Include ObjectDBX/ObjectARX headers
//- Uncomment one of the following lines to bring a given library in your project.
//#define _BREP_SUPPORT_					//- Support for the BRep API
//#define _HLR_SUPPORT_						//- Support for the Hidden Line Removal API
//#define _AMODELER_SUPPORT_				//- Support for the AModeler API
#include "arxHeaders.h"
#include "boost/tokenizer.hpp"
#include "DocData.h" //- Your document specific data class holder
#include "CSSDocData.h" //- Your document specific data class holder
extern AcApDataManager<CDocData> DocVars ;

#include "ObjUtils/ObjUtils.h"
#include "Elements/CSSLineElement.h"
#include "Node/CSSNode.h"
#include "Recorders/CSSNodeRecorder.h"
#include "Elements/CSSTruss.h"
#include "Elements/CSSCorotTruss.h"
#include "Elements/CSSElasticBeamColumn.h"
#include "Elements/CSSTwoNodeLink.h"
#include "Elements/CSSForceBeamColumn.h"
#include "Elements/beamWithHinges.h"
#include "Elements/CSSDispBeamColumn.h"
#include "Elements/CSSModElasticBeamColumn.h"
#include "Elements/CSSZeroLength.h"
#include "Elements/CSSJoint2dElement.h"
#include "Elements/CSSBrickElement.h"
#include "Cube/CSSCube.h"
#include "Pile/CSSPile.h"
#include "Matrix/Matrix.h"
#include "Matrix/Vector.h"

#define wireColor (Adesk::UInt16)(254)
#define elementColor (Adesk::UInt16)(7)
#define eleDfrmdColor (Adesk::UInt16)(4)
#define nodeColor (Adesk::UInt16)(7)
#define nodeDfrmdColor (Adesk::UInt16)(30)

#define DOCDATA DocVars.docData(curDoc()).getData()
#define DISPOPTIONS DocVars.docData(curDoc()).getDispOptions()
#define BTRID DocVars.docData(curDoc()).btrId
#define NODEATCRDSMAP DocVars.docData(curDoc()).nodeAtCrdsMap
#define NODEATTAGMAP DocVars.docData(curDoc()).nodeAtTagMap
#define ELEMATTAGMAP DocVars.docData(curDoc()).elemAtTagMap
#pragma pack (pop)

