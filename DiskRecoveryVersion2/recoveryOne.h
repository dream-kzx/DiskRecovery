#include"Drives.h"
#include"Directory.h"
#include"tools.h"



//�����Ŀ¼���������ɾ����ǵ��ļ���
BOOL disposeRootDirectory(Drives& drive, PDirectoryStruct& directoryHead);

//������Ŀ¼���������ɾ����ǵ��ļ���
int disposeDirectory(Drives drive, PDirectoryStruct& parent, DWORD directoryLocation, int n);


//�����ļ���Ŀ¼��16�����룬ת���ļ���Ŀ¼����Ϣ
void getDetailMessage(PDirectoryStruct& PNode);



//�ָ��ļ�
BOOL recoverFile(Drives drive, PDirectoryStruct directoryHead);









