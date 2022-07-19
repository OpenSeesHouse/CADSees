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
#include "commands/commands.h"
#include "resource.h"
#include "dialogs/DispOptionsDlg.h"
#include "dialogs/DispDeformedDlg.h"
//-----------------------------------------------------------------------------
#define szRDS _RXST("CSS")


//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
class CSSEdReactor : public AcEditorReactor
{
public:

	virtual void endDwgOpen(const ACHAR* fileName, AcDbDatabase* pDb)
	{

	}
};

class CCADSeesApp : public AcRxArxApp {

public:
	CCADSeesApp() : AcRxArxApp() {}

	virtual AcRx::AppRetCode On_kInitAppMsg(void* pkt) {
		// TODO: Load dependencies here

		// You *must* call On_kInitAppMsg here
		AcRx::AppRetCode retCode = AcRxArxApp::On_kInitAppMsg(pkt);

		// TODO: Add your initialization code here
		//acedAlert(_T("\n******CADSees Program Loaded successfully*****\n**Developed by Seyed Alireza Jalali\n**at Civil Soft Science, Tehran, Iran\n**Description:\nA graphical post-processor for OpenSees software:\n**more at:\n***************www.CivilSoftScience.com***************:\n"));

		//acutPrintf(_T("\n******CADSees Program Loaded successfully*****\n"));
		//acutPrintf(_T("**Developed by Seyed Alireza Jalali\n"));
		//acutPrintf(_T("**at Civil Soft Science, Tehran, Iran\n"));
		//acutPrintf(_T("**Description:\n"));
		//acutPrintf(_T("     A graphical pre/post-processor for OpenSees software:\n"));
		//acutPrintf(_T("**more at:\n"));
		//acutPrintf(_T("***************www.CivilSoftScience.com***************:\n"));
		//CSSEdReactor* pEdr = new CSSEdReactor();
		//DocVars.docData(curDoc()).pDocReactor = pEdr;
		//acdbHostApplicationServices()->()->addReactor(pEdr);
		acutPrintf(_T("\n******CADSees.arx Loaded successfully*****\n"));
		return (retCode);

	}

	virtual AcRx::AppRetCode On_kLoadDwgMsg(void* pkt) {
		AcRx::AppRetCode retCode = AcRxArxApp::On_kLoadDwgMsg(pkt);
		DOCDATA;
		DISPOPTIONS.nodeSizeChanged = true;
		ObjUtils::RedrawGraphics(true);
		return (retCode);
	}
	virtual AcRx::AppRetCode On_kUnloadAppMsg(void* pkt) {
		// TODO: Add your code here

		// You *must* call On_kUnloadAppMsg here
		AcRx::AppRetCode retCode = AcRxArxApp::On_kUnloadAppMsg(pkt);

		// TODO: Unload dependencies here
		acutPrintf(_T("\n******CADSees.arx Unloaded successfully*****\n"));

		return (retCode);
	}

	virtual void RegisterServerComponents() {
	}


	// The ACED_ARXCOMMAND_ENTRY_AUTO macro can be applied to any static member
	static void CADSees_test()
	{
		AcDbBlockTableRecord* pBTR = ObjUtils::getModelSpace(OpenMode::kForWrite);
		for (int i = 1; i <= 10; i++)
		{
			AcGePoint3d cntr(i * 100, 0, 0);
			CSSCube* pCube = new CSSCube(cntr, 25);
			CSSCube* pCube2 = new CSSCube(cntr + AcGeVector3d(0, 100, 0), 25);
			pCube2->setSize(40);
			pBTR->appendAcDbEntity(pCube);
			pBTR->appendAcDbEntity(pCube2);
			pCube->close();
			pCube2->close();
		}
		pBTR->close();
	}

	static void CADSees_importOPS() {
		std::vector<std::vector<std::string>> allEleCommands;
		std::vector<std::vector<std::string>> allNodeCommands;
		resbuf* file;
		file = acutNewRb(RTSTR);

		AcApDocManager* DcMngr = acDocManagerPtr();
		if (DcMngr->curDocument() != NULL)
		{
			int ret;
			if ((ret = acedGetFileD(_T("CADSees: Select Input File"),
				AcString(INPUTFILE.c_str()).kACharPtr(), _T("ops;tcl"), NULL, file)) != RTNORM)
			{
				if (ret == RTCAN)
					acutPrintf(_T("*Cancel*\n"));
				else
					acutPrintf(_T("importOPS::failed to get folder address"));
				acutRelRb(file);
				return;
			}
		}
		else
			return;
		std::string fileName = AcString(file->resval.rstring).utf8Ptr();
		acutRelRb(file);
		std::ifstream from(fileName.c_str());
		std::string line;
		double maxEleLength = 0;
		while (getline(from, line, '\n'))
		{
			if (line.size() == 0 || line.find_first_not_of(' ') == std::string::npos)
				continue;
			std::vector<std::string> words = ObjUtils::pars(line, " ");
			if (words[0].compare("#Units") == 0 || words[0].compare("#units") == 0)
			{
				if (words[1].compare("M") == 0 || words[1].compare("m") == 0)
					LENGTHUNIT = m;
				else if (words[1].compare("cm") == 0 || words[1].compare("Cm") == 0)
					LENGTHUNIT = cm;
				else if (words[1].compare("mm") == 0 || words[1].compare("MM") == 0)
					LENGTHUNIT = mm;
				else if (words[1].compare("in") == 0)
					LENGTHUNIT = in;
				else if (words[1].compare("ft") == 0)
					LENGTHUNIT = ft;
				else
				{
					acutPrintf(_T("Unrecognized length unit: %s; use either m, cm, mm, ft or in\n"), AcString(words[1].c_str()).kACharPtr());
					continue;
				}
				LENGTHFAC = 1; //conversion factor from m to the defined unit
				switch (LENGTHUNIT)
				{
				case m:
					break;
				case mm:
					LENGTHFAC = 1000;
					break;
				case cm:
					LENGTHFAC = 100;
					break;
				case ft:
					LENGTHFAC = 0.3048;
					break;
				case in:
					LENGTHFAC = 0.0254;
					break;
				}
				DOCDATA->setNodeSize(DISPOPTIONS.nodeSize * LENGTHFAC);
				DOCDATA->setTagsSize(DISPOPTIONS.tagSize * LENGTHFAC);
				continue;
			}
			if (words[0].compare("model") == 0)
			{
				setModel(words);
				continue;
			}
			if (words[0].compare("node") == 0)
			{
				allNodeCommands.push_back(words);
				continue;
			}
			if (words[0].compare("element") == 0)
			{
				allEleCommands.push_back(words);
				continue;
			}
			if (words[0].compare("recorder") == 0)
			{
				addRecorder(words);
				continue;
			}
			if (words[0].compare("Cube") == 0)
			{
				addCube(words);
				continue;
			}
			if (words[0].compare("Pile") == 0)
			{
				addPile(words);
				continue;
			}
		}
		AcTransaction* pTrans = actrTransactionManager->startTransaction();
		assert(pTrans != NULL);
		for (int i = 0; i < allNodeCommands.size(); i++)
		{
			std::vector<std::string> cmnd = allNodeCommands[i];
			addNode(cmnd);
		}
		acedCommandS(RTSTR, _T("_Zoom"), RTSTR, _T("Extents"), RTSTR, _T(""), RTNONE);
		for (int i = 0; i < allEleCommands.size(); i++)
		{
			std::vector<std::string> cmnd = allEleCommands[i];
			addElement(cmnd);
		}
		actrTransactionManager->endTransaction();

		std::string::size_type ind = fileName.find_last_of("\\");
		fileName.erase(ind + 1);
		INPUTFILE = fileName;

		//ReadResponse(fileName);
	}

	static void CADSees_readResponse()
	{
		std::string::size_type ind = INPUTFILE.find_last_of("\\");
		std::string folder = INPUTFILE;
		folder.erase(ind + 1);
		AcString folderStr((folder + "press ENTER for selecting current folder").c_str());
		resbuf* file;
		file = acutNewRb(RTSTR);
		AcApDocManager* DcMngr = acDocManagerPtr();
		AcApDocument* pCurDoc = DcMngr->curDocument();
		if (pCurDoc != NULL)
		{
			if (acedGetFileD(_T("CADSees: Select Folder Containing Analysis Outputs"),
				folderStr, _T(""), 33, file) != RTNORM)
			{
				acedAlert(_T("CSS_ReadResponse::error getting folder address"));
				acutRelRb(file);
				return;
			}
		}
		else
			return;
		folder = AcString(file->resval.rstring).utf8Ptr();
		acutRelRb(file);
		ind = folder.find_last_of("\\");
		folder.erase(ind + 1);
		ReadResponse(folder);
	}

	static void CADSees_setDispOptions()
	{
		CAcModuleResourceOverride resOverride;
		DispOptionsDlg dlg(CWnd::FromHandle(adsw_acadMainWnd()));
		INT_PTR val = dlg.DoModal();
	}

	static void CADSees_dispDeformedShape()
	{
		CAcModuleResourceOverride resOverride;
		Vector timeVec = CSSRecorder::getTimeVec();
		int num;
		if (timeVec.Size() == 0)
			num = 0;
		else
			num = timeVec.Size();
		DispDeformedDlg dlg(num, false, CWnd::FromHandle(adsw_acadMainWnd()));
		INT_PTR val = dlg.DoModal();
	}
};

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CCADSeesApp)

ACED_ARXCOMMAND_ENTRY_AUTO(CCADSeesApp, CADSees, _test, test, ACRX_CMD_MODAL, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CCADSeesApp, CADSees, _importOPS, ImportOPS, ACRX_CMD_MODAL, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CCADSeesApp, CADSees, _readResponse, ReadResponse, ACRX_CMD_MODAL, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CCADSeesApp, CADSees, _setDispOptions, SetDispOptions, ACRX_CMD_MODAL, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CCADSeesApp, CADSees, _dispDeformedShape, DispDeformedShapes, ACRX_CMD_MODAL, NULL)

// Modal Command with pickfirst selection
//ACED_ARXCOMMAND_ENTRY_AUTO(CCADSeesApp, CSSMyGroup, MyPickFirst, MyPickFirstLocal, ACRX_CMD_MODAL | ACRX_CMD_USEPICKSET, NULL)

// Application Session Command with localized name
//ACED_ARXCOMMAND_ENTRY_AUTO(CCADSeesApp, CSSMyGroup, MySessionCmd, MySessionCmdLocal, ACRX_CMD_MODAL | ACRX_CMD_SESSION, NULL)

