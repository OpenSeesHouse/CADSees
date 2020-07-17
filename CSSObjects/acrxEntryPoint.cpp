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
//----- acrxEntryPoint.cpp
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "resource.h"

//-----------------------------------------------------------------------------
#define szRDS _RXST("CSS")

//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
class CCSSObjectsApp : public AcRxDbxApp {

public:
	CCSSObjectsApp () : AcRxDbxApp () {}

	virtual AcRx::AppRetCode On_kInitAppMsg (void *pkt) {
		// TODO: Load dependencies here

		// You *must* call On_kInitAppMsg here
		AcRx::AppRetCode retCode =AcRxDbxApp::On_kInitAppMsg (pkt) ;
		
		// TODO: Add your initialization code here
		acutPrintf(_T("\n******CSSObjects.dbx Loaded successfully*****\n"));
		//acutPrintf(_T("\n******CADSees Program Loaded successfully*****\n"));
		//acutPrintf(_T("**Developed by Seyed Alireza Jalali\n"));
		//acutPrintf(_T("**at Civil Soft Science, Tehran, Iran\n"));
		//acutPrintf(_T("**Description:\n"));
		//acutPrintf(_T("     A graphical pre/post-processor for OpenSees software:\n"));
		//acutPrintf(_T("**more at:\n"));
		//acutPrintf(_T("***************www.CivilSoftScience.com***************:\n"));

		return (retCode) ;
	}

	virtual AcRx::AppRetCode On_kUnloadAppMsg (void *pkt) {
		// TODO: Add your code here

		// You *must* call On_kUnloadAppMsg here
		AcRx::AppRetCode retCode =AcRxDbxApp::On_kUnloadAppMsg (pkt) ;

		// TODO: Unload dependencies here
		acutPrintf(_T("\n******CSSObjects.dbx Unoaded successfully*****\n"));

		return (retCode) ;
	}

	virtual void RegisterServerComponents () {
	}
	
} ;

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CCSSObjectsApp)

