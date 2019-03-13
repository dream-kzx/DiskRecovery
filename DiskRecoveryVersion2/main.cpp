#include<iostream>

#include<Windows.h>
#include"Drives.h"
#include"recoveryOne.h"
#include"recoveryTwo.h"
#include"recoveryThree.h"
using namespace std;


int _tmain(void) {
	//初始化获取分区信息
	Drives* drives = Drives::getDrivesMessage();
	int num = Drives::num;
	int judge[10] = { 0 };
	PDirectoryStruct directoryHead = NULL;	//链式保存匹配要删除标志的文件和目录，为链表头

	vector<PDirectoryStruct> test;
	//for(int i=0;i<num;i++) {
	//	cout << "磁盘：";
	//	wcout << drives[i].getName()[0] << endl;
	//	cout << "文件系统为：" << drives[i].getFileSystem()<< endl;
	//	cout << "分区大小为：" << drives[i].getAllSize() << "G" << endl;
	//	cout << "可用分区大小为：" << drives[i].getAvailableSize() << "G" << endl;
	//	cout << "磁盘类型：" << typeMessage[drives[i].getdrivesType()] << endl;
	//	cout << "总簇的个数：" << drives[i].getTotalClusters() << endl;
	//	cout << "可用簇的个数：" << drives[i].getFreeClusters() << endl;
	//	cout << "每个簇的扇区数：" << drives[i].getSectPerClust() << endl;
	//	cout << endl;
	//}

	int j = 0;
	for (int i = 0; i < num; i++) {
		if ((drives[i].getdrivesType() == 2) && (drives[i].getFileSystem().compare("FAT32") == 0)) {
			judge[i] = 1;
			cout << j + 1 << "、" << "磁盘：";
			wcout << drives[i].getName()[0] << endl;
			cout << "文件系统为：" << drives[i].getFileSystem() << endl;
			cout << "分区大小为：" << drives[i].getAllSize() << "G" << endl;
			cout << "可用分区大小为：" << drives[i].getAvailableSize() << "G" << endl;
			cout << "磁盘类型：" << typeMessage[drives[i].getdrivesType()] << endl;
			cout << endl;
			j++;
		}
	}

	int ok = -1;
	cout << endl << "请选择磁盘" << endl;
	cin >> ok;
	if ((ok < 1) && (ok > j)) {
		cout << "输入有误" << endl;
	}
	for (int i = 0, j = 1; i < num; i++) {
		if (judge[i] == 1) {
			if (j == ok) {
				drives[i].setFirstSectByClust();
				cout << "请选择恢复类型：" << endl;
				cout << "1.删除恢复" << endl;
				cout << "2.格式化后的恢复" << endl;
				cout << "3.office片段匹配" << endl;

				cout << "请选择：" << endl;
				int takeOk = 1;
				cin >> takeOk;
				switch (takeOk)
				{
				case 1:
					disposeRootDirectory(drives[i], directoryHead);
					recoverFile(drives[i], directoryHead);
					break;
				case 2:
					
					matchFile(drives[i],test);
					recoveryFileInTwo(drives[i],test);
					break;
				case 3:
					//matchFileFragmentation(drives[i]);
					recoveryFileInThree(drives[i], "test.csv");
				default:
					break;
				}
				break;
			}
			j++;
		}
	}


	cout << "测试完毕!" << endl;
	return 0;
}






