#include "stdafx.h"
#include "commands.h"
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
	ObjUtils::setNdm(ndm);
	ObjUtils::setNdof(ndf);
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

double addElement(std::vector<std::string> line)
{
	std::vector<std::string> suprtdElems = CSSElement::getSupportedEleList();
	bool suprtd = false;
	for (int i = 0; i < suprtdElems.size(); i++)
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
		return 0;
	}
	int tag = atoi(line[2].c_str());
	int iNode = atoi(line[3].c_str());
	int jNode = atoi(line[4].c_str());
	int npnts = 0;
	if (line[1].compare("dispBeamColumn") == 0 ||
		line[1].compare("forceBeamColumn") == 0 ||
		line[1].compare("nonlinearBeamColumn") == 0)
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

			npnts = atoi(line[5].c_str());
		} else {
			npnts = atoi(line[8].c_str());
		}
	}
	double l = ObjUtils::addElement(line[1], tag, iNode, jNode, npnts);
	return l;
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
		for (int i = 2; i < line.size(); i++)
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
			int ;
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
	for (int i = 0; i < resIds.size(); i++)
	{
		AcDbObject* pObj = 0;
		CSSRecorder* pRcrdr;
		es = acdbOpenObject(pObj, resIds[i], kForWrite);
		if (es != eOk)
		{
			acutPrintf(_T("Commands::ReadRecorders - error getting object"));
			continue;
		}
		pRcrdr = CSSRecorder::cast(pObj);
		if (pRcrdr == nullptr)
		{
			acutPrintf(_T("Commands::ReadRecorders - pRcrdr is null"));
			pRcrdr->close();
			continue;
		}
		if (!pRcrdr->recordResponse(folder))
		{
			pRcrdr->close();
			return;
		}
		pRcrdr->applySelf(-1.);
		pRcrdr->close();
	}
	DISPOPTIONS.dispDeformedShape = true;
	ObjUtils::setShowDeformed(true);
	ObjUtils::RedrawGraphics(true);
}


double SetDeformedState(int n)
{
	std::vector<AcDbObjectId> resIds;
	ObjUtils::GetAllRecorders(resIds);
	ErrorStatus es;
	for (int i = 0; i < resIds.size(); i++)
	{
		AcDbObject* pObj = 0;
		CSSRecorder* pRcrdr;
		es = acdbOpenObject(pObj, resIds[i], kForWrite);
		if (es != eOk)
		{
			acutPrintf(_T("Commands::ReadRecorders - error getting object"));
			continue;
		}
		pRcrdr = CSSRecorder::cast(pObj);
		if (pRcrdr == nullptr)
		{
			acutPrintf(_T("Commands::ReadRecorders - pRcrdr is null"));
			pRcrdr->close();
			continue;
		}
		if (! pRcrdr->applySelf(n))
			return false;
		pRcrdr->close();
	}
	DISPOPTIONS.dispDeformedShape = true;
	ObjUtils::setShowDeformed(true);
	ObjUtils::RedrawGraphics(true);
	VECTYPE *timeVec = CSSRecorder::getTimeVec();
	if (timeVec == 0)
		return 0;
	if (timeVec->size() <= n)
		return 0;
	//acDocManager->sendStringToExecute(curDoc(), _T("REGEN "), false, false, false);
	return timeVec->at(n);
}