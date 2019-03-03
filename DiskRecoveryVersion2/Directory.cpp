#include "Directory.h"

Directory::Directory()
{
	index = 0;			//�������±�һ��������ƥ�䲻���ļ���
	num = 0;		//��ʾ��ǰĿ¼ռ���ٸ�Ŀ¼��
	firstClusters = 0;	//��ʼ�غ�
	length = 0;			//�ļ�����
	memset(str, 0, sizeof(str));
}


Directory::~Directory()
{
}


int Directory::getI() {
	return num;
}

int Directory::getIndex() {
	return index;
}
void Directory::setIndex(int index) {
	this->index = index;
}

byte* Directory::getStr() {

	byte* result=new byte[SECTIONSIZE];
	memcpy(result, str, SECTIONSIZE);
	return result;
}
void Directory::setStr(byte* str1) {
	memcpy(str + num, str1, 32);
	num += 32;
}

DWORD Directory::getFirstClusters() {
	return firstClusters;
}
void Directory::setFirstClusters(DWORD num) {
	firstClusters = num;
}

DWORD Directory::getLength() {
	return length;
}
void Directory::setLength(DWORD num) {
	length = num;
}

string Directory::getName() {
	return name;
}
void Directory::setName(string name) {
	this->name = name;
}

unsigned int Directory::getType() {
	return type;
}
void Directory::setType(unsigned int type) {
	this->type = type;
}

string Directory::getCreateTime() {
	return createTime;
}
void Directory::setCreateTime(string time) {
	createTime.assign(time);
}

string Directory::getUpdateTime() {
	return updateTime;
}
void Directory::setUpdateTime(string time) {
	updateTime = time;
}

string Directory::getVisitTime() {
	return visitTime;
}
void Directory::setVisitTime(string time) {
	visitTime = time;
}


//�����ʱ���������ַ���16bit
//0~4bitΪ�룬5~10bit Ϊ�֣�11~15bit Ϊʱ
//�õ���ʱ��Ϊhh/mm/ss
string Directory::parseTime(unsigned char* time) {
	int second = (time[0] & 0b00011111) * 2;
	int min = (time[0] >> 5) + ((time[1] & 0b00000111) << 3);
	int hour = time[1] >> 3;
	if (second > 60 || min > 60 || hour > 24) {
		return "";
	}
	char okTime[9];
	sprintf_s(okTime, "%02d:%02d:%02d", hour, min, second);

	return okTime;
}

//�����������2���ַ���16bit
//0~4bit Ϊ��,5~8bit Ϊ��,9~15bit Ϊ��+1980
string Directory::parseDate(unsigned char* date) {
	int day = date[0] & 0b00011111;
	int month = (date[0] >> 5) + ((date[1] & 0b0001) << 1);
	int year = (date[1] >> 1) + 1980;
	if (year <= 1980 || month > 12 || day > 31) {
		return "";
	}
	char okDate[11];

	sprintf_s(okDate, "%04d/%02d/%02d", year, month, day);

	return okDate;
}


void Directory::setStrNULL() {
	memset(str, '\0', 512);
	num = 0;
}




//���������ļ������νṹ,ת���ļ���Ϣ
void traverseTree(PDirectoryStruct& PNode) {
	if (PNode->directoryType != 2) {
		getDetailMessage(PNode);
	}

	if (PNode->directoryType == 0) {
		return;
	}

	int num = PNode->childrens.size();
	for (int i = 0; i < num; i++) {
		PDirectoryStruct p = PNode->childrens.at(i);
		traverseTree(p);
	}
}


//�����ļ���Ŀ¼��16�����룬��ȡ�ļ���Ŀ¼����Ϣ
void getDetailMessage(PDirectoryStruct& PNode) {
	byte name[256];
	byte name1[12];
	byte* str;

	unsigned type;
	DWORD firstClusters;
	DWORD length;

	byte extractDT[2];
	string time;
	string date;

	int i, j, k1, k2, num;

	string okName;
	wchar_t* wname = NULL;
	string s;


	memset(name, 0, 256);
	memset(name1, 0, 12);
	str = PNode->directory->getStr();

	k1 = 0, k2 = 0;
	num = PNode->directory->getI() - 32;
	if (num > 0) {//����ǳ�Ŀ¼�ļ�����ȡ���ļ���
		for (i = num - 32; i >= 0; i -= 32) {
			for (j = 0x01; j <= 0x0a; j++) {//0x01~0x0a unicode
				if (str[i + j] == 0xff && str[i + j + 1] == 0xff && str[i + j + 2] == 0xff) {
					goto breakGo;
				}
				name[k1] = str[i + j];
				k1++;
			}
			for (j = 0x0e; j <= 0x19; j++) {//0x0e~0x19 unicode
				if (str[i + j] == 0xff && str[i + j + 1] == 0xff && str[i + j + 2] == 0xff) {
					goto breakGo;
				}
				name[k1] = str[i + j];
				k1++;
			}
			for (j = 0x1c; j <= 0x1f; j++) {//0x1c~0x1f unicode
				if (str[i + j] == 0xff && str[i + j + 1] == 0xff && str[i + j + 2] == 0xff) {
					goto breakGo;
				}
				name[k1] = str[i + j];
				k1++;
			}
		}
	breakGo:
		//���泤�ļ����ļ�
		charSaveWchar_t(wname, name, k1);
		okName = tcharToString(wname);
		PNode->directory->setName(okName);
		//cout << okName << endl;
	}
	k2 = 0;

	////////���Ƕ��ļ���ʱ����ȡ�ļ���������///////
	if (PNode->directory->getName().empty()) {

		for (i = 1; i < 8; i++, k2++) {
			if (i == 1 && str[num + i] > 127) {
				i += 1;
			}
			name1[k2] = str[num + i];
		}
		if (PNode->directoryType == 0) {
			name1[k2] = '.';
			k2++;

			for (i = 8; i < 11; i++, k2++) {
				name1[k2] = str[num + i];
			}
		}


		s = ucharToString(name1, k2, str[num + 0x0C]);
		myTrim(s);

		s = "~" + s;

		PNode->directory->setName(s);
		//cout << s << endl;
	}


	/////////////////��ȡ��������ʱ�䲢����//////////////
	extractDT[0] = str[num + 0x0e];
	extractDT[1] = str[num + 0x0f];
	time = Directory::parseTime(extractDT);

	extractDT[0] = str[num + 0x10];
	extractDT[1] = str[num + 0x11];
	date = Directory::parseDate(extractDT);
	date = date + " " + time;
	PNode->directory->setCreateTime(date);

	/////////////��ȡ����������//////////////
	extractDT[0] = str[num + 0x12];
	extractDT[1] = str[num + 0x13];
	date = Directory::parseDate(extractDT);
	PNode->directory->setVisitTime(date);

	////////////��ȡ����޸�����ʱ��//////////////
	extractDT[0] = str[num + 0x16];
	extractDT[1] = str[num + 0x17];
	time = Directory::parseTime(extractDT);

	extractDT[0] = str[num + 0x18];
	extractDT[1] = str[num + 0x19];
	date = Directory::parseDate(extractDT);
	date = date + " " + time;
	PNode->directory->setUpdateTime(date);

	///////////��ȡ�ļ�����////////////////
	type = str[num + 0x0b];
	PNode->directory->setType(type);

	/////////////��ȡ�ļ�����ʼ�غ�///////////
	firstClusters = str[num + 0x1a] + str[num + 0x1b] * 256 + str[num + 0x14] * 256 * 256 + str[num + 0x15] * 256 * 256 * 256;
	PNode->directory->setFirstClusters(firstClusters);

	////////////��ȡ�ļ��ĳ���//////////////
	length = str[num + 0x1c] + str[num + 0x1d] * 256 + str[num + 0x1e] * 256 * 256 + str[num + 0x1f] * 256 * 256 * 256;
	PNode->directory->setLength(length);

	delete str;
}


//�������νڵ���Ϣ����ӡ����
void traverseNode(PDirectoryStruct& PNode, string parentName, int* n, vector<string>& showDetails, string fileName) {
	string thisName;
	if (PNode->directoryType != 2) {
		if (PNode->directoryType == 1) {
			thisName = parentName + PNode->directory->getName().append("\\");
		}
		else {
			thisName = parentName + PNode->directory->getName();
		}
		string result;

		showDetails.push_back(thisName);

		cout << *n << "." << thisName << endl;
		(*n)++;

		disposeStr(thisName);
		replaceComma(thisName);

		result += thisName + ",";
		result = result + PNode->directory->getCreateTime() + ",";
		result = result + PNode->directory->getUpdateTime() + ",";
		result = result + PNode->directory->getVisitTime() + ",";
		if (PNode->directoryType == 0) {
			result = result + "�ļ�,";
		}
		else {
			result = result + "Ŀ¼,";
		}
		result = result + to_string(PNode->directory->getFirstClusters())+",";
		
		result = result + to_string(PNode->directory->getLength()) + ",";
		
		ofstream out;
		out.open(fileName, ios::out | ios::app);
		out << result << endl;
		out.close();

	}

	if (PNode->directoryType == 0) {
		return;
	}

	int num = PNode->childrens.size();
	for (int i = 0; i < num; i++) {
		PDirectoryStruct p = PNode->childrens.at(i);
		traverseNode(p, thisName, n, showDetails, fileName);
	}
}




//����Ҫ�ָ����ļ���Ŀ¼
BOOL saveRecovery(Drives drive, PDirectoryStruct PNode, string parentName) {



	if (_access(parentName.data(), 0) == -1) { //����ļ��в�����
		TCHAR* middle;
		charToWchar_t(middle, parentName.data(), parentName.size());
		if (CreateDirectory(middle, NULL) == 0) {
			cout << "�����ļ���ʧ��" << endl;
			return FALSE;
		}
	}

	if (PNode->directoryType == 2) {//����Ǹ�Ŀ¼
		for (int i = 0; i < PNode->childrens.size(); i++) {
			saveRecovery(drive, PNode->childrens.at(i), parentName);
		}
	}
	else if (PNode->directoryType == 1) {//�����Ŀ¼
		string name = PNode->directory->getName();
		name = parentName + "\\" + name;
		for (int i = 0; i < PNode->childrens.size(); i++) {
			saveRecovery(drive, PNode->childrens.at(i), name);
		}

	}
	else {
		string name = PNode->directory->getName();
		name = parentName + "\\" + name;
		DWORD firstClusters = PNode->directory->getFirstClusters();	//�ļ�����ʼ��
		DWORD startSectors = drive.getLocationByClus(firstClusters);	//�ļ�����ʼ����
		DWORD fileSectors[2];		//�ļ���ռ������С

		PNode->directory->getFileSize(fileSectors);

		isCreateFile(name);//�ж��ļ����Ƿ��Ѵ��ڣ�����������ں����_1
		unsigned char buffer[512];
		FILE* fp;
		int result = fopen_s(&fp, name.data(), "wb");
		if (fp == NULL) {
			cout << "���ļ�ʧ�ܣ�" << endl;
			return FALSE;
		}
		int i;
		for (i = 0; i < fileSectors[0]; i++) {
			drive.getDriveByN(buffer, startSectors + i);
			fwrite(buffer, 1, 512, fp);
			cout << i << endl;
		}
		drive.getDriveByN(buffer, startSectors + i);
		fwrite(buffer, 1, fileSectors[1], fp);

		fclose(fp);
	}
}


//�ж��ļ����Ƿ��Ѵ��ڣ�����������ں����_1
void isCreateFile(string& name) {
	if (!(_access(name.data(), 0) == -1)) {//����Ѵ�����ͬ�ļ���
		int x = name.find_first_of(".");
		name.insert(x, "_1");
		isCreateFile(name);
	}
}








