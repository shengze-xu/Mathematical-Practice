#include <stdio.h>
#include <string.h>
int main()
{
    char s[100];
    FILE *fp;
    int lines=0;
    fp=fopen("data1.txt", "r");
    if(fp)
    {
        while((fgets(s,100,fp)) != NULL)
            if(s[strlen(s)-1]=='\n') lines++;
        printf("%d\n",lines);
        fclose(fp);
    }
    return 0;
}