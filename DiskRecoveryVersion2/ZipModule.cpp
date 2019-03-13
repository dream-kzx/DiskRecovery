#include "ZipModule.h"



ZipModule::ZipModule()
{

}


ZipModule::~ZipModule()
{
}



//byte* ZipModule::getZipHead() {
//	return zipHead;
//}
//void ZipModule::setZipHead(byte* zipHead) {
//	try {
//		for (int i = 0; i < 30; i++) {
//			zipHead[i] = zipHead[i];
//		}
//	}
//	catch (range_error e) {
//		cout << "��ʼ��zipHead���鳬��!" << endl;
//	}
//}

string ZipModule::getLastDateTime() {
	return this->lastDateTime;
}


int ZipModule::getCompressSize() {
	return this->compressSize;
}


int ZipModule::getUnCompressSize() {
	return this->unCompressSize;
}


int ZipModule::getFileNameLength() {
	return this->fileNameLength;
}


int ZipModule::getExtraLength() {
	return this->extraLength;
}


string ZipModule::getFileName() {
	return this->fileName;
}

int ZipModule::getZipSize()
{
	return this->zipSize;
}


DWORD ZipModule::getLocation() {
	return this->location;
}
void ZipModule::setLocation(DWORD location) {
	this->location = location;
}



//long compressSize = 0;//18~21bit	ѹ�����С	
//long unCompressSize = 0;//22~25bit δѹ����С
//int fileNameLength = 0;//26~27bit �ļ�������
//int extraLength = 0;//28~29bit ��չ������
//string fileName = NULL;//30~(330+fileNameLength-1) �ļ���
//long zipSize = 0;
bool ZipModule::parseZip(byte* zipHead) {
	//��������޸�����
	//byte date[2];
	//date[0] = zipHead[10];
	//date[1] = zipHead[11];
	//this->lastDate = parseDate(date);
	////��������޸�ʱ��
	//byte time[2];
	//time[0] = zipHead[12];
	//time[1] = zipHead[13];
	//this->lastTime = parseTime(time);
	byte dateTime[4];
	dateTime[0] = zipHead[10];
	dateTime[1] = zipHead[11];
	dateTime[2] = zipHead[12];
	dateTime[3] = zipHead[13];
	this->lastDateTime = parseDateTime(dateTime);

	//����δѹ����С
	byte lengthStr[4];
	lengthStr[0] = zipHead[18];
	lengthStr[1] = zipHead[19];
	lengthStr[2] = zipHead[20];
	lengthStr[3] = zipHead[21];
	this->compressSize = parseLength4(lengthStr);
	if (this->compressSize < 0) {
		return false;
	}
	//����ѹ�����С
	lengthStr[0] = zipHead[22];
	lengthStr[1] = zipHead[23];
	lengthStr[2] = zipHead[24];
	lengthStr[3] = zipHead[25];
	this->unCompressSize = parseLength4(lengthStr);

	//�����ļ�����С
	lengthStr[0] = zipHead[26];
	lengthStr[1] = zipHead[27];
	this->fileNameLength = parseLength2(lengthStr);
	
	if (this->fileNameLength > 200) {
		std::cout << "                                           " << this->fileNameLength << endl;
		return false;
	}

	//������չ����С
	lengthStr[0] = zipHead[28];
	lengthStr[1] = zipHead[29];
	this->extraLength = parseLength2(lengthStr);

	//���zip�ļ���С
	this->zipSize = this->compressSize + 30 + this->fileNameLength + this->extraLength;

	return true;

}

//����zip�ļ��ļ���
void ZipModule::parseZipName(byte* zipHead) {
	//�����ļ���
	string fileName = "";
	//bool real = false;//�ж��Ƿ���utf-8�����
	for (int i = 0; i < this->fileNameLength; i++) {
		/*if ((zipHead[30+i] > 127 || zipHead[30 + i] < 32)&& real!=true) {
			real = true;
		}*/
		fileName += zipHead[30 + i];
	}
	string temp;
	if (utf8_check(fileName.c_str(),this->fileNameLength)) {
		temp = UTF_82ASCII(fileName);
		
	}
	myTrim(temp, "\0");

	this->fileName = temp;
}

//int ZipModule::isZipIdentify(byte* identify) {
//
//	try {
//		if ((this->identifyHead[0] == identify[0]) && (this->identifyHead[1] == identify[1]) &&
//			(this->identifyHead[2] == identify[2]) && (this->identifyHead[3] == identify[3])) {
//			return 1;
//		}
//		else if ((this->identifyCore[0] == identify[0]) && (this->identifyCore[1] == identify[1]) &&
//			(this->identifyCore[2] == identify[2]) && (this->identifyCore[3] == identify[3])) {
//			return 2;
//		}
//		else if ((this->identifyEnd[0] == identify[0]) && (this->identifyEnd[1] == identify[1]) &&
//			(this->identifyEnd[2] == identify[2]) && (this->identifyEnd[3] == identify[3])) {
//			return 3;
//		}
//		else {
//			return -1;
//		}
//	}
//	catch (exception e) {
//		cout << "���鳬�磡" << endl;
//		return -1;
//	}
//
//}
//
//byte* ZipModule::getIdentifyHead() {
//	return this->identifyHead;
//}


//byte* ZipModule::getZipData() {
//	return this->zipData;
//}
//void ZipModule::setZipData(byte* zipData) {
//	this->zipData = new byte[this->zipSize];
//	for (int i = 0; i < this->zipSize; i++) {
//		this->zipData[i] = zipData[i];
//	}
//}



