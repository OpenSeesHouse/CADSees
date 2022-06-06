#include "stdafx.h"
#include "ObjUtils.h"


std::vector<std::string> ObjUtils::pars(std::string str, const char* delimiter)
{
	typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
	boost::char_separator<char> sep(delimiter, "", boost::drop_empty_tokens);
	tokenizer tokens(str, sep);
	tokenizer::iterator tok_iter = tokens.begin();
	std::vector<std::string> res;
	int sz = tok_iter->size();
	res.reserve(sz);
	while (!tok_iter.at_end())
		res.push_back(*tok_iter++);
	return res;
}

AcDbObjectId ObjUtils::getModelSpaceId()
{
	ErrorStatus es;
	AcDbDatabase* pDwg = acdbHostApplicationServices()->workingDatabase();
	if (NULL == pDwg) {
		acutPrintf(_T("getModelSpaceId::Could not get dataBase"));
		return (NULL);
	}

	AcDbBlockTable* pBlockTable;
	es = pDwg->getSymbolTable(pBlockTable, kForRead);
	if (eOk != es) {
		acutPrintf(L"getModelSpaceId::Could not get block table");
		return (NULL);
	}

	AcDbObjectId id;
	es = pBlockTable->getAt(ACDB_MODEL_SPACE, id);
	if (eOk != es) {
		acutPrintf(L"getModelSpaceId::Could not getAt()");
		pBlockTable->close();
		return (NULL);
	}
	es = pBlockTable->close();
	if (es != eOk)
	{
		acutPrintf(_T("getModelSpaceId:: error closing symbol table"));
		return (NULL);
	}
	return id;
}

AcDbBlockTableRecord* ObjUtils::getModelSpace(OpenMode mode)
{
	ErrorStatus es;
	if (BTRID == NULL)
		BTRID = getModelSpaceId();
	AcDbBlockTableRecord* pModelSpace;
	es = actrTransactionManager->getObject((AcDbObject*&)pModelSpace, BTRID, mode);
	if (es != eOk)
	{
		acutPrintf(_T("getModelSpace:: error getting pBTR from transaction"));
		return NULL;
	}
	return (pModelSpace);
}


void ObjUtils::setNodesSize(double val)
{
	DISPOPTIONS.nodeSize = val;
	DISPOPTIONS.nodeSizeChanged = true;
}

void ObjUtils::addPile(AcGePoint3d pnt1, AcGePoint3d pnt2)
{
	AcDbObjectId id;
	CSSPile* pNode = new CSSPile(pnt1, pnt2);
	AcDbBlockTableRecord* pBTR = getModelSpace(OpenMode::kForWrite);
	assert(pBTR != NULL);
	pBTR->appendAcDbEntity(id, pNode);
	pNode->close();
	pBTR->close();
}

void ObjUtils::addCube(AcGePoint3d vertex, double sizeX, double sizeY, double sizeZ)
{
	AcDbObjectId id;
	CSSCube* pNode = new CSSCube(vertex, sizeX, sizeY, sizeZ);
	AcDbBlockTableRecord* pBTR = getModelSpace(OpenMode::kForWrite);
	assert(pBTR != NULL);
	pBTR->appendAcDbEntity(id, pNode);
	pNode->close();
	pBTR->close();
}

void ObjUtils::addNode(int tag, AcGePoint3d pnt)
{
	AcDbObjectId id;
	if (getNode(id, tag))
	{
		acutPrintf(_T("Node with tag %d already exists in model\n"), tag);
		return;
	}
	int num = getNumNodesAtCrd(pnt);
	CSSNode* pNode = new CSSNode(tag, DOCDATA.NDOF, pnt, num);
	AcDbBlockTableRecord* pBTR = getModelSpace(OpenMode::kForWrite);
	assert(pBTR != NULL);
	ErrorStatus es = pBTR->appendAcDbEntity(id, pNode);
	if (es != eOk)
	{
		acutPrintf(_T("failed to add Node with tag %d to BTR\n"), tag);
		return;
	}
	//pNode->draw();
	pNode->close();
	NODEATTAGMAP.insert(std::pair<int, AcDbObjectId>(tag, id));
	AcString crdStr;
	crdStr.format(_T("%.3f-%.3f-%.3f"), pnt.x, pnt.y, pnt.z);
	NODEATCRDSMAP.insert(std::pair<AcString, int>(crdStr, tag));

}

void ObjUtils::addElement(std::string type, int tag, std::vector<int> nodeTags, std::vector<std::string> line)
{
	AcDbObjectId id;
	if (getElement(&id, tag))
	{
		acutPrintf(_T("Element with tag %d already exists in model\n"), tag);
		return;
	}
	int nPnts = 0;
	if (type.compare("dispBeamColumn") == 0 ||
		type.compare("forceBeamColumn") == 0 ||
		type.compare("nonlinearBeamColumn") == 0)
	{
		//obtain npnts;
		if ((line[6].compare("Lobatto") != 0) &&
			(line[6].compare("Legendre") != 0) &&
			(line[6].compare("Radau") != 0) &&
			(line[6].compare("NewtonCotes") != 0) &&
			(line[6].compare("UserDefined") != 0) &&
			(line[6].compare("HingeMidpoint") != 0) &&
			(line[6].compare("HingeEndpoint") != 0) &&
			(line[6].compare("HingeRadau") != 0) &&
			(line[6].compare("HingeRadauTwo") != 0) &&
			(line[6].compare("UserHinge") != 0) &&
			(line[6].compare("DistHinge") != 0) &&
			(line[6].compare("RegularizedHinge") != 0) &&
			(line[6].compare("Trapezoidal") != 0) &&
			(line[6].compare("CompositeSimpson") != 0) &&
			(line[6].compare("FixedLocation") != 0) &&
			(line[6].compare("LowOrder") != 0) &&
			(line[6].compare("GaussQ") != 0) &&
			(line[6].compare("MidDistance") != 0))
		{

			nPnts = atoi(line[5].c_str());
		}
		else {
			nPnts = atoi(line[8].c_str());
		}
	}
	CSSElement* pEle = 0;
	if (type.compare("truss") == 0)
	{
		pEle = new CSSTruss(tag, nodeTags);
	}
	else if (type.compare("elasticBeamColumn") == 0)
	{
		pEle = new CSSElasticBeamColumn(tag, nodeTags, nPnts);

	}
	else if (type.compare("twoNodeLink") == 0)
	{
		pEle = new CSSTwoNodeLink(tag, nodeTags);

	}
	else if (type.compare("ModElasticBeam2d") == 0 || type.compare("ModElasticBeam3d") == 0)
	{
		pEle = new CSSModElasticBeamColumn(tag, nodeTags, nPnts);

	}
	else if (type.compare("dispBeamColumn") == 0)
	{
		pEle = new CSSDispBeamColumn(tag, nodeTags, nPnts);

	}
	else if (type.compare("forceBeamColumn") == 0 || type.compare("nonlinearBeamColumn") == 0)
	{
		pEle = new CSSForceBeamColumn(tag, nodeTags, nPnts);

	}
	else if (type.compare("zeroLength") == 0)
	{
		pEle = new CSSZeroLength(tag, nodeTags);

	}
	else if (type.compare("corotTruss") == 0)
	{
		pEle = new CSSCorotTruss(tag, nodeTags);
	}
	else if (type.compare("Joint2D") == 0)
	{
		pEle = new CSSJoint2dElement(tag, nodeTags);
	}
	else if (type.compare("20_8_BrickUP") == 0 || type.compare("SSPbrickUP") == 0
		|| type.compare("SSPbrick") == 0 || type.compare("stdBrick") == 0
		|| type.compare("bbarBrick") == 0 || type.compare("bbarBrickUP") == 0
		)
	{
		pEle = new CSSBrickElement(tag, nodeTags, type);
	}

	bool stat = pEle->updateGeometry(false);
	if (!stat || pEle->getIsNull())
	{
		acutPrintf(_T("Could not add element with tag %d to the model\n"), tag);
		delete pEle;
		return;
	}

	AcDbBlockTableRecord* pBTR = getModelSpace(OpenMode::kForWrite);
	assert(pBTR != NULL);
	ErrorStatus es = pBTR->appendAcDbEntity(id, pEle);
	if (es != eOk)
	{
		acutPrintf(_T("failed to add element with tag %d to BTR\n"), tag);
		return;
	}
	pEle->draw();
	pEle->close();
	ELEMATTAGMAP.insert(std::pair<int, AcDbObjectId>(tag, id));
}

void ObjUtils::addNodeRecorder(int* objTags, int* dofs, int num, std::string path, int* dataCols, bool hasTime)
{
	AcDbObjectId id;
	if (num == 1)
		if (getRecorder(&id, objTags[0], dofs[0], "disp"))
		{
			acutPrintf(_T("Recorder with objTag %d and dof %d already exists in model; ignoring new recorder"), objTags[0], dofs[0]);
			return;
		}
	AcDbDictionary* pNamedobj;
	AcDbDictionary* pDict = NULL;
	AcDbDatabase* pCurDwg = acdbHostApplicationServices()->workingDatabase();
	pCurDwg->getNamedObjectsDictionary(pNamedobj, AcDb::kForWrite);
	ErrorStatus es = pNamedobj->getAt(NODENTRYKEY, (AcDbObject*&)pDict, AcDb::kForWrite);
	if (es != Acad::eOk)
	{
		pDict = new AcDbDictionary;
		AcDbObjectId DictId;
		pNamedobj->setAt(NODENTRYKEY, pDict, DictId);
	}
	pNamedobj->close();
	for (int i = 0; i < num; i++)
	{
		CSSNodeRecorder* pRcrdr = new CSSNodeRecorder(objTags[i], dofs[i], path, dataCols[i], hasTime);
		AcString str;
		str.format(_T("%d"), pRcrdr->getRcrdrTag());
		pDict->setAt(str.kACharPtr(), pRcrdr, id);
		pRcrdr->close();
	}
	pDict->close();
}

void ObjUtils::GetAllNodes(std::vector<AcDbObjectId>& resIds)
{
	for (std::map<int, AcDbObjectId>::iterator iter = NODEATTAGMAP.begin(); iter != NODEATTAGMAP.end(); iter++)
	{
		resIds.push_back(iter->second);
	}
}

void ObjUtils::GetAllElements(std::vector<AcDbObjectId>& resIds)
{
	for (std::map<int, AcDbObjectId>::iterator iter = ELEMATTAGMAP.begin(); iter != ELEMATTAGMAP.end(); iter++)
	{
		resIds.push_back(iter->second);
	}
}

//AcDbObjectId ObjUtils::GetAnyNode()
//{
//	AcDbDatabase *pDb = acdbHostApplicationServices()->workingDatabase();
//	AcTransaction *pTrans = acTransactionManagerPtr()->startTransaction();
//	
//	//AcDbBlockTableRecordPointer pBTR(ACDB_MODEL_SPACE, pDb, OpenMode::kForRead);
//	//if (pBTR.openStatus() != eOk)
//	//{
//	//	acedAlert(_T("GetAllNodes:: error closing symbol table"));
//	//	return;
//	//}
//	AcDbBlockTable * pBT;
//	ErrorStatus es = pDb->getSymbolTable(pBT, kForRead);
//	if (es != eOk)
//	{
//		acedAlert(_T("GetAllNodes:: error getting symbol table"));
//		return AcDbObjectId::kNull;
//	}
//	AcDbBlockTableRecord *pBTR;
//	AcDbObjectId id;
//	es = pBT->getAt(ACDB_MODEL_SPACE, id);
//	if (es != eOk)
//	{
//		acedAlert(_T("GetAllNodes:: error getting model space ObjectId"));
//		return AcDbObjectId::kNull;
//	}
//	es = pBT->close();
//	if (es != eOk)
//	{
//		acedAlert(_T("GetAllNodes:: error closing symbol table"));
//		return AcDbObjectId::kNull;
//	}
//	es = pTrans->getObject((AcDbObject*&)pBTR, id, kForRead);
//	if (es != eOk)
//	{
//		acedAlert(_T("GetAllNodes:: error getting pBTR from transaction"));
//		return AcDbObjectId::kNull;
//	}
//	AcDbBlockTableRecordIterator *pIter;
//	es = pBTR->newIterator(pIter);
//	if (es != eOk)
//	{
//		acedAlert(_T("GetAllNodes:: error getting iterator"));
//		return AcDbObjectId::kNull;
//	}
//	AcDbEntity *pEnt;
//	CSSNode* pNode;
//	id = AcDbObjectId::kNull;
//	for(pIter->start(); !pIter->done(); pIter->step())
//	{
//		es = pIter->getEntity(pEnt, kForRead);
//		if (es != eOk)
//		{
//			/*acedAlert(_T("GetAllNodes:: error getting entity"));
//			return false;*/
//			continue;
//		}
//		pNode = CSSNode::cast(pEnt);
//		if (pNode == NULL)
//		{
//			es = pEnt->close();
//			if (es != eOk)
//			{
//				acedAlert(_T("GetAllNodes:: error closing entity"));
//				return id;
//			}
//			continue;
//		}
//		es = pNode->close();
//		if (es != eOk)
//		{
//			acedAlert(_T("GetAllNodes:: error closing panel"));
//			return id;
//		}
//		id = pNode->objectId();
//		break;
//	}
//	es = pBTR->close();
//	if (es != eOk)
//	{
//		acedAlert(_T("GetAllNodes:: error closing BTR"));
//		return AcDbObjectId::kNull;
//	}
//	delete pIter;
//	acTransactionManagerPtr()->abortTransaction();
//	return id;
//}

void ObjUtils::RedrawGraphics(bool redrawBody)
{
	RedrawNodeGraphics(redrawBody);
	RedrawElementsGraphics(redrawBody);
}

void ObjUtils::RedrawNodeGraphics(bool redrawBody)
{
	AcTransaction* pTrans = actrTransactionManager->startTransaction();
	std::vector<AcDbObjectId> resIds;
	GetAllNodes(resIds);
	ErrorStatus es;
	for (int i = 0; i < resIds.size(); i++)
	{
		AcDbObject* pObj;
		actrTransactionManager->getObject(pObj, resIds[i], kForWrite);
		CSSNode* pNode = CSSNode::cast(pObj);
		assert(pNode != NULL);
		if (redrawBody)
		{
			pNode->updateDeformedGeometry();
		}
		pNode->recordGraphicsModified();
		pNode->draw();
	}
	actrTransactionManager->endTransaction();
	DISPOPTIONS.nodeSizeChanged = false;
}

void ObjUtils::RedrawElementsGraphics(bool redrawBody)
{
	AcTransaction* pTrans = actrTransactionManager->startTransaction();
	std::vector<AcDbObjectId> resIds;
	GetAllElements(resIds);
	for (int i = 0; i < resIds.size(); i++)
	{
		AcDbObject* pObj;
		actrTransactionManager->getObject(pObj, resIds[i], kForWrite);
		assert(pObj != NULL);
		CSSElement* pEle = CSSElement::cast(pObj);
		assert(pEle != NULL);
		if (redrawBody)
		{
			pEle->updateGeometry(true);
		}
		pEle->recordGraphicsModified();
		pEle->draw();
	}
	actrTransactionManager->endTransaction();
}

bool ObjUtils::getShowDeformed()
{
	return DISPOPTIONS.dispDeformedShape;
}


double ObjUtils::getNodeSize()
{
	return DISPOPTIONS.nodeSize;
}

bool ObjUtils::getNode(AcDbObjectId& resId, int tag)
{
	std::map<int, AcDbObjectId>::iterator it = NODEATTAGMAP.find(tag);
	if (it == NODEATTAGMAP.end())
	{
		resId = NULL;
		return false;
	}
	resId = (it->second);
	return true;
}

bool ObjUtils::getElement(AcDbObjectId* pResId, int tag)
{
	std::map<int, AcDbObjectId>::iterator it = ELEMATTAGMAP.find(tag);
	if (it == ELEMATTAGMAP.end())
	{
		pResId = NULL;
		return false;
	}
	pResId = &(it->second);
	return true;
}

bool ObjUtils::getRecorder(AcDbObjectId* pResId, int obj_tag, int dof, const char* respType)
{
	AcDbDictionary* pNamedobj;
	AcDbDictionary* pDict = NULL;
	AcDbDatabase* pCurDwg = acdbHostApplicationServices()->workingDatabase();
	pCurDwg->getNamedObjectsDictionary(pNamedobj, AcDb::kForWrite);
	if (pNamedobj->getAt(NODENTRYKEY, (AcDbObject*&)pDict,
		AcDb::kForWrite) == Acad::eKeyNotFound)
	{
		pDict = new AcDbDictionary;
		AcDbObjectId DictId;
		pNamedobj->setAt(NODENTRYKEY, pDict, DictId);
	}
	pNamedobj->close();
	AcDbDictionaryIterator* pIter = pDict->newIterator();
	AcDbObject* pObj;
	CSSRecorder* pRcrdr;
	bool found = false;
	ErrorStatus es;
	for (; !pIter->done(); pIter->next())
	{
		es = pIter->getObject(pObj, AcDb::kForRead);
		if (es != eOk)
		{
			/*acedAlert(_T("getRecorder:: error getting entity"));
			return false;*/
			continue;
		}
		pRcrdr = CSSRecorder::cast(pObj);
		if (pRcrdr == NULL)
		{
			es = pObj->close();
			if (es != eOk)
			{
				acedAlert(_T("getRecorder:: error closing entity"));
				es = pDict->close();
				delete pIter;
				return false;
			}
			continue;
		}
		if (pRcrdr->getObjTag() != obj_tag)
		{
			es = pObj->close();
			if (es != eOk)
			{
				acedAlert(_T("getRecorder:: error closing entity"));
				es = pDict->close();
				delete pIter;
				return false;
			}
			continue;
		}
		if (pRcrdr->getDof() != dof)
		{
			es = pObj->close();
			if (es != eOk)
			{
				acedAlert(_T("getRecorder:: error closing entity"));
				es = pDict->close();
				delete pIter;
				return false;
			}
			continue;
		}

		// we should add check for respType in future
		*pResId = pIter->objectId();
		if (*pResId == AcDbObjectId::kNull)
		{
			acedAlert(_T("getRecorder:: error getting Id"));
			es = pDict->close();
			delete pIter;
			return false;
		}
		found = true;
		es = pRcrdr->close();
		if (es != eOk)
		{
			acedAlert(_T("getRecorder:: error closing panel"));
			es = pDict->close();
			delete pIter;
			return false;
		}
		break;
	}
	es = pDict->close();
	if (es != eOk)
	{
		acedAlert(_T("getRecorder:: error closing Dictionary"));
		delete pIter;
		return false;
	}
	delete pIter;
	return found;
}

void ObjUtils::GetAllRecorders(std::vector<AcDbObjectId>& resIds)
{
	AcDbDictionary* pNamedobj;
	AcDbDictionary* pDict = NULL;
	AcDbDatabase* pCurDwg = acdbHostApplicationServices()->workingDatabase();
	pCurDwg->getNamedObjectsDictionary(pNamedobj, AcDb::kForWrite);
	if (pNamedobj->getAt(NODENTRYKEY, (AcDbObject*&)pDict,
		AcDb::kForWrite) == Acad::eKeyNotFound)
	{
		pDict = new AcDbDictionary;
		AcDbObjectId DictId;
		pNamedobj->setAt(NODENTRYKEY, pDict, DictId);
	}
	pNamedobj->close();
	AcDbDictionaryIterator* pIter = pDict->newIterator();
	AcDbObject* pObj;
	CSSRecorder* pRcrdr;
	bool found = false;
	ErrorStatus es;
	for (; !pIter->done(); pIter->next())
	{
		es = pIter->getObject(pObj, AcDb::kForRead);
		if (es != eOk)
		{
			/*acedAlert(_T("getRecorder:: error getting entity"));
			return false;*/
			continue;
		}
		pRcrdr = CSSRecorder::cast(pObj);
		if (pRcrdr == NULL)
		{
			acutPrintf(_T("getAllRecorders:: pRcrdr is null"));
			continue;
		}
		es = pRcrdr->close();
		if (es != eOk)
		{
			acutPrintf(_T("getAllRecorders:: error closing pRcrdr"));
			continue;
		}
		// we should add check for respType in future
		resIds.push_back(pIter->objectId());
	}
	es = pDict->close();
	if (es != eOk)
	{
		acutPrintf(_T("getRecorder:: error closing Dictionary"));
	}
	delete pIter;
}

int ObjUtils::getNdm()
{
	return DOCDATA.NDM;
}

int ObjUtils::getNdof()
{
	return DOCDATA.NDOF;
}

void ObjUtils::setNdm(int val)
{
	//AcApDocument *pDoc = curDoc();
	DOCDATA.NDM = val;
	//DocVars.docData(curDoc()).NDM = val;
	//NDM = val;
}

void ObjUtils::setNdof(int val)
{
	DOCDATA.NDOF = val;
}

void ObjUtils::setShowDeformed(bool val)
{
	DISPOPTIONS.dispDeformedShape = val;
}

void ObjUtils::setShowUndeformedWire(bool val)
{
	DISPOPTIONS.dispUndeformedWire = val;
}

void ObjUtils::setShowNodeTags(bool val)
{
	DISPOPTIONS.dispNodeTags = val;
}

void ObjUtils::setShowEleTags(bool val)
{
	DISPOPTIONS.dispEleTags = val;
}

void ObjUtils::setTagsSize(double val)
{
	DISPOPTIONS.tagSize = val;
}

int ObjUtils::getNumNodesAtCrd(AcGePoint3d pnt)
{
	AcString crdStr;
	crdStr.format(_T("%.3f-%.3f-%.3f"), pnt.x, pnt.y, pnt.z);
	/*std::pair <std::multimap<AcString, int>::iterator, std::multimap<AcString, int>::iterator> ret;
	ret = NODEATCRDSMAP.equal_range(crdStr);
	for (std::multimap<AcString, int>::iterator it = ret.first; it != ret.second; ++it)*/
	int num = NODEATCRDSMAP.count(crdStr);
	return num;
}