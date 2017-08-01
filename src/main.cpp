#include"werC.h"

const int ENTRY_BUFF_SIZE = 40960;


int main(int argc, char *argv[])
{
    if(argc < 4)
    {
        printf("usage: %s file_label  file_test  file_out", argv[0]);
        return 0;
    }

    int len1,len2;
    int err_count =0;

    //// 替换 插入  删除  
    //const char *str1 = "aaaaabbbbbccccc"; 
    //const char *str2 = "ammaabbmmmbbbccc"; 
    const char *str1 = "一二三四五六七八九十"; 
    const char *str2 = "一mm四五八九mm十"; 

    printf("测试函数GetChar:str1=%s\n",str1);
    for(int ii=1; ii <= UTF8Util::StrCharLength(str1); ii++)
    {
        string tmp = UTF8Util::GetChar(str1, ii);
        printf("str[%d]=%s\n",ii,tmp.c_str());
    }
    printf("测试函数GetChar:str2=%s\n",str2);
    for(int ii=1; ii <= UTF8Util::StrCharLength(str2); ii++)
    {
        string tmp = UTF8Util::GetChar(str2, ii);
        printf("str[%d]=%s\n",ii,tmp.c_str());
    }


    err_count = lcs_string(str1,str2, len1, len2);
    printf("str1=%s\tstr2=%s\n",str1, str2);
	printf("err_count=%d, len1=%d len2=%d\n",err_count, len1, len2);


    //str1 = "abcdefghij"; 
    //str2 = "ammmcdmfj"; 
    //err_count = lcs_string(str1,str2, len1, len2);
    //printf("str1=%s\tstr2=%s\n",str1, str2);
	//printf("err_count=%d, len1=%d len2=%d\n",err_count, len1, len2);
    

	char buff[ENTRY_BUFF_SIZE];

	FILE* fp = fopen(argv[1], "r");
	FILE* fpw = fopen(argv[3], "w");

	UTF8Util::SkipUtf8Bom(fp);
	while(fgets(buff, ENTRY_BUFF_SIZE, fp))
	{

		const char* inbuff = buff;
		size_t length;

		const char* pbuff = UTF8Util::FindNextInline(inbuff, '\n');
		if(UTF8Util::IsLineEndingOrFileEnding(*pbuff))
		{}
		
		length = static_cast<size_t>(pbuff-inbuff);
        // 获取 inbuff 后面 length 个字 
		string key = UTF8Util::FromSubstr(inbuff, length);
	
		string seg_str = key;

		//cout << seg_str << endl;
		fwrite(seg_str.c_str(), seg_str.length(), 1, fpw);
		fwrite("\n", 1, 1, fpw);

	}
	

	fclose(fp);
	fclose(fpw);





}
