#include "recoveryOne.h"


//////////////////////////////////////处理根目录，检索添加删除标记的文件名/////////////////////////////////////
BOOL disposeRootDirectory(Drives& drive, PDirectoryStruct& directoryHead) {

	byte* buffer = new byte[SECTIONSIZE];
	byte directoryStr[32];	//定义一个目录项，每个目录项占32个字节
	DWORD directoryRootLocation;	//根目录所占首扇区的位置

	directoryHead = new DirectoryStruct();
	directoryHead->directoryType = 2;


	directoryRootLocation = drive.getFirstSectByClust();//获取根目录所占的起始扇区

	PDirectoryStruct p=new DirectoryStruct();
	p->directory = new Directory();


	////////////////////////匹配根目录中所有有删除标志的目录项//////////////////////////////////////
	int number = 0;//number用来遍历根目录的扇区，number代表第几个扇区
	while (true) {
		//读取根目录的第number个扇区的数据
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

			if (directoryStr[0x0B] == 0x0F) {
				p->directory->setStr(directoryStr);
			}
			else {
				//如果文件是目录
				if (directoryStr[0x0B] == 0x10) {
					p->directory->setStr(directoryStr);
					p->directoryType = 1;
					

					//获取这个目录项的起始簇
					DWORD firstClusters = directoryStr[0x1a] + directoryStr[0x1b] * 256
						+ directoryStr[0x14] * 256 * 256 + directoryStr[0x15] * 256 * 256 * 256;

					//如果起始簇小于分区的总簇
					if (firstClusters <= drive.getTotalClusters())
					{
						int result=disposeDirectory(drive, p, drive.getLocationByClus(firstClusters), 0);//最后一个参数用来记录递归的次数
						if (result == 0) {
							delete p;
						}
						else {
							directoryHead->childrens.push_back(p);
						}
					}
					else {
						delete p;
					}

					PDirectoryStruct q = new DirectoryStruct();
					q->directory = new Directory();

					p = q;

				}
				else {
					if ((int)directoryStr[0] == 0xE5) {//如果是删除的文件
						p->directory->setStr(directoryStr);
						p->directoryType = 0;
						directoryHead->childrens.push_back(p);
						PDirectoryStruct q = new DirectoryStruct();
						q->directory = new Directory();
						p = q;
					}
					else {
						p->directory->setStrNULL();
					}
				}
			}

		}
		if (i < SECTIONSIZE) {
			break;
		}
		number++;
	}

	traverseTree(directoryHead);

	return true;
}

//处理子目录
int disposeDirectory(Drives drive, PDirectoryStruct& parent, DWORD directoryLocation, int n) {
	int num = 0;//用来记录该目录是否含有删除的文件

	if ((n > 10) || (directoryLocation >= drive.getSectorsSum())) {
		return num;
	}

	unsigned char* buffer = new unsigned char[SECTIONSIZE];
	unsigned char directoryStr[32];	//定义一个目录项，每个目录项占32个字节

	PDirectoryStruct p=new DirectoryStruct();
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

				//如果起始簇小于分区的总簇
				if (firstClusters <= drive.getTotalClusters())
				{
					int result=disposeDirectory(drive, p, drive.getLocationByClus(firstClusters), n+1);//最后一个参数用来记录递归的次数
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

				PDirectoryStruct q=new DirectoryStruct();
				q->directory = new Directory();

				p = q;

			}
			else {
				if ((int)directoryStr[0] == 0xE5) {//如果是删除的文件
					p->directory->setStr(directoryStr);
					if ((int)directoryStr[0x0B] != 0x0F) {//如果不是长目录文件的文件名，
						p->directoryType = 0;
						num++;

						parent->childrens.push_back(p);
						PDirectoryStruct q=new DirectoryStruct();
						q->directory = new Directory();
						p = q;

					}
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




/////////////////////////////////////////恢复文件///////////////////////////////////////////////////
BOOL recoverFile(Drives drive, PDirectoryStruct directoryHead) {
	vector<string> showDetails;
	int n = 1;

	cout << endl << "被删除的文件和目录有：" << endl;


	ofstream out;
	//ios:out文件以输出的方式打开，即写文件
	//ios:trunc如果文件存在，把文件长度设置为0
	out.open("test1.csv", ios::out | ios::trunc);
	out.close();
	traverseNode(directoryHead, "", &n, showDetails, "test1.csv");
	
	int takeIndex;
	cout << endl << "请选择要恢复的文件：" << endl;
	cin >> takeIndex;

	string takeName = showDetails.at(takeIndex-1);

	vector<string> splitName = split(takeName, "\\");
	PDirectoryStruct p = directoryHead;


	for (int i = 0; i < splitName.size(); i++) {
		string thisName = splitName.at(i);
		for (int j = 0; j < p->childrens.size(); j++) {
			if (thisName == p->childrens.at(j)->directory->getName()) {
				p = p->childrens.at(j);
			}
		}
	}

	if (p->directoryType == 0) {
		cout << p->directory->getName() <<"(文件)"<< endl;
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




////保存要恢复的文件或目录
//BOOL saveRecovery(Drives drive, PDirectoryStruct PNode, string parentName) {
//	
//	
//
//	if (_access(parentName.data(), 0) == -1) { //如果文件夹不存在
//		TCHAR* middle;
//		charToWchar_t(middle, parentName.data(), parentName.size());
//		if (CreateDirectory(middle, NULL) == 0) {
//			cout << "创建文件夹失败" << endl;
//			return FALSE;
//		}
//	}
//
//	if (PNode->directoryType == 2) {//如果是根目录
//		for (int i = 0; i < PNode->childrens.size(); i++) {
//			saveRecovery(drive, PNode->childrens.at(i), parentName);
//		}
//	}
//	else if (PNode->directoryType == 1) {//如果是目录
//		string name = PNode->directory->getName();
//		name = parentName + "\\" + name;
//		for (int i = 0; i < PNode->childrens.size(); i++) {
//			saveRecovery(drive, PNode->childrens.at(i), name);
//		}
//
//	}
//	else {
//		string name = PNode->directory->getName();
//		name = parentName + "\\" + name;
//		DWORD firstClusters = PNode->directory->getFirstClusters();	//文件的起始簇
//		DWORD startSectors = drive.getLocationByClus(firstClusters);	//文件的起始扇区
//		DWORD fileSectors[2];		//文件所占扇区大小
//
//		PNode->directory->getFileSize(fileSectors);
//
//		isCreateFile(name);//判断文件名是否已存在，如果存在则在后面加_1
//		unsigned char buffer[512];
//		FILE* fp;
//		int result = fopen_s(&fp, name.data(), "wb");
//		if (fp == NULL) {
//			cout << "打开文件失败！" << endl;
//			return FALSE;
//		}
//		int i;
//		for (i = 0; i < fileSectors[0]; i++) {
//			drive.getDriveByN(buffer, startSectors + i);
//			fwrite(buffer, 1, 512, fp);
//			cout << i << endl;
//		}
//		drive.getDriveByN(buffer, startSectors + i);
//		fwrite(buffer, 1, fileSectors[1], fp);
//
//		fclose(fp);
//	}
//}


