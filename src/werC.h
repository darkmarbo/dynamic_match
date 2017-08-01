#ifndef _WERC_H_
#define _WERC_H_

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cstring>
#include <stdio.h>

using namespace std;



#define MAN_LEN 500

int ch(const char *st);

double lcs_string(const char *string1, const char *string2, int &len1, int &len2);

int replace(char chString[], const char chOldWord[], const char chNewWord[]);

int convert_to_ArabicNumbers(char *src);

int wer(char *lab, char *res, double &match);

#endif
