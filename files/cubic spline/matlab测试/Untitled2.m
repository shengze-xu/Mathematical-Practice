var2=load('C:\Users\DELL\Desktop\cubic spline\matlab测试\柯南\data2.txt');
x2=var2(:,1);
y2=var2(:,2);
plot(x2,y2,'b');
x=2.44:0.01:7.6;
n2=length(x2);
N=length(x);
caculatedvalue2=zeros(1,N);
M2=sort2spline(x2,y2,s0,sn);
for i=1:N
    for j=1:n2-1
        if (x(i)>=x2(j)) && (x(i)<=x2(j+1))
            caculatedvalue2(i)=(1+2*1.0*(x(i)-x2(j))/(x2(j+1)-x2(j)))*(x(i)-x2(j+1))*(x(i)-x2(j+1))/((x2(j)-x2(j+1))^2)*y2(j)+(1+2*1.0*(x(i)-x2(j+1))/(x2(j)-x2(j+1)))*(x(i)-x2(j))*(x(i)-x2(j))/((x2(j)-x2(j+1))^2)*y2(j+1)+(x(i)-x2(j))*1.0*(x(i)-x2(j+1))*(x(i)-x2(j+1))/((x2(j)-x2(j+1))^2)*M2(j)+(x(i)-x2(j+1))*1.0*(x(i)-x2(j))*(x(i)-x2(j))/((x2(j)-x2(j+1))^2)*M2(j+1);
        end
    end
end
plot(x,caculatedvalue2,'r');