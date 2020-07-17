#pragma once


// MainDialogue

#include "acui.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "../resource.h"

class MainDialogue : public CAcUiDialog
{
	DECLARE_DYNAMIC(MainDialogue)

public:
	MainDialogue(CWnd *pParent =NULL, HINSTANCE hInstance =NULL);   // standard constructor
	virtual ~MainDialogue();

// Dialog Data
	enum { IDD = IDD_MAINDIALOGUE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	virtual void OnCancel();
	afx_msg LRESULT OnAcadKeepFocus (WPARAM, LPARAM) ;
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
};
