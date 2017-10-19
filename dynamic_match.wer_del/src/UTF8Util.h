#pragma once

/**
* UTF8 string utilities
* @ingroup opencc_cpp_api
*/

#include <string.h>
#include <stdio.h>
//#include <tchar.h>
#include <string>
#include <iostream>
#include <vector>
//#include <conio.h>
#include <sstream>
#include <fstream>

//#define StringW vector<wchar_t>;
//#define StringA vector<char>;

using namespace std;

class UTF8Util
{

public:
//	UTF8Util(void);
//	~UTF8Util(void);

	/**
	* Detect UTF8 BOM and skip it.
	*/
	static void SkipUtf8Bom(FILE* fp);

    /*
     *  第x个元素 x = 1, 2, 3
     * */
    static string GetChar(string str, int x)
    {
        string tmp;
        if(1 == x)
        {
          tmp = UTF8Util::NextNChars(str.c_str(), 1);
        }
        else
        {
            tmp = UTF8Util::SkipNChars(str.c_str(), x-1);
            tmp = UTF8Util::NextNChars(tmp.c_str(), 1);
        }
        return tmp;
    }

	/**
	* Return the length of the str
	*/
	static size_t StrCharLength(const char* str)
	{
		size_t length_count = 0;
		//size_t iter_i = 0;
		//size_t length = 0;

		const char* inbuff = str;
		const char* pbuff = FindNextInline(inbuff, '\t');
		//length = static_cast<size_t>(pbuff-inbuff);
		while(inbuff != pbuff)
		{
			//size_t charLength = UTF8Util.NextCharLength(inbuff);
			inbuff = NextChar(inbuff);
			length_count += 1;
		}
		return length_count;
	}

	//static string NextNChars(const char* str, size_t length)
	//{
	//	size_t length_count = 0;
	//	size_t iter_i = 0;
	//	string inbuff;
	//	inbuff.resize(strlen(str));
	//	strncpy(const_cast<char*>(inbuff.c_str()), str, strlen(str));
	//	//cout<<"str=\t"<<str<<", length=\t"<<StrCharLength(str)<<endl;
	//	//cout<<"inbuff=\t"<<inbuff<<", length=\t"<<StrCharLength(inbuff.c_str())<<endl;
	//	const char* pbuff = FindNextInline(inbuff.c_str(), '\t');
	//	//cout<<"Here!"<<endl;
	//	//cout<<"pbuff=\t"<<pbuff<<endl;
	//	while(inbuff != pbuff)
	//	{
	//		//cout<<"inbuff=\t"<<inbuff<<endl;
	//		size_t charLength = NextCharLength(inbuff.c_str());
	//		inbuff = NextChar(inbuff.c_str());
	//		length_count += 1;
	//		iter_i += charLength;

	//		if(length_count >= length)
	//		{
	//			string outbuff;
	//			outbuff.resize(iter_i);
	//			strncpy(const_cast<char*>(outbuff.c_str()), str, iter_i);
	//			//cout<<"outbuff=\t"<<outbuff<<endl;
	//			return outbuff;
	//			//return FromSubstr(str, iter_i);
	//		}
	//	}
	//	//cout<<"return"<<endl;
	//	return str;
	//}

	static string NextNChars(const char* str, size_t length)
	{
		size_t length_count = 0;
		//size_t iter_i = 0;

		const char* tempStr = str;
		while(length_count < length)
		{
			length_count += 1;
			//cout<<"tempStr="<<tempStr<<endl;
			tempStr = NextChar(tempStr);
		}

		return FromSubstr(str, tempStr-str);
	}

	static string PrevNChars(const char* str, size_t length)
	{
		size_t length_count = 0;
		
		const char* tempStr = str;
		while(length_count < length)
		{
			length_count += 1;
			tempStr = PrevChar(tempStr);
		}

		return tempStr;
	}


	/**
	* Return the length of the str
	*/
	static const char* SkipNChars(const char* str, size_t length)
	{
		size_t length_count = 0;

		const char* inbuff = str;
		const char* pbuff = FindNextInline(inbuff, '\t');
		while(inbuff != pbuff)
		{
			inbuff = NextChar(inbuff);
			length_count += 1;
			if(length_count >= length)
			{
				return inbuff;
			}
		}
		return str;
	}

	/**
	* Returns the length in byte for the next UTF8 character.
	* On error returns 0.
	*/
	static size_t NextCharLengthNoException(const char* str) {
		char ch = *str;
		if ((ch & 0x80) == 0x00) {
			return 1;
		} else if ((ch & 0xE0) == 0xC0) {
			return 2;
		} else if ((ch & 0xF0) == 0xE0) {
			return 3;
		} else if ((ch & 0xF8) == 0xF0) {
			return 4;
		} else if ((ch & 0xFC) == 0xF8) {
			return 5;
		} else if ((ch & 0xFE) == 0xFC) {
			return 6;
		}
		return 0;
	}
  
	/**
	* Returns the length in byte for the next UTF8 character.
	*/
	static size_t NextCharLength(const char* str) {
		size_t length = NextCharLengthNoException(str);
		if (length == 0) {
			//throw InvalidUTF8(str);
			//yqw 20150817 抛出异常，注释
		}
		return length;
	}

	/**
	* Returns the length in byte for the previous UTF8 character.
	*/
	static size_t PrevCharLength(const char* str) {
		for (size_t i = 1; i <= 6; i++) {
			str--;
			size_t length = NextCharLengthNoException(str);
			if (length == i) {
				return length;
			}
		}
		//throw InvalidUTF8(str);
		return 1;
		//yqw 20150817 抛出异常，注释
	}

	/**
	* Returns the char* pointer over the next UTF8 character.
	*/
	static const char* NextChar(const char* str) {
		return str + NextCharLength(str);
	}

	/**
	* Move the char* pointer before the previous UTF8 character.
	*/
	static const char* PrevChar(const char* str) {
		return str - PrevCharLength(str);
	}

	 /**
	 * Finds a character in the same line.
	* @param str The text to be searched in.
	* @param ch  The character to find.
	* @return    The pointer that points to the found chacter in str or EOL/EOF.
	*/
	static const char* FindNextInline(const char* str, const char ch) {
		while (!IsLineEndingOrFileEnding(*str) && *str != ch) {
			str = NextChar(str);
		}
		return str;
	}

	/**
	* Returns ture if the character is a line ending or end of file.
	*/
	static bool IsLineEndingOrFileEnding(const char ch) {
		return ch == '\0' || ch == '\n' || ch == '\r';
	}

	/**
	* Copies a substring with given length to a new std::string.
	*/
	static string FromSubstr(const char* str, size_t length) {
		string newStr;
		newStr.resize(length);
		strncpy(const_cast<char*>(newStr.c_str()), str, length);
		return newStr;
	}

	/**
	* Returns true if the given string is longer or as long as the given length.
	*/
	static bool NotShorterThan(const char* str, size_t length) {
		while (length > 0) {
			if (*str == '\0') {
				return false;
			}
			length--;
			str++;
		}
		return true;
	}

	/**
	* Truncates a string with a maximal length.
	* No UTF8 character will be broken.
	*/
	static string TruncateUTF8(const char* str, size_t maxLength) {
		string wordTrunc;
		if (NotShorterThan(str, maxLength)) {
			size_t len = 0;
			const char* pStr = str;
			while (len < maxLength) {
				size_t nextLen = NextCharLength(pStr);
				pStr += nextLen;
				len += nextLen;
			}
			wordTrunc = FromSubstr(str, len);
		} else {
			wordTrunc = str;
		}
		return wordTrunc;
	}

	/**
	* Replaces all patterns in a string in place.
	*/
	static void ReplaceAll(string& str, const char* from, const char* to) {
		string::size_type pos = 0;
		string::size_type fromLen = strlen(from);
		string::size_type toLen = strlen(to);
		while ((pos = str.find(from, pos)) != string::npos) {
			str.replace(pos, fromLen, to);
			pos += toLen;
		}
	}

	/**
	* Joins a string vector in to a string with a separator.
	*/
	static string Join(const vector<string>& strings, const string& separator) {
		std::ostringstream buffer;
		bool first = true;
		//yqw 20150817
		for(int index=0; index<strings.size(); ++index)
		{
			string str = strings.at(index);
			if(!first)
			{
				buffer << separator;
			}
			buffer << str;
			first = false;
		}
		return buffer.str();

//		for (const &str : strings) {
//			if (!first) {
//				buffer << separator;
//			}
//			buffer << str;
//			first = false;
//		}
//		return buffer.str();
	}

	/**
	* Joins a string vector in to a string.
	*/
	static string Join(const vector<string>& strings) {
		std::ostringstream buffer;
		for(int index=0; index<strings.size(); ++index)
		{
			string str = strings.at(index);
			buffer << str;
		}
		return buffer.str();

//		for (const auto& str : strings) {
//			buffer << str;
//		}
//		return buffer.str();

	}
};

