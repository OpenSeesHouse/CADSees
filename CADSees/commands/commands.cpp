#include "stdafx.h"
#include "commands.h"

AcDbObjectId getModelSpaceId()
{
	 ErrorStatus es;
	 AcDbDatabase* pDwg = acdbHostApplicationServices()->workingDatabase();
	 if (NULL == pDwg) {
		  acedAlert(_T("getModelSpaceId::Could not get dataBase"));
		  return (NULL);
	 }

	 AcDbBlockTable* pBlockTable;
	 es = pDwg->getSymbolTable(pBlockTable, kForRead);
	 if (eOk != es) {
		  acedAlert(L"getModelSpaceId::Could not get block table");
		  return (NULL);
	 }

	 AcDbObjectId id;
	 es = pBlockTable->getAt(ACDB_MODEL_SPACE, id);
	 if (eOk != es) {
		  acedAlert(L"getModelSpaceId::Could not getAt()");
		  pBlockTable->close();
		  return (NULL);
	 }
	 es = pBlockTable->close();
	 if (es != eOk)
	 {
		  acedAlert(_T("getModelSpaceId:: error closing symbol table"));
		  return (NULL);
	 }
	 return id;
}

AcDbBlockTableRecord* getModelSpace(OpenMode mode)
{
	 ErrorStatus es;
	 AcDbBlockTableRecord* pModelSpace;
	 if (BTRID == NULL)
		  BTRID = getModelSpaceId();
	 es = actrTransactionManager->getObject((AcDbObject*&)pModelSpace, BTRID, mode);
	 if (es != eOk)
	 {
		  acedAlert(_T("getModelSpace:: error getting pBTR from transaction"));
		  return (NULL);
	 }
	 return (pModelSpace);
}

void setModel(std::vector<std::string> line)
{
	int ndm = 0, ndf = 0;
	for (int i = 0; i < line.size(); i++)
	{
		if (line[i].compare("-ndm") == 0)
		{
			i++;
			ndm = atoi(line[i].c_str());
			continue;
		}
		if (line[i].compare("-ndf") == 0)
		{
			i++;
			ndf = atoi(line[i].c_str());
		}
	}
	if (ndm == 0)
	{
		acedAlert(_T("Commands::setModel - error, ndm is not defined"));
	}
	if (ndf == 0)
	{
		switch(ndm)
		{
		case 1:
			ndf = 1;
			break;
		case 2:
			ndf = 3;
			break;
		case 3:
			ndf = 6;
			break;
		default:
			AcString msg;
			msg.format(_T("Commands::setModel - not recognized ndm: %d"), ndm);
			acedAlert(msg.kACharPtr());
		}
	}
	DOCDATA->setNdm(ndm);
	DOCDATA->setNdof(ndf);
}

void addNode(std::vector<std::string> line)
{
	int tag = atoi(line[1].c_str());
	AcGePoint3d pnt(0.,0.,0.);
	for (int i = 2; i < line.size(); i++)
	{
		double crd = atof(line[i].c_str());
		if (i  == 2)
			pnt.x = crd;
		else if(i == 3)
			pnt.y = crd;
		else if(i == 4)
			pnt.z = crd;
	}
	ObjUtils::addNode(tag, pnt);
}

void addElement(std::vector<std::string> line)
{
	std::vector<int> eleNumNodes;
	std::vector<std::string> suprtdElems = CSSElement::getSupportedEleList(eleNumNodes);
	bool suprtd = false;
	int i = 0;
	for (; i < suprtdElems.size(); i++)
	{
		if (line[1].compare(suprtdElems[i]) == 0)
		{
			suprtd = true;
			break;
		}
	}
	if (! suprtd)
	{
		acutPrintf(_T("Element Type: %s is not supported;\n"), AcString(line[1].c_str()).kACharPtr());
		acutPrintf(_T("For a list of supported elements please visit %s\n"), WEBADDRESS);
		return;
	}
	int numNodes = eleNumNodes[i];
	std::vector<int> nodeTags;
	nodeTags.reserve(numNodes);
	int tag = atoi(line[2].c_str());
	for (i = 3; i < 3+numNodes; i++)
		 nodeTags.push_back(atoi(line[i].c_str()));
	int npnts = 0;
	ObjUtils::addElement(line[1], tag, nodeTags, line);
}

void addRecorder(std::vector<std::string> line)
{
	if (line[1].compare("Node") == 0)
	{
		std::string resp = line[line.size()-1];
		if (resp.compare("disp") != 0)
			return;
		std::string path;
		bool hasTime = false;
		std::vector<int> tagList;
		std::vector<int> dofList;
		for (int i = 2; i < line.size()-1; i++)
		{
			if (line[i].compare("-file") == 0)
			{
				i++;
				path = line[i];
				continue;
			}
			if (line[i].compare("-time") == 0)
			{
				hasTime = true;
				continue;
			}
			if (line[i].compare("-node") == 0)
			{
				for (int j = i+1; j < line.size(); j++)
				{
					if (line[j].substr(0, 1) == "-")
						break;
					i++;
					tagList.push_back(atoi(line[j].c_str()));
				}
				continue;
			}
			if (line[i].compare("-dof") == 0)
			{
				for (int j = i+1; j < line.size(); j++)
				{
					if (line[j].substr(0, 1) == "-")
						break;
					int dof = atoi(line[j].c_str());
					if (dof == 0)
						break;
					dofList.push_back(dof);
					i++;
				}
			}
		}
		int num = dofList.size() * tagList.size();
		int* tags = new int[num];
		int* dofs = new int[num];
		int* colIds = new int[num];
		int colId = hasTime? 1: 0;
		int k = 0;
		for (int i = 0; i < tagList.size(); i++)
		{
			for (int j = 0; j < dofList.size(); j++)
			{
				dofs[k] = dofList[j];
				tags[k] = tagList[i];
				colIds[k] = colId++;
				k++;
			}
		}
		ObjUtils::addNodeRecorder(tags, dofs, k - 1, path, colIds, hasTime);
		delete[] tags;
		delete[] dofs;
		delete[] colIds;
	}
}

void ReadResponse(std::string folder)
{
	acutPrintf(_T("\nReading input files may take a while ... Please wait\n"));
	std::vector<AcDbObjectId> resIds;
	ObjUtils::GetAllRecorders(resIds);
	ErrorStatus es;
	actrTransactionManager->startTransaction();
	for (int i = 0; i < resIds.size(); i++)
	{
		AcDbObject* pObj = 0;
		CSSRecorder* pRcrdr;
		es = actrTransactionManager->getObject(pObj, resIds[i], kForWrite);
		if (es != eOk)
		{
			acutPrintf(_T("Commands::ReadRecorders - error getting object"));
			continue;
		}
		pRcrdr = CSSRecorder::cast(pObj);
		if (pRcrdr == nullptr)
		{
			acutPrintf(_T("Commands::ReadRecorders - pRcrdr is null"));
			continue;
		}
		if (!pRcrdr->recordResponse(folder))
		{
			return;
		}
		pRcrdr->applySelf(-1., 1);
	}
	actrTransactionManager->endTransaction();
	DOCDATA->dispDeformedShape(true, true);
}


double SetDeformedState(int n, double fac)
{
	std::vector<AcDbObjectId> resIds;
	ObjUtils::GetAllRecorders(resIds);
	ErrorStatus es;
	actrTransactionManager->startTransaction();
	for (int i = 0; i < resIds.size(); i++)
	{
		AcDbObject* pObj = 0;
		CSSRecorder* pRcrdr;
		es = actrTransactionManager->getObject(pObj, resIds[i], kForWrite);
		if (es != eOk)
		{
			acutPrintf(_T("SetDeformedState - error getting object"));
			actrTransactionManager->abortTransaction();
			return false;
		}
		pRcrdr = CSSRecorder::cast(pObj);
		if (pRcrdr == nullptr)
		{
			acutPrintf(_T("Commands::ReadRecorders - pRcrdr is null"));
			continue;
		}
		if (!pRcrdr->applySelf(n, fac))
		{
			 actrTransactionManager->abortTransaction();
			 return false;
		}
	}
	actrTransactionManager->endTransaction();
	DOCDATA->dispDeformedShape(true, true);
	Vector timeVec = CSSRecorder::getTimeVec();
	if (timeVec.Size() <= n)
		return 0;
	//acDocManager->sendStringToExecute(curDoc(), _T("REGEN "), false, false, false);
	return timeVec(n);
}

void addPile(std::vector<std::string> line)
{
	 AcGePoint3d pt1, pt2;
	 pt1.x = atof(line[1].c_str());
	 pt1.y = atof(line[2].c_str());
	 pt1.z = atof(line[3].c_str());
	 pt2.x = atof(line[4].c_str());
	 pt2.y = atof(line[5].c_str());
	 pt2.z = atof(line[6].c_str());
	 ObjUtils::addPile(pt1, pt2);
}

void addCube(std::vector<std::string> line)
{
	 AcGePoint3d pt1;
	 pt1.x = atof(line[1].c_str());
	 pt1.y = atof(line[2].c_str());
	 pt1.z = atof(line[3].c_str());
	 double szx = atof(line[4].c_str());
	 double szy = atof(line[5].c_str());
	 double szz = atof(line[6].c_str());
	 ObjUtils::addCube(pt1, szx, szy, szz);
}
