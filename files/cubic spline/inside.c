#include <stdio.h>

#define  MAXNUM  1000  

typedef struct SPLINE{
    float x[MAXNUM+1];    
    float y[MAXNUM+1];  
    float xmax,xmin,ymax,ymin; 
    unsigned int number;  
}SPLINE,*pSPLINE;  

int main(){
    int splinenumber=4;
    char name[16];
    int i,j;
    FILE *fp;
    SPLINE line[splinenumber];
    pSPLINE pline[splinenumber];
    for(i=0;i<splinenumber;i++){
        pline[i]=&line[i];
    }
    //-----人工输入各文件中数据量
    line[0].number=4;
    line[1].number=4;
    line[2].number=8;
    line[3].number=4;
    
    //-----读取各文件中的数据
    for(i=0;i<splinenumber;i++){
        sprintf(name,"a%d.txt",i);
        fp=fopen(name,"r");//打开指定名称的文件，便于依次读取文件中数据
        //-----存入各曲线坐标点的数据
            for(j=0;j<line[i].number;j++){
            fscanf(fp,"%f %f",&line[i].x[j],&line[i].y[j]);
            //printf("%f %f\n",line[i].x[j],line[i].y[j]);
        }
        line[i].xmax=line[i].x[0];line[i].xmin=line[i].x[0];
        line[i].ymax=line[i].y[0];line[i].ymin=line[i].y[0];
        //-----求出各闭曲线横纵坐标点的最值并存入数组
        for(j=0;j<line[i].number;j++){
            line[i].xmax=(line[i].xmax>line[i].x[j])?line[i].xmax:line[i].x[j];
            line[i].xmin=(line[i].xmin<line[i].x[j])?line[i].xmin:line[i].x[j];
            line[i].ymax=(line[i].ymax>line[i].y[j])?line[i].ymax:line[i].y[j];
            line[i].ymin=(line[i].ymin<line[i].y[j])?line[i].ymin:line[i].y[j];
        }
        printf("%f %f %f %f\n",line[i].xmax,line[i].xmin,line[i].ymax,line[i].ymin);
        //printf("\n");
    }
    
    int a[splinenumber][splinenumber];
    for(i=0;i<splinenumber;i++){
        for(j=0;j<splinenumber;j++){
            if(i==j){
                a[i][j]=0;
            }
            else{
                if((line[i].xmax>line[j].xmax)&&(line[i].xmin<line[j].xmin)&&(line[i].ymax>line[j].ymax)&&(line[i].ymin<line[j].ymin)){
                    a[i][j]=1;
                }
                else if((line[i].xmax<line[j].xmax)&&(line[i].xmin>line[j].xmin)&&(line[i].ymax<line[j].ymax)&&(line[i].ymin>line[j].ymin)){
                    a[i][j]=-1;
                }
                else{
                    a[i][j]=0;
                }
            }
        }
    }
    for(i=0;i<splinenumber;i++){
        for(j=0;j<splinenumber;j++){
            printf("%d ",a[i][j]);
        }
        printf("\n");
    } 
    return 0;
}