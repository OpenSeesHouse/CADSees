#pragma once


// DispDeformedDlg

#include "acui.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "..\resource.h"

// DispDeformedDlg

class DispDeformedDlg : public CAcUiDialog
{
	DECLARE_DYNAMIC(DispDeformedDlg)

public:
	DispDeformedDlg(int numSteps, bool hasEnvRcrdr, CWnd *pParent =NULL, HINSTANCE hInstance =NULL);
	virtual ~DispDeformedDlg();
// Dialog Data
	enum { IDD = IDD_DISPDEFORMEDDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnCancel();
	afx_msg LRESULT OnAcadKeepFocus (WPARAM, LPARAM) ;
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
	bool m_initiated;
	int m_numSteps;
public:
	CString m_respTimeStr;
	CString m_lastStepStr;
	BOOL m_dispDeformed;
	BOOL m_dispWire;
	int m_respStep;
	double m_scaleFac;
	CSpinButtonCtrl m_stepSpin;
	CSliderCtrl m_stepSlider;
	afx_msg void OnEnChangeRespStepEdit();
	afx_msg void OnEnChangeScaleFacEdit();
	afx_msg void OnTRBNThumbPosChangingRespStepSlider(NMHDR *pNMHDR, LRESULT *pResult);
	CButton m_dispDeformedCheck;
	CButton m_dispWireCheck;
	afx_msg void OnBnClickedDispDeformedCheck();
	afx_msg void OnBnClickedDispWireCheck();
};


