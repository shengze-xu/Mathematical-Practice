#include <stdio.h>

#define  MAXNUM  1000  

typedef struct SPLINE{
    float x[MAXNUM+1];    
    float y[MAXNUM+1];  
    //float xmax,xmin,ymax,ymin; 
    unsigned int number;  
}SPLINE,*pSPLINE;  

int PonitInPolygon(pSPLINE pline1,pSPLINE pline2){
        int inside[pline2->number];
    int flag=1;
    int i,j;
    //printf("%d %d\n",pline2->number,pline1->number);
    for(i=0;i<pline2->number;i++){
        inside[i]=0;
        for(j=0;j<pline1->number;j++){
            float slope=(pline1->y[(j+1)%pline1->number]-pline1->y[j])/(pline1->x[(j+1)%pline1->number]-pline1->x[j]);
            int xbetween,yabove;
            if((pline1->x[j]<pline2->x[i]&&pline2->x[i]<pline1->x[(j+1)%pline1->number])||(pline1->x[j]>pline2->x[i]&&pline2->x[i]>pline1->x[(j+1)%pline1->number])){
                xbetween=1;
            }
            else{
                xbetween=0;
            }
            //printf("xbetween=%d\n",xbetween);
            if(pline2->y[i]<(slope*(pline2->x[i]-pline1->x[j])+pline1->y[j])){
                yabove=1;
            }
            else{
                yabove=0;
            }
            //printf("yabove=%d\n",yabove);
            if(xbetween&&yabove){
                inside[i]=1-inside[i];
            }
        }
    }
    for(i=0;i<pline2->number;i++){
        if(inside[i]==0){
            flag=0;
        }
    }
    return flag;
} 

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
    }
    
    int a[splinenumber][splinenumber];
    for(i=0;i<splinenumber;i++){
        for(j=0;j<splinenumber;j++){
            if(i==j){
                a[i][j]=0;
            }
            else{
                if(PonitInPolygon(pline[i],pline[j])){
                    a[i][j]=1;
                }
                else if(PonitInPolygon(pline[j],pline[i])){
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