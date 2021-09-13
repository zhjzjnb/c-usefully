
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "xxtea.h"

unsigned char * getFileData(const char* pszFileName, const char* pszMode, unsigned long * pSize){
    
    FILE *fp = fopen(pszFileName, pszMode);
    if (fp == NULL) {
        return  NULL;
    }
    unsigned char * pBuffer = NULL;
    
    fseek(fp,0,SEEK_END);
    *pSize = ftell(fp);
    fseek(fp,0,SEEK_SET);
    pBuffer = new unsigned char[*pSize];
    *pSize = fread(pBuffer,sizeof(unsigned char), *pSize,fp);
    fclose(fp);
    
    return pBuffer;
}


int main(int argc, const char * argv[]) {
    if (argc != 4) {
        printf("usage: ./xxtea file sign key\n");
        return -1;
    }
    const char *pszFileName = argv[1];
    const char *_xxteaSign = argv[2];
    const char *_xxteaKey = argv[3];
    size_t _xxteaSignLen = strlen(_xxteaSign);
    size_t _xxteaKeyLen = strlen(_xxteaKey);
    
    
    
    
    unsigned long s = 0;
    unsigned char* bytes = getFileData(pszFileName,"rb",&s);
    if (bytes == NULL) {
        printf("文件不存在:%s\n",pszFileName);
        return -1;
    }
    ssize_t size = s;
    
    bool isXXTEA = memcmp(_xxteaSign, bytes, _xxteaSignLen) == 0;
    if(!isXXTEA){
        printf("没有xxtea加密\n");
        return 0;
    }
    
    void *buffer = NULL;
    
    xxtea_long len = 0;
    buffer = xxtea_decrypt(bytes + _xxteaSignLen,
                           (xxtea_long)size - (xxtea_long)_xxteaSignLen,
                           (unsigned char*)_xxteaKey,
                           (xxtea_long)_xxteaKeyLen,
                           &len);;
    

    FILE *fp = fopen(pszFileName, "wb");
    fwrite(buffer, len, 1, fp);
    fclose(fp);
    
    return 0;
}
