

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iconv.h> 
using namespace std;
#include <string>

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

void hex_dump(unsigned char *bytes, unsigned long len ){
    int line = 0;
    int tab = 0;
    for (int i=0; i<len; i++) {
        printf("0x%02x ",bytes[i]);
        line++;
        if (line == 8) {
            printf("\n");
            line = 0;
        }
    }
    printf("\n");
}

std::string basename(const std::string& path)
{
    size_t found = path.find_last_of("/\\");
    
    if (std::string::npos != found)
    {
        return path.substr(0, found);
    }
    else
    {
        return path;
    }
}


std::string code_convert(char *source_charset, char *to_charset, const std::string& sourceStr) //sourceStr是源编码字符串
{
    iconv_t cd = iconv_open(to_charset, source_charset);//获取转换句柄，void*类型
    if (cd == 0)
        return "";
    
    size_t inlen = sourceStr.size();
    size_t outlen = inlen*2;
    char* inbuf = (char*)sourceStr.c_str();

    char *outbuf = new char[outlen];
    memset(outbuf, 0, outlen);
    
    char *poutbuf = outbuf; //多加这个转换是为了避免iconv这个函数出现char(*)[255]类型的实参与char**类型的形参不兼容
    if (iconv(cd, &inbuf, &inlen, &poutbuf,&outlen) == -1)
        return "";
    
    std::string strTemp(outbuf);//此时的strTemp为转换编码之后的字符串
    iconv_close(cd);
    delete [] outbuf;
    return strTemp;
}

int main(int argc, const char * argv[]) {

    if (argc != 2) {
        printf("use: ./convert csv\n");
        return -1;
    }
    
    std::string fileName = argv[1];
    
    std::string dir = basename(fileName);

    
    unsigned long s = 0;
    unsigned char* bytes = getFileData(fileName.c_str(),"rb",&s);
    if (bytes == NULL) {
        printf("文件不存在:%s\n",fileName.c_str());
        return -1;
    }
//    hex_dump(bytes,s);
    
    
    unsigned char utf[] = { 0xEF, 0xBB, 0xBF };
    std::string out = dir+"/out.csv";
    FILE *fp = fopen(out.c_str(), "wb");

    fwrite(utf,3,1,fp);
    
    std::string input = std::string((const char *)bytes,s);
    std::string outstr = code_convert("gbk","utf-8",input);
    
    
    fwrite(outstr.c_str(), outstr.size(), 1, fp);
    fclose(fp);
    
    free(bytes);
    
    return 0;
}
