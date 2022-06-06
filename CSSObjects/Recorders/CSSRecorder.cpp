// (C) Copyright 2002-2007 by Autodesk, Inc. 
//
// Permission to use, copy, modify, and distribute this software in
// object code form for any purpose and without fee is hereby granted, 
// provided that the above copyright notice appears in all copies and 
// that both that copyright notice and the limited warranty and
// restricted rights notice below appear in all supporting 
// documentation.
//
// AUTODESK PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF
// MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK, INC. 
// DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//
// Use, duplication, or disclosure by the U.S. Government is subject to 
// restrictions set forth in FAR 52.227-19 (Commercial Computer
// Software - Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
// (Rights in Technical Data and Computer Software), as applicable.
//

//-----------------------------------------------------------------------------
//----- CSSRecorder.cpp : Implementation of CSSRecorder
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "CSSRecorder.h"
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/file.hpp>
#include <string>

int CSSRecorder::lastTag = 0;
Vector CSSRecorder::timeVec(0);
std::map<AcString, Matrix*> CSSRecorder::m_fileMap;
//-----------------------------------------------------------------------------
Adesk::UInt32 CSSRecorder::kCurrentVersionNumber =1 ;

//-----------------------------------------------------------------------------
ACRX_DXF_DEFINE_MEMBERS (
	CSSRecorder, AcDbObject,
	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent, 
	AcDbProxyEntity::kNoOperation, CSSRECORDER,
CADSees
|Product Desc:     An OpenSees pre/post-processor
|Company:          Civil Soft Science
|WEB Address:      www.CivilSoftScience.com
)

//-----------------------------------------------------------------------------
CSSRecorder::CSSRecorder () : AcDbObject () {
	m_respVec = 0;
}

CSSRecorder::CSSRecorder(int objTag, int dof, std::string path, int dataCol, bool hasTime) : AcDbObject (),
	m_objTag(objTag), m_rcrdrTag(++lastTag), m_dof(dof), m_relFilePath(AcString(path.c_str(),AcString::Encoding::Utf8)), m_dataColId(dataCol), m_hasTime(hasTime), m_respVec(0)
{
	
}

CSSRecorder::~CSSRecorder () {
}

//-----------------------------------------------------------------------------
//----- AcDbObject protocols
//- Dwg Filing protocol
Acad::ErrorStatus CSSRecorder::dwgOutFields (AcDbDwgFiler *pFiler) const {
	assertReadEnabled () ;
	//----- Save parent class information first.
	Acad::ErrorStatus es =AcDbObject::dwgOutFields (pFiler) ;
	if ( es != Acad::eOk )
		return (es) ;
	//----- Object version number needs to be saved first
	if ( (es =pFiler->writeUInt32 (CSSRecorder::kCurrentVersionNumber)) != Acad::eOk )
		return (es) ;
	//----- Output params
	if ( (es =pFiler->writeItem (m_objTag)) != Acad::eOk )
		return (es) ;
	if ( (es =pFiler->writeItem (m_rcrdrTag)) != Acad::eOk )
		return (es) ;
	if ( (es =pFiler->writeItem (m_dof)) != Acad::eOk )
		return (es) ;
	if ( (es =pFiler->writeItem (m_relFilePath.kACharPtr())) != Acad::eOk )
		return (es) ;
	if ( (es =pFiler->writeItem (m_dataColId)) != Acad::eOk )
		return (es) ;
	if ( (es =pFiler->writeItem (m_hasTime)) != Acad::eOk )
		return (es) ;
	return (pFiler->filerStatus ()) ;
}

Acad::ErrorStatus CSSRecorder::dwgInFields (AcDbDwgFiler *pFiler) {
	assertWriteEnabled () ;
	//----- Read parent class information first.
	Acad::ErrorStatus es =AcDbObject::dwgInFields (pFiler) ;
	if ( es != Acad::eOk )
		return (es) ;
	//----- Object version number needs to be read first
	Adesk::UInt32 version =0 ;
	if ( (es =pFiler->readUInt32 (&version)) != Acad::eOk )
		return (es) ;
	if ( version > CSSRecorder::kCurrentVersionNumber )
		return (Acad::eMakeMeProxy) ;
	//- Uncomment the 2 following lines if your current object implementation cannot
	//- support previous version of that object.
	//if ( version < CSSRecorder::kCurrentVersionNumber )
	//	return (Acad::eMakeMeProxy) ;
	//----- Read params
	if ( (es =pFiler->readItem(&m_objTag)) != Acad::eOk )
		return (es) ;
	if ( (es =pFiler->readItem(&m_rcrdrTag)) != Acad::eOk )
		return (es) ;
	if ( (es =pFiler->readItem(&m_dof)) != Acad::eOk )
		return (es) ;
	if ( (es =pFiler->readString(m_relFilePath)) != Acad::eOk )
		return (es) ;
	if ( (es =pFiler->readItem(&m_dataColId)) != Acad::eOk )
		return (es) ;
	if ( (es =pFiler->readItem(&m_hasTime)) != Acad::eOk )
		return (es) ;
	return (pFiler->filerStatus ()) ;
}

bool CSSRecorder::readFileData(AcString filePath, std::string folder, bool hasTime)
{
	std::string file = folder + "/";
	file +=  filePath.utf8Ptr();
	boost::iostreams::stream<boost::iostreams::file_source> from(file.c_str());
	//std::ifstream from(file.c_str());
	if (from.bad())
	{
		acutPrintf(L"CSSRecorder::Failed to open data file: %s",
			AcString(file.c_str(), AcString::Encoding::Utf8).kACharPtr());
		return false;
	}
	from.seekg (0, from.end);
    int length = from.tellg();
    from.seekg (0, from.beg);
	if (length < 1)
	{
		acutPrintf(L"CSSRecorder::Failed to open data file: %s",
			AcString(file.c_str(), AcString::Encoding::Utf8).kACharPtr());
		from.close();
		return false;
	}

    char * buffer = new char [length];

    from.read (buffer,length);
	/*int read = from.gcount();
    if (read < length)
	{
		acutPrintf(_T("CSSRecorder::recordResponse:failed to read file completely"));
	}*/
    from.close();
	std::vector<std::string> allLines = ObjUtils::pars(buffer, "\n");
    delete[] buffer;
	int nLines = allLines.size();
	if (allLines[nLines-1].length() == 0)
	{
		allLines.erase(allLines.end()-1);
		nLines --;
	}
	std::string line;
	line = allLines[0];
	std::vector<std::string> words = ObjUtils::pars(line, " ");
	int nCols = words.size();
	m_fileMap[filePath] = new Matrix(nLines, nCols);
	Matrix& theMatrix = *m_fileMap[filePath];

	_CRT_DOUBLE  val;
	for (int i = 0; i < nLines; i++)
	{
		line = allLines[i];
		words.clear();
		words = ObjUtils::pars(line, " ");
		if (words.size() < nCols)
			continue;
		for (int j = 0; j < words.size(); j++)
		{
			
			int retCode = _atodbl(&val, (char*)words[j].data());
			if (retCode !=  0)
			{
				acutPrintf(L"CSSRecorder::Failed to convert %s value to double",
					AcString(words[j].c_str(), AcString::Encoding::Utf8).kACharPtr());
				return false;
			}
			theMatrix(j,i) = val.x;
		}
	}
	if (hasTime && timeVec.Size() == 0)
	{
		timeVec = theMatrix[0];
	}
	return true;
}

AcString CSSRecorder::getRelFilePath() const
{
	assertReadEnabled();
	return m_relFilePath;
}

int CSSRecorder::getFileColId() const
{
	assertReadEnabled();
	return m_dataColId;
}

int CSSRecorder::getRcrdrTag() const
{
	assertReadEnabled();
	return m_rcrdrTag;
}

int CSSRecorder::getDof() const
{
	assertReadEnabled();
	return m_dof;
}

int CSSRecorder::getObjTag() const
{
	assertReadEnabled();
	return m_objTag;
}

bool CSSRecorder::getHasTime() const
{
	assertReadEnabled();
	return m_hasTime;
}

Vector& CSSRecorder::getTimeVec()
{
	return timeVec;
}

int CSSRecorder::getLastTag()
{
	return lastTag;
}

void CSSRecorder::setDof(int val)
{
	assertWriteEnabled(false, true);
	m_dof = val;
}

void CSSRecorder::setHasTime(bool val)
{
	assertWriteEnabled(false, true);
	m_hasTime = val;
}

void CSSRecorder::setPath(std::string path)
{
	assertWriteEnabled(false, true);
	m_relFilePath = AcString(path.c_str(), AcString::Encoding::Utf8);
}

void CSSRecorder::setDataCol(int dataCol)
{
	assertWriteEnabled(false, true);
	m_dataColId = dataCol;
}

bool CSSRecorder::applySelf(double t, double fac)
{
	return false;
}

bool CSSRecorder::applySelf(int nStep, double fac)
{
	return false;
}

bool CSSRecorder::recordResponse(std::string folder)
{
	if (m_fileMap.find(m_relFilePath) == m_fileMap.end())
		if (! readFileData(m_relFilePath, folder, m_hasTime))
			return false;
	Matrix* mat = m_fileMap[m_relFilePath];
	m_respVec = (*mat)[m_dataColId];
	return true;
}

void CSSRecorder::setLastTag(int val)
{
	lastTag = val;
}