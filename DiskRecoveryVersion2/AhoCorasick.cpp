#include "AhoCorasick.h"


//建立时插入字符串，构建树
void insert(char* str, acNode* root, int identity) {
	acNode* p = root;
	
	byte index;

	for (int i = 0; str[i] != '\0'; i++) {
		index = str[i];

		if ( p->next[index]==NULL ) {
			acNode* q = new acNode();
			/*q->str = index;*/
			p->next[index] = q;
			p = q;
		}
		else {
			p = p->next[index];
		}
	}
	p->count = identity;
}

//构建fail
void buildACFail(acNode* root) {
	if (root == NULL) {
		return;
	}

	root->fail = root;

	queue<acNode*> acQueue;
	

	for ( int i = 0; i < 256;i++ ) {
		if (root->next[i] != NULL) {
			root->next[i]->fail = root;
			acQueue.push(root->next[i]);
		}
		
	}


	while (!acQueue.empty()) {
		acNode* p = acQueue.front();
		acQueue.pop();

		for (int i = 0; i < 256; i++) {
			
			if (p->next[i] != NULL) {
				if (p->fail->next[i] != NULL) {
					p->next[i]->fail = p->fail->next[i];
				}
				else {
					p->next[i]->fail = p->fail;
				}
				acQueue.push(p->next[i]);
			}

		}

	}
}


int searchKeyword(byte* buffer, int length, acNode* root) {



	acNode* p = root;
	byte str;
	int number = 0;


	for (int i = 0; i < length; i++) {
		str = buffer[i];

		while ((p->next[str]==NULL) && (p != root)) {
			p = p->fail;
		}

		if ( p->next[str] != NULL ) {
			p = p->next[str];
			if (p->count > 0) {
				number++;
				cout << "位置为：" << i << endl;
			}
		}
	}

	return number;
}