#include <string.h>
#include <stdio.h>
#include <stdlib.h>


typedef unsigned uchar;
int* split_string(const char *str, int *osize){
    
#define tuchar(c) ((unsigned char)c)
    if (str==NULL) {
        if(osize){
            *osize = 0;
        }
        return NULL;
    }
    size_t ms = 8;
    int *ret = malloc(sizeof(int)*ms);
    size_t len = strlen(str);
    int count = 0;
    int find = 0;
    for (int i=0; i<len; i++) {
        uchar c = tuchar(str[i]);
        find = 0;
        if(c>=1&&c<=127){
            find = 1;
        }else{
            if (c>=194&&c<=244) {
                while (i<len) {
                    i++;
                    c = tuchar(str[i]);
                    if (c>=128&&c<=191) {
                        
                    }else{
                        i--;
                        break;
                    }
                }
                find = 1;
            }
        }
        if(count>=ms){
            ms*=2;
            ret = realloc(ret, sizeof(int)*ms);
        }
        if (find) {
            ret[count++] = i;
        }
        
    }
    
    if(osize){
        *osize = count;
    }
    return ret;
}
int main (int argc, char **argv) {
    
//    char a[4] = {229, 178, 177, 0};
//    printf("%s\n", a);
    
   const char *str = "0🐂1你好";
    
	// char str[] = {
	//     0x21,0x22,0x5D
	    
	//     ,0
	// };
    
    
    
    int size=0;
    int *ret  = split_string(str,&size);
    int start = -1;
    int len = 0;
    int off = 0;
    char temp[6];
    for (int i=0; i<size; i++) {
        len = ret[i]-start;
        start = ret[i];
        strncpy(temp,str+off,len);
        temp[len]=0;
        off+=len;
        printf("%d %s\n",ret[i],temp);
    }
    free(ret);

    return 0;
}
