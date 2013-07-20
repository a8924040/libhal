#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <zlib.h>

int main(int argc, char *argv[])
{
    char text[] = "zlib compress and uncompress test\nturingo@163.com\n2012-11-05\n";
    unsigned long tlen = strlen(text) + 1;   /* ��Ҫ���ַ����Ľ�����'\0'Ҳһ������ */
    char *buf = NULL;
    unsigned long blen;
    /* ���㻺������С����Ϊ������ڴ� */
    blen = compressBound(tlen);   /* ѹ����ĳ����ǲ��ᳬ��blen�� */
    if((buf = (char *)malloc(sizeof(char) * blen)) == NULL)
    {
        printf("no enough memory!\n");
        return -1;
    }
    /* ѹ�� */
    if(compress(buf, &blen, text, tlen) != Z_OK)
    {
        printf("compress failed!\n");
        return -1;
    }
    /* ��ѹ�� */
    if(uncompress(text, &tlen, buf, blen) != Z_OK)
    {
        printf("uncompress failed!\n");
        return -1;
    }
    /* ��ӡ��������ͷ��ڴ� */
    printf("%s", text);
    if(buf != NULL)
    {
        free(buf);
        buf = NULL;
    }
    return 0;
}