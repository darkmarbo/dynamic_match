#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cstring>
#include <stdio.h>

#include"common.h"
#include"werC.h"

using namespace std;

map<string,string> map_del;
map<string,string> map_rep;

int load_map()
{
    int ret = 0;
    FILE *fp = fopen("wer.conf", "r");
    if(!fp)
    {
        fprintf(stderr, "open wer.conf 失败！");
        return -1;
    }

    fprintf(stderr, "load_map wer.conf :\n");
    char buff[MAX_LEN];
    UTF8Util::SkipUtf8Bom(fp);
    while(fgets(buff, MAX_LEN, fp))
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
        if(vec_tmp.size() < 3)
        {
            printf("input line format err:%s\n",key.c_str());
            ret = -2;
            continue;
        }

        string word = vec_tmp[0];
        int flag = atoi(vec_tmp[1].c_str());
        string word_new = vec_tmp[2];
        if(flag == 0)
        {
            map_del[word] = "";
        }
        else if(flag == 1)
        {
            map_rep[word] = word_new;

        }
        else
        {
            fprintf(stderr, "\tchar==%s==\n", vec_tmp[0].c_str());

        }

    }

    return ret;
}

/*
 * 处理字符串str 去掉内部的空格和标点符号  
 * 将全角英文转换成半角  数字和阿拉伯数字转换
 * */
string str_pro(const string &str)
{

    string str_new = "";
    int len = UTF8Util::StrCharLength(str.c_str());
    for(int ii=1; ii<= len; ii++)
    {
        string ch = UTF8Util::GetChar(str, ii);
        //fprintf(stderr, "ch=%s===\n", ch.c_str());
        if(map_del.count(ch) == 0)
        {
            // 替换 
            if(map_rep.count(ch) == 1)
            {
                ch = map_rep[ch];
            }
            str_new += ch;
        }
    }

    return str_new; 
}

int wer(const char *lab, const char *res, WER_RES &wer_res)
{
	
    int ret = 0;

	//char lab_tmp[MAX_LEN * 2] = {0};
	//snprintf(lab_tmp, MAX_LEN*2, "%s", lab);
	//char res_tmp[MAX_LEN * 2] = {0};
	//snprintf(res_tmp, MAX_LEN*2, "%s", res);

    string lab_tmp = str_pro(lab); 
    string res_tmp = str_pro(res); 
    //fprintf(stderr, "lab=%s\tlab_pro=%s\n", lab, lab_tmp.c_str());

	//convert_to_ArabicNumbers(lab_tmp);	
	//convert_to_ArabicNumbers(res_tmp);
	
	
	ret = lcs_string(lab_tmp.c_str(), res_tmp.c_str(), wer_res);	
	
	return ret;
}

int ch(const char *st)
{
	int ret=int(st[0])*256+st[1];
	//printf("%d\n",ret);
	return ret;
}

/*
 * 累加所有 WER_RES 的值
 * */
int add_wer(WER_RES &wer_all, WER_RES wer_res)
{
   wer_all.len_lab += wer_res.len_lab; 
   wer_all.len_res += wer_res.len_res; 

   wer_all.err_ins += wer_res.err_ins; 
   wer_all.err_del += wer_res.err_del; 
   wer_all.err_rep += wer_res.err_rep; 
   wer_all.err_all += wer_res.err_all; 

}

/**
 * @brief 求两个串的LCS长度 
 *  如果字符串一的所有字符按其在字符串中的顺序出现在另外一个字符串二中，
 *  则字符串一称之为字符串二的子串。
 *  注意，并不要求子串（字符串一）的字符必须连续出现在字符串二中。
 *      a1b2c3d4 与 hm12mm3yy4     最长公共子串  1234
 * @fc1 : 输入串1 (识别结果) 
 * @fc2 : 输入串2 (标注结果) 
 * @return : 0 
 */

int lcs_string(const char *str1, const char *str2, WER_RES &wer_res)
{

    std::string fc1 = str1;
    std::string fc2 = str2;


    fc1 = str_pro(fc1); 
    fc2 = str_pro(fc2); 
    // 两个字符串  字 的个数 
	int cv1 = UTF8Util::StrCharLength(fc1.c_str());
	int cv2 = UTF8Util::StrCharLength(fc2.c_str());

    // 开辟存储空间 
	double **lcs_ff= new double*[MAX_LEN];
	for(int ii=0; ii<MAX_LEN; ii++)
	{
		lcs_ff[ii] = new double[MAX_LEN];
		for(int jj=0; jj<MAX_LEN; jj++)
		{
			lcs_ff[ii][jj]=0;
		}
	}

    // 对应lcs_ff 记录当前最优值 来自于哪里(-1 0 1 )(插入 替换 删除)
	int **lcs_cc= new int*[MAX_LEN];
	for(int ii=0; ii<MAX_LEN; ii++)
	{
		lcs_cc[ii] = new int[MAX_LEN];
		for(int jj=0; jj<MAX_LEN; jj++)
		{
			lcs_cc[ii][jj]=-2;
		}
	}
	

    // 第1行 第1列 初始化为0
    //str1 构成 竖列 1:cv1
	for (int ct0=1; ct0<=cv1; ct0++)
	{
		lcs_ff[ct0][0]=ct0;
        // 删除  
        lcs_cc[ct0][0] = 1;
	}
    // str2 构成 横列 1:cv2
	for (int ct1=1;ct1<=cv2;ct1++)
	{
		lcs_ff[0][ct1]=ct1;
        // 插入  
		lcs_cc[0][ct1]=-1;
	}

    // 动态规划 计算过程
    // 取 小的那个值  最好都是0
    int err_insert = 0;
    int err_delete = 0;
    int err_replace = 0;
    int flag = 0;
	for (int ct0=1;ct0<=cv1;ct0++)
	{
		for (int ct1=1; ct1<=cv2; ct1++)
		{
            // 不相等1 相等0
            // 获取 fc1 的第ct0 个字 
            string tmp_fc1 = UTF8Util::GetChar(fc1, ct0);
            string tmp_fc2 = UTF8Util::GetChar(fc2, ct1);

			double cur_judge = 1;
            if(tmp_fc1.compare(tmp_fc2) == 0)
            {
                // printf("mmmm=%s",tmp_fc1.c_str());
                cur_judge = 0;
            }
            // 替换 
			double mm = lcs_ff[ct0-1][ct1-1] + cur_judge;
            // 删除:  
			double mm0       = lcs_ff[ct0-1][ct1] + 1;
            // 插入:  
			double mm1        = lcs_ff[ct0][ct1-1] + 1;

            if(mm <= mm0 && mm <= mm1)
            {
                // 替换:    str1和str2都增加一个
                lcs_cc[ct0][ct1] = 0;
			    lcs_ff[ct0][ct1] = mm;
            }
            else if (mm1 <= mm && mm1 <= mm0)
            {
                // 插入
                lcs_cc[ct0][ct1] = -1;
			    lcs_ff[ct0][ct1] = mm1;
            }
            else
            {
                // 删除 
                lcs_cc[ct0][ct1] = 1;
			    lcs_ff[ct0][ct1] = mm0;
            }

		}
	}


    /*
    printf("输出 lcs_ff 矩阵:\n");
    for (int ct0=0; ct0<=cv1; ct0++)
	{
		for (int ct1=0; ct1<=cv2;ct1++)
		{
			printf("%02d ",int(lcs_ff[ct0][ct1]));
		}
		printf("\n");
	}

	printf("输出lcs_cc矩阵\n");
    for (int ct0=0;ct0<=cv1;ct0++)
	{
		for (int ct1=0;ct1<=cv2;ct1++)
		{
			printf("%02d ",int(lcs_cc[ct0][ct1]));
		}
		printf("\n");
	}
    */

    std::vector<int> vec_1;
    std::vector<int> vec_2;
	//printf("最优路径回溯\n");
    int ii=cv1;
    int jj=cv2; 


    wer_res.del_st = 0;

    //printf("del_st:\t%s\t%s\n", str1, str2);
    while(ii != 0 && jj != 0)
    {
        int vv = lcs_cc[ii][jj];
        //printf("[%d,%d]=%d\n", ii, jj, vv);
        vec_1.insert(vec_1.begin(),ii);
        vec_2.insert(vec_2.begin(),jj);
        if(vv == 0)
        {
            if(ii>1 && jj>1 && lcs_ff[ii-1][jj-1] == lcs_ff[ii-2][jj-2] + 1)
            {
                err_replace++;
            }
            ii--;
            jj--;
        }
        else if(vv == -1)
        {
            jj--;
            err_insert++;
        }
        else if(vv == 1)
        {


            ii--;
            err_delete++;

        }
        //// 第一个字 是否出现删除错误 
        //// if(jj == 1 && ii == 2) // 删除第二个字
        if(jj == 0 && ii > 0) // 删除第二个字
        {
            wer_res.del_st = 1;
            //printf("del_st\t%s\t%s\n", str1, str2);
            //fprintf(stderr, "\tii=%d\tjj=%d\n", ii, jj);
        }

    }


    /*
    printf("对齐结果:\n");
    for(int ii=0; ii<vec_1.size(); ii++)
    {
        if(ii<vec_1.size() -1 && vec_1[ii+1] == vec_1[ii])
        {
            printf(" \t");
        }
        else
        {
            // fc1 的第 vec_1[ii] 个字 
            string tmp = UTF8Util::GetChar(fc1, vec_1[ii]);
            printf("%s\t",tmp.c_str());
        }
    }
    printf("\n");
    for(int ii=0; ii<vec_2.size(); ii++)
    {
        if(ii>0 && vec_2[ii] == vec_2[ii-1])
        {
            printf(" \t");
        }
        else
        {
            // fc2 的第 vec_2[ii] 个字 
            string tmp = UTF8Util::GetChar(fc2, vec_2[ii]);
            printf("%s\t", tmp.c_str());
        }
    }
    printf("\n");
    */


	//printf("insert=%d\tdelete=%d\treplace=%d\n",err_insert, err_delete, err_replace);
    wer_res.len_lab = cv1;
    wer_res.len_res = cv2;
    wer_res.err_ins = err_insert;
    wer_res.err_del = err_delete;
    wer_res.err_rep = err_replace;
	wer_res.err_all = lcs_ff[cv1][cv2];



	for(int jj=0; jj<MAX_LEN; jj++)
	{
		delete [] lcs_ff[jj];
		delete [] lcs_cc[jj];
	}
	delete []lcs_ff;
	delete []lcs_cc;

	return 0;
}

/**
 * @brief 求两个串的LCS长度 
 *  如果字符串一的所有字符按其在字符串中的顺序出现在另外一个字符串二中，
 *  则字符串一称之为字符串二的子串。
 *  注意，并不要求子串（字符串一）的字符必须连续出现在字符串二中。
 *      a1b2c3d4 与 hm12mm3yy4     最长公共子串  1234
 * @fc1 : 输入串1 (识别结果) 
 * @fc2 : 输入串2 (标注结果) 
 * @len1: 输入串1的长度（返回真实的字个数）
 * @len2: 输入串2的长度（返回） 
 * @return : LCS长度 
 */

double lcs_string(const char *str1, const char *str2, int &len1, int &len2)
{
    // UTF8
    // 得到 str的长度  
	// len  = UTF8Util::StrCharLength(str.c_str());
    // 获取 str 前面 len 个字
	// string temp_word = UTF8Util::NextNChars(str.c_str(), len);
		
    // 删除 str 前面 len 个字  并且把剩余字符串更新到 str中 
    // str = UTF8Util::SkipNChars(str.c_str(),len);

    // 从 str 末尾取 len 个字 
	//string temp_word = UTF8Util::PrevNChars(segStr.c_str()+strlen(segStr.c_str()), len);
    
    // 特别注意 矩阵的index和fc1 fc2 有一个ii-1的关系 



    std::string fc1 = str1;
    std::string fc2 = str2;

    fc1 = str_pro(fc1); 
    fc2 = str_pro(fc2); 
    // 两个字符串  字 的个数 
	int cv1 = UTF8Util::StrCharLength(fc1.c_str());
	int cv2 = UTF8Util::StrCharLength(fc2.c_str());

    // 开辟存储空间 
	double **lcs_ff= new double*[MAX_LEN];
	for(int ii=0; ii<MAX_LEN; ii++)
	{
		lcs_ff[ii] = new double[MAX_LEN];
		for(int jj=0; jj<MAX_LEN; jj++)
		{
			lcs_ff[ii][jj]=0;
		}
	}

    // 对应lcs_ff 记录当前最优值 来自于哪里(-1 0 1 )(插入 替换 删除)
	int **lcs_cc= new int*[MAX_LEN];
	for(int ii=0; ii<MAX_LEN; ii++)
	{
		lcs_cc[ii] = new int[MAX_LEN];
		for(int jj=0; jj<MAX_LEN; jj++)
		{
			lcs_cc[ii][jj]=-2;
		}
	}
	

    // 第1行 第1列 初始化为0
    //str1 构成 竖列 1:cv1
	for (int ct0=1; ct0<=cv1; ct0++)
	{
		lcs_ff[ct0][0]=ct0;
        // 删除  
        lcs_cc[ct0][0] = 1;
	}
    // str2 构成 横列 1:cv2
	for (int ct1=1;ct1<=cv2;ct1++)
	{
		lcs_ff[0][ct1]=ct1;
        // 插入  
		lcs_cc[0][ct1]=-1;
	}

    // 动态规划 计算过程
    // 取 小的那个值  最好都是0
    int err_insert = 0;
    int err_delete = 0;
    int err_replace = 0;
    int flag = 0;
	for (int ct0=1;ct0<=cv1;ct0++)
	{
		for (int ct1=1; ct1<=cv2; ct1++)
		{
            // 不相等1 相等0
            // 获取 fc1 的第ct0 个字 
            string tmp_fc1 = UTF8Util::GetChar(fc1, ct0);
            string tmp_fc2 = UTF8Util::GetChar(fc2, ct1);

			double cur_judge = 1;
            if(tmp_fc1.compare(tmp_fc2) == 0)
            {
                // printf("mmmm=%s",tmp_fc1.c_str());
                cur_judge = 0;
            }
            // 替换 
			double mm = lcs_ff[ct0-1][ct1-1] + cur_judge;
            // 删除:  
			double mm0       = lcs_ff[ct0-1][ct1] + 1;
            // 插入:  
			double mm1        = lcs_ff[ct0][ct1-1] + 1;

            if(mm <= mm0 && mm <= mm1)
            {
                // 替换:    str1和str2都增加一个
                lcs_cc[ct0][ct1] = 0;
			    lcs_ff[ct0][ct1] = mm;
            }
            else if (mm1 <= mm && mm1 <= mm0)
            {
                // 插入
                lcs_cc[ct0][ct1] = -1;
			    lcs_ff[ct0][ct1] = mm1;
            }
            else
            {
                // 删除 
                lcs_cc[ct0][ct1] = 1;
			    lcs_ff[ct0][ct1] = mm0;
            }

		}
	}


    /*
    printf("输出 lcs_ff 矩阵:\n");
    for (int ct0=0; ct0<=cv1; ct0++)
	{
		for (int ct1=0; ct1<=cv2;ct1++)
		{
			printf("%02d ",int(lcs_ff[ct0][ct1]));
		}
		printf("\n");
	}

	printf("输出lcs_cc矩阵\n");
    for (int ct0=0;ct0<=cv1;ct0++)
	{
		for (int ct1=0;ct1<=cv2;ct1++)
		{
			printf("%02d ",int(lcs_cc[ct0][ct1]));
		}
		printf("\n");
	}
    */

    std::vector<int> vec_1;
    std::vector<int> vec_2;
	//printf("最优路径回溯\n");
    int ii=cv1;
    int jj=cv2; 
    while(ii != 0 && jj != 0)
    {
        int vv = lcs_cc[ii][jj];
        //printf("[%d,%d]=%d\n", ii, jj, vv);
        vec_1.insert(vec_1.begin(),ii);
        vec_2.insert(vec_2.begin(),jj);
        if(vv == 0)
        {
            if(ii>1 && jj>1 && lcs_ff[ii-1][jj-1] == lcs_ff[ii-2][jj-2] + 1)
            {
                err_replace++;
            }
            ii--;
            jj--;
        }
        else if(vv == -1)
        {
            jj--;
            err_insert++;
        }
        else if(vv == 1)
        {
            ii--;
            err_delete++;
        }
    }


    /*
    printf("对齐结果:\n");
    for(int ii=0; ii<vec_1.size(); ii++)
    {
        if(ii<vec_1.size() -1 && vec_1[ii+1] == vec_1[ii])
        {
            printf(" \t");
        }
        else
        {
            // fc1 的第 vec_1[ii] 个字 
            string tmp = UTF8Util::GetChar(fc1, vec_1[ii]);
            printf("%s\t",tmp.c_str());
        }
    }
    printf("\n");
    for(int ii=0; ii<vec_2.size(); ii++)
    {
        if(ii>0 && vec_2[ii] == vec_2[ii-1])
        {
            printf(" \t");
        }
        else
        {
            // fc2 的第 vec_2[ii] 个字 
            string tmp = UTF8Util::GetChar(fc2, vec_2[ii]);
            printf("%s\t", tmp.c_str());
        }
    }
    printf("\n");
    */


	printf("insert=%d\tdelete=%d\treplace=%d\n",err_insert, err_delete, err_replace);

    // 错误个数 
	double final_lcs = lcs_ff[cv1][cv2];
	len1=cv1;
	len2=cv2;

	for(int jj=0; jj<MAX_LEN; jj++)
	{
		delete [] lcs_ff[jj];
		delete [] lcs_cc[jj];
	}
	delete []lcs_ff;
	delete []lcs_cc;

	return final_lcs;
}

/**
 * @brief 求两个串的LCS长度 
 *  如果字符串一的所有字符按其在字符串中的顺序出现在另外一个字符串二中，
 *  则字符串一称之为字符串二的子串。
 *  注意，并不要求子串（字符串一）的字符必须连续出现在字符串二中。
 *      a1b2c3d4 与 hm12mm3yy4     最长公共子串  1234
 * @fc1 : 输入串1 (识别结果) 
 * @fc2 : 输入串2 (标注结果) 
 * @len1: 输入串1的长度（返回真实的字个数）
 * @len2: 输入串2的长度（返回） 
 * @return : LCS长度 
 */

/*
double lcs_string(const char *fc1, const char *fc2, int &len1, int &len2)
{
    // UTF8
    // 得到 str的长度  
	// len  = UTF8Util::StrCharLength(str.c_str());
    // 获取 str 前面 len 个字
	// string temp_word = UTF8Util::NextNChars(str.c_str(), len);
		
    // 删除 str 前面 len 个字  并且把剩余字符串更新到 str中 
    // str = UTF8Util::SkipNChars(str.c_str(),len);

    // 从 str 末尾取 len 个字 
	//string temp_word = UTF8Util::PrevNChars(segStr.c_str()+strlen(segStr.c_str()), len);
    
    
    // 特别注意 矩阵的index和fc1 fc2 有一个ii-1的关系 
	int cv1 = strlen(fc1);
	int cv2 = strlen(fc2);

    // 开辟存储空间 
	double **lcs_ff= new double*[MAX_LEN];
	for(int ii=0; ii<MAX_LEN; ii++)
	{
		lcs_ff[ii] = new double[MAX_LEN];
		for(int jj=0; jj<MAX_LEN; jj++)
		{
			lcs_ff[ii][jj]=0;
		}
	}

    // 对应lcs_ff 记录当前最优值 来自于哪里(-1 0 1 )(插入 替换 删除)
	int **lcs_cc= new int*[MAX_LEN];
	for(int ii=0; ii<MAX_LEN; ii++)
	{
		lcs_cc[ii] = new int[MAX_LEN];
		for(int jj=0; jj<MAX_LEN; jj++)
		{
			lcs_cc[ii][jj]=-2;
		}
	}
	

    // 第1行 第1列 初始化为0
    //str1 构成 竖列 1:cv1
	for (int ct0=1;ct0<=cv1;ct0++)
	{
		lcs_ff[ct0][0]=ct0;
        // 删除  
        lcs_cc[ct0][0] = 1;
	}
    // str2 构成 横列 1:cv2
	for (int ct1=1;ct1<=cv2;ct1++)
	{
		lcs_ff[0][ct1]=ct1;
        // 插入  
		lcs_cc[0][ct1]=-1;
	}

    // 动态规划 计算过程
    // 取 小的那个值  最好都是0
    int err_insert = 0;
    int err_delete = 0;
    int err_replace = 0;
    int flag = 0;
	for (int ct0=1;ct0<=cv1;ct0++)
	{
		for (int ct1=1; ct1<=cv2; ct1++)
		{
            // 不相等1 相等0
			double cur_judge = 0;
            if(fc1[ct0-1] != fc2[ct1-1])
              cur_judge = 1;
            // 替换 
			double mm = lcs_ff[ct0-1][ct1-1] + cur_judge;
            // 删除:  
			double mm0       = lcs_ff[ct0-1][ct1] + 1;
            // 插入:  
			double mm1        = lcs_ff[ct0][ct1-1] + 1;

            if(mm<mm0 && mm<mm1)
            {
                // 替换:    str1和str2都增加一个
                lcs_cc[ct0][ct1] = 0;
			    lcs_ff[ct0][ct1] = mm;
            }
            else if (mm1<mm && mm1 < mm0)
            {
                // 插入
                lcs_cc[ct0][ct1] = -1;
			    lcs_ff[ct0][ct1] = mm1;
            }
            else
            {
                // 删除 
                lcs_cc[ct0][ct1] = 1;
			    lcs_ff[ct0][ct1] = mm0;
            }

		}
	}

    // 测试 


    printf("输出 lcs_ff 矩阵:\n");
    for (int ct0=0; ct0<=cv1; ct0++)
	{
		for (int ct1=0; ct1<=cv2;ct1++)
		{
			printf("%02d ",int(lcs_ff[ct0][ct1]));
		}
		printf("\n");
	}

    // lcs_cc 矩阵 
	printf("输出lcs_cc矩阵\n");
    for (int ct0=0;ct0<=cv1;ct0++)
	{
		for (int ct1=0;ct1<=cv2;ct1++)
		{
			printf("%02d ",int(lcs_cc[ct0][ct1]));
		}
		printf("\n");
	}

    std::vector<int> vec_1;
    std::vector<int> vec_2;
	printf("最优路径回溯\n");
    int ii=cv1;
    int jj=cv2; 
    while(ii != 0 && jj != 0)
    {
        int vv = lcs_cc[ii][jj];
        printf("[%d,%d]=%d\n", ii, jj, vv);
        vec_1.insert(vec_1.begin(),ii);
        vec_2.insert(vec_2.begin(),jj);
        if(vv == 0)
        {
            if(ii>1 && jj>1 && lcs_ff[ii-1][jj-1] == lcs_ff[ii-2][jj-2] + 1)
            {
                err_replace++;
            }
            ii--;
            jj--;
        }
        else if(vv == -1)
        {
            jj--;
            err_insert++;
        }
        else if(vv == 1)
        {
            ii--;
            err_delete++;
        }
    }

    printf("对齐展示:\n");
    for(int ii=0; ii<vec_1.size(); ii++)
    {
        if(ii>0 && vec_1[ii] == vec_1[ii-1])
        {
            printf("  ");
        }
        else
        {
            printf(" %c",fc1[vec_1[ii]-1]);
        }
    }
    printf("\n");
    for(int ii=0; ii<vec_2.size(); ii++)
    {
        if(ii>0 && vec_2[ii] == vec_2[ii-1])
        {
            printf("  ");
        }
        else
        {
            printf(" %c",fc2[vec_2[ii]-1]);
        }
    }
    printf("\n");


	printf("insert=%d\tdelete=%d\treplace=%d\n",err_insert, err_delete, err_replace);

    // 错误个数 
	double final_lcs = lcs_ff[cv1][cv2];
	len1=cv1;
	len2=cv2;

	for(int jj=0; jj<MAX_LEN; jj++)
	{
		delete [] lcs_ff[jj];
	}
	delete []lcs_ff;

	return final_lcs;
}
*/

/**
 * @brief: 将字符串1中所有与字符串2相同的子串替换成字符串3 
 * @chString : 原始字符串
 * @chOldWord : 待替换的老字符串
 * @chNewWord : 替换的新字符串
 * @return -1 替换失败
 * @return 0 替换成功
 **/
int replace(char chString[], const char chOldWord[], const char chNewWord[])
{
	if(chString==NULL)
	{
		return -1;
	}
	if(chOldWord==NULL)
	{
		return -1;
	}
	if(chNewWord==NULL)
	{
		return -1;
	}
	int i;
	int nStartPos=0;
	int nLen1=0;
	int nLen2=0;
	int nLen3=0;
	int nFound;
	/*计算旧词和新词的长度*/
	while(chOldWord[nLen2++]!='\0'){}
	nLen2--;
	while(chNewWord[nLen3++]!='\0'){}
	nLen3--;	
	/* chString中可能有多个旧词，均要替换为新词利用循环向前拨动查找位置，逐次进行比较和替换*/
	while(chString[nStartPos]!='\0')
	{
		/*从nStartPos位置开始，Len2长度的字符串是否和旧词相同？*/
		nFound=1;
		for(i=0;i<nLen2;i++)
		{
			if(chString[nStartPos+i]!=chOldWord[i])
			{
				nFound=0;
				break;
			}
		} 
		if(nFound==1)/*相同，这Len2个字符需要被替换掉*/
		{
			/*计算输入字串chString 的长度，注意在循环中每次计算chString 
			  长度是必要的，因为完成一次替换后，chString的长度可能发生变化*/
			while(chString[nLen1++]!='\0'){}
			nLen1--;			
			/*新词、旧词长度可能不同，先将chString长度调至正确的位置，
			  chString中nStartPos 后的字符可能需要前移或后移若干位*/
			if(nLen3-nLen2>=0)/*新词比旧词长，从后向前移动*/
			{
				for(i=nLen1-1;i>=nStartPos;i--)
				{	chString[i+nLen3-nLen2]=chString[i];
				}
			}
			else/*新词比旧词短，从前向后移动*/
			{
				for(i=nStartPos+nLen2;i<nLen1;i++)
				{   chString[i+nLen3-nLen2]=chString[i];}
			}
			chString[nLen1+nLen3-nLen2]='\0';			
			/*将新词复制到chString，替换原来的旧词 */
			for(i=0;i<nLen3;i++)
			{   chString[nStartPos+i]=chNewWord[i];}			
			/*下一次检查的位置：从替换后新词后面的位置开始*/
			nStartPos+=nLen3;
		}
		else
		{/*不同，则从下一个字符开始，继续进行检查*/
			if(chString[nStartPos]<0)//中文字符跳过2个字节
			{
				nStartPos+=2;
			}
			else//非中文字符
			{
				nStartPos++;
			}
		}
	}
	return 0;
}

/**
 * @brief: 将结果归一化为阿拉伯数字 
 * @src : 未归一化的字符串
 * @des : 归一化后的字符串
 * @return -1 归一化失败
 * @return 0 归一化成功
 **/
int convert_to_ArabicNumbers(char *src)
{ 
	//替换列表
	const char  *numberlist[]={"1","2","3","4","5","6","7", \
		"8","9","0","1","2","3","4","5","6","7","8","9","1"};
	///替换列表
	const char *wordlist[]={"壹","贰","弎","肆","伍","陆", \
		"柒","捌","玖","零","一","二","三","四","五","六","七","八","九","幺"};
	if(src==NULL)
	{
		return -1;
	}
	for(int i=0;i<20;i++)
	{
		replace(src,wordlist[i],numberlist[i]);
	}
	int cl=strlen(src);
	for (int ct0=0;ct0<cl;ct0++)
	{
		if ((unsigned char)src[ct0]>=0x80)
		{
			ct0++;
			continue;
		}
		if (src[ct0]>='A' && src[ct0]<='Z')
		{
			src[ct0]+='a'-'A';
		}
	}
	return 0;
}






