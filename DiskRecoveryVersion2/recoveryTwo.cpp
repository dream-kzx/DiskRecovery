#include "recoveryTwo.h"

//��һ��ɨ������Ŀ¼�����Ѵ���Ŀ¼�ṹ�Ĵؽ��б�ʶ����ʾ��ƥ��ô�



////////////////////////////////////////////////////�ڶ�����ʼƥ���ļ�/////////////////////////////////////
BOOL matchFile(Drives drive, vector<PDirectoryStruct>& directoryHead) {
	clustersSum = drive.getTotalClusters();
	byte* clusterIdentity = new byte[clustersSum+2];//��ʶ��

	//ÿ�����ֽ���
	DWORD byteClusters = drive.getSectPerClust()*drive.getBytesPerSect();
	
	for (int i = 0; i < clustersSum+2; i++) {
		clusterIdentity[i] = 0;
	}

	byte* buffer = new byte[SECTIONSIZE];//һ�������Ĵ�С
	byte directoryStr[32];	//����һ��Ŀ¼�ÿ��Ŀ¼��ռ32���ֽ�
	DWORD directoryRootLocation;	//��Ŀ¼��ռ��������λ��
	directoryRootLocation = drive.getFirstSectByClust();//��ȡ��Ŀ¼��ռ����ʼ����

	for (int i = 0; i < 2; i++) {
		clusterIdentity[i] = 1;
	}

	///////////////////////////����Ŀ¼�ṹ������ʹ�õĴؽ��б�ʶ/////////////////////////////////////////////
	int number = 0;//number����������Ŀ¼��������number����ڼ�������
	while (true) {
		//��ȡ��Ŀ¼�ĵ�number������������
		cout << directoryRootLocation + number << endl;
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

		
			//����ļ���Ŀ¼
			if ((directoryStr[0x0B] == 0x10) && (directoryStr[0] != 0xE5)) {

				//��ȡ���Ŀ¼�����ʼ��
				DWORD firstClusters = directoryStr[0x1a] + directoryStr[0x1b] * 256
					+ directoryStr[0x14] * 256 * 256 + directoryStr[0x15] * 256 * 256 * 256;


				//�����ʼ��С�ڷ������ܴ�
				if (firstClusters <= drive.getTotalClusters())
				{
					clusterIdentity[firstClusters] = 1;
					queryChildrenDirectory(drive, drive.getLocationByClus(firstClusters), clusterIdentity, 0);
				}
			}
			else {//������ļ�
				if (((int)directoryStr[0] != 0xE5) && ((int)directoryStr[0x0B] != 0x0F)) {
					/////////////��ȡ�ļ�����ʼ�غ�///////////
					DWORD firstClusters = directoryStr[0x1a] + directoryStr[0x1b] * 256 + directoryStr[0x14] * 256 * 256
						+ directoryStr[0x15] * 256 * 256 * 256;


					////////////��ȡ�ļ��ĳ���//////////////
					DWORD length = directoryStr[0x1c] + directoryStr[0x1d] * 256 + directoryStr[0x1e] * 256 * 256
						+ directoryStr[0x1f] * 256 * 256 * 256;



					//�����ļ���ռ�صĸ���
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

	int readEach = 2000;//ÿ�ζ�ȡ���̶�ȡ���ٸ���

	buffer = new byte[byteClusters * readEach];
	

	int directoryNum = 0;

	for (DWORD i = 2; i < clustersSum + 2; i = i + readEach) {
		cout << i << endl;

		int realReadNum = readEach;//����Ҫ��ȡ��������

		if (i + readEach < clustersSum + 2) {//����������һ������������
			realReadNum = readEach;
			if (!drive.getClustersByN(drive, buffer, i, realReadNum)) {
				cout << "��ȡ��������" << endl;
				return FALSE;
			}
		}
		else {
			realReadNum = clustersSum + 2 - i;
			if (!drive.getClustersByN(drive, buffer, i, realReadNum)) {
				cout << "��ȡ��������" << endl;
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

			if (isDirectory == TRUE) {//�����Ŀ¼
				cout << "This is directory" << endl;

				if (clusterIdentity[i + id] != 1) {//�������ز����Ѿ���ʶ�Ĵ�
					clusterIdentity[i + id] = 1;

					DWORD okSect = drive.getLocationByClus(i + id); //��ȡ��Ŀ¼���ڵ���ʼ����

					if ((okSect < drive.getSectorsSum()) && (okSect >= drive.getFirstSectByClust())) {
						PDirectoryStruct p = new DirectoryStruct();
						p->directory = new Directory();
						

						if(disposeDirectoryInTwo(drive, p, okSect, clusterIdentity, 0)>0){
							char nameBuff[10];
							sprintf_s(nameBuff, "%d", directoryNum);
							string name = "Ŀ¼" + string(nameBuff);
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

//�����Ѵ��ڵ���Ŀ¼���дر�ʶ
void queryChildrenDirectory(Drives drive, DWORD directoryLocation, byte* clusterIdentity, int n) {

	if ((n > 20) || (directoryLocation >= drive.getSectorsSum())) {
		return;
	}

	byte* buffer = new byte[SECTIONSIZE];//һ�������Ĵ�С
	byte directoryStr[32];	//����һ��Ŀ¼�ÿ��Ŀ¼��ռ32���ֽ�
	//ÿ�����ֽ���
	DWORD byteClusters = drive.getSectPerClust()*drive.getBytesPerSect();

	////////////////////////ƥ��Ŀ¼��������ɾ����־��Ŀ¼��//////////////////////////////////////
	int number = 0;//number����������Ŀ¼��������number����ڼ�������
	while (true) {
		//��ȡ��Ŀ¼�ĵ�number������������
		if (!drive.getDriveByN(buffer, directoryLocation + number)) {
			cout << "��ȡ��������" << endl;
			return;
		}

		if (readDirectoryEnd(buffer)) {//��ʾ��ȡ��Ŀ¼����
			break;
		}

		//ƥ��һ�����������е�Ŀ¼�ÿ��Ŀ¼��Ϊ32���ֽ�
		int i = 0;
		for (; i < SECTIONSIZE; i += 32) {
			memmove(directoryStr, buffer + i, sizeof(directoryStr));
			if (readDirectoryEnd(directoryStr)) {//��ȡ��ǰ��������
				break;
			}

			//���Ŀ¼���Ǹ�Ŀ¼����Ŀ¼û���ϼ�Ŀ¼�ͱ�Ŀ¼�򷵻�
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

			//������ϼ�Ŀ¼����Ŀ¼����ѭ��
			if ((directoryStr[0] == 0x2e && directoryStr[1] == 0x20) ||
				(directoryStr[0] == 0x2e && directoryStr[1] == 0x2e && directoryStr[2] == 0x20)) {
				continue;
			}

			//����ļ���Ŀ¼
			if ((directoryStr[0x0B] == 0x10) && (directoryStr[0] != 0xE5)) {

				//��ȡ���Ŀ¼�����ʼ��
				DWORD firstClusters = directoryStr[0x1a] + directoryStr[0x1b] * 256
					+ directoryStr[0x14] * 256 * 256 + directoryStr[0x15] * 256 * 256 * 256;

				//�����ʼ��С�ڷ������ܴ�
				if (firstClusters < drive.getTotalClusters()+2 )
				{
					clusterIdentity[firstClusters] = 1;
					queryChildrenDirectory(drive, drive.getLocationByClus(firstClusters), clusterIdentity, n+1);
				}
			}
			else {//������ļ�
				if (((int)directoryStr[0] != 0xE5) && (directoryStr[0x0B] != 0x0F)) {
					/////////////��ȡ�ļ�����ʼ�غ�///////////
					DWORD firstClusters = directoryStr[0x1a] + directoryStr[0x1b] * 256 + directoryStr[0x14] * 256 * 256
						+ directoryStr[0x15] * 256 * 256 * 256;

					//�����ʼ��С�ڷ������ܴ�
					if (firstClusters < drive.getTotalClusters() + 2)
					{
						////////////��ȡ�ļ��ĳ���//////////////
						DWORD length = directoryStr[0x1c] + directoryStr[0x1d] * 256 + directoryStr[0x1e] * 256 * 256
							+ directoryStr[0x1f] * 256 * 256 * 256;
						//�����ļ���ռ�صĸ���
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


//������Ŀ¼
int disposeDirectoryInTwo(Drives drive, PDirectoryStruct& parent, DWORD directoryLocation, byte* clusterIdentity, int n) {
	int num = 0;

	cout << "             " << directoryLocation << endl;
	if ( n > 10 ) {
		return num;
	}

	byte buffer[SECTIONSIZE];
	byte directoryStr[32];	//����һ��Ŀ¼�ÿ��Ŀ¼��ռ32���ֽ�

	PDirectoryStruct p = new DirectoryStruct();
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

				
				if (clusterIdentity[firstClusters] != 1) {//�������ز����Ѿ���ʶ�Ĵ�
					clusterIdentity[firstClusters] = 1;

					DWORD okSect = drive.getLocationByClus(firstClusters);//��ȡ��Ŀ¼���ڵ���ʼ����

					if ((okSect < drive.getSectorsSum()) && (okSect >= drive.getFirstSectByClust())) {
						//int result = disposeDirectory(drive, p, drive.getLocationByClus(firstClusters), 0);//���һ������������¼�ݹ�Ĵ���
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
				if ((int)directoryStr[0x0B] != 0x0F) {//������ǳ�Ŀ¼�ļ����ļ�����
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


//����ÿ���ļ�����Ϣ
void eachDirectory(Directory* p) {

	string result = "";

	/////////////////////��ȡÿ��Ŀ¼�����Ϣ///////////////////////////////////
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
	if (num1 > 0) {//����ǳ�Ŀ¼�ļ�����ȡ���ļ���
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
		//���泤�ļ����ļ�
		charSaveWchar_t(wname, name, k1);
		okName = tcharToString(wname);
		replaceComma(okName);
		result = result + okName + ",";
		p->setName(okName);
		//cout << okName << endl;
	}
	k2 = 0;

	////////���Ƕ��ļ���ʱ����ȡ�ļ���������///////
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
		s = "��" + s;
		p->setName(s);
		result = result + s + ",";
		//cout << s << endl;
	}

	string change = "";//��ʱ������������תΪstring������

	/////////////////��ȡ��������ʱ�䲢����//////////////
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

	/////////////��ȡ����������//////////////
	extractDT[0] = str[num1 + 0x12];
	extractDT[1] = str[num1 + 0x13];
	date = Directory::parseDate(extractDT);
	if (date.empty()) {
		return;
	}
	result = result + date + ",";

	////////////��ȡ����޸�����ʱ��//////////////
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

	///////////��ȡ�ļ�����////////////////
	type = str[num1 + 0x0b];
	change = to_string(type);
	if (change.empty()) {
		return;
	}
	result = result + change + ",";

	/////////////��ȡ�ļ�����ʼ�غ�///////////
	firstClusters = str[num1 + 0x1a] + str[num1 + 0x1b] * 256 + str[num1 + 0x14] * 256 * 256 + str[num1 + 0x15] * 256 * 256 * 256;
	if (firstClusters > clustersSum) {
		return;
	}
	change = to_string(firstClusters);
	if (change.empty()) {
		return;
	}
	result = result + change + ",";

	////////////��ȡ�ļ��ĳ���//////////////
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


//�ָ��ļ�
BOOL recoveryFileInTwo(Drives drive, vector<PDirectoryStruct> directoryHead) {
	vector<string> showDetails;
	int n = 1;

	cout << endl << "��ɾ�����ļ���Ŀ¼�У�" << endl;


	ofstream out;
	//ios:out�ļ�������ķ�ʽ�򿪣���д�ļ�
	//ios:trunc����ļ����ڣ����ļ���������Ϊ0
	out.open("test2.csv", ios::out | ios::trunc);
	out.close();
	for (int i = 0; i < directoryHead.size(); i++) {
		traverseNode(directoryHead.at(i), "", &n, showDetails, "test2.csv");
	}

	
	int takeIndex;
	cout << endl << "��ѡ��Ҫ�ָ����ļ���" << endl;
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
		cout << p->directory->getName() << "(�ļ�)" << endl;
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