#include <stdio.h>

int main(){
    float a[80],b[80]={0};
    FILE *fpRead = fopen("data.txt","r");  
    for(int i=0;i<79;i++){
        fscanf(fpRead,"%f %f",&a[i],&b[i]);
        printf("%.2f,",b[i]);
    }
    return 0;
}


