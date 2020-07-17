// DispDeformedDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DispDeformedDlg.h"
#include "../resource.h"
#include "../commands/commands.h"
// DispDeformedDlg

IMPLEMENT_DYNAMIC(DispDeformedDlg, CAcUiDialog)

DispDeformedDlg::DispDeformedDlg(int numSteps, bool hasEnvRcrdr, CWnd* pParent, HINSTANCE hInstance) : CAcUiDialog (DispDeformedDlg::IDD, pParent, hInstance)
, m_numSteps(numSteps)
, m_respTimeStr(_T(""))
, m_lastStepStr(_T(""))
, m_dispDeformed(DISPOPTIONS.dispDeformedShape)
, m_dispWire(DISPOPTIONS.dispUndeformedWire)
, m_respStep(0)
, m_initiated(false)
{
	m_respTimeStr.Format(_T("%d"), m_dispDeformed);
	m_lastStepStr.Format(_T("(%d)"), m_numSteps);
}

DispDeformedDlg::~DispDeformedDlg()
{
}

void DispDeformedDlg::DoDataExchange(CDataExchange* pDX)
{
	CAcUiDialog::DoDataExchange (pDX) ;
	DDX_Text(pDX, IDC_RESP_TIME_TEXT, m_respTimeStr);
	DDX_Text(pDX, IDC_LAST_STEP_TEXT, m_lastStepStr);
	DDX_Check(pDX, IDC_DISP_DEFORMED_CHECK, m_dispDeformed);
	DDX_Check(pDX, IDC_DISP_DEFORMED_CHECK, m_dispWire);
	DDX_Text(pDX, IDC_RESP_STEP_EDIT, m_respStep);
	DDX_Control(pDX, IDC_RESP_STEP_SPIN, m_stepSpin);
	DDX_Control(pDX, IDC_RESP_STEP_SLIDER, m_stepSlider);
	DDX_Control(pDX, IDC_DISP_DEFORMED_CHECK, m_dispDeformedCheck);
	DDX_Control(pDX, IDC_DISP_DEFORMED_CHECK2, m_dispWireCheck);
}

LRESULT DispDeformedDlg::OnAcadKeepFocus(WPARAM, LPARAM)
{
	return (FALSE) ;
}

BOOL DispDeformedDlg::OnInitDialog()
{
	CAcUiDialog::OnInitDialog();
	m_stepSlider.SetRange(1, m_numSteps+1);
	m_stepSpin.SetRange(1, m_numSteps+1);
	if (m_numSteps == 0)
	{
		DISPOPTIONS.dispDeformedShape = false;
		m_dispDeformedCheck.SetCheck(0);
		m_dispDeformedCheck.EnableWindow(FALSE);
		m_stepSlider.EnableWindow(FALSE);
		m_stepSpin.EnableWindow(FALSE);
	}
	else
	{
		DISPOPTIONS.dispDeformedShape = true;
		m_dispDeformedCheck.SetCheck(1);		
		m_stepSlider.SetPos(1);
		m_stepSpin.SetPos(1);
		double t = SetDeformedState(0);
		acedUpdateDisplay();
		m_respTimeStr.Format(_T("%.3f"), t);
	}
	m_initiated = true;
	return true;
}

BEGIN_MESSAGE_MAP(DispDeformedDlg, CAcUiDialog)
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, OnAcadKeepFocus)
	ON_EN_CHANGE(IDC_RESP_STEP_EDIT, &DispDeformedDlg::OnEnChangeRespStepEdit)
	ON_NOTIFY(TRBN_THUMBPOSCHANGING, IDC_RESP_STEP_SLIDER, &DispDeformedDlg::OnTRBNThumbPosChangingRespStepSlider)
	ON_BN_CLICKED(IDC_DISP_DEFORMED_CHECK, &DispDeformedDlg::OnBnClickedDispDeformedCheck)
	ON_BN_CLICKED(IDC_DISP_DEFORMED_CHECK2, &DispDeformedDlg::OnBnClickedDispWireCheck)
END_MESSAGE_MAP()

// DispDeformedDlg message handlers




void DispDeformedDlg::OnEnChangeRespStepEdit()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CAcUiDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	if (!m_initiated)
		return;
	UpdateData(TRUE);
	double t = SetDeformedState(m_respStep-1);
	acedUpdateDisplay();
	m_respTimeStr.Format(_T("%.3f"), t);
	m_stepSlider.SetPos(m_respStep);
	UpdateData(FALSE);
}


void DispDeformedDlg::OnTRBNThumbPosChangingRespStepSlider(NMHDR *pNMHDR, LRESULT *pResult)
{
	// This feature requires Windows Vista or greater.
	// The symbol _WIN32_WINNT must be >= 0x0600.
	NMTRBTHUMBPOSCHANGING *pNMTPC = reinterpret_cast<NMTRBTHUMBPOSCHANGING *>(pNMHDR);
	*pResult = 0;
	// TODO: Add your control notification handler code here
	int num = pNMTPC->dwPos;
	m_stepSpin.SetPos(num);
}


void DispDeformedDlg::OnBnClickedDispDeformedCheck()
{
	// TODO: Add your control notification handler code here
	if (!m_initiated)
		return;
	UpdateData(TRUE);
	bool showDfrmd;
	if (m_dispDeformed == 0)
	{
		DISPOPTIONS.dispDeformedShape = false;
		showDfrmd = false;
		m_dispDeformedCheck.EnableWindow(FALSE);
		m_stepSlider.EnableWindow(FALSE);
		m_stepSpin.EnableWindow(FALSE);
	}
	else
	{
		DISPOPTIONS.dispDeformedShape = true;
		showDfrmd = true;
		m_dispDeformedCheck.EnableWindow(TRUE);
		m_stepSlider.EnableWindow(TRUE);
		m_stepSpin.EnableWindow(TRUE);		
	}
	ObjUtils::setShowDeformed(showDfrmd);
	ObjUtils::RedrawGraphics(false);
	acedUpdateDisplay();
}

void DispDeformedDlg::OnBnClickedDispWireCheck()
{
	// TODO: Add your control notification handler code here
	if (!m_initiated)
		return;
	UpdateData(TRUE);
	bool showDfrmd;
	if (m_dispWire == 0)
	{
		DISPOPTIONS.dispUndeformedWire = false;
		showDfrmd = false;
		m_dispWireCheck.EnableWindow(FALSE);
	}
	else
	{
		DISPOPTIONS.dispUndeformedWire = true;
		showDfrmd = true;
		m_dispWireCheck.EnableWindow(TRUE);
	}
	ObjUtils::setShowUndeformedWire(showDfrmd);
	ObjUtils::RedrawGraphics(false);
	acedUpdateDisplay();
}
