#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void compute_next(char *pattern,int *next)
{
    int len=strlen(pattern);
    int i=0,j=-1;
    next[0]=-1;
    while (i<len)
    {
        if(j==-1||pattern[i]==pattern[j])
        {
            i++;j++;
            next[i]=j;
        }
        else
        {
            j=next[j];
        }
    } 
}

int kmp_search(char *text,char *pattern)
{
    int len1=strlen(text);
    int len2=strlen(pattern);
    int *next=(int *)malloc(sizeof(int) * len2);
    int i=0,j=0;
    compute_next(pattern,next);
    while (i<len1 && j<len2)
    {
        if(j==-1||text[i]==pattern[j])
        {
            i++;j++;
        }
        else
        {
            j=next[j];
        }
    }
    free(next);
    if(j==len2)
    {
        return i-j;
    }
    else
    {
        return -1;
    }
}

int main(void)
{
    char text[]="hello world";
    char pattern[]="world";
    int pos =kmp_search(text,pattern);
    if(pos ==-1)
    {
        printf("Failed to match '%s' in '%s'.\n",pattern,text);
    }
    else
    {
        printf("Match '%s' in '%s' at position %d.\n",pattern,text,pos);
    }
    return 0;
}