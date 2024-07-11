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
    int splinenumber=13;
    char name[20];
    int i,j;
    FILE *fp;
    SPLINE line[splinenumber];
    pSPLINE pline[splinenumber];
    for(i=0;i<splinenumber;i++){
        pline[i]=&line[i];
    }

    line[0].number=100;
    line[1].number=91;
    line[2].number=52;
    line[3].number=14;
    line[4].number=8;
    line[5].number=9;
    line[6].number=12;
    line[7].number=20;
    line[8].number=11;
    line[9].number=9;
    line[10].number=11;
    line[11].number=12;
    line[12].number=30;
    
    for(i=0;i<splinenumber;i++){
        sprintf(name,"panda%d.txt",i+1);
        fp=fopen(name,"r");
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
            printf("%2d ",a[i][j]);
        }
        printf("\n");
    } 
    printf("a[i][j] represents the partial order relationship between the i-th curve and the j-th curve.\n");
    printf("1 means the former contains the latter, -1 is the opposite, 0 means no containment.\n");
    return 0;
}
