#include<iostream>

#include<Windows.h>
#include"Drives.h"
#include"recoveryOne.h"
#include"recoveryTwo.h"
#include"recoveryThree.h"
using namespace std;


int _tmain(void) {
	//��ʼ����ȡ������Ϣ
	Drives* drives = Drives::getDrivesMessage();
	int num = Drives::num;
	int judge[10] = { 0 };
	PDirectoryStruct directoryHead = NULL;	//��ʽ����ƥ��Ҫɾ����־���ļ���Ŀ¼��Ϊ����ͷ

	vector<PDirectoryStruct> test;
	//for(int i=0;i<num;i++) {
	//	cout << "���̣�";
	//	wcout << drives[i].getName()[0] << endl;
	//	cout << "�ļ�ϵͳΪ��" << drives[i].getFileSystem()<< endl;
	//	cout << "������СΪ��" << drives[i].getAllSize() << "G" << endl;
	//	cout << "���÷�����СΪ��" << drives[i].getAvailableSize() << "G" << endl;
	//	cout << "�������ͣ�" << typeMessage[drives[i].getdrivesType()] << endl;
	//	cout << "�ܴصĸ�����" << drives[i].getTotalClusters() << endl;
	//	cout << "���ôصĸ�����" << drives[i].getFreeClusters() << endl;
	//	cout << "ÿ���ص���������" << drives[i].getSectPerClust() << endl;
	//	cout << endl;
	//}

	int j = 0;
	for (int i = 0; i < num; i++) {
		if ((drives[i].getdrivesType() == 2) && (drives[i].getFileSystem().compare("FAT32") == 0)) {
			judge[i] = 1;
			cout << j + 1 << "��" << "���̣�";
			wcout << drives[i].getName()[0] << endl;
			cout << "�ļ�ϵͳΪ��" << drives[i].getFileSystem() << endl;
			cout << "������СΪ��" << drives[i].getAllSize() << "G" << endl;
			cout << "���÷�����СΪ��" << drives[i].getAvailableSize() << "G" << endl;
			cout << "�������ͣ�" << typeMessage[drives[i].getdrivesType()] << endl;
			cout << endl;
			j++;
		}
	}

	int ok = -1;
	cout << endl << "��ѡ�����" << endl;
	cin >> ok;
	if ((ok < 1) && (ok > j)) {
		cout << "��������" << endl;
	}
	for (int i = 0, j = 1; i < num; i++) {
		if (judge[i] == 1) {
			if (j == ok) {
				drives[i].setFirstSectByClust();
				cout << "��ѡ��ָ����ͣ�" << endl;
				cout << "1.ɾ���ָ�" << endl;
				cout << "2.��ʽ����Ļָ�" << endl;
				cout << "3.officeƬ��ƥ��" << endl;

				cout << "��ѡ��" << endl;
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


	cout << "�������!" << endl;
	return 0;
}






