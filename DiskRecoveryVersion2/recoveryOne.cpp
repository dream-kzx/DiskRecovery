#include "recoveryOne.h"


//////////////////////////////////////�����Ŀ¼���������ɾ����ǵ��ļ���/////////////////////////////////////
BOOL disposeRootDirectory(Drives& drive, PDirectoryStruct& directoryHead) {

	byte* buffer = new byte[SECTIONSIZE];
	byte directoryStr[32];	//����һ��Ŀ¼�ÿ��Ŀ¼��ռ32���ֽ�
	DWORD directoryRootLocation;	//��Ŀ¼��ռ��������λ��

	directoryHead = new DirectoryStruct();
	directoryHead->directoryType = 2;


	directoryRootLocation = drive.getFirstSectByClust();//��ȡ��Ŀ¼��ռ����ʼ����

	PDirectoryStruct p=new DirectoryStruct();
	p->directory = new Directory();


	////////////////////////ƥ���Ŀ¼��������ɾ����־��Ŀ¼��//////////////////////////////////////
	int number = 0;//number����������Ŀ¼��������number����ڼ�������
	while (true) {
		//��ȡ��Ŀ¼�ĵ�number������������
		if (!drive.getDriveByN(buffer, directoryRootLocation + number)) {
			cout << "��ȡ��������" << endl;
			return NULL;
		}

		if (readDirectoryEnd(buffer)) {//��ʾ��ȡ��Ŀ¼����
			break;
		}

		//ƥ��һ�����������д��б�־��Ŀ¼�ÿ��Ŀ¼��Ϊ32���ֽ�
		int i = 0;
		if (number == 0) {
			i = 128;
		}

		for (; i < SECTIONSIZE; i += 32) {
			memmove(directoryStr, buffer + i, sizeof(directoryStr));
			if (readDirectoryEnd(directoryStr)) {//��ȡ��ǰ��������
				break;
			}

			if (directoryStr[0x0B] == 0x0F) {
				p->directory->setStr(directoryStr);
			}
			else {
				//����ļ���Ŀ¼
				if (directoryStr[0x0B] == 0x10) {
					p->directory->setStr(directoryStr);
					p->directoryType = 1;
					

					//��ȡ���Ŀ¼�����ʼ��
					DWORD firstClusters = directoryStr[0x1a] + directoryStr[0x1b] * 256
						+ directoryStr[0x14] * 256 * 256 + directoryStr[0x15] * 256 * 256 * 256;

					//�����ʼ��С�ڷ������ܴ�
					if (firstClusters <= drive.getTotalClusters())
					{
						int result=disposeDirectory(drive, p, drive.getLocationByClus(firstClusters), 0);//���һ������������¼�ݹ�Ĵ���
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
					if ((int)directoryStr[0] == 0xE5) {//�����ɾ�����ļ�
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

//������Ŀ¼
int disposeDirectory(Drives drive, PDirectoryStruct& parent, DWORD directoryLocation, int n) {
	int num = 0;//������¼��Ŀ¼�Ƿ���ɾ�����ļ�

	if ((n > 10) || (directoryLocation >= drive.getSectorsSum())) {
		return num;
	}

	unsigned char* buffer = new unsigned char[SECTIONSIZE];
	unsigned char directoryStr[32];	//����һ��Ŀ¼�ÿ��Ŀ¼��ռ32���ֽ�

	PDirectoryStruct p=new DirectoryStruct();
	p->directory = new Directory();

	////////////////////////ƥ��Ŀ¼��������ɾ����־��Ŀ¼��//////////////////////////////////////
	int number = 0;
	while (1) {
		if (!drive.getDriveByN(buffer, directoryLocation + number)) {
			cout << "��ȡ��������" << endl;
			return num;
		}

		if (readDirectoryEnd(buffer)) {//��ʾ��ȡ��Ŀ¼����
			break;
		}

		int i = 0;
		for (; i < SECTIONSIZE; i += 32) {//ƥ��һ�����������д��б�־��Ŀ¼��
			memmove(directoryStr, buffer + i, sizeof(directoryStr));
			if (readDirectoryEnd(directoryStr)) {//��ȡ��ǰ��������
				break;
			}

			//���Ŀ¼���Ǹ�Ŀ¼����Ŀ¼û���ϼ�Ŀ¼�ͱ�Ŀ¼�򷵻�
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

			//������ϼ�Ŀ¼����Ŀ¼����ѭ��
			if ((directoryStr[0] == 0x2e && directoryStr[1] == 0x20) ||
				(directoryStr[0] == 0x2e && directoryStr[1] == 0x2e && directoryStr[2] == 0x20)) {
				continue;
			}

			//����ļ���Ŀ¼
			if (directoryStr[0x0B] == 0x10) {
				p->directory->setStr(directoryStr);
				p->directoryType = 1;
				

				//��ȡ���Ŀ¼�����ʼ��
				DWORD firstClusters = directoryStr[0x1a] + directoryStr[0x1b] * 256
					+ directoryStr[0x14] * 256 * 256 + directoryStr[0x15] * 256 * 256 * 256;

				//�����ʼ��С�ڷ������ܴ�
				if (firstClusters <= drive.getTotalClusters())
				{
					int result=disposeDirectory(drive, p, drive.getLocationByClus(firstClusters), n+1);//���һ������������¼�ݹ�Ĵ���
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
				if ((int)directoryStr[0] == 0xE5) {//�����ɾ�����ļ�
					p->directory->setStr(directoryStr);
					if ((int)directoryStr[0x0B] != 0x0F) {//������ǳ�Ŀ¼�ļ����ļ�����
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




/////////////////////////////////////////�ָ��ļ�///////////////////////////////////////////////////
BOOL recoverFile(Drives drive, PDirectoryStruct directoryHead) {
	vector<string> showDetails;
	int n = 1;

	cout << endl << "��ɾ�����ļ���Ŀ¼�У�" << endl;


	ofstream out;
	//ios:out�ļ�������ķ�ʽ�򿪣���д�ļ�
	//ios:trunc����ļ����ڣ����ļ���������Ϊ0
	out.open("test1.csv", ios::out | ios::trunc);
	out.close();
	traverseNode(directoryHead, "", &n, showDetails, "test1.csv");
	
	int takeIndex;
	cout << endl << "��ѡ��Ҫ�ָ����ļ���" << endl;
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
		cout << p->directory->getName() <<"(�ļ�)"<< endl;
	}
	else {
		cout << p->directory->getName() << "(Ŀ¼)" << endl;
	}
	
	cout << "����ʱ��Ϊ��          " << p->directory->getCreateTime() << endl;
	cout << "����޸�ʱ��Ϊ��      " << p->directory->getUpdateTime() << endl;
	cout << "������ʱ��Ϊ��      " << p->directory->getVisitTime() << endl;
	cout << "Ŀ¼���ļ������ڵ���ʼ�غ�Ϊ��" << p->directory->getFirstClusters() << "(�غ�Ϊ0��ʾΪ���ļ�)" << endl;
	cout << "�ļ�����Ϊ��          " << p->directory->getLength() << endl;
	
	//����Ҫ�ָ����ļ���Ŀ¼
	saveRecovery(drive, p, "D:\\test");


	return TRUE;
}




////����Ҫ�ָ����ļ���Ŀ¼
//BOOL saveRecovery(Drives drive, PDirectoryStruct PNode, string parentName) {
//	
//	
//
//	if (_access(parentName.data(), 0) == -1) { //����ļ��в�����
//		TCHAR* middle;
//		charToWchar_t(middle, parentName.data(), parentName.size());
//		if (CreateDirectory(middle, NULL) == 0) {
//			cout << "�����ļ���ʧ��" << endl;
//			return FALSE;
//		}
//	}
//
//	if (PNode->directoryType == 2) {//����Ǹ�Ŀ¼
//		for (int i = 0; i < PNode->childrens.size(); i++) {
//			saveRecovery(drive, PNode->childrens.at(i), parentName);
//		}
//	}
//	else if (PNode->directoryType == 1) {//�����Ŀ¼
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
//		DWORD firstClusters = PNode->directory->getFirstClusters();	//�ļ�����ʼ��
//		DWORD startSectors = drive.getLocationByClus(firstClusters);	//�ļ�����ʼ����
//		DWORD fileSectors[2];		//�ļ���ռ������С
//
//		PNode->directory->getFileSize(fileSectors);
//
//		isCreateFile(name);//�ж��ļ����Ƿ��Ѵ��ڣ�����������ں����_1
//		unsigned char buffer[512];
//		FILE* fp;
//		int result = fopen_s(&fp, name.data(), "wb");
//		if (fp == NULL) {
//			cout << "���ļ�ʧ�ܣ�" << endl;
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


