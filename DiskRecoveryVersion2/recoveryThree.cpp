#include "recoveryThree.h"

Semaphore semaphore(4);//信号量

void matchFileFragmentation(Drives drive) {

	byte* sectorsIdentity;
	sectorsIdentity = new byte[drive.getSectorsSum()];


	//分区的总簇数
	DWORD clustersSum = drive.getTotalClusters();

	//每个簇字节数
	DWORD byteClusters = drive.getSectPerClust() * drive.getBytesPerSect();

	//一个扇区的大小的空间
	byte* buffer = new byte[SECTIONSIZE];

	//定义一个目录项，每个目录项占32个字节
	byte directoryStr[32];	

	//根目录所占首扇区的位置
	DWORD directoryRootLocation= drive.getFirstSectByClust();
	

	///////////////////////////遍历目录结构对正在使用的簇进行标识/////////////////////////////////////////////
	int number = 0;//number用来遍历根目录的扇区，number代表第几个扇区
	while (true) {
		cout << directoryRootLocation + number << endl;
		//读取根目录的第number个扇区的数据
		if (!drive.getDriveByN(buffer, directoryRootLocation + number)) {
			cout << "读取磁盘有误！" << endl;
			return;
		}

		if (readDirectoryEnd(buffer)) {//表示读取根目录结束
			break;
		}

		sectorsIdentity[directoryRootLocation + number] = 1;

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
				if ((firstClusters < drive.getTotalClusters() + 2) && (firstClusters > 2))
				{

					queryChildrenDirectoryThree(drive, drive.getLocationByClus(firstClusters), sectorsIdentity, 0);
				}
			}
			else {//如果是文件
				if (((int)directoryStr[0] != 0xE5) && ((int)directoryStr[0x0B] == 0x20)) {
					/////////////提取文件的起始簇号///////////
					DWORD firstClusters = directoryStr[0x1a] + directoryStr[0x1b] * 256 + directoryStr[0x14] * 256 * 256
						+ directoryStr[0x15] * 256 * 256 * 256;

					//如果起始簇小于分区的总簇
					if ((firstClusters < drive.getTotalClusters() + 2) && (firstClusters > 2))
					{
						DWORD firstSectors = drive.getLocationByClus(firstClusters);

						////////////提取文件的长度//////////////
						DWORD length = directoryStr[0x1c] + directoryStr[0x1d] * 256 + directoryStr[0x1e] * 256 * 256
							+ directoryStr[0x1f] * 256 * 256 * 256;



						//计算文件所占扇区的个数
						int num = length / SECTIONSIZE;
						if (length % SECTIONSIZE > 0) {
							num++;
						}

						cout << "文件长度为：" << num << endl;
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


	////初始化AC自动机
	//acNode* root = new acNode();

	//char str1[] = { 0x50,0x4B,0x03,0x04,'\0' };
	//insert(str1, root, 1);
	//char str2[] = { 0x50,0x4B,0x01,0x02,'\0' };
	//insert(str2, root, 2);
	//char str3[] = { 0x50,0x4B,0x05,0x06,'\0' };
	//insert(str3, root, 3);

	////构建ac自动机fail跳转
	//buildACFail(root);

	int readEach = 1000;//每次读取磁盘读取多少个簇

	

	for (DWORD i = 2; i < clustersSum + 2; i = i + readEach) {
		buffer = new byte[byteClusters * readEach];
		cout << i << endl;
		////printf("       数组地址为：%xkk" ,buffer);

		int realReadNum = readEach;//真正要读取的扇区数

		if (i + readEach < clustersSum + 2) {//如果不是最后一次余数的簇
			realReadNum = readEach;
			if (!drive.getClustersByN(drive, buffer, i, realReadNum)) {
				cout << "读取磁盘有误！" << endl;
				return;
			}
		}
		else {
			realReadNum = clustersSum + 2 - i;
			if (!drive.getClustersByN(drive, buffer, i, realReadNum)) {
				cout << "读取磁盘有误！" << endl;
				return;
			}
		}

		//for (int k = 0; k < realReadNum * byteClusters; k++) {
		//
		//	cout <<setiosflags(ios::uppercase) <<setfill('0') << setw(2)<<hex<< int(buffer[k]);
		//}

		//AC自动机算法匹配部分


		//searchZipIdentity(drive, drive.getLocationByClus(i), sectorsIdentity, buffer, byteClusters* realReadNum, root);

		SundaySearch(buffer, byteClusters* realReadNum, drive.getLocationByClus(i), sectorsIdentity, drive);

		//semaphore.wait();
		//thread t1(SundaySearch, buffer, byteClusters* realReadNum, drive.getLocationByClus(i), sectorsIdentity, drive);
		//t1.detach();
		
		
		
	}

	Sleep(1000 * 5);

	cout << "ok" << endl;

}


//遍历已存在的子目录进行簇标识
void queryChildrenDirectoryThree(Drives drive, DWORD directoryLocation, byte* sectorsIdentity, int n) {

	if ((n > 20) || (directoryLocation >= drive.getSectorsSum())) {
		return;
	}

	byte* buffer = new byte[SECTIONSIZE];//一个扇区的大小
	byte directoryStr[32];	//定义一个目录项，每个目录项占32个字节
	//每个簇字节数
	DWORD byteClusters = drive.getSectPerClust() * drive.getBytesPerSect();

	////////////////////////匹配目录中所有有删除标志的目录项//////////////////////////////////////
	int number = 0;//number用来遍历根目录的扇区，number代表第几个扇区
	while (true) {
		cout <<"				" << directoryLocation + number << endl;
		//读取目录的第number个扇区的数据
		if (!drive.getDriveByN(buffer, directoryLocation + number)) {
			cout << "读取磁盘有误！" << endl;
			return;
		}

		if (readDirectoryEnd(buffer)) {//表示读取根目录结束
			break;
		}

		sectorsIdentity[directoryLocation + number] = 1;

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
			if ((directoryStr[0x0B] == 0x10)&&(directoryStr[0] != 0xE5)) {

				//获取这个目录项的起始簇
				DWORD firstClusters = directoryStr[0x1a] + directoryStr[0x1b] * 256
					+ directoryStr[0x14] * 256 * 256 + directoryStr[0x15] * 256 * 256 * 256;

				//如果起始簇小于分区的总簇
				if ((firstClusters < drive.getTotalClusters() + 2) && (firstClusters > 2))
				{
					queryChildrenDirectoryThree(drive, drive.getLocationByClus(firstClusters), sectorsIdentity, n+1);
				}
			}
			else {//如果是文件
				if (((int)directoryStr[0] != 0xE5) && (directoryStr[0x0B] == 0x20)) {
					/////////////提取文件的起始簇号///////////
					DWORD firstClusters = directoryStr[0x1a] + directoryStr[0x1b] * 256 + directoryStr[0x14] * 256 * 256
						+ directoryStr[0x15] * 256 * 256 * 256;

					if ((firstClusters < drive.getTotalClusters() + 2)&&(firstClusters>2)) {
						DWORD firstSectors = drive.getLocationByClus(firstClusters);

						////////////提取文件的长度//////////////
						DWORD length = directoryStr[0x1c] + directoryStr[0x1d] * 256 + directoryStr[0x1e] * 256 * 256
							+ directoryStr[0x1f] * 256 * 256 * 256;


						//计算文件所占扇区的个数
						int num = length / SECTIONSIZE;
						if (length % SECTIONSIZE > 0) {
							num++;
						}

						cout << "文件长度为：" << num << endl;
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

			cout << "扇区：" << thisSector << endl;
		}

		
		str = buffer[i];


		
		while ((p->next[str]==NULL) && (p != root)) {
			p = p->fail;
			
		}

		if (p->next[str] != NULL) {
			p = p->next[str];
			if (p->count == 1) {
				cout << "0x504b0304位置为：" << sectorsNumber * SECTIONSIZE + i << endl;
			}
			else if (p->count == 2) {
				cout << "0x504b0102位置为：" << sectorsNumber * SECTIONSIZE + i << endl;
			}
			else if (p->count == 3) {
				cout << "0x504b0506位置为：" << sectorsNumber * SECTIONSIZE + i << endl;
			}
		}
	}

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	cout << asctime(timeinfo) << endl;
	return number;
}



void SundaySearch(byte* buffer, int length, DWORD sectorsNumber, byte* sectorsIdentity, Drives drive) {
	//printf("       数组地址为：%xzz", buffer);

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

	map<int, bool>::iterator iter;



	while (i <= length - length2) {
		if (i % SECTIONSIZE == 0) {
			//cout << "扇区为：" << sectorsNumber + i / SECTIONSIZE << endl;
		}
		if (sectorsIdentity[sectorsNumber + i / SECTIONSIZE] == 1) {//如果是标识的扇区，就跳过
			i += SECTIONSIZE - i % SECTIONSIZE;
			//cout << "扇区为：" << sectorsNumber + i / SECTIONSIZE << endl;
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
				cout << sectorsNumber + i / SECTIONSIZE << ":" << i % SECTIONSIZE << endl;
				cout << "匹配成功,0x50db0304"<< endl;


				//根据偏移读取zip文件的信息

				byte zipHead[30];

				DWORD offsetNumber = sectorsNumber * SECTIONSIZE + i;
				if (!drive.getDriveByOffset(zipHead, offsetNumber, 30)) {
					cout << "读取磁盘有误！" << endl;
				}
				else {
					ZipModule zipFile;
					zipFile.setLocation(offsetNumber);
					zipFile.parseZip(zipHead);
					int headAddName = zipFile.getFileNameLength() + 30;
					byte* headAddNameBuffer = new byte[headAddName];
					if (!drive.getDriveByOffset(headAddNameBuffer, offsetNumber, headAddName)) {
						cout << "读取磁盘有误！" << endl;

					}
					zipFile.parseZipName(headAddNameBuffer);
					
					ReadWriteCSV mycsvW("test.csv", 1);

					queue<string> writeLine;
					writeLine.push(zipFile.getFileName());
					writeLine.push(zipFile.getLastTime());
					writeLine.push(to_string(zipFile.getLocation()));
					writeLine.push(to_string(zipFile.getUnCompressSize()));
					writeLine.push(to_string(zipFile.getCompressSize()));
					writeLine.push(to_string(zipFile.getZipSize()));
					
					mycsvW.writeCSV(writeLine);
					mycsvW.close();

					cout << zipFile.getFileName() << endl;
					cout << "文件长度为：" << zipFile.getZipSize() << endl;

					ReadWriteCSV mycsvR("test.csv", 0);
					string lineData;
					while ((lineData = mycsvR.readCSV()) != "") {
						cout << lineData << endl;
					}

					

					delete headAddNameBuffer;
				}
				
				match++;
			}
			else if(buffer[i + length2] == 0x01 && buffer[i + length2 + 1] == 0x02){
				cout << "匹配成功,0x50db0102" << endl;
				match++;
			}
			else if (buffer[i + length2] == 0x05 && buffer[i + length2 + 1] == 0x06) {
				cout << "匹配成功,0x50db0506" << endl;
				match++;
			}
		}

		middle = buffer[i + length2];
		if (move[middle] == -1) {//末尾下一位不匹配
			i += length2 + 1;
		}
		else {
			i += move[middle];
		}
		

	}
	if (match > 0) {
		cout << "匹配成功字符串个数为：" << match << endl;
	}
	
	delete buffer;
	semaphore.signal();
}