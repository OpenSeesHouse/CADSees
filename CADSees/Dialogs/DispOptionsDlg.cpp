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
, m_defTagSize (DISPOPTIONS.tagSize)
, m_defNodeSize (DISPOPTIONS.nodeSize)
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
	DOCDATA->dispNodeTags((bool)m_dispNodeTags);
	acedUpdateDisplay();
}


void DispOptionsDlg::OnBnClickedDispElementsCheck()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	DOCDATA->dispEleTags((bool)m_dispEleTags);
	acedUpdateDisplay();
}


void DispOptionsDlg::OnEnChangeTagSizeEdit()
{
	if (!m_initiated)
		return;
	UpdateData(TRUE);
	if (m_tagSizeScale < 1)
		m_tagSizeScale = 1;
	double val = m_defTagSize;
	val *= (double)m_tagSizeScale/100.0;
	DOCDATA->setTagsSize(val);
	acedUpdateDisplay();
}


void DispOptionsDlg::OnEnChangeNodeSizeEdit()
{
	if (!m_initiated)
		return;
	UpdateData(TRUE);
	if (m_nodeSizeScale < 1)
		m_nodeSizeScale = 1;
	double val = m_defNodeSize;
	val *= (double)m_nodeSizeScale/100.0;
	DOCDATA->setNodeSize(val);
	acedUpdateDisplay();
}

void DispOptionsDlg::OnCancel()
{
	DOCDATA->save();
	CAcUiDialog::OnCancel();
}
