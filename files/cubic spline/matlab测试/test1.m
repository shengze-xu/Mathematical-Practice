
x1=[1.49 1.62 2.42 3.56 4.47 5.56 6.65 7.31 8.29 8.86];
y1=[8.27 9.29 9.81 9.19 9.54 9.51 9.23 9.87 9.74 8.79];

x=1:0.01:10;
n1=length(x1);

N=length(x);
caculatedvalue1=zeros(1,N);

s0=0;sn=0;
M1=sort1spline(x1,y1,s0,sn);
for i=1:N
    for j=1:n1-1
        if (x(i)>=x1(j)) && (x(i)<=x1(j+1))
            caculatedvalue1(i)=(1+2*1.0*(x(i)-x1(j))/(x1(j+1)-x1(j)))*(x(i)-x1(j+1))*(x(i)-x1(j+1))/((x1(j)-x1(j+1))^2)*y1(j)+(1+2*1.0*(x(i)-x1(j+1))/(x1(j)-x1(j+1)))*(x(i)-x1(j))*(x(i)-x1(j))/((x1(j)-x1(j+1))^2)*y1(j+1)+(x(i)-x1(j))*1.0*(x(i)-x1(j+1))*(x(i)-x1(j+1))/((x1(j)-x1(j+1))^2)*M1(j)+(x(i)-x1(j+1))*1.0*(x(i)-x1(j))*(x(i)-x1(j))/((x1(j)-x1(j+1))^2)*M1(j+1);
        end
    end
end
plot(x,caculatedvalue1,'r');


