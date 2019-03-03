#include "Directory.h"

Directory::Directory()
{
	index = 0;			//向数组下标一样，用来匹配不是文件的
	num = 0;		//表示当前目录占多少个目录项
	firstClusters = 0;	//起始簇号
	length = 0;			//文件长度
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


//传入的时间是两个字符，16bit
//0~4bit为秒，5~10bit 为分，11~15bit 为时
//得到的时间为hh/mm/ss
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

//传入的日期是2个字符，16bit
//0~4bit 为日,5~8bit 为月,9~15bit 为年+1980
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




//遍历保存文件的树形结构,转换文件信息
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


//根据文件或目录的16进制码，读取文件或目录的信息
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
	if (num > 0) {//如果是长目录文件，提取长文件名
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
		//保存长文件名文件
		charSaveWchar_t(wname, name, k1);
		okName = tcharToString(wname);
		PNode->directory->setName(okName);
		//cout << okName << endl;
	}
	k2 = 0;

	////////当是短文件名时，提取文件名并保存///////
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


	/////////////////提取创建日期时间并保存//////////////
	extractDT[0] = str[num + 0x0e];
	extractDT[1] = str[num + 0x0f];
	time = Directory::parseTime(extractDT);

	extractDT[0] = str[num + 0x10];
	extractDT[1] = str[num + 0x11];
	date = Directory::parseDate(extractDT);
	date = date + " " + time;
	PNode->directory->setCreateTime(date);

	/////////////提取最后访问日期//////////////
	extractDT[0] = str[num + 0x12];
	extractDT[1] = str[num + 0x13];
	date = Directory::parseDate(extractDT);
	PNode->directory->setVisitTime(date);

	////////////提取最近修改日期时间//////////////
	extractDT[0] = str[num + 0x16];
	extractDT[1] = str[num + 0x17];
	time = Directory::parseTime(extractDT);

	extractDT[0] = str[num + 0x18];
	extractDT[1] = str[num + 0x19];
	date = Directory::parseDate(extractDT);
	date = date + " " + time;
	PNode->directory->setUpdateTime(date);

	///////////提取文件类型////////////////
	type = str[num + 0x0b];
	PNode->directory->setType(type);

	/////////////提取文件的起始簇号///////////
	firstClusters = str[num + 0x1a] + str[num + 0x1b] * 256 + str[num + 0x14] * 256 * 256 + str[num + 0x15] * 256 * 256 * 256;
	PNode->directory->setFirstClusters(firstClusters);

	////////////提取文件的长度//////////////
	length = str[num + 0x1c] + str[num + 0x1d] * 256 + str[num + 0x1e] * 256 * 256 + str[num + 0x1f] * 256 * 256 * 256;
	PNode->directory->setLength(length);

	delete str;
}


//遍历树形节点信息并打印出来
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
			result = result + "文件,";
		}
		else {
			result = result + "目录,";
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




//保存要恢复的文件或目录
BOOL saveRecovery(Drives drive, PDirectoryStruct PNode, string parentName) {



	if (_access(parentName.data(), 0) == -1) { //如果文件夹不存在
		TCHAR* middle;
		charToWchar_t(middle, parentName.data(), parentName.size());
		if (CreateDirectory(middle, NULL) == 0) {
			cout << "创建文件夹失败" << endl;
			return FALSE;
		}
	}

	if (PNode->directoryType == 2) {//如果是根目录
		for (int i = 0; i < PNode->childrens.size(); i++) {
			saveRecovery(drive, PNode->childrens.at(i), parentName);
		}
	}
	else if (PNode->directoryType == 1) {//如果是目录
		string name = PNode->directory->getName();
		name = parentName + "\\" + name;
		for (int i = 0; i < PNode->childrens.size(); i++) {
			saveRecovery(drive, PNode->childrens.at(i), name);
		}

	}
	else {
		string name = PNode->directory->getName();
		name = parentName + "\\" + name;
		DWORD firstClusters = PNode->directory->getFirstClusters();	//文件的起始簇
		DWORD startSectors = drive.getLocationByClus(firstClusters);	//文件的起始扇区
		DWORD fileSectors[2];		//文件所占扇区大小

		PNode->directory->getFileSize(fileSectors);

		isCreateFile(name);//判断文件名是否已存在，如果存在则在后面加_1
		unsigned char buffer[512];
		FILE* fp;
		int result = fopen_s(&fp, name.data(), "wb");
		if (fp == NULL) {
			cout << "打开文件失败！" << endl;
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


//判断文件名是否已存在，如果存在则在后面加_1
void isCreateFile(string& name) {
	if (!(_access(name.data(), 0) == -1)) {//如果已存在相同文件名
		int x = name.find_first_of(".");
		name.insert(x, "_1");
		isCreateFile(name);
	}
}








