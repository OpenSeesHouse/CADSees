// DispOptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DispOptionsDlg.h"
#include "../resource.h"
// DispOptionsDlg

IMPLEMENT_DYNAMIC(DispOptionsDlg, CAcUiDialog)

DispOptionsDlg::DispOptionsDlg(CWnd* pParent, HINSTANCE hInstance) : CAcUiDialog (DispOptionsDlg::IDD, pParent, hInstance)
, m_dispNodeTags(DISPOPTIONS.dispNodeTags)
, m_dispEleTags(DISPOPTIONS.dispEleTags)
, m_tagSizeScale(1)
, m_nodeSizeScale(1)
, m_initiated(false)
{

}

DispOptionsDlg::~DispOptionsDlg()
{
}

void DispOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CAcUiDialog::DoDataExchange (pDX) ;
	DDX_Check(pDX, IDC_DISPNODESCHECK, m_dispNodeTags);
	DDX_Check(pDX, IDC_DISPELEMENTSCHECK, m_dispEleTags);
	DDX_Text(pDX, IDC_TAGSIZEEDIT, m_tagSizeScale);
	DDX_Control(pDX, IDC_TAGSIZESPIN, m_tagSizeSpin);
	DDX_Text(pDX, IDC_NODESIZEEDIT, m_nodeSizeScale);
	DDX_Control(pDX, IDC_NODESIZESPIN, m_nodeSizeSpin);
}

LRESULT DispOptionsDlg::OnAcadKeepFocus(WPARAM, LPARAM)
{
	return (FALSE) ;
}

BOOL DispOptionsDlg::OnInitDialog()
{
	CAcUiDialog::OnInitDialog();
	m_tagSizeSpin.SetRange(1, 10000);
	m_tagSizeSpin.SetPos(100);
	m_nodeSizeSpin.SetRange(1, 10000);
	m_nodeSizeSpin.SetPos(100);
	m_initiated = true;
	return true;
}

BEGIN_MESSAGE_MAP(DispOptionsDlg, CAcUiDialog)
	//ON_MESSAGE(WM_ACAD_KEEPFOCUS, OnAcadKeepFocus)
	ON_BN_CLICKED(IDC_DISPNODESCHECK, &DispOptionsDlg::OnBnClickedDispNodesCheck)
	ON_BN_CLICKED(IDC_DISPELEMENTSCHECK, &DispOptionsDlg::OnBnClickedDispElementsCheck)
	ON_EN_CHANGE(IDC_TAGSIZEEDIT, &DispOptionsDlg::OnEnChangeTagSizeEdit)
	ON_EN_CHANGE(IDC_NODESIZEEDIT, &DispOptionsDlg::OnEnChangeNodeSizeEdit)
END_MESSAGE_MAP()



// DispOptionsDlg message handlers




void DispOptionsDlg::OnBnClickedDispNodesCheck()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if (m_dispNodeTags == 1)
	{
		DISPOPTIONS.dispNodeTags = true;
		ObjUtils::setShowNodeTags(true);
	}
	else
	{
		DISPOPTIONS.dispNodeTags = false;
		ObjUtils::setShowNodeTags(false);
	}
	ObjUtils::RedrawNodeGraphics(false);
	acedUpdateDisplay();
}


void DispOptionsDlg::OnBnClickedDispElementsCheck()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if (m_dispEleTags == 1)
	{
		DISPOPTIONS.dispEleTags = true;
		ObjUtils::setShowEleTags(true);
	}
	else
	{
		DISPOPTIONS.dispEleTags = false;
		ObjUtils::setShowEleTags(false);
	}
	ObjUtils::RedrawElementsGraphics(false);
	acedUpdateDisplay();
}


void DispOptionsDlg::OnEnChangeTagSizeEdit()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CAcUiDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	if (!m_initiated)
		return;
	UpdateData(TRUE);
	if (m_tagSizeScale < 1)
		m_tagSizeScale = 1;
	double val = DISPOPTIONS.defTagSize;
	val *= (double)m_tagSizeScale/100.0;
	ObjUtils::setTagsSize(val);
	ObjUtils::RedrawGraphics(false);
	acedUpdateDisplay();
}


void DispOptionsDlg::OnEnChangeNodeSizeEdit()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CAcUiDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	if (!m_initiated)
		return;
	UpdateData(TRUE);
	if (m_nodeSizeScale < 1)
		m_nodeSizeScale = 1;
	double val = DISPOPTIONS.defNodeSize;
	val *= (double)m_nodeSizeScale/100.0;
	ObjUtils::setNodesSize(val);
	ObjUtils::RedrawNodeGraphics(true);
	acedUpdateDisplay();
}

void DispOptionsDlg::OnCancel()
{
	DISPOPTIONS.nodeSize = DISPOPTIONS.defNodeSize * (double)m_nodeSizeScale/100.;
	DISPOPTIONS.tagSize = DISPOPTIONS.defTagSize * (double)m_tagSizeScale/100.0;
	CAcUiDialog::OnCancel();
}
