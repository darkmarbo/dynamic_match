#include"werC.h"
#include"common.h"

const int ENTRY_BUFF_SIZE = 40960;

// 保存输入文本格式  id:content
typedef struct _Text{
    string id; // wav name
    string text;

}Text;

// 从哪个小语音ii开始 到那个jj(包含)结束  匹配度是多少
typedef struct _MATCH{
    // 开始小语音的idx=0
    int st;
    // 结束小语音的idx(包含)
    int end;
    double match;
}MATCH;

// 动态规划中的 结点 
// 存储 lab的当前Mi结点  匹配上res中第j个小句子Nj 时 的总路径得分  
//   
typedef struct _SCORE{
    // 上一层M[i-1]结点的位置: 对应的j的值
    int last_j;
    double score;
}SCORE;

vector<Text> vec_lab;
vector<Text> vec_res;

int file2vec(FILE *fp, vector<Text> &ttt);

/* 
 * 在vec中寻找对应id 有返回idx 没有返回-1 
 * */
int find_id(const vector<Text> &vec, string id);


/*
 * 输入两个文件 lab和res  计算对应句子的wer 
 * */
int main(int argc, char *argv[])
{
    if(argc < 4)
    {
        printf("usage: %s file_label  file_res  file_out\n", argv[0]);
        return 0;
    }

    int ret;
    int len1,len2;
    int err_count =0;
    int M,N;
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

    printf("载入label和res文件到vector...");
    ret = file2vec(fp_lab, vec_lab);
    ret = file2vec(fp_res, vec_res);
    M = vec_lab.size();
    N = vec_res.size();

    vector< vector<MATCH> > vec_match(M, vector<MATCH>(N));
    vector< vector<SCORE> > vec_score(M, vector<SCORE>(N));

    // 计算 res 中所有数据的wer  
    WER_RES wer_all;
    memset(&wer_all, 0, sizeof(wer_all));
    printf("开始计算每句话和所有小语音的基础匹配度... \n", M);

    // lab的每句话  跟res的每个小句子  对比计算match 保存到矩阵  
    for(int idx=0; idx < M; idx++)
    {
        printf("当前 %d 个lab \n", idx+1);
        // label 中 每句话 
        string id = vec_lab[idx].id;
        string text_lab = vec_lab[idx].text;

        //fprintf(fp_out, "%s\t%s\n", id.c_str(), text_lab.c_str());
        // 与 res 中每个小句子 进行匹配 
        for(int ii = 0; ii<N; ii++)
        {
            double wer_tmp = -10.0;
            int jj_tmp = ii;  // 记录拼接到jj的最好
            string text_tmp;  // 记录拼接最好的结果 

            // 从 第ii个小句子开始  往后拼接到jj  
            // 然后计算wer  选取其中wer最小的 对应的jj  
            for(int jj=ii; jj<N; jj++)
            {
                WER_RES wer_res;
                string text_res = "";
                // 从 ii 拼接到 jj 
                for(int kk=ii; kk<=jj; kk++)
                {
                    //printf("kk=%d\t%s\n", kk, vec_res[kk].text.c_str());
                    text_res += vec_res[kk].text;
                }
                ret = lcs_string(text_lab.c_str(), text_res.c_str(), wer_res);
                double wer_now = 1 - double(wer_res.err_all)/double(wer_res.len_lab);
                if(wer_now > wer_tmp)
                {
                    wer_tmp = wer_now;
                    jj_tmp = jj;
                    text_tmp = text_res;
                }

                // 记录当前拼接的长度 是否大于len_lab的两倍 
                if(2 * wer_res.len_lab < wer_res.len_res)
                {
                    //int flag_len = 0; 
                    break;
                }

            }

            // 测试每个lab句子  与所有小语音对齐 的wer值  
            //fprintf(fp_out, "\t%d\t%.4f\t%s\n", jj_tmp, wer_tmp, text_tmp.c_str());
            MATCH mt;
            mt.st = ii;
            mt.end = jj_tmp;
            mt.match = wer_tmp;
            vec_match[idx][ii] = mt;

        }


    }


    // 通过 vec_match 计算vec_score 
    //// 测试 vec_match   打印出每句话 单独 对应的最佳匹配点 
    printf("开始计算动态规划中每个句子的截止得分...");
    for(int ii=0; ii<M; ii++)
    {
        printf("当前 %d ...\n");
        fprintf(fp_out, "%s\t%s\n", vec_lab[ii].id.c_str(), vec_lab[ii].text.c_str());
        int jj_ok = 0;
        MATCH mt_ok;
        mt_ok.match = -10.0;

        // 当前 节点  [ii,jj]
        for(int jj=0; jj<N; jj++)
        {
            MATCH mt = vec_match[ii][jj];
            //fprintf(fp_out, "\t%d\t%d\t%.4f\n", mt.st, mt.end, mt.match);
            if(mt.match > mt_ok.match)
            {
                jj_ok = jj;
                mt_ok = mt;
            }


            // 计算 ii=0 score 
            if(0 == ii)
            {
                SCORE sc;
                sc.last_j = -1;  // 没有上一层的结点 
                sc.score = mt.match;
                vec_score[ii][jj] = sc; 
            }
            else
            {
                int max_jj = 0;
                SCORE max_sc = vec_score[ii-1][0];

                // 寻找上一层中 所有符合条件的结点中  最大的一个  
                for(int kk = 1; kk<N; kk++)
                {
                    // 上一层中 节点 kk 对应的match
                    MATCH last_mt = vec_match[ii-1][kk];
                    SCORE last_sc = vec_score[ii-1][kk];
                    // 上一层中 结点 kk  累计积分
                    if( last_mt.end < mt.st && last_sc.score > max_sc.score)
                    {
                        max_jj = kk;
                        max_sc = last_sc;

                    }
                }

                // 当前结点的值
                SCORE sc;
                sc.last_j = max_jj;
                sc.score = max_sc.score + mt.match;
                
                vec_score[ii][jj] = sc; 

            }

        }

        fprintf(fp_out, "\t%d\t%d\t%.4f\n", mt_ok.st, mt_ok.end, mt_ok.match);

    }


    // 测试 vec_score 矩阵 
    for(int ii=0; ii<M; ii++)
    {
        for(int jj=0; jj<N; jj++)
        {

            fprintf(fp_out, "%d/%.2f ", vec_score[ii][jj].last_j, vec_score[ii][jj].score);
            if(jj%10 == 9)
            {
                fprintf(fp_out,"\n");
            }
        }
        fprintf(fp_out, "\n\n");
    }

    // 打印出最优路径 
    int ii = M-1; 
    int jj= 0;

    // 寻找 最后一行的 最大值 
    int max_jj = 0;
    SCORE max_sc = vec_score[ii][0];
    for(int jj=1; jj<N; jj++)
    {
        SCORE sc = vec_score[ii][jj];
        if(sc.score > max_sc.score)
        {
           max_sc = sc;
           max_jj = jj;
        }
    }

    while(ii >= 0)
    {
        fprintf(fp_out, "%d\t%s\n", ii, vec_lab[ii].text.c_str());
        MATCH mt = vec_match[ii][max_jj];
        SCORE sc = vec_score[ii][max_jj];
        fprintf(fp_out, "%d\t%d\t%.4f\n", mt.st, mt.end, sc.score);

        // 上一层的 jj 
        max_jj = sc.last_j;
        ii--;
    }

	

	fclose(fp_lab);
	fclose(fp_res);
	fclose(fp_out);


    
    return 0;

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


