// MainDialogue.cpp : implementation file
//

#include "stdafx.h"
#include "MainDialogue.h"
extern MainDialogue* theDialogue;

// MainDialogue

IMPLEMENT_DYNAMIC(MainDialogue, CAcUiDialog)

MainDialogue::MainDialogue(CWnd* pParent, HINSTANCE hInstance)
	: CAcUiDialog (MainDialogue::IDD, pParent, hInstance)
{
}

MainDialogue::~MainDialogue()
{
}

void MainDialogue::PostNcDestroy()
{
	delete this;
	if (theDialogue != NULL)
		theDialogue = NULL;
	PostNcDestroy();
}

void MainDialogue::OnCancel()
{
	DestroyWindow();
}

void MainDialogue::DoDataExchange(CDataExchange* pDX)
{
}

LRESULT MainDialogue::OnAcadKeepFocus(WPARAM, LPARAM)
{
	return (TRUE) ;
}

void MainDialogue::OnOK()
{
	/*if (!UpdateData(TRUE))
	{
		return;
	}*/
	CAcUiDialog::OnOK();
}

BOOL MainDialogue::OnInitDialog()
{
	CAcUiDialog::OnInitDialog();
	return TRUE;
}

BEGIN_MESSAGE_MAP(MainDialogue, CAcUiDialog)
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, OnAcadKeepFocus)
END_MESSAGE_MAP()



// MainDialogue message handlers


