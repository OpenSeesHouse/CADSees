#include "stdafx.h"
#include "CSSDocData.h"

Adesk::UInt32 CSSDocData::kCurrentVersionNumber = 1;
//double CSSNode::douplShift = 0;
//-----------------------------------------------------------------------------
ACRX_DXF_DEFINE_MEMBERS(
	CSSDocData, AcDbObject,
	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
	AcDbProxyEntity::kNoOperation, CSSDocData,
	CADSees
	| Product Desc : An OpenSees pre / post - processor
	| Company : Civil Soft Science
	| WEB Address : www.CivilSoftScience.com
)

CSSDocData::CSSDocData()
{
	//dispOptions.dispNodeTags = false;
	//dispOptions.dispEleTags = false;
	//dispOptions.dispDeformedShape = false;
	//dispOptions.dispUndeformedWire = true;
	//dispOptions.tagSize = 0.15;
	//dispOptions.nodeSize = 0.05;
	//dispOptions.nodeSizeChanged = false;
	NDM = 0;
	NDOF = 0;
	changed = true;
	dataId = NULL;
}

Acad::ErrorStatus CSSDocData::dwgOutFields(AcDbDwgFiler* pFiler) const
{
	assertReadEnabled();
	//----- Save parent class information first.
	Acad::ErrorStatus es = AcDbObject::dwgOutFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	if ((es = pFiler->writeItem(dispOptions.dispNodeTags)) != Acad::eOk)
		return (es);
	if ((es = pFiler->writeItem(dispOptions.dispEleTags)) != Acad::eOk)
		return (es);
	if ((es = pFiler->writeItem(dispOptions.dispDeformedShape)) != Acad::eOk)
		return (es);
	if ((es = pFiler->writeItem(dispOptions.dispUndeformedWire)) != Acad::eOk)
		return (es);
	if ((es = pFiler->writeItem(dispOptions.nodeSize)) != Acad::eOk)
		return (es);
	if ((es = pFiler->writeItem(dispOptions.tagSize)) != Acad::eOk)
		return (es);
	if ((es = pFiler->writeItem(NDM)) != Acad::eOk)
		return (es);
	if ((es = pFiler->writeItem(NDOF)) != Acad::eOk)
		return (es);
	return pFiler->filerStatus();

}

Acad::ErrorStatus CSSDocData::dwgInFields(AcDbDwgFiler* pFiler)
{
	assertWriteEnabled();
	//----- Save parent class information first.
	Acad::ErrorStatus es = AcDbObject::dwgInFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	if ((es = pFiler->readItem(&dispOptions.dispNodeTags)) != Acad::eOk)
		return (es);
	if ((es = pFiler->readItem(&dispOptions.dispEleTags)) != Acad::eOk)
		return (es);
	if ((es = pFiler->readItem(&dispOptions.dispDeformedShape)) != Acad::eOk)
		return (es);
	if ((es = pFiler->readItem(&dispOptions.dispUndeformedWire)) != Acad::eOk)
		return (es);
	if ((es = pFiler->readItem(&dispOptions.nodeSize)) != Acad::eOk)
		return (es);
	if ((es = pFiler->readItem(&dispOptions.tagSize)) != Acad::eOk)
		return (es);
	if ((es = pFiler->readItem(&NDM)) != Acad::eOk)
		return (es);
	if ((es = pFiler->readItem(&NDOF)) != Acad::eOk)
		return (es);
	if (pFiler->filerType() == AcDb::FilerType::kFileFiler)
		changed = false;
	return pFiler->filerStatus();
}

void CSSDocData::setNodeSize(double val) {
	if (fabs(val - dispOptions.nodeSize) < 0.001)
		return;
	changed = true;
	dispOptions.nodeSize = val;
	dispOptions.nodeSizeChanged = true;
	//ObjUtils::setDocData(this);
	ObjUtils::RedrawNodeGraphics(true);
	dispOptions.nodeSizeChanged = false;

}

void CSSDocData::setTagsSize(double val) {
	if (fabs(val - dispOptions.tagSize) < 0.001)
		return;
	changed = true;
	dispOptions.tagSize = val;
	//ObjUtils::setTagsSize(val);
	ObjUtils::RedrawGraphics(false);
}

void CSSDocData::setNdm(int val) {
	if (val == NDM)
		return;
	changed = true;
	NDM = val;
	//ObjUtils::setNdm(val);
}

void CSSDocData::setNdof(int val) {
	if (val == NDOF)
		return;
	changed = true;
	NDOF = val;
	//ObjUtils::setNdof(val);
}

void CSSDocData::dispEleTags(bool val) {
	if (val == dispOptions.dispEleTags)
		return;
	changed = true;
	dispOptions.dispEleTags = val;
	//ObjUtils::setShowEleTags(val);
	ObjUtils::RedrawElementsGraphics(false);
}

void CSSDocData::dispNodeTags(bool val) {
	if (val == dispOptions.dispNodeTags)
		return;
	changed = true;
	dispOptions.dispNodeTags = val;
	//ObjUtils::setShowNodeTags(val);
	ObjUtils::RedrawNodeGraphics(false);
}

void CSSDocData::dispDeformedShape(bool val, bool apply) {
	if (val != dispOptions.dispDeformedShape)
		changed = true;
	dispOptions.dispDeformedShape = val;
	if (apply)
		ObjUtils::RedrawGraphics(true);
}

void CSSDocData::dispUndeformedWire(bool val) {
	if (val == dispOptions.dispUndeformedWire)
		return;
	changed = true;
	dispOptions.dispUndeformedWire = val;
	//ObjUtils::setShowUndeformedWire(val);
	ObjUtils::RedrawGraphics(false);
}

void CSSDocData::save()
{
	if (!changed || dataId == NULL)
		return;
	AcDbObject* pBase;
	if (acdbOpenObject(pBase, dataId, kForWrite) != eOk)
	{
		acutPrintf(_T("CSSDocData::save() : error opening object\n"));
		return;
	}

	pBase->copyFrom(this);
	pBase->close();
	changed = false;
}

CSSDocData::DispOptions CSSDocData::defaultDispOptions()
{
	return DispOptions();
}
