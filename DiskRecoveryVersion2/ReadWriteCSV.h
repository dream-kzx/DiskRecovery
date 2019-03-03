#pragma once
#include<fstream>
#include<queue>

class ReadWriteCSV
{
private:
	std::fstream fp;


public:
	ReadWriteCSV(std::string filename, int type) {
		if (type == 0) {//读取文件
			fp.open(filename,std::ios::in | ios::binary);
		}
		else if (type == 1) {//写文件,追加
			fp.open(filename, std::ios::out | std::ios::app | ios::binary);
		}
		else if(type == 2){//写文件，截断方式
			fp.open(filename, std::ios::out | std::ios::trunc | ios::binary);
		}
	}
	
	~ReadWriteCSV() {};

	std::string readCSV() {
		string lineData;
		if (std::getline(fp, lineData)) {
			return lineData;
		}
		else {
			return "";
		}
	}

	void writeCSV(std::queue<std::string> data) {
		std::string writeLine("");

		while (!data.empty())
		{
			std::string middle = data.front();
			data.pop();
			if (data.empty()) {
				writeLine += middle + "\n";
			}
			else {
				writeLine += middle + ",";
			}
			
		}

		fp.write(writeLine.c_str(),writeLine.size());
	}

	void close() {
		fp.close();
	}
};

