#include"werC.h"


int main(int argc, char *argv[])
{

    int len1,len2;
    int err_count =0;

    char *str1 = "abcdefghij"; 
    char *str2 = "amcdemmmmmmmfj"; 
    err_count = lcs_string(str1,str2, len1, len2);
    printf("str1=%s\tstr2=%s\n",str1, str2);
	printf("err_count=%d, len1=%d len2=%d\n",err_count, len1, len2);





}
