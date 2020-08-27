#include "stdafx.h"


std::vector<std::string> ObjUtils::pars(std::string str, const char* delimiter)
{
	typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
	boost::char_separator<char> sep(delimiter,"", boost::drop_empty_tokens);
	tokenizer tokens(str, sep);
	tokenizer::iterator tok_iter = tokens.begin();
	std::vector<std::string> res;
	int sz = tok_iter->size();
	res.reserve(sz);
	while (!tok_iter.at_end())
		res.push_back(*tok_iter++);
	return res;
}

AcDbBlockTableRecord* ObjUtils::getModelSpace(OpenMode mode)
{
    ErrorStatus es ; // Real type is Acad::ErrorStatus
    AcDbBlockTableRecord* pModelSpace ;

    // Get hold of the drawing...
    AcDbDatabase* pDwg ;
    pDwg = acdbHostApplicationServices()->workingDatabase() ;
    if (NULL == pDwg) {
        acedAlert (_T("Could not get dataBase")) ;
        return (NULL) ;
    }

    // Get hold of the block table...
    AcDbBlockTable* pBlockTable ;
    es = pDwg->getSymbolTable (pBlockTable,kForRead) ;
    if (eOk != es) { // Real value is Acad::eOk
        // Error opening the block table, report it
        acedAlert (L"Could not get block table") ;
        return (NULL) ;
    }

    // Get hold of the model space block...
    es = pBlockTable->getAt (ACDB_MODEL_SPACE,pModelSpace, mode);
    if (eOk != es) { // Real value is Acad::eOk
        // Error opening the block table, report it
        acedAlert (L"Could not getAt()") ;
        pBlockTable->close();
        return (NULL) ;
    }

    // Close the block table, we have no more use for it...
    pBlockTable->close();

    return (pModelSpace) ;
}


void ObjUtils::setNodesSize(double val)
{
	DISPOPTIONS.nodeSize = val;
	DISPOPTIONS.nodeSizeChanged = true;
}

void ObjUtils::addNode(int tag, AcGePoint3d pnt)
{
	AcDbObjectId id;
	if (getNode(&id, tag))
	{
		acutPrintf(_T("Node with tag %d already exists in model"), tag);
		return;
	}
	int num = getNodesAtCrd(pnt);
	CSSNode* pNode = new CSSNode(tag, DOCDATA.NDOF, pnt, num);
	AcDbBlockTableRecord* pBTR = getModelSpace(OpenMode::kForWrite);
	assert(pBTR != NULL);
	pBTR->appendAcDbEntity(id, pNode);
	pNode->close();
	pBTR->close();
}

double ObjUtils::addElement(std::string type, int tag, int iNode, int jNode, int nPnts)
{
	AcDbObjectId id;
	if (getElement(&id, tag))
	{
		acutPrintf(_T("Element with tag %d already exists in model"), tag);
		return 0;
	}
	CSSElement* pEle = 0;
	if (type.compare("truss") == 0)
	{
		pEle = new CSSTruss(tag, iNode, jNode);
	} else if(type.compare("elasticBeamColumn") == 0)
	{
		pEle =new CSSElasticBeamColumn(tag, iNode, jNode, nPnts);
		
	} else if(type.compare("ModElasticBeam2d") == 0 || type.compare("ModElasticBeam3d") == 0)
	{
		pEle =new CSSModElasticBeamColumn(tag, iNode, jNode, nPnts);
		
	} else if(type.compare("dispBeamColumn") == 0)
	{
		pEle =new CSSDispBeamColumn(tag, iNode, jNode, nPnts);
		
	} else if(type.compare("forceBeamColumn") == 0 || type.compare("nonlinearBeamColumn") == 0)
	{
		pEle =new CSSForceBeamColumn(tag, iNode, jNode, nPnts);
		
	} else if(type.compare("zeroLength") == 0)
	{
		pEle =new CSSZeroLength(tag, iNode, jNode);
		
	} else if(type.compare("corotTruss") == 0)
	{
		pEle = new CSSCorotTruss(tag, iNode, jNode);
	}

	pEle->updateGeometry(false);
	if(pEle->getIsNull())
	{
		acutPrintf(_T("Could not add element with tag %d to the model"), tag);
		delete pEle;
		return 0;
	}
	double l = pEle->getLength();

	AcDbBlockTableRecord* pBTR = getModelSpace(OpenMode::kForWrite);
	assert(pBTR != NULL);
	pBTR->appendAcDbEntity(id, pEle);
	pEle->close();
	pBTR->close();
	return l;
}

void ObjUtils::addNodeRecorder(int* objTags, int* dofs, int num, std::string path, int* dataCols, bool hasTime)
{
	AcDbObjectId id;
	if (num == 1)
		if (getRecorder(&id, objTags[0], dofs[0], "disp"))
		{
			acutPrintf(_T("Element with objTag %d and dof %d already exists in model; ignoring new recorder"), objTags[0], dofs[0]);
			return;
		}
	AcDbDictionary *pNamedobj;
    AcDbDictionary *pDict = NULL;
	AcDbDatabase *pCurDwg = acdbHostApplicationServices()->workingDatabase();
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
	AcDbDatabase *pDb = acdbHostApplicationServices()->workingDatabase();
	AcTransaction *pTrans = acTransactionManagerPtr()->startTransaction();
	
	//AcDbBlockTableRecordPointer pBTR(ACDB_MODEL_SPACE, pDb, OpenMode::kForRead);
	//if (pBTR.openStatus() != eOk)
	//{
	//	acedAlert(_T("GetAllNodes:: error closing symbol table"));
	//	return;
	//}
	AcDbBlockTable * pBT;
	ErrorStatus es = pDb->getSymbolTable(pBT, kForRead);
	if (es != eOk)
	{
		acedAlert(_T("GetAllNodes:: error getting symbol table"));
		return;
	}
	AcDbBlockTableRecord *pBTR;
	AcDbObjectId id;
	es = pBT->getAt(ACDB_MODEL_SPACE, id);
	if (es != eOk)
	{
		acedAlert(_T("GetAllNodes:: error getting model space ObjectId"));
		return;
	}
	es = pBT->close();
	if (es != eOk)
	{
		acedAlert(_T("GetAllNodes:: error closing symbol table"));
		return;
	}
	es = pTrans->getObject((AcDbObject*&)pBTR, id, kForRead);
	if (es != eOk)
	{
		acedAlert(_T("GetAllNodes:: error getting pBTR from transaction"));
		return;
	}
	AcDbBlockTableRecordIterator *pIter;
	es = pBTR->newIterator(pIter);
	if (es != eOk)
	{
		acedAlert(_T("GetAllNodes:: error getting iterator"));
		return;
	}
	AcDbEntity *pEnt;
	CSSNode* pNode;
	for(pIter->start(); !pIter->done(); pIter->step())
	{
		es = pIter->getEntity(pEnt, kForRead);
		if (es != eOk)
		{
			/*acedAlert(_T("GetAllNodes:: error getting entity"));
			return false;*/
			continue;
		}
		pNode = CSSNode::cast(pEnt);
		if (pNode == NULL)
		{
			es = pEnt->close();
			if (es != eOk)
			{
				acedAlert(_T("GetAllNodes:: error closing entity"));
				return ;
			}
			continue;
		}
		es = pNode->close();
		if (es != eOk)
		{
			acedAlert(_T("GetAllNodes:: error closing panel"));
			return;
		}
		resIds.push_back(pNode->objectId());
	}
	es = pBTR->close();
	if (es != eOk)
	{
		acedAlert(_T("GetAllNodes:: error closing BTR"));
		return;
	}
	delete pIter;
	acTransactionManagerPtr()->endTransaction();
}

void ObjUtils::GetAllElements(std::vector<AcDbObjectId>& resids)
{
	AcDbDatabase *pDb = acdbHostApplicationServices()->workingDatabase();
	AcTransaction *pTrans = acTransactionManagerPtr()->startTransaction();
	
	//AcDbBlockTableRecordPointer pBTR(ACDB_MODEL_SPACE, pDb, OpenMode::kForRead);
	//if (pBTR.openStatus() != eOk)
	//{
	//	acedAlert(_T("GetAllElements:: error closing symbol table"));
	//	return;
	//}
	AcDbBlockTable * pBT;
	ErrorStatus es = pDb->getSymbolTable(pBT, kForRead);
	if (es != eOk)
	{
		acedAlert(_T("GetAllElements:: error getting symbol table"));
		return;
	}
	AcDbBlockTableRecord *pBTR;
	AcDbObjectId id;
	es = pBT->getAt(ACDB_MODEL_SPACE, id);
	if (es != eOk)
	{
		acedAlert(_T("GetAllElements:: error getting model space ObjectId"));
		return;
	}
	es = pBT->close();
	if (es != eOk)
	{
		acedAlert(_T("GetAllElements:: error closing symbol table"));
		return;
	}
	es = pTrans->getObject((AcDbObject*&)pBTR, id, kForRead);
	if (es != eOk)
	{
		acedAlert(_T("GetAllElements:: error getting pBTR from transaction"));
		return;
	}
	AcDbBlockTableRecordIterator *pIter;
	es = pBTR->newIterator(pIter);
	if (es != eOk)
	{
		acedAlert(_T("GetAllElements:: error getting iterator"));
		return;
	}
	AcDbEntity *pEnt;
	CSSLineElement* pEle;
	bool found = false;
	for(pIter->start(); !pIter->done(); pIter->step())
	{
		es = pIter->getEntity(pEnt, kForRead);
		if (es != eOk)
		{
			/*acedAlert(_T("GetAllElements:: error getting entity"));
			return;*/
			continue;
		}
		pEle = CSSLineElement::cast(pEnt);
		if (pEle == NULL)
		{
			es = pEnt->close();
			if (es != eOk)
			{
				acedAlert(_T("GetAllElements:: error closing entity"));
				return;
			}
			continue;
		}
		resids.push_back(pEle->objectId());
		es = pEle->close();
		if (es != eOk)
		{
			acedAlert(_T("GetAllElements:: error closing panel"));
			return;
		}
	}
	es = pBTR->close();
	if (es != eOk)
	{
		acedAlert(_T("GetAllElements:: error closing BTR"));
		return;
	}
	delete pIter;
	acTransactionManagerPtr()->endTransaction();
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
	std::vector<AcDbObjectId> resIds;
	GetAllNodes(resIds);
	ErrorStatus es;
	for (int i = 0; i < resIds.size(); i++)
	{
		AcDbObject* pObj;
		es = acdbOpenObject(pObj, resIds[i], kForWrite);
		assert(pObj != NULL);
		CSSNode* pNode = CSSNode::cast(pObj);
		assert(pNode != NULL);
		if (redrawBody)
		{
			pNode->updateDeformedGeometry();
		}
		pNode->recordGraphicsModified();
		pNode->draw();
		pNode->close();
	}
	DISPOPTIONS.nodeSizeChanged = false;
}

void ObjUtils::RedrawElementsGraphics(bool redrawBody)
{
	std::vector<AcDbObjectId> resIds;
	GetAllElements(resIds);
	for (int i = 0; i < resIds.size(); i++)
	{
		AcDbObject* pObj;
		ErrorStatus es = acdbOpenObject(pObj, resIds[i], kForWrite);
		assert(pObj != NULL);
		CSSLineElement* pEle = CSSLineElement::cast(pObj);
		assert(pEle != NULL);
		if (redrawBody)
		{
			pEle->updateGeometry(true);
		}
		pEle->recordGraphicsModified();
		pEle->draw();
		pEle->close();
	}
}

bool ObjUtils::getShowDeformed()
{
	return DISPOPTIONS.dispDeformedShape;
}


double ObjUtils::getNodeSize()
{
	return DISPOPTIONS.nodeSize;
}

bool ObjUtils::getNode(AcDbObjectId* pResId,int tag)
{
	AcDbDatabase *pDb = acdbHostApplicationServices()->workingDatabase();
	AcTransaction *pTrans = acTransactionManagerPtr()->startTransaction();
	
	//AcDbBlockTableRecordPointer pBTR(ACDB_MODEL_SPACE, pDb, OpenMode::kForRead);
	//if (pBTR.openStatus() != eOk)
	//{
	//	acedAlert(_T("getNode:: error closing symbol table"));
	//	return false;
	//}
	AcDbBlockTable * pBT;
	ErrorStatus es = pDb->getSymbolTable(pBT, kForRead);
	if (es != eOk)
	{
		acedAlert(_T("getNode:: error getting symbol table"));
		return false;
	}
	AcDbBlockTableRecord *pBTR;
	AcDbObjectId id;
	es = pBT->getAt(ACDB_MODEL_SPACE, id);
	if (es != eOk)
	{
		acedAlert(_T("getNode:: error getting model space ObjectId"));
		return false;
	}
	es = pBT->close();
	if (es != eOk)
	{
		acedAlert(_T("getNode:: error closing symbol table"));
		return false;
	}
	es = pTrans->getObject((AcDbObject*&)pBTR, id, kForRead);
	if (es != eOk)
	{
		acedAlert(_T("getNode:: error getting pBTR from transaction"));
		return false;
	}
	AcDbBlockTableRecordIterator *pIter;
	es = pBTR->newIterator(pIter);
	if (es != eOk)
	{
		acedAlert(_T("getNode:: error getting iterator"));
		return false;
	}
	AcDbEntity *pEnt;
	CSSNode* pNode;
	bool found = false;
	for(pIter->start(); !pIter->done(); pIter->step())
	{
		es = pIter->getEntity(pEnt, kForRead);
		if (es != eOk)
		{
			/*acedAlert(_T("getNode:: error getting entity"));
			return false;*/
			continue;
		}
		pNode = CSSNode::cast(pEnt);
		if (pNode == NULL)
		{
			es = pEnt->close();
			if (es != eOk)
			{
				acedAlert(_T("getNode:: error closing entity"));
				return false;
			}
			continue;
		}
		if (pNode->getTag() != tag)
		{
			es = pEnt->close();
			if (es != eOk)
			{
				acedAlert(_T("getNode:: error closing entity"));
				return false;
			}
			continue;
		}
		es = pIter->getEntityId(*pResId);
		if (es != eOk)
		{
			acedAlert(_T("getNode:: error getting Id"));
			return false;
		}
		found = true;
		es = pNode->close();
		if (es != eOk)
		{
			acedAlert(_T("getNode:: error closing panel"));
			return false;
		}
		break;
	}
	es = pBTR->close();
	if (es != eOk)
	{
		acedAlert(_T("getNode:: error closing BTR"));
		return false;
	}
	delete pIter;
	acTransactionManagerPtr()->endTransaction();
	return found;
}

bool ObjUtils::getElement(AcDbObjectId* pResId,int tag)
{
	AcDbDatabase *pDb = acdbHostApplicationServices()->workingDatabase();
	AcTransaction *pTrans = acTransactionManagerPtr()->startTransaction();
	
	//AcDbBlockTableRecordPointer pBTR(ACDB_MODEL_SPACE, pDb, OpenMode::kForRead);
	//if (pBTR.openStatus() != eOk)
	//{
	//	acedAlert(_T("getElement:: error closing symbol table"));
	//	return false;
	//}
	AcDbBlockTable * pBT;
	ErrorStatus es = pDb->getSymbolTable(pBT, kForRead);
	if (es != eOk)
	{
		acedAlert(_T("getElement:: error getting symbol table"));
		return false;
	}
	AcDbBlockTableRecord *pBTR;
	AcDbObjectId id;
	es = pBT->getAt(ACDB_MODEL_SPACE, id);
	if (es != eOk)
	{
		acedAlert(_T("getElement:: error getting model space ObjectId"));
		return false;
	}
	es = pBT->close();
	if (es != eOk)
	{
		acedAlert(_T("getElement:: error closing symbol table"));
		return false;
	}
	es = pTrans->getObject((AcDbObject*&)pBTR, id, kForRead);
	if (es != eOk)
	{
		acedAlert(_T("getElement:: error getting pBTR from transaction"));
		return false;
	}
	AcDbBlockTableRecordIterator *pIter;
	es = pBTR->newIterator(pIter);
	es = pBTR->newIterator(pIter);
	if (es != eOk)
	{
		acedAlert(_T("getElement:: error getting iterator"));
		return false;
	}
	AcDbEntity *pEnt;
	CSSLineElement* pEle;
	bool found = false;
	for(pIter->start(); !pIter->done(); pIter->step())
	{
		es = pIter->getEntity(pEnt, kForRead);
		if (es != eOk)
		{
			/*acedAlert(_T("getElement:: error getting entity"));
			return false;*/
			continue;
		}
		pEle = CSSLineElement::cast(pEnt);
		if (pEle == NULL)
		{
			es = pEnt->close();
			if (es != eOk)
			{
				acedAlert(_T("getElement:: error closing entity"));
				return false;
			}
			continue;
		}
		if (pEle->getTag() != tag)
		{
			es = pEnt->close();
			if (es != eOk)
			{
				acedAlert(_T("getElement:: error closing entity"));
				return false;
			}
			continue;
		}
		es = pIter->getEntityId(*pResId);
		if (es != eOk)
		{
			acedAlert(_T("getElement:: error getting Id"));
			return false;
		}
		found = true;
		es = pEle->close();
		if (es != eOk)
		{
			acedAlert(_T("getElement:: error closing panel"));
			return false;
		}
		break;
	}
	es = pBTR->close();
	if (es != eOk)
	{
		acedAlert(_T("getElement:: error closing BTR"));
		return false;
	}
	delete pIter;
	acTransactionManagerPtr()->endTransaction();
	return found;
}

bool ObjUtils::getRecorder(AcDbObjectId* pResId, int obj_tag, int dof, const char* respType)
{
	AcDbDictionary *pNamedobj;
    AcDbDictionary *pDict = NULL;
	AcDbDatabase *pCurDwg = acdbHostApplicationServices()->workingDatabase();
	pCurDwg->getNamedObjectsDictionary(pNamedobj, AcDb::kForWrite);
	if (pNamedobj->getAt(NODENTRYKEY, (AcDbObject*&) pDict,
        AcDb::kForWrite) == Acad::eKeyNotFound)
    {
        pDict = new AcDbDictionary;
        AcDbObjectId DictId;
        pNamedobj->setAt(NODENTRYKEY, pDict, DictId);
    }
    pNamedobj->close();
	AcDbDictionaryIterator* pIter = pDict->newIterator();
	AcDbObject *pObj;
	CSSRecorder* pRcrdr;
	bool found = false;
	ErrorStatus es;
	for(; !pIter->done(); pIter->next())
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
	AcDbDictionary *pNamedobj;
    AcDbDictionary *pDict = NULL;
	AcDbDatabase *pCurDwg = acdbHostApplicationServices()->workingDatabase();
	pCurDwg->getNamedObjectsDictionary(pNamedobj, AcDb::kForWrite);
	if (pNamedobj->getAt(NODENTRYKEY, (AcDbObject*&) pDict,
        AcDb::kForWrite) == Acad::eKeyNotFound)
    {
        pDict = new AcDbDictionary;
        AcDbObjectId DictId;
        pNamedobj->setAt(NODENTRYKEY, pDict, DictId);
    }
    pNamedobj->close();
	AcDbDictionaryIterator* pIter = pDict->newIterator();
	AcDbObject *pObj;
	CSSRecorder* pRcrdr;
	bool found = false;
	ErrorStatus es;
	for(; !pIter->done(); pIter->next())
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

int ObjUtils::getNodesAtCrd(AcGePoint3d pnt)
{
	std::vector<AcDbObjectId> resIds;
	GetAllNodes(resIds);
	AcTransaction *pTr = acTransactionManagerPtr()->startTransaction();
	CSSNode *pNode;
	ErrorStatus es;
	int num = 0;
	for (int i = 0; i < resIds.size(); i++)
	{
		es = pTr->getObject((AcDbObject*&)pNode, resIds[i], OpenMode::kForRead);
		if (es != eOk)
		{
			acedAlert(_T("ObjUtils::getNodesAtCrd- error opening node"));
			return 0;
		}
		pNode = CSSNode::cast(pNode);
		if (pNode == 0)
		{
			acedAlert(_T("ObjUtils::getNodesAtCrd- pNode is null"));
			return 0;
		}
		if ((pNode->getCrds()-pnt).length() < 1.e-5)
			num++;
	}
	acTransactionManagerPtr()->abortTransaction();
	return num;
}