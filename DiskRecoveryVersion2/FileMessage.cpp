#include "FileMessage.h"



FileMessage::FileMessage()
{
}


FileMessage::~FileMessage()
{
}


byte* FileMessage::getFileHead() {
	return fileHead;
}
void FileMessage::setFileHead(byte* fileHead) {
	memcpy(this->fileHead, fileHead, HEADNUM);
}

byte* FileMessage::getFileEnd() {
	return fileEnd;
}
void FileMessage::setFileEnd(byte* fileEnd) {
	memcpy(this->fileEnd, fileEnd, ENDNUM);

}

int* FileMessage::getFileLength() {
	return fileLength;
}
void FileMessage::setFileLength(int* fileLength) {
	memcpy(this->fileLength, fileLength, 5);
}


string FileMessage::getSuffix() {
	return suffix;
}
void FileMessage::setSuffix(string suffix) {
	this->suffix = suffix;
}

void FileMessage::setMessage(string suffix, byte* fileHead, byte* fileEnd, int* fileLength) {
	this->suffix = suffix;
	memcpy(this->fileHead, fileHead, HEADNUM);
	memcpy(this->fileEnd, fileEnd, ENDNUM);
	memcpy(this->fileLength, fileLength, 5);
}
