#include "recoveryTwo.h"

//第一步扫描所有目录，将已存在目录结构的簇进行标识，表示不匹配该簇



////////////////////////////////////////////////////第二步开始匹配文件/////////////////////////////////////
BOOL matchFile(Drives drive, vector<PDirectoryStruct>& directoryHead) {
	clustersSum = drive.getTotalClusters();
	byte* clusterIdentity = new byte[clustersSum+2];//标识簇

	//每个簇字节数
	DWORD byteClusters = drive.getSectPerClust()*drive.getBytesPerSect();
	
	for (int i = 0; i < clustersSum+2; i++) {
		clusterIdentity[i] = 0;
	}

	byte* buffer = new byte[SECTIONSIZE];//一个扇区的大小
	byte directoryStr[32];	//定义一个目录项，每个目录项占32个字节
	DWORD directoryRootLocation;	//根目录所占首扇区的位置
	directoryRootLocation = drive.getFirstSectByClust();//获取根目录所占的起始扇区

	for (int i = 0; i < 2; i++) {
		clusterIdentity[i] = 1;
	}

	///////////////////////////遍历目录结构对正在使用的簇进行标识/////////////////////////////////////////////
	int number = 0;//number用来遍历根目录的扇区，number代表第几个扇区
	while (true) {
		//读取根目录的第number个扇区的数据
		cout << directoryRootLocation + number << endl;
		if (!drive.getDriveByN(buffer, directoryRootLocation + number)) {
			cout << "读取磁盘有误！" << endl;
			return NULL;
		}

		if (readDirectoryEnd(buffer)) {//表示读取根目录结束
			break;
		}

		//匹配一个扇区中所有带有标志的目录项，每个目录项为32个字节
		int i = 0;
		if (number == 0) {
			i = 128;
		}
		for (; i < SECTIONSIZE; i += 32) {
			memmove(directoryStr, buffer + i, sizeof(directoryStr));
			if (readDirectoryEnd(directoryStr)) {//读取当前扇区结束
				break;
			}

		
			//如果文件是目录
			if ((directoryStr[0x0B] == 0x10) && (directoryStr[0] != 0xE5)) {

				//获取这个目录项的起始簇
				DWORD firstClusters = directoryStr[0x1a] + directoryStr[0x1b] * 256
					+ directoryStr[0x14] * 256 * 256 + directoryStr[0x15] * 256 * 256 * 256;


				//如果起始簇小于分区的总簇
				if (firstClusters <= drive.getTotalClusters())
				{
					clusterIdentity[firstClusters] = 1;
					queryChildrenDirectory(drive, drive.getLocationByClus(firstClusters), clusterIdentity, 0);
				}
			}
			else {//如果是文件
				if (((int)directoryStr[0] != 0xE5) && ((int)directoryStr[0x0B] != 0x0F)) {
					/////////////提取文件的起始簇号///////////
					DWORD firstClusters = directoryStr[0x1a] + directoryStr[0x1b] * 256 + directoryStr[0x14] * 256 * 256
						+ directoryStr[0x15] * 256 * 256 * 256;


					////////////提取文件的长度//////////////
					DWORD length = directoryStr[0x1c] + directoryStr[0x1d] * 256 + directoryStr[0x1e] * 256 * 256
						+ directoryStr[0x1f] * 256 * 256 * 256;



					//计算文件所占簇的个数
					int num = length / byteClusters;
					if (length % byteClusters > 0) {
						num++;
					}

					for (int k = firstClusters; k < firstClusters + num; k++) {
						clusterIdentity[k] = 1;
					}


				}

				
			}

		}
		if (i < SECTIONSIZE) {
			break;
		}

		number++;
	}

	delete buffer;
	//for (int i = 0; i < clustersSum; i++) {
	//	if (clusterIdentity[i] == 1) {
	//		cout << i << endl;
	//	}
	//}

	int readEach = 2000;//每次读取磁盘读取多少个簇

	buffer = new byte[byteClusters * readEach];
	

	int directoryNum = 0;

	for (DWORD i = 2; i < clustersSum + 2; i = i + readEach) {
		cout << i << endl;

		int realReadNum = readEach;//真正要读取的扇区数

		if (i + readEach < clustersSum + 2) {//如果不是最后一次余数的扇区
			realReadNum = readEach;
			if (!drive.getClustersByN(drive, buffer, i, realReadNum)) {
				cout << "读取磁盘有误！" << endl;
				return FALSE;
			}
		}
		else {
			realReadNum = clustersSum + 2 - i;
			if (!drive.getClustersByN(drive, buffer, i, realReadNum)) {
				cout << "读取磁盘有误！" << endl;
				return FALSE;
			}
		}

		//for (int k = 0; k < realReadNum * byteClusters; k++) {
		//
		//	cout <<setiosflags(ios::uppercase) <<setfill('0') << setw(2)<<hex<< int(buffer[k]);
		//}
		

		for (int id = 0; id < realReadNum; id++) {
			BOOL isDirectory = TRUE;
			for (int j = 0; j <= 32; j = j + 32) {
				if (j == 0) {
					if (!((buffer[0 + id * byteClusters] == 0x2e) && (buffer[1 + id * byteClusters] == 0x20))) {
						isDirectory = FALSE;
						break;
					}
				}
				if (j == 32) {
					if (!((buffer[j + 0 + id * byteClusters] == 0x2e) && (buffer[j + 1 + id * byteClusters] == 0x2e)
						&& (buffer[j + 2 + id * byteClusters] == 0x20))) {

						isDirectory = FALSE;
						break;
					}
				}
			}

			if (isDirectory == TRUE) {//如果是目录
				cout << "This is directory" << endl;

				if (clusterIdentity[i + id] != 1) {//如果这个簇不是已经标识的簇
					clusterIdentity[i + id] = 1;

					DWORD okSect = drive.getLocationByClus(i + id); //获取该目录所在的起始扇区

					if ((okSect < drive.getSectorsSum()) && (okSect >= drive.getFirstSectByClust())) {
						PDirectoryStruct p = new DirectoryStruct();
						p->directory = new Directory();
						

						if(disposeDirectoryInTwo(drive, p, okSect, clusterIdentity, 0)>0){
							char nameBuff[10];
							sprintf_s(nameBuff, "%d", directoryNum);
							string name = "目录" + string(nameBuff);
							p->directory->setName(name);
							p->directoryType = 1;
							p->directory->setFirstClusters(i + id);

							directoryHead.push_back(p);
							directoryNum++;
						}
						else {
							delete p->directory;
							delete p;
						}
						
					}
				}

			}
		}
	}

	delete clusterIdentity;
	delete buffer;

	for (int i = 0; i < directoryHead.size(); i++) {
		PDirectoryStruct p = directoryHead.at(i);
		for (int j = 0; j < p->childrens.size(); j++) {
			traverseTree(p->childrens.at(j));
		}
	}

	return TRUE;
}

//遍历已存在的子目录进行簇标识
void queryChildrenDirectory(Drives drive, DWORD directoryLocation, byte* clusterIdentity, int n) {

	if ((n > 20) || (directoryLocation >= drive.getSectorsSum())) {
		return;
	}

	byte* buffer = new byte[SECTIONSIZE];//一个扇区的大小
	byte directoryStr[32];	//定义一个目录项，每个目录项占32个字节
	//每个簇字节数
	DWORD byteClusters = drive.getSectPerClust()*drive.getBytesPerSect();

	////////////////////////匹配目录中所有有删除标志的目录项//////////////////////////////////////
	int number = 0;//number用来遍历根目录的扇区，number代表第几个扇区
	while (true) {
		//读取根目录的第number个扇区的数据
		if (!drive.getDriveByN(buffer, directoryLocation + number)) {
			cout << "读取磁盘有误！" << endl;
			return;
		}

		if (readDirectoryEnd(buffer)) {//表示读取根目录结束
			break;
		}

		//匹配一个扇区中所有的目录项，每个目录项为32个字节
		int i = 0;
		for (; i < SECTIONSIZE; i += 32) {
			memmove(directoryStr, buffer + i, sizeof(directoryStr));
			if (readDirectoryEnd(directoryStr)) {//读取当前扇区结束
				break;
			}

			//如果目录不是根目录，且目录没有上级目录和本目录则返回
			if (drive.getFirstSectByClust() != directoryLocation) {
				if (number == 0 && i == 0 && directoryStr[0] != 0x2e && directoryStr[1] != 0x20)
				{
					return;
				}

				if (number == 0 && i == 32 && directoryStr[0] != 0x2e && directoryStr[1] != 0x2e && directoryStr[2] != 0x20)
				{
					return;
				}
			}
			else {
				return;
			}

			//如果是上级目录，或本目录跳过循环
			if ((directoryStr[0] == 0x2e && directoryStr[1] == 0x20) ||
				(directoryStr[0] == 0x2e && directoryStr[1] == 0x2e && directoryStr[2] == 0x20)) {
				continue;
			}

			//如果文件是目录
			if ((directoryStr[0x0B] == 0x10) && (directoryStr[0] != 0xE5)) {

				//获取这个目录项的起始簇
				DWORD firstClusters = directoryStr[0x1a] + directoryStr[0x1b] * 256
					+ directoryStr[0x14] * 256 * 256 + directoryStr[0x15] * 256 * 256 * 256;

				//如果起始簇小于分区的总簇
				if (firstClusters < drive.getTotalClusters()+2 )
				{
					clusterIdentity[firstClusters] = 1;
					queryChildrenDirectory(drive, drive.getLocationByClus(firstClusters), clusterIdentity, n+1);
				}
			}
			else {//如果是文件
				if (((int)directoryStr[0] != 0xE5) && (directoryStr[0x0B] != 0x0F)) {
					/////////////提取文件的起始簇号///////////
					DWORD firstClusters = directoryStr[0x1a] + directoryStr[0x1b] * 256 + directoryStr[0x14] * 256 * 256
						+ directoryStr[0x15] * 256 * 256 * 256;

					//如果起始簇小于分区的总簇
					if (firstClusters < drive.getTotalClusters() + 2)
					{
						////////////提取文件的长度//////////////
						DWORD length = directoryStr[0x1c] + directoryStr[0x1d] * 256 + directoryStr[0x1e] * 256 * 256
							+ directoryStr[0x1f] * 256 * 256 * 256;
						//计算文件所占簇的个数
						int num = length / byteClusters;
						if (length % byteClusters > 0) {
							num++;
						}

						for (int k = firstClusters; k < firstClusters + num; k++) {
							clusterIdentity[k] = 1;
						}
					}
				}

			}

		}
		if (i < SECTIONSIZE) {
			break;
		}

		number++;
	}
	delete buffer;

}


//处理子目录
int disposeDirectoryInTwo(Drives drive, PDirectoryStruct& parent, DWORD directoryLocation, byte* clusterIdentity, int n) {
	int num = 0;

	cout << "             " << directoryLocation << endl;
	if ( n > 10 ) {
		return num;
	}

	byte buffer[SECTIONSIZE];
	byte directoryStr[32];	//定义一个目录项，每个目录项占32个字节

	PDirectoryStruct p = new DirectoryStruct();
	p->directory = new Directory();

	////////////////////////匹配目录中所有有删除标志的目录项//////////////////////////////////////
	int number = 0;
	while (1) {
		if (!drive.getDriveByN(buffer, directoryLocation + number)) {
			cout << "读取磁盘有误！" << endl;
			return num;
		}

		if (readDirectoryEnd(buffer)) {//表示读取本目录结束
			break;
		}

		int i = 0;
		for (; i < SECTIONSIZE; i += 32) {//匹配一个扇区中所有带有标志的目录项
			memmove(directoryStr, buffer + i, sizeof(directoryStr));
			if (readDirectoryEnd(directoryStr)) {//读取当前扇区结束
				break;
			}

			//如果目录不是根目录，且目录没有上级目录和本目录则返回
			if (drive.getFirstSectByClust() != directoryLocation) {
				if (number == 0 && i == 0 && directoryStr[0] != 0x2e && directoryStr[1] != 0x20)
				{
					return num;
				}

				if (number == 0 && i == 32 && directoryStr[0] != 0x2e && directoryStr[1] != 0x2e && directoryStr[2] != 0x20)
				{
					return num;
				}
			}
			else {
				return num;
			}

			//如果是上级目录，或本目录跳过循环
			if ((directoryStr[0] == 0x2e && directoryStr[1] == 0x20) ||
				(directoryStr[0] == 0x2e && directoryStr[1] == 0x2e && directoryStr[2] == 0x20)) {
				continue;
			}

			//如果文件是目录
			if (directoryStr[0x0B] == 0x10) {
				p->directory->setStr(directoryStr);
				p->directoryType = 1;


				//获取这个目录项的起始簇
				DWORD firstClusters = directoryStr[0x1a] + directoryStr[0x1b] * 256
					+ directoryStr[0x14] * 256 * 256 + directoryStr[0x15] * 256 * 256 * 256;

				
				if (clusterIdentity[firstClusters] != 1) {//如果这个簇不是已经标识的簇
					clusterIdentity[firstClusters] = 1;

					DWORD okSect = drive.getLocationByClus(firstClusters);//获取该目录所在的起始扇区

					if ((okSect < drive.getSectorsSum()) && (okSect >= drive.getFirstSectByClust())) {
						//int result = disposeDirectory(drive, p, drive.getLocationByClus(firstClusters), 0);//最后一个参数用来记录递归的次数
						int result= disposeDirectoryInTwo(drive, p, okSect, clusterIdentity, n+1);
						if (result == 0) {
							delete p;
						}
						else {
							parent->childrens.push_back(p);
							num += result;
						}
					}
					else {
						delete p;
					}

						
					
				}

				PDirectoryStruct q = new DirectoryStruct();
				q->directory = new Directory();

				p = q;

			}
			else {
				p->directory->setStr(directoryStr);
				if ((int)directoryStr[0x0B] != 0x0F) {//如果不是长目录文件的文件名，
					p->directoryType = 0;
					num++;

					parent->childrens.push_back(p);
					PDirectoryStruct q = new DirectoryStruct();
					q->directory = new Directory();
					p = q;

				}
			}
		}
		if (i < SECTIONSIZE) {
			break;
		}
		number++;
	}


	return num;
}


//设置每个文件的信息
void eachDirectory(Directory* p) {

	string result = "";

	/////////////////////获取每个目录项的信息///////////////////////////////////
	unsigned char name[256];
	unsigned char name1[12];
	unsigned char* str;

	unsigned type;
	DWORD firstClusters;
	DWORD length;

	unsigned char extractDT[2];
	string time;
	string date;

	int i, j, k1, k2, num1;

	string okName;
	wchar_t* wname = NULL;
	string s;

	memset(name, 0, 256);
	memset(name1, 0, 12);
	str = p->getStr();

	k1 = 0, k2 = 0;
	num1 = p->getI() - 32;
	if (num1 > 0) {//如果是长目录文件，提取长文件名
		for (i = num1 - 32; i >= 0; i -= 32) {
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
		replaceComma(okName);
		result = result + okName + ",";
		p->setName(okName);
		//cout << okName << endl;
	}
	k2 = 0;

	////////当是短文件名时，提取文件名并保存///////
	if (p->getName().empty()) {

		for (i = 1; i < 8; i++, k2++) {
			if (i == 1 && str[num1 + i] > 127) {
				i += 1;
			}
			name1[k2] = str[num1 + i];
		}
		if (str[num1 + 0x0b] != 0x10) {
			name1[k2] = '.';
			k2++;
		}
		for (i = 8; i < 11; i++, k2++) {
			name1[k2] = str[num1 + i];
		}
		s = ucharToString(name1, sizeof(name1));
		myTrim(s);
		disposeStr(s);
		replaceComma(s);
		s = "？" + s;
		p->setName(s);
		result = result + s + ",";
		//cout << s << endl;
	}

	string change = "";//暂时保存其他类型转为string的内容

	/////////////////提取创建日期时间并保存//////////////
	extractDT[0] = str[num1 + 0x0e];
	extractDT[1] = str[num1 + 0x0f];
	time = Directory::parseTime(extractDT);
	if (time.empty()) {
		return;
	}

	extractDT[0] = str[num1 + 0x10];
	extractDT[1] = str[num1 + 0x11];
	date = Directory::parseDate(extractDT);
	date = date + " " + time;
	if (date.empty()) {
		return;
	}
	result = result + date + ",";
	//p->directory->setCreateTime(date);
	//p->directory->setCreateTime("11111");

	/////////////提取最后访问日期//////////////
	extractDT[0] = str[num1 + 0x12];
	extractDT[1] = str[num1 + 0x13];
	date = Directory::parseDate(extractDT);
	if (date.empty()) {
		return;
	}
	result = result + date + ",";

	////////////提取最近修改日期时间//////////////
	extractDT[0] = str[num1 + 0x16];
	extractDT[1] = str[num1 + 0x17];
	time = Directory::parseTime(extractDT);
	if (time.empty()) {
		return;
	}


	extractDT[0] = str[num1 + 0x18];
	extractDT[1] = str[num1 + 0x19];
	date = Directory::parseDate(extractDT);
	date = date + " " + time;
	if (date.empty()) {
		return;
	}
	result = result + date + ",";

	///////////提取文件类型////////////////
	type = str[num1 + 0x0b];
	change = to_string(type);
	if (change.empty()) {
		return;
	}
	result = result + change + ",";

	/////////////提取文件的起始簇号///////////
	firstClusters = str[num1 + 0x1a] + str[num1 + 0x1b] * 256 + str[num1 + 0x14] * 256 * 256 + str[num1 + 0x15] * 256 * 256 * 256;
	if (firstClusters > clustersSum) {
		return;
	}
	change = to_string(firstClusters);
	if (change.empty()) {
		return;
	}
	result = result + change + ",";

	////////////提取文件的长度//////////////
	length = str[num1 + 0x1c] + str[num1 + 0x1d] * 256 + str[num1 + 0x1e] * 256 * 256 + str[num1 + 0x1f] * 256 * 256 * 256;
	change = to_string(length);
	if (change.empty()) {
		return;
	}
	result = result + change;


	ofstream out;
	out.open("test.csv", ios::out | ios::app);
	out << result << endl;
	out.close();
}


//恢复文件
BOOL recoveryFileInTwo(Drives drive, vector<PDirectoryStruct> directoryHead) {
	vector<string> showDetails;
	int n = 1;

	cout << endl << "被删除的文件和目录有：" << endl;


	ofstream out;
	//ios:out文件以输出的方式打开，即写文件
	//ios:trunc如果文件存在，把文件长度设置为0
	out.open("test2.csv", ios::out | ios::trunc);
	out.close();
	for (int i = 0; i < directoryHead.size(); i++) {
		traverseNode(directoryHead.at(i), "", &n, showDetails, "test2.csv");
	}

	
	int takeIndex;
	cout << endl << "请选择要恢复的文件：" << endl;
	cin >> takeIndex;

	string takeName = showDetails.at(takeIndex - 1);

	vector<string> splitName = split(takeName, "\\");
	PDirectoryStruct p=NULL;


	for (int i = 0; i < splitName.size(); i++) {
		string thisName = splitName.at(i);
		if (i == 0) {
			for (int j = 0; j < directoryHead.size(); j++) {
				if (thisName == directoryHead.at(j)->directory->getName()) {
					p = directoryHead.at(j);
					break;
				}
			}
		}
		else {
			for (int j = 0; j < p->childrens.size(); j++) {
				if (thisName == p->childrens.at(j)->directory->getName()) {
					p = p->childrens.at(j);
					break;
				}
			}
		}
		

		
		
	}

	if (p->directoryType == 0) {
		cout << p->directory->getName() << "(文件)" << endl;
	}
	else {
		cout << p->directory->getName() << "(目录)" << endl;
	}

	cout << "创建时间为：          " << p->directory->getCreateTime() << endl;
	cout << "最后修改时间为：      " << p->directory->getUpdateTime() << endl;
	cout << "最后访问时间为：      " << p->directory->getVisitTime() << endl;
	cout << "目录（文件）所在的起始簇号为：" << p->directory->getFirstClusters() << "(簇号为0表示为空文件)" << endl;
	cout << "文件长度为：          " << p->directory->getLength() << endl;

	//保存要恢复的文件或目录
	saveRecovery(drive, p, "D:\\test");


	return TRUE;

}