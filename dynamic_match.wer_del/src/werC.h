#ifndef _WERC_H_
#define _WERC_H_

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cstring>
#include <stdio.h>

#include "UTF8Util.h"

using namespace std;



#define MAX_LEN 500

typedef struct _WER_RES{

    // 第一个字 是否出现删除错误 
    int del_st;

    int len_lab;
    int len_res;

    int err_ins;
    int err_del;
    int err_rep;
    int err_all;

    char str_lab[MAX_LEN]; // 对齐的lab 
    char str_res[MAX_LEN]; // 对齐的res 
}WER_RES;

int ch(const char *st);

int load_map();

string str_pro(const string &str);

int add_wer(WER_RES &wer_all, WER_RES wer_res);

int lcs_string(const char *string1, const char *string2, WER_RES &wer_res);

double lcs_string(const char *string1, const char *string2, int &len1, int &len2);

int replace(char chString[], const char chOldWord[], const char chNewWord[]);

int convert_to_ArabicNumbers(char *src);

int wer(const char *lab, const char *res, WER_RES &wer_res);

#endif
