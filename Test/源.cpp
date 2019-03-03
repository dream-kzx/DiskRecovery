#include<windows.h>
#include<iostream>
#include<thread>
#include<string>
#include<utility>
#include<functional>
#include<atomic>
#include<chrono>
using namespace std;

//void test(int i) {
//	cout << i << endl;
//}

void f1(int n) {
	for (int i = 0; i < 5; i++) {
		this_thread::sleep_for(chrono::milliseconds(100)); 
		cout << "Thread" << n << " executing\n";

	}
}

//void f2(int& n) {
//	for (int i = 0; i < 5; i++) {
//		cout << "Thread 2 executing\n";
//		++n;
//		this_thread::sleep_for(chrono::milliseconds(10));
//	}
//}

int main(void) {
	int n = 0;
	for (int i = 0; i < 10; i++) {
		
		thread t1(f1, i);
		if (i == 9) {
			t1.join();
		}
		else {
			t1.detach();
		}
		
	}


	return 0;
}






//int main(void) {
//
//	char text[] =  "I love  loveleleleleve fjdslj love fjksdljf!";
//								 
//	char pattern[4] = { 'l','e','l','e' };
//
//	int i = 0;
//	int j = 0;
//	int length1 = sizeof(text);
//	int length2 = sizeof(pattern);
//
//	int move[128];
//
//	memset(move, -1, 128*sizeof(int));
//	for (int k = 0; k < length2; k++) {
//		move[pattern[k]] = length2 - k;
//	}
//
//
//	int match = 0;
//	int compare = 0;
//	int moveNumber = 0;
//	char middle;
//
//	while ( i <= length1 - length2 ) {
//		int k = length2 - 1;
//		for (; k >= 0; k--) {
//			compare++;
//			if (pattern[k] != text[i + k]) {
//				break;
//			}
//		}
//
//		if (k < 0) {
//			cout << "匹配成功,位置为：" << i << endl;
//
//			match++;
//			//i += length2;
//
//			//middle = text[i + length2 + length2 - 1];
//			//if (move[middle] == -1) {//末尾下一位不匹配
//			//	i += 2 * length2;
//			//}
//			//else {
//			//	i += move[middle] + length2 - 1;
//			//}
//			//moveNumber++;
//		}
//		//else {
//		//	middle = text[i + length2];
//		//	if (move[middle] == -1) {//末尾下一位不匹配
//		//		i += length2 + 1;
//		//	}
//		//	else {
//		//		i += move[middle];
//		//	}
//		//	moveNumber++;
//		//}
//		middle = text[i + length2];
//		if (move[middle] == -1) {//末尾下一位不匹配
//			i += length2 + 1;
//		}
//		else {
//			i += move[middle];
//		}
//		moveNumber++;
//
//	}
//
//	cout << "匹配成功字符串个数为：" << match << endl;
//	cout << "比较次数为：" << compare << endl;
//	cout << "移动次数为：" << moveNumber << endl;
//	return 0;
//}