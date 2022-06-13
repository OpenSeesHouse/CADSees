#pragma once


#include "acui.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "..\resource.h"

// DispOptionsDlg

class DispOptionsDlg : public CAcUiDialog
{
	DECLARE_DYNAMIC(DispOptionsDlg)

public:
	DispOptionsDlg(CWnd *pParent =NULL, HINSTANCE hInstance =NULL);
	virtual ~DispOptionsDlg();
// Dialog Data
	enum { IDD = IDD_DISPOPTIONSDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg LRESULT OnAcadKeepFocus (WPARAM, LPARAM) ;
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
	bool m_initiated;
	virtual void OnCancel();
public:
	double m_defTagSize, m_defNodeSize;
	BOOL m_dispNodeTags;
	BOOL m_dispEleTags;
	int m_tagSizeScale;
	CSpinButtonCtrl m_tagSizeSpin;
	int m_nodeSizeScale;
	CSpinButtonCtrl m_nodeSizeSpin;
	afx_msg void OnBnClickedDispNodesCheck();
	afx_msg void OnBnClickedDispElementsCheck();
	afx_msg void OnEnChangeTagSizeEdit();
	afx_msg void OnEnChangeNodeSizeEdit();
};


