#include"werC.h"
#include"common.h"

const int ENTRY_BUFF_SIZE = 40960;

// 保存输入文本格式  id:content
typedef struct _Text{
    string id;
    string text;

}Text;

vector<Text> vec_lab;
vector<Text> vec_res;

int file2vec(FILE *fp, vector<Text> &ttt);

/* 在vec中寻找对应id 有返回idx 没有返回-1 */
int find_id(const vector<Text> &vec, string id);


/*
 * 输入两个文件 lab和res  计算对应句子的wer 
 * */
int main(int argc, char *argv[])
{
    if(argc < 4)
    {
        printf("usage: %s file_label  file_test  file_out\n", argv[0]);
        return 0;
    }

    int ret;
    int len1,len2;
    int err_count =0;
	FILE* fp_lab = NULL;
	FILE* fp_res = NULL;
	FILE* fp_out = NULL;

    fp_lab = fopen(argv[1], "r");
    fp_res = fopen(argv[2], "r");
    fp_out = fopen(argv[3], "w");
    if(!fp_lab || !fp_res || !fp_out)
    {
        printf("ERROR:打开文件失败！\n");
        return 0;
    }

    ret = load_map();
    if(ret < 0)
    {
        fprintf(stderr, "load_map failed!\n");
        return 0;
    }

    ret = file2vec(fp_lab, vec_lab);
    ret = file2vec(fp_res, vec_res);

    // 计算 res 中所有数据的wer  
    WER_RES wer_all;
    memset(&wer_all, 0, sizeof(wer_all));
    for(vector<Text>::iterator it = vec_res.begin(); it != vec_res.end(); it++)
    {
        string id = it->id;
        string text_res = it->text;

        // 找到对应的 lab 
        int idx = find_id(vec_lab, id);
        if(idx < 0)
        {
            printf("WARNING:此id=%s 不在 %s 中", id.c_str(), argv[1]);
            continue;
        }
        string text_lab = vec_lab[idx].text;

        fprintf(fp_out, "id=%s\nlab=%s\nres=%s\n", id.c_str(), 
                    text_lab.c_str(), text_res.c_str());

        //err_count = lcs_string(text_lab.c_str(), text_res.c_str(), len1, len2);
        WER_RES wer_res;
        ret = wer(text_lab.c_str(), text_res.c_str(), wer_res);
	fprintf(fp_out, "len_lab=%d len_res=%d err_ins=%d err_del=%d err_rep=%d wer=%.4f\n",
                    wer_res.len_lab, wer_res.len_res, wer_res.err_ins, wer_res.err_del,
                    wer_res.err_rep, double(wer_res.err_all)/double(wer_res.len_lab));

        add_wer(wer_all, wer_res);

    }
    fprintf(fp_out, "===========================================================\n");
	fprintf(fp_out, "len_lab=%d len_res=%d err_ins=%d err_del=%d err_rep=%d wer=%.4f\n",
            wer_all.len_lab, wer_all.len_res, wer_all.err_ins, wer_all.err_del,
            wer_all.err_rep, double(wer_all.err_all)/double(wer_all.len_lab));



	

	fclose(fp_lab);
	fclose(fp_res);
	fclose(fp_out);

    /*
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
    */
    

}

int file2vec(FILE *fp, vector<Text> &vec_lab)
{
	char buff[ENTRY_BUFF_SIZE];
	UTF8Util::SkipUtf8Bom(fp);
	while(fgets(buff, ENTRY_BUFF_SIZE, fp))
	{

		const char* inbuff = buff;
		size_t length;

		const char* pbuff = UTF8Util::FindNextInline(inbuff, '\n');
		if(UTF8Util::IsLineEndingOrFileEnding(*pbuff)){}
		
        // 获取 inbuff 后面 length 个字 
		length = static_cast<size_t>(pbuff-inbuff);
		string key = UTF8Util::FromSubstr(inbuff, length);

        vector<string> vec_tmp;
        split(vec_tmp, key, '\t');
        if(vec_tmp.size() < 2)
        {
            printf("input line format err:%s\n",key.c_str());
            continue;
        }

        Text ttt;
        ttt.id = vec_tmp[0]; 
        ttt.text = vec_tmp[1]; 
        vec_lab.push_back(ttt);


	}

    return 0;
}


int find_id(const vector<Text> &vec, string id)
{

    for(int ii = 0; ii < vec.size(); ii++)
    {
        if(vec[ii].id.compare(id) == 0)
        {
            return ii;
        }
    }
    return -1;
}


