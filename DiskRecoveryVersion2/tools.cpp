#include "tools.h"

void copyStr(TCHAR* str1, TCHAR* str2) {
	for (int i = 0; i < sizeof(str2); i++) {
		str1[i] = str2[i];
	}
}


void copyStr(byte* str1, byte* str2) {
	for (int i = 0; i < sizeof(str2); i++) {
		str1[i] = str2[i];
	}
}

//��unsigned char����ת��Ϊstring
string ucharToString(unsigned char* str, int n) {
	string s = "";

	for (int i = 0; i < n; i++) {
		s += str[i];
	}
	return s;
}

//��unsigned char����ת��Ϊstring,����typeת����Сд
string ucharToString(unsigned char* str, int n, byte type) {
	string s = "";

	if (n <= 8) {//������ļ���
		if ((type == 0x18)||(type == 0x08)) {//����ļ�����Сд�������ת��
			for (int i = 0; i < n; i++) {
				if (str[i] >= 'A'&&str[i] <= 'Z') {
					s += (str[i] - 'A' + 'a');
				}
				else {
					s += str[i];
				}
				
			}
		}
		else {//����ļ����Ǵ�д
			for (int i = 0; i < n; i++) {
				s += str[i];
			}
		}
	}
	else {//������ļ�
		int middle = 8;
		//�������ҡ�.����λ��
		for (int i = 0; i < n; i++) {
			if (str[i] == '.') {
				middle = i;
				break;
			}
		}

		for (int i = 0; i < middle; i++) {//�����ļ���
			if ((type == 0x18) || (type == 0x08)) {//����ļ�����Сд�������ת��
				if (str[i] >= 'A'&&str[i] <= 'Z') {
					s += (str[i] - 'A' + 'a');
				}
				else {
					s += str[i];
				}
			}
			else {//����ļ����Ǵ�д
				s += str[i];
			}
		}

		for (int i = middle; i < n; i++) {//�����ļ���׺
			if ((type == 0x18) || (type == 0x10)) {//�����׺��Сд�������ת��
				if (str[i] >= 'A'&&str[i] <= 'Z') {
					s += (str[i] - 'A' + 'a');
				}
				else {
					s += str[i];
				}
			}
			else {//�����׺���Ǵ�д
				s += str[i];
			}
		}
	}


	return s;
}

/////////////////////////��TCHARתΪstring����/////////////////////////////////
string tcharToString(TCHAR *str)
{
	if (str == NULL) {
		return "\0";
	}
	//��ȡת���ĳ���
	int iLen = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);

	//����һ�������鱣��ת������ַ�
	char* chRtn = new char[iLen * sizeof(char)];

	//��ת�����ַ�������������
	WideCharToMultiByte(CP_ACP, 0, str, -1, chRtn, iLen, NULL, NULL);

	//�������ʽ��Ϊstring����
	string str1(chRtn);

	return str1;
}

//��char�д洢��unicode����wchar_t�洢
void charSaveWchar_t(wchar_t*& str1, unsigned char* str2, int n) {
	str1 = new wchar_t[n / 2 + 1];
	for (int i = 0, j = 0; i < (n / 2 + 1); i++, j += 2) {
		str1[i] = str2[j] + str2[j + 1] * 16 * 16;
	}
}



//��char�д洢��unicode����wchar_t�洢
void charToWchar_t(wchar_t*& str1, const char* str2, int n) {
	int length = n + 1;
	str1 = new wchar_t[length];
	memset(str1, 0, length * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, str2, n, str1, length);

}

//ȥ���ַ����ո�
BOOL myTrim(string& s,string str) {
	if (s.empty()) {
		return TRUE;
	}
	int x = s.find_last_of(str);
	if (x >= 0) {
		s.erase(x, 1);
		myTrim(s, str);
	}
	else {
		return TRUE;
	}
}

//��string���͵��ַ���������Ƭ
vector<string> split(const string& src, string separate_character)
{
	vector<string> strs;

	int separate_characterLen = separate_character.size();//�ָ��ַ����ĳ���,�����Ϳ���֧���硰,,�����ַ����ķָ���
	int lastPosition = 0, index = -1;
	while (-1 != (index = src.find(separate_character, lastPosition)))
	{
		strs.push_back(src.substr(lastPosition, index - lastPosition));
		lastPosition = index + separate_characterLen;
	}
	string lastString = src.substr(lastPosition);//��ȡ���һ���ָ����������
	if (!lastString.empty()) {
		strs.push_back(lastString);//������һ���ָ����������ݾ����
	}


	return strs;
}

//����ȡ��Ŀ¼��ǰ3���ֽڶ�Ϊ0ʱ����ʾ��Ŀ¼��Ϣ�Ѷ���
BOOL readDirectoryEnd(byte* buffer) {
	if (buffer[0] == 0 && buffer[1] == 0 && buffer[2] == 0) {
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//�����Ÿ�Ϊ�����ţ��������.csv�ļ�
BOOL replaceComma(string& str) {
	if (str.empty()) {
		return TRUE;
	}
	int x = str.find(",");

	if (x >= 0) {
		str[x] = '\'';
		replaceComma(str);
	}
	else {
		return TRUE;
	}
}

//�����ַ��е�'\0','\n','\r'
BOOL disposeStr(string& str) {
	if (str.empty()) {
		return TRUE;
	}
	int y = str.find("\r");
	int x = str.find("\n");
	int z = str.find('\0');
	if (x >= 0) {
		str.erase(x, 1);
		disposeStr(str);
	}
	else if (y >= 0) {
		str.erase(y, 1);
		disposeStr(str);
	}
	else if (z >= 0) {
		str.erase(z, 1);
		disposeStr(str);
	}
	else {
		return TRUE;
	}
}


//�����ʱ���������ַ���16bit
//0~4bitΪ�룬5~10bit Ϊ�֣�11~15bit Ϊʱ
//�õ���ʱ��Ϊhh/mm/ss
string parseTime(unsigned char* time) {
	int second = (time[0] & 0b00011111) * 2;
	int min = (time[0] >> 5) + ((time[1] & 0b00000111) << 3);
	int hour = time[1] >> 3;
	if (second > 60 || min > 60 || hour > 24) {
		return "";
	}
	char okTime[9];
	sprintf_s(okTime, "%02d:%02d:%02d", hour, min, second);

	return okTime;
}

//�����������2���ַ���16bit
//0~4bit Ϊ��,5~8bit Ϊ��,9~15bit Ϊ��+1980
string parseDate(unsigned char* date) {
	int day = date[0] & 0b00011111;
	int month = (date[0] >> 5) + ((date[1] & 0b0001) << 1);
	int year = (date[1] >> 1) + 1980;
	if (year <= 1980 || month > 12 || day > 31) {
		return "";
	}
	char okDate[11];

	sprintf_s(okDate, "%04d/%02d/%02d", year, month, day);

	return okDate;
}

string parseDateTime(byte* dateTime) {
	FILETIME temp;
	SYSTEMTIME result;

	int time = dateTime[0] + dateTime[1] * 256;
	int date = dateTime[2] + dateTime[3] * 256;

	DosDateTimeToFileTime(date, time, &temp);
	FileTimeToSystemTime(&temp, &result);

	if (result.wDay > 9999 || result.wMonth > 12 || result.wDay > 31 
		|| result.wHour > 24 || result.wMinute > 60 || result.wSecond > 60) {
		return "";
	}

	char okDateTime[21];
	sprintf_s(okDateTime, "%04d/%02d/%02d %02d:%02d:%02d", result.wYear,result.wMonth,result.wDay,
		result.wHour,result.wMinute,result.wSecond);
	return okDateTime;
}


//����4�ֽڵĳ���
DWORD parseLength4(byte* length) {
	return length[0] + length[1] * 256 + length[2] * 256 * 256 + length[3] * 256 * 256 * 256;
}

//����2�ֽڵĳ���
int parseLength2(byte* length) {
	return length[0] + length[1] * 256;
}

//���ֽ�תascii
std::string WideByte2Acsi(std::wstring& wstrcode)
{
	int asciisize = ::WideCharToMultiByte(CP_OEMCP, 0, wstrcode.c_str(), -1, NULL, 0, NULL, NULL);
	if (asciisize == ERROR_NO_UNICODE_TRANSLATION)
	{
		throw std::exception("Invalid UTF-8 sequence.");
	}
	if (asciisize == 0)
	{
		throw std::exception("Error in conversion.");
	}
	std::vector<char> resultstring(asciisize);
	int convresult = ::WideCharToMultiByte(CP_OEMCP, 0, wstrcode.c_str(), -1, &resultstring[0], asciisize, NULL, NULL);

	if (convresult != asciisize)
	{
		throw std::exception("La falla!");
	}

	return std::string(&resultstring[0]);
}


//utf-8תunicode
std::wstring Utf82Unicode(const std::string& utf8string)
{
	int widesize = ::MultiByteToWideChar(CP_UTF8, 0, utf8string.c_str(), -1, NULL, 0);
	if (widesize == ERROR_NO_UNICODE_TRANSLATION)
	{
		throw std::exception("Invalid UTF-8 sequence.");
	}
	if (widesize == 0)
	{
		throw std::exception("Error in conversion.");
	}

	std::vector<wchar_t> resultstring(widesize);

	int convresult = ::MultiByteToWideChar(CP_UTF8, 0, utf8string.c_str(), -1, &resultstring[0], widesize);

	if (convresult != widesize)
	{
		throw std::exception("La falla!");
	}

	return std::wstring(&resultstring[0]);
}

//utf-8תascii
std::string UTF_82ASCII(std::string& strUtf8Code)
{
	std::string strRet("");
	//�Ȱ� utf8 תΪ unicode  
	std::wstring wstr = Utf82Unicode(strUtf8Code);
	//���� unicode תΪ ascii  
	strRet = WideByte2Acsi(wstr);
	return strRet;
}


int utf8_check(const char* str, size_t length) {
	size_t i;
	int nBytes;
	unsigned char chr;

	i = 0;
	nBytes = 0;
	while (i < length) {
		chr = *(str + i);

		if (nBytes == 0) { //�����ֽ���
			if ((chr & 0x80) != 0) {
				while ((chr & 0x80) != 0) {
					chr <<= 1;
					nBytes++;
				}
				if ((nBytes < 2) || (nBytes > 6)) {
					return 0; //��һ���ֽ�����Ϊ110x xxxx
				}
				nBytes--; //��ȥ����ռ��һ���ֽ�
			}
		}
		else { //���ֽڳ��˵�һ���ֽ���ʣ�µ��ֽ�
			if ((chr & 0xC0) != 0x80) {
				return 0; //ʣ�µ��ֽڶ���10xx xxxx����ʽ
			}
			nBytes--;
		}
		i++;
	}
	return (nBytes == 0);
}


//|| name.find("docProps/")
//|| name.find("word/") || name.find("xl/")
//|| name.find("ppt") || name.find("[Content_Types].xml")
//office�ļ��ж�
bool checkOfficeFragment(const std::string name) {
	if (name.find("_rels/") != string::npos|| name.find("docProps/") != string::npos
		|| name.find("word/") != string::npos|| name.find("xl/") != string::npos
		|| name.find("ppt/") != string::npos|| name.find("[Content_Types].xml") != string::npos) {
		return true;
	}
	else {
		return false;
	}
	
}