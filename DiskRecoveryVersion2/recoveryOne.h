#include"Drives.h"
#include"Directory.h"
#include"tools.h"



//处理根目录，检索添加删除标记的文件名
BOOL disposeRootDirectory(Drives& drive, PDirectoryStruct& directoryHead);

//处理子目录，检索添加删除标记的文件名
int disposeDirectory(Drives drive, PDirectoryStruct& parent, DWORD directoryLocation, int n);


//根据文件或目录的16进制码，转换文件或目录的信息
void getDetailMessage(PDirectoryStruct& PNode);



//恢复文件
BOOL recoverFile(Drives drive, PDirectoryStruct directoryHead);









