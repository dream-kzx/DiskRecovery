#include "recoveryThree.h"

Semaphore semaphore(4);//�ź���
std::mutex writeMutex;

void matchFileFragmentation(Drives drive) {

	byte* sectorsIdentity;
	sectorsIdentity = new byte[drive.getSectorsSum()];


	//�������ܴ���
	DWORD clustersSum = drive.getTotalClusters();

	//ÿ�����ֽ���
	DWORD byteClusters = drive.getSectPerClust() * drive.getBytesPerSect();

	//һ�������Ĵ�С�Ŀռ�
	byte* buffer = new byte[SECTIONSIZE];

	//����һ��Ŀ¼�ÿ��Ŀ¼��ռ32���ֽ�
	byte directoryStr[32];	

	//��Ŀ¼��ռ��������λ��
	DWORD directoryRootLocation= drive.getFirstSectByClust();
	

	///////////////////////////����Ŀ¼�ṹ������ʹ�õĴؽ��б�ʶ/////////////////////////////////////////////
	int number = 0;//number����������Ŀ¼��������number����ڼ�������
	while (true) {
		//cout << directoryRootLocation + number << endl;
		//��ȡ��Ŀ¼�ĵ�number������������
		if (!drive.getDriveByN(buffer, directoryRootLocation + number)) {
			cout << "��ȡ��������" << endl;
			return;
		}

		if (readDirectoryEnd(buffer)) {//��ʾ��ȡ��Ŀ¼����
			break;
		}

		sectorsIdentity[directoryRootLocation + number] = 1;

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
				if ((firstClusters < drive.getTotalClusters() + 2) && (firstClusters > 2))
				{

					queryChildrenDirectoryThree(drive, drive.getLocationByClus(firstClusters), sectorsIdentity, 0);
				}
			}
			else {//������ļ�
				if (((int)directoryStr[0] != 0xE5) && ((int)directoryStr[0x0B] == 0x20)) {
					/////////////��ȡ�ļ�����ʼ�غ�///////////
					DWORD firstClusters = directoryStr[0x1a] + directoryStr[0x1b] * 256 + directoryStr[0x14] * 256 * 256
						+ directoryStr[0x15] * 256 * 256 * 256;

					//�����ʼ��С�ڷ������ܴ�
					if ((firstClusters < drive.getTotalClusters() + 2) && (firstClusters > 2))
					{
						DWORD firstSectors = drive.getLocationByClus(firstClusters);

						////////////��ȡ�ļ��ĳ���//////////////
						DWORD length = directoryStr[0x1c] + directoryStr[0x1d] * 256 + directoryStr[0x1e] * 256 * 256
							+ directoryStr[0x1f] * 256 * 256 * 256;



						//�����ļ���ռ�����ĸ���
						int num = length / SECTIONSIZE;
						if (length % SECTIONSIZE > 0) {
							num++;
						}

						cout << "�ļ�����Ϊ��" << num << endl;
						for (int k = firstSectors; k < firstSectors + num; k++) {
							sectorsIdentity[k] = true;
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


	////��ʼ��AC�Զ���
	//acNode* root = new acNode();

	//char str1[] = { 0x50,0x4B,0x03,0x04,'\0' };
	//insert(str1, root, 1);
	//char str2[] = { 0x50,0x4B,0x01,0x02,'\0' };
	//insert(str2, root, 2);
	//char str3[] = { 0x50,0x4B,0x05,0x06,'\0' };
	//insert(str3, root, 3);

	////����ac�Զ���fail��ת
	//buildACFail(root);

	int readEach = 1000;//ÿ�ζ�ȡ���̶�ȡ���ٸ���

	ReadWriteCSV mycsvW("test.csv", 2);
	queue<string> writeLine;
	writeLine.push("�ļ���");
	writeLine.push("������ʱ��");
	writeLine.push("�ļ�ƫ��λ��");
	writeLine.push("�ļ�������δѹ��ʱ��С");
	writeLine.push("�ļ�������ѹ�����С");
	writeLine.push("zip�ļ���С");
	mycsvW.writeCSV(writeLine);
	mycsvW.close();

	for (DWORD i = 2; i < clustersSum + 2; i = i + readEach) {
		buffer = new byte[byteClusters * readEach];
		cout << i << endl;
		////printf("       �����ַΪ��%xkk" ,buffer);

		int realReadNum = readEach;//����Ҫ��ȡ��������

		if (i + readEach < clustersSum + 2) {//����������һ�������Ĵ�
			realReadNum = readEach;
			if (!drive.getClustersByN(drive, buffer, i, realReadNum)) {
				cout << "��ȡ��������" << endl;
				return;
			}
		}
		else {
			realReadNum = clustersSum + 2 - i;
			if (!drive.getClustersByN(drive, buffer, i, realReadNum)) {
				cout << "��ȡ��������" << endl;
				return;
			}
		}

		//for (int k = 0; k < realReadNum * byteClusters; k++) {
		//
		//	cout <<setiosflags(ios::uppercase) <<setfill('0') << setw(2)<<hex<< int(buffer[k]);
		//}

		//AC�Զ����㷨ƥ�䲿��


		//searchZipIdentity(drive, drive.getLocationByClus(i), sectorsIdentity, buffer, byteClusters* realReadNum, root);
		

		//SundaySearch(buffer, byteClusters* realReadNum, drive.getLocationByClus(i), sectorsIdentity, drive);

		semaphore.wait();
		thread t1(SundaySearch, buffer, byteClusters* realReadNum, drive.getLocationByClus(i), sectorsIdentity, drive);
		t1.detach();
		
		
		
	}

	Sleep(1000 * 5);



	cout << "ok" << endl;

}


//�����Ѵ��ڵ���Ŀ¼���дر�ʶ
void queryChildrenDirectoryThree(Drives drive, DWORD directoryLocation, byte* sectorsIdentity, int n) {

	if ((n > 20) || (directoryLocation >= drive.getSectorsSum())) {
		return;
	}

	byte* buffer = new byte[SECTIONSIZE];//һ�������Ĵ�С
	byte directoryStr[32];	//����һ��Ŀ¼�ÿ��Ŀ¼��ռ32���ֽ�
	//ÿ�����ֽ���
	DWORD byteClusters = drive.getSectPerClust() * drive.getBytesPerSect();

	////////////////////////ƥ��Ŀ¼��������ɾ����־��Ŀ¼��//////////////////////////////////////
	int number = 0;//number����������Ŀ¼��������number����ڼ�������
	while (true) {
		//cout <<"				" << directoryLocation + number << endl;
		//��ȡĿ¼�ĵ�number������������
		if (!drive.getDriveByN(buffer, directoryLocation + number)) {
			cout << "��ȡ��������" << endl;
			return;
		}

		if (readDirectoryEnd(buffer)) {//��ʾ��ȡ��Ŀ¼����
			break;
		}

		sectorsIdentity[directoryLocation + number] = 1;

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
			if ((directoryStr[0x0B] == 0x10)&&(directoryStr[0] != 0xE5)) {

				//��ȡ���Ŀ¼�����ʼ��
				DWORD firstClusters = directoryStr[0x1a] + directoryStr[0x1b] * 256
					+ directoryStr[0x14] * 256 * 256 + directoryStr[0x15] * 256 * 256 * 256;

				//�����ʼ��С�ڷ������ܴ�
				if ((firstClusters < drive.getTotalClusters() + 2) && (firstClusters > 2))
				{
					queryChildrenDirectoryThree(drive, drive.getLocationByClus(firstClusters), sectorsIdentity, n+1);
				}
			}
			else {//������ļ�
				if (((int)directoryStr[0] != 0xE5) && (directoryStr[0x0B] == 0x20)) {
					/////////////��ȡ�ļ�����ʼ�غ�///////////
					DWORD firstClusters = directoryStr[0x1a] + directoryStr[0x1b] * 256 + directoryStr[0x14] * 256 * 256
						+ directoryStr[0x15] * 256 * 256 * 256;

					if ((firstClusters < drive.getTotalClusters() + 2)&&(firstClusters>2)) {
						DWORD firstSectors = drive.getLocationByClus(firstClusters);

						////////////��ȡ�ļ��ĳ���//////////////
						DWORD length = directoryStr[0x1c] + directoryStr[0x1d] * 256 + directoryStr[0x1e] * 256 * 256
							+ directoryStr[0x1f] * 256 * 256 * 256;


						//�����ļ���ռ�����ĸ���
						int num = length / SECTIONSIZE;
						if (length % SECTIONSIZE > 0) {
							num++;
						}

						cout << "�ļ�����Ϊ��" << num << endl;
						for (int k = firstSectors; k < firstSectors + num; k++) {
							sectorsIdentity[k] = 1;
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


int searchZipIdentity(Drives drive, DWORD sectorsNumber, byte* sectorsIdentity, byte* buffer, int length, acNode* root) {
	
	MyDict::iterator iter;

	acNode* p = root;
	byte str;
	int number = 0;

	map<int, bool>::iterator identityIter;

	time_t rawtime;
	struct tm* timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	cout << asctime(timeinfo) << endl;

	for (int i = 0; i < length; i++) {
		if (i% SECTIONSIZE == 0) {
			DWORD thisSector = sectorsNumber + i / SECTIONSIZE;
			
			/*identityIter = sectorsIdentity.find(thisSector);
			if (identityIter != sectorsIdentity.end()) {
				i += SECTIONSIZE - 1;
				p = root;
				continue;
			}*/

			cout << "������" << thisSector << endl;
		}

		
		str = buffer[i];


		
		while ((p->next[str]==NULL) && (p != root)) {
			p = p->fail;
			
		}

		if (p->next[str] != NULL) {
			p = p->next[str];
			if (p->count == 1) {
				cout << "0x504b0304λ��Ϊ��" << sectorsNumber * SECTIONSIZE + i << endl;
			}
			else if (p->count == 2) {
				cout << "0x504b0102λ��Ϊ��" << sectorsNumber * SECTIONSIZE + i << endl;
			}
			else if (p->count == 3) {
				cout << "0x504b0506λ��Ϊ��" << sectorsNumber * SECTIONSIZE + i << endl;
			}
		}
	}

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	cout << asctime(timeinfo) << endl;
	return number;
}

//��sunday�㷨�ڷ�����ƥ��ؼ���
void SundaySearch(byte* buffer, int length, DWORD sectorsNumber, byte* sectorsIdentity, Drives drive) {
	//printf("       �����ַΪ��%xzz", buffer);

	byte pattern[2] = { 0x50,0x4B };

	int i = 0;
	int j = 0;

	int length2 = 2;

	int move[256];

	memset(move, -1, 256 * sizeof(int));
	for (int k = 0; k < length2; k++) {
		move[pattern[k]] = length2 - k;
	}


	int match = 0;

	byte middle;


	while (i <= length - length2) {
		if (i % SECTIONSIZE == 0) {
			//cout << "����Ϊ��" << sectorsNumber + i / SECTIONSIZE << endl;
		}
		if (sectorsIdentity[sectorsNumber + i / SECTIONSIZE] == 1) {//����Ǳ�ʶ��������������
			i += SECTIONSIZE - i % SECTIONSIZE;
			//cout << "����Ϊ��" << sectorsNumber + i / SECTIONSIZE << endl;
			continue;
		}
		

		int k = length2 - 1;
		for (; k >= 0; k--) {
			
			if (pattern[k] != buffer[i + k]) {
				break;
			}
		}

		if (k < 0) {
			if (buffer[i + length2] == 0x03 && buffer[i + length2 + 1] == 0x04) {
				

				//cout << sectorsNumber + i / SECTIONSIZE << ":" << i % SECTIONSIZE << endl;
				cout << sectorsNumber*SECTIONSIZE + i << endl;

				cout << "ƥ��ɹ�,0x504b0304"<< endl;

				//����ƫ�ƶ�ȡzip�ļ�����Ϣ

				byte zipHead[30];

				DWORD offsetNumber = sectorsNumber * SECTIONSIZE + i;
				if (!drive.getDriveByOffset(zipHead, offsetNumber, 30)) {
					cout << "��ȡ��������" << endl;
				}
				else {
					ZipModule zipFile;
					zipFile.setLocation(offsetNumber);//���ø�zip�ļ��ڷ����е�ƫ��
					if (zipFile.parseZip(zipHead)) { //����zipͷ�ļ�����ȡzip�ļ���Ϣ
						int headAddName = zipFile.getFileNameLength() + 30;//����zip�ļ�ͷ���ļ����ĳ���

						byte* headAddNameBuffer = new byte[headAddName];
						if (!drive.getDriveByOffset(headAddNameBuffer, offsetNumber, headAddName)) {
							cout << "��ȡ��������" << endl;

						}

						zipFile.parseZipName(headAddNameBuffer);//����zip�ļ���
						if (checkOfficeFragment(zipFile.getFileName())) {
							//��zip�ļ���Ϣд��.csv�ļ�
							

							queue<string> writeLine;
							writeLine.push(zipFile.getFileName());
							writeLine.push(zipFile.getLastDateTime());
							writeLine.push(to_string(zipFile.getLocation()));
							writeLine.push(to_string(zipFile.getUnCompressSize()));
							writeLine.push(to_string(zipFile.getCompressSize()));
							writeLine.push(to_string(zipFile.getZipSize()));

							//��д�ļ���
							writeMutex.lock();
							ReadWriteCSV mycsvW("test.csv", 1);
							mycsvW.writeCSV(writeLine);
							mycsvW.close();
							writeMutex.unlock();

							cout << zipFile.getFileName() << endl;
							cout << "�ļ�����Ϊ��" << zipFile.getZipSize() << endl;

							delete headAddNameBuffer;
						}


						
					}
					

					
				}



				
				match++;
			}
			else if(buffer[i + length2] == 0x01 && buffer[i + length2 + 1] == 0x02){
				cout << "ƥ��ɹ�,0x50db0102" << endl;
				match++;
			}
			else if (buffer[i + length2] == 0x05 && buffer[i + length2 + 1] == 0x06) {
				cout << "ƥ��ɹ�,0x50db0506" << endl;
				match++;
			}
		}

		middle = buffer[i + length2];
		if (move[middle] == -1) {//ĩβ��һλ��ƥ��
			i += length2 + 1;
		}
		else {
			i += move[middle];
		}
		

	}
	if (match > 0) {
		cout << "ƥ��ɹ��ַ�������Ϊ��" << match << endl;
	}
	
	delete buffer;
	semaphore.signal();
}

bool recoveryFileInThree(Drives drive,string csvFileName) {
	//��pythonд��exe��csv�ļ���������
	system("BC.exe test.csv");

	//��ȡcsv�ļ���ͨ��ƫ�ƺ�zip�ļ���С���ж��Ƿ�Ϊͬһ���ļ�
	ReadWriteCSV readCsv(csvFileName, 0);
	
	string lineData;
	string catalogFirst = "D:\\test\\catalog_";

	bool firstLine = true;
	int i = 0;
	int mmm = 0;
	DWORD preFileEndOffset = 0;

	while ((lineData=readCsv.readCSV())!=""){
		mmm++;
		cout << mmm << endl;
		if (firstLine) {
			firstLine = false;
			continue;
		}

		//��һ�����ݽ�����Ƭ
		
		vector<string> splitName = split(lineData, ",");
		string fileName = splitName.at(0);
		string dateTime = splitName.at(1);
		string temp = splitName.at(2);
 		DWORD offset = std::atoi(temp.c_str());
		int fileSize = std::stoi(splitName.at(5));
		
		if (preFileEndOffset != 0&&offset-preFileEndOffset>50) {
			i++;
		}

		string catalog =catalogFirst + to_string(i);
		if (_access(catalog.data(), 0) == -1) { //����ļ��в�����
			TCHAR* middle;
			charToWchar_t(middle, catalog.data(), catalog.size());
			if (CreateDirectory(middle, NULL) == 0) {
				cout << "�����ļ���ʧ��" << endl;
				return FALSE;
			}
		}

		splitName = split(fileName, "/");
		if (splitName.size() > 1) {
			//int j = 0;
			//for (; j < splitName.size() - 1; j++) {
			//	catalog += "\\" + splitName.at(j);
			//	if (_access(catalog.data(), 0) == -1) { //����ļ��в�����
			//		TCHAR* middle;
			//		charToWchar_t(middle, catalog.data(), catalog.size());
			//		if (CreateDirectory(middle, NULL) == 0) {
			//			cout << "�����ļ���ʧ��" << endl;
			//			return FALSE;
			//		}
			//	}
			//}
			//fileName = splitName.at(j);
			fileName = splitName.at(splitName.size()-1);

		}


		
		

		fileName = catalog + "\\" + fileName + ".zip";
		
		preFileEndOffset = offset + fileSize;
		
		

		//��ƫ���ڷ�����ȡ��Ϣ
		byte* buffer = new byte[fileSize];
		if (!drive.getDriveByOffset(buffer, offset, fileSize)) {
			cout << "�ָ��ļ�����ȡ����ʧ�ܣ�" << endl;
			readCsv.close();
			return false;
		}

		//����ȡ����16��������д���ļ�
		FILE* fp;
		int result = fopen_s(&fp, fileName.data(), "wb");
		if (fp == NULL) {
			cout << "���ļ�ʧ�ܣ�" << endl;
			readCsv.close();
			return FALSE;
		}
		
		fwrite(buffer, 1, fileSize, fp);
		fclose(fp);
		delete buffer;


	}


	readCsv.close();
	return true;

	//if (_access(parentName.data(), 0) == -1) { //����ļ��в�����
	//	TCHAR* middle;
	//	charToWchar_t(middle, parentName.data(), parentName.size());
	//	if (CreateDirectory(middle, NULL) == 0) {
	//		cout << "�����ļ���ʧ��" << endl;
	//		return FALSE;
	//	}
	//}
}