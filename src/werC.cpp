#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cstring>
#include <stdio.h>

#include"werC.h"

using namespace std;

map<int, int> fr;
map<int, int> c_conv;


int wer(char *lab, char *res, double &match)
{
	
    // 忽略的字
	fr[ch("。")]=1;
	fr[ch("，")]=1;

    // 转换的字 
	c_conv[ch("Ｙ")]='y';
	c_conv[ch("Ｚ")]='z';

	int len1,len2;

	char lab_tmp[MAX_LCS_STR_LEN * 2] = {0};
	snprintf(lab_tmp, MAX_LCS_STR_LEN*2, "%s", lab);
	char res_tmp[MAX_LCS_STR_LEN * 2] = {0};
	snprintf(res_tmp, MAX_LCS_STR_LEN*2, "%s", res);

	convert_to_ArabicNumbers(lab_tmp);	
	convert_to_ArabicNumbers(res_tmp);
	
	
	double lcs_r = lcs_string(lab_tmp, res_tmp, len1, len2);	
	double len_ave=double(len1+len2)/2.0;
	match = 1 - lcs_r*double(len1)/(len_ave);
	if(match<0)
	{
		match=0.0;
	}
	//printf("mmmmmm len_ave:%f\tmatch:%f\n",len_ave,match);	

	
	return 0;
}

int ch(char *st)
{
	int ret=int(st[0])*256+st[1];
	//printf("%d\n",ret);
	return ret;
}

/**
 * @brief 求两个串的LCS长度 
 *  如果字符串一的所有字符按其在字符串中的顺序出现在另外一个字符串二中，
 *  则字符串一称之为字符串二的子串。
 *  注意，并不要求子串（字符串一）的字符必须连续出现在字符串二中。
 *      a1b2c3d4 与 hm12mm3yy4     最长公共子串  1234
 * @string1 : 输入串1 (识别结果) 
 * @string2 : 输入串2 (标注结果) 
 * @len1: 输入串1的长度（返回真实的字个数）
 * @len2: 输入串2的长度（返回） 
 * @return : LCS长度 
 */


double lcs_string(const char *string1, char *string2, int &len1, int &len2)
{

	int sz1 = strlen(string1);
	int sz2 = strlen(string2);
	// double lcs_ff[MAX_LCS_STR_LEN][MAX_LCS_STR_LEN]={0};
    // 开辟存储空间 
	double **lcs_ff= new double*[MAX_LCS_STR_LEN];
	for(int ii=0; ii<MAX_LCS_STR_LEN; ii++)
	{
		lcs_ff[ii] = new double[MAX_LCS_STR_LEN];
		for(int jj=0; jj<MAX_LCS_STR_LEN; jj++)
		{
			lcs_ff[ii][jj]=0;
		}
	}
	
    // 存储最终需要计算的真正字符串 
	short fc1[MAX_LCS_STR_LEN];
	int cv1=0;
	for (int ct0=0; ct0<sz1; ct0++)
	{
        // 中文字符 
		if ((unsigned char)string1[ct0] >= 0x80)
		{
			int value=(int)string1[ct0]*256+string1[ct0+1];
			//printf("%c%c\t%d\n",string1[ct0],string1[ct0+1],value);
			ct0++;

            // 标点等 不计算的符号 
			if (fr.find(value)!=fr.end())
			{
				continue;
			}
            // 全角字母等 需要转换的符号
			if (c_conv.find(value) != c_conv.end())
			{
				value=c_conv[value];
			}
			fc1[cv1]=value;
		}
		else // 单字节的英文等 abc
		{
            // 忽略
			if ((string1[ct0]<48) ||
					(string1[ct0]>=58 && string1[ct0]<65) ||
					(string1[ct0]>=91 && string1[ct0]<97))
			{
				continue;
			}
			fc1[cv1]=string1[ct0];
		}
		cv1++;
	}

	short fc2[MAX_LCS_STR_LEN];
	int cv2=0;
	for (int ct0=0; ct0<sz2; ct0++)
	{
		if ((unsigned char)string2[ct0]>=0x80)
		{
			int value=(int)string2[ct0]*256+string2[ct0+1];
			//printf("%c%c\t%d\n",string2[ct0],string2[ct0+1],value);
			ct0++;
			if (fr.find(value)!=fr.end())
			{
				continue;
			}
			if (c_conv.find(value)!=c_conv.end())
			{
				value=c_conv[value];
			}
			fc2[cv2]=value;
		}
		else
		{
			if ((string2[ct0]<48) ||
					(string2[ct0]>=58 && string2[ct0]<65) ||
					(string2[ct0]>=91 && string2[ct0]<97))
			{
				continue;
			}
			fc2[cv2]=string2[ct0];
		}
		cv2++;
	}

    // 第1行 第1列 初始化为0
    //str1 构成 竖列 1:cv1
	for (int ct0=1;ct0<=cv1;ct0++)
	{
		lcs_ff[ct0][0]=ct0;
	}
    // str2 构成 横列 1:cv2
	for (int ct1=1;ct1<=cv2;ct1++)
	{
		lcs_ff[0][ct1]=ct1;
	}

    // 动态规划 计算过程
    // 取 小的那个值  最好都是0
	for (int ct0=1;ct0<=cv1;ct0++)
	{
		for (int ct1=1;ct1<=cv2;ct1++)
		{
            // 不相等1 相等0
			double cur_judge    = (fc1[ct0-1] != fc2[ct1-1]);

            // min_ov 为两者较小的 
			double min_ov       = lcs_ff[ct0-1][ct1]+1;
			double cur_v        = lcs_ff[ct0][ct1-1]+1;
			if (cur_v<min_ov)
			{
				min_ov=cur_v;
			}

            // 如果它 更小  那么取这个 
			cur_v = lcs_ff[ct0-1][ct1-1] + cur_judge;
			if (cur_v<min_ov)
			{
				min_ov = cur_v;
			}
			lcs_ff[ct0][ct1] = min_ov;
		}
	}

    // 测试 

	printf("   0 ");
    for(int ii= 0 ; ii< strlen(string2);ii++)
    {
	    printf("%c  ",string2[ii]);
    }
	printf("\n");

    for (int ct0=0;ct0<=cv1;ct0++)
	{
        if(ct0 == 0)
		    printf("0 ");
        else
		    printf("%c ",string1[ct0-1]);

		for (int ct1=0;ct1<=cv2;ct1++)
		{
			printf("%02d ",int(lcs_ff[ct0][ct1]));
		}
		printf("\n");
	}

    // 错误个数 
	double final_lcs = lcs_ff[cv1][cv2];
	len1=cv1;
	len2=cv2;

	for(int jj=0; jj<MAX_LCS_STR_LEN; jj++)
	{
		delete [] lcs_ff[jj];
	}
	delete []lcs_ff;

	return final_lcs;
}

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
static int convert_to_ArabicNumbers(char *src)
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






