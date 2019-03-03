//#include<Windows.h>
//#include<string>
//#include<iostream>
//#include<queue>
//#include<map>
//#include<string>
//#include<stdlib.h>
//#include<fstream>
//
//#define MyDict map<byte, acNode*>
//
//using namespace std;
//
//struct acNode {
//	acNode* fail;
//	MyDict next;
//	byte str;
//	int count;
//
//	acNode() {
//		fail = NULL;
//		count = 0;
//		str = '\0';
//	}
//};
//
//
////建立时插入字符串，构建树
//void insert(char* str, acNode* root) {
//	acNode* p = root;
//	MyDict::iterator iter;
//	byte index;
//
//	for (int i = 0; str[i]!='\0'; i++) {
//		index = str[i];
//
//		iter = p->next.find(index);
//		if (iter == p->next.end()) {
//			acNode* q = new acNode();
//			q->str = index;
//			p->next[index] = q;
//			p = q;
//		}
//		else {
//			p = iter->second;
//		}
//	}
//	p->count++;
//}
//
////构建fail
//void buildACFail(acNode* root) {
//	if (root == NULL) {
//		return;
//	}
//
//	root->fail = root;
//
//	queue<acNode*> acQueue;
//	MyDict::iterator iter;
//
//	for (iter = root->next.begin(); iter != root->next.end(); iter++) {
//		iter->second->fail = root;
//		acQueue.push(iter->second);
//	}
//
//	/*for (int i = 0; i < ACSIZE; i++) {
//		if (root->next[i] != NULL) {
//			root->next[i]->fail = root;
//			acQueue.push(root->next[i]);
//		}
//	}*/
//
//	while (!acQueue.empty()) {
//		acNode* p = acQueue.front();
//		acQueue.pop();
//
//		for (iter = p->next.begin(); iter != p->next.end(); iter++) {
//			MyDict::iterator childIter;
//			acNode* q;
//
//			q = iter->second;
//			byte str = q->str;
//
//			childIter = p->fail->next.find(str);
//			if (childIter != p->fail->next.end()) {
//				q->fail = childIter->second;
//			}
//			else {
//				q->fail = p->fail;
//			}
//			acQueue.push(q);
//		}
//
//	}
//}
//
//
//int searchKeyword(byte* buffer, int length, acNode* root) {
//	
//	MyDict::iterator iter;
//
//	acNode* p = root;
//	byte str;
//	int number = 0;
//
//
//	for (int i = 0; i < length; i++) {
//		str = buffer[i];
//
//	
//		iter = p->next.find(str);
//		while ((iter == p->next.end()) && (p != root)) {
//			p = p->fail;
//			iter = p->next.find(str);
//		}
//
//		if (iter != p->next.end()) {
//			p = iter->second;
//			if (p->count > 0) {
//				number++;
//				cout << "位置为：" << i << endl;
//			}
//		}
//	}
//
//
//	//cout << "匹配到的个数为：" << number << endl;
//	return number;
//}
//
//
//int main(void) {
//
//	acNode* root = new acNode();
//	//char str1[] = { 0x50,0x4B,0x03,0x04,'\0' };
//	//insert(str1, root);
//	char str2[] = { 0x50,0x4B,0x01,0x02,'\0' };
//	insert(str2, root);
//	//char str3[] = { 0x50,0x4B,0x05,0x06,'\0' };
//	//insert(str3, root);
//
//
//	buildACFail(root);
//
//	ifstream readfile;
//
//	readfile.open("test1.docx", ios::binary);
//
//	byte buffer[512];
//
//	int number = 0;
//	int i = 0;
//	while (readfile) {
//		readfile.read((char*)buffer, 512);
//		number+=searchKeyword((byte*)buffer, 512, root);
//		cout << i << endl;
//		i++;
//	}
//	
//	cout << "匹配到：" << number << endl;
//	
//	readfile.close();
//	
//
//	return 0;
//
//}