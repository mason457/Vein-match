#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "image.h"
//#include "infile.h"
//#include "outfile.h"
#include "RefFile.h"

extern float savematch(int th,int tw,IMG *R,IMG *RI,IMG *T, IMG *V, char *filename);
extern float matchrate(int th,int tw,IMG *R,IMG *RI,IMG *T, int *mc, int *vc, int *misc);
extern void resizeR(IMG *R,IMG *RI);
extern void rotate(int k, IMG *I,IMG *T);

int main(int argc, char* argv[])
{
	register int i, j,  k;
	IMG *I, *R, *T, *RI;
	int  mink; 
	int tw, th, miscount, mcount, vcount, mh, mw;
	float rate, maxrate; 
	IMG *V;
	char str[80], str1[80], str2[80];
	FILE *fp, *fp1, *fp2;
	int person, i1, i2;

	fp1 = fopen("score_diff.txt", "w");
	fp = fopen("record_diff.txt", "w");
	fp2 = fopen("error.txt", "w+"); 
	//�إߩT�w �v�� 380x320 
	RI = get_img(1, 380, 320); //�إ�RI
	T = get_img(1, 380, 320);  //�إ�T�v��, I����K�׫᪺�v����T

	for(person=0;person<30;person++){  //60�i��..�Ҧp�令30
		i1=person*10; i2=i1+10; 
		for(i=i1;i<i2;i++){  
			//Ū�J�v��R ��reference�v��.. �NR��JRi�v����.
			R = load_tif(reffile[i]);  //Ū�J��i�i�v��
			if(R == NULL) {
				fprintf(fp2, "%d %s\n",i, reffile[i]);
				continue;
			}
			resizeR(R, RI);  //�NR��JRi�v����.
			V=get_img(3, R->h, R->w);  //�x�s�v��. 

			for(j=i2; j<i1+100;j++){  //�qi2�}�l��.��� 6000, 6000�i��, �i�令600=60*10 
				//Ū�J�v��I �� input �v��  
				I = load_tif(reffile[j]); //Ū�J��j�i�v�� 
				if (I == NULL) {
					fprintf(fp2, "%d %s\n", j, reffile[j]);
					continue;
				}
				maxrate=0.0;  
				for (k = -10; k <= 10; k++){ //k�����ਤ�� -10, 10 �i�վ�
					rotate(k,I, T); //�NI����᪺�v���� T  �HT�i���Ri�v�����..

					//�첾����.. th�������դ�60/2��=30��..
					for (th = -40; th <= 40; th+=2){ // th+=2 �i�令 th++ ���|��60
						//tw �����첾����
						for (tw = -40; tw <= 40; tw+=2){ // tw+=2 �i�令 tw++ ���|��60
							//�I�s machrate �i����..
							rate=matchrate(th,tw,R,RI,T, &mcount, &vcount, &miscount);		
							//vcount�O���䳣�O�R��match���I��..
							//mcount�O���䳣�O���match���I��
							//miscount�O ���䤣�k�X���I��  
							//rate=(mcount+10*vcount)/( �`�I��), �`�I��=mcount+10*vcount+miscount
							//vcount ���W10, ��weight�ܤj.
							if(maxrate<rate){
								maxrate=rate;
								mink=k; mh=th; mw=tw;  //�O���̨α��ਤ��, �첾�q..
								//printf("max: %d %d %d %f %d\n",mink, mh, mw, maxrate, (int) fcount);
							}
						} //tw
					}//th
				}//k


				printf("%d %s<-->%d %s\t%f %d %d %d  %d %d %d\n",i, reffile[i],j, reffile[j], 
					maxrate, vcount, mcount, miscount, mink, mh, mw); 

				fprintf(fp, "%d %s<-->%d %s\t%f\t%d\t%d\t%d\t%d\t%d\t%d\n",i, reffile[i],j, reffile[j], 				
					maxrate, vcount, mcount, miscount, mink, mh, mw);	
				fprintf(fp1, "%f\n", maxrate);  


				//���F�O�� ���e�Xmatch���v��..
				rotate(mink,I, T); //I�v���Hmink����o��T�v��.

				itoa(i,str1, 10); itoa(j,str2, 10); 
				strcpy(str, "d:/matchimgs/diff");
				strcat(str, str1); 	strcat(str, "_"); 
				strcat(str, str2);  strcat(str, ".tif"); 
				savematch(mh,mw,R,RI,T,V, str);	  //T�Hmh,mw���� �PRI�i����..�e�X�v��..

				free_img(I); //free memory
			} //for j 
			free_img(R); free_img(V);
		} //i

	} //person
	fclose(fp);  fclose(fp1); fclose(fp2);
	free_img(RI); free_img(T);
	return 1;
}

//itoa(k+10,str1, 10);
//strcpy(str, str1);
//strcat(str,".tif");
////save_tif(str,T);
//strcat(str,".tif");
//save_tif(str,U);

/*
// h' =[cos(th)   sin(th)] [ h ]
// w' =[-sin(th)  cos(th)] [ w ]
//ch=160; cw=120;

minh = 600; maxh = -600;
minw = 600; maxw = -600;
for (k = -25; k <= 25; k++){
for (h = 0; h < 320; h++) {
h1 = h - 160;
for (w = 0; w < 240; w++){
w1 = w - 120;
th = k / 180.0 * M_PI;
h2 = cos(th) *h1 + sin(th) * w1;
w2 = -sin(th) *h1 + cos(th) * w1;
minh = (minh < h2) ? minh : h2;
minw = (minw < w2) ? minw : w2;
maxh = (maxh > h2) ? maxh : h2;
maxw = (maxw > w2) ? maxw : w2;
}
}
}
printf("%d %d %d %d\n", minh, maxh, minw, maxw);
*/


//h1=dh+th; 
//for (tw = -40; tw <= 40; tw+=2){
//	rate=matchrate(th, tw, R, RI,T);

//	w1=dw+tw; 
//	memset(V->base, 0, V->h*V->w);
//	miscount=mcount=vcount=0; 
//	for(h=0;h<H; h++) {
//		h2=h+h1;
//		if(h2<0 || h2>=T->h) continue;
//		for(w=0;w<W;w++){
//			w2=w+w1;
//			if(w2<0 || w2>=T->w) continue; 
//			// h+(cth-ch)+th, w+(ctw-cw)+tw <==> h,w
//			//T->me[h2][w2]= R->me[h][w];

//			g1=T->me[h2][w2]; 
//			if(g1==0) continue;
//			g2=R->me[h][w]; 			
//			if(g2==0) continue; 

//			if(g1==g2){
//				if(g1==240) {
//					vcount ++;
//					V->me[h2][w2]=255;
//				}
//				else {
//					mcount ++;
//					V->me[h2][w2]=200;
//				}
//			}
//			else {
//				miscount ++;
//				V->me[h2][w2]=100;
//			}
//		} //w
//	} //h
//	fcount=mcount+10*vcount;
//	rate=fcount/(fcount+miscount);
//	printf("%d %d %d %f %d\n",k, th, tw, rate, (int) fcount);
//	save_tif("vv.tif",V);



/*

h1=dh+mh;   w1=dw+mw; 
for(h=0;h<H; h++) {
h2=h+h1;
if(h2<0 || h2>=T->h) continue;
for(w=0;w<W;w++){
w2=w+w1;
if(w2<0 || w2>=T->w) continue; 
// h+(cth-ch)+th, w+(ctw-cw)+tw <==> h,w
//T->me[h2][w2]= R->me[h][w];

g1=T->me[h2][w2]; 
if(g1==0) continue;
g2=R->me[h][w]; 			
if(g2==0) continue; 
if(g1==g2){
if(g1==240) {
vcount ++;
U->rme[h][w]=255;
}
else {
U->gme[h][w]=255;
mcount ++;
}
}
else {
miscount ++;
if(g1==255) {
U->rme[h][w]=255;
U->bme[h][w]=255;
}
else if(g2==255){
U->bme[h][w]=255;
U->gme[h][w]=255;
}
else U->bme[h][w]=255;
}
} //w
} //h

save_tif("uu.tif",U);
*

/*

//minh = 600; maxh = -600;
//minw = 600; maxw = -600;
//for (k = -15; k <= 15; k++){
//	for (h = 0; h < 320; h++) {
//		h1 = h - 160;
//		for (w = 0; w < 240; w++){
//			w1 = w - 120;
//			th1 = k / 180.0 * M_PI;
//			h2 = cos(th1) *h1 + sin(th1) * w1;
//			w2 = -sin(th1) *h1 + cos(th1) * w1;
//			minh = (minh < h2) ? minh : h2;
//			minw = (minw < w2) ? minw : w2;
//			maxh = (maxh > h2) ? maxh : h2;
//			maxw = (maxw > w2) ? maxw : w2;
//		}
//	}
//}
//printf("%d %d %d %d\n", minh, maxh, minw, maxw); 
//  minh, maxh, minw, maxw= -185 184 -157 156
//185*2= 370x  157x2= 316  380x320
//640x480  320x240  160x120 
*/
/*int level,exp,h,nextlevel,goal,beseh,beseexp,nowexp,heart;
printf("�п�J����:");
scanf("%d",&level);
printf("�п�J�ثe�g���:");
scanf("%d",&exp);
printf("�п�J�ɶ�(���:�p��):");
scanf("%d",&h);
printf("�п�J�ؼФ���:");
scanf("%d",&goal);
beseh=h*10;
beseexp=(beseh/25)*83;
nowexp=exp+beseexp;
/*
int value[600] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
3,3,3,3,3,3,3,3,3,
4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
6,6,6,6,6,6,6,6,6,6,6,6,6,6,
7,7,7,7,7,7,7,7,
8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
9,9,9,9,9,9,9,9,9,9,9,9,
10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,
11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,
13,13,13,13,13,13,13,13,13,13,13,
14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,
15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
16,16,16,16,16,16,16,16,16,16,
17,17,17,17,17,17,17,17,17,17,17,
18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,
19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,
20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,
21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,
22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,
23,23,23,23,23,23,23,23,23,23,23,23,
24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,
25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,
26,26,26,26,26,26,26,26,26,26,26,26,26,26,26,26,26,26,
27,27,27,27,27,27,27,27,27,27,27,27,27,27,27,27,27,27,27,
28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,
29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,
30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,
31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,
32,32,32,32,32,32,32,32,32,32,32,32,32,
33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,
34,34,34,34,34,34,34,34,34,34,
35,35,35,35,35,35,35,35,35,35,35,35,35,35,35,
36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,
37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,
38,38,38,38,38,38,38,38,38,38,38,38};
/*
int v[60];
int b[38]={4,2,2,2,5,2,2,0,1,3,5,1,2,1,2,0,1,1,1,2,2,1,2,2,0,2,1,1,1,0,0,0,0,1,1,3,2,2};
int sum =0,x,u= 0;
for(int i = 0; i < 38;i++)
{
sum += b[i];
}
x=0;
int j;
while(j < 39)
{
for(int i = x;i<= sum;i++)
{
if(i > b[j])break;
}
if(j>=)
{
v[i] = j+1;
}
x += b[j];
}
for(int t=0;t<=sum;t++)
{
//printf("%.2d ",v[t]);
}
*/
/*int value[6]= {1,2,3,4,5,6};
srand((unsigned)time(NULL));
int num[6],count = 0,a,i,k;
while(count < 6)
{
a = rand()%6+1;
for(i = 0;i <count;i++)
{
if(value[a] == num[i]) break;
}
if(i>=count)
{
num[count++]=value[a];
}
}
for(k=0;k<6;k++){
printf("%.2d ",num[k]);
}
printf("\n");

/*
srand((unsigned)time(NULL));
int num[6],count = 0,a,b,i,k;
while(count < 6)
{
a = rand()%38+1;
for(i = 0;i <count;i++)
{
if(a == num[i]) break;
}
if(i>=count)
{
num[count++]=a;
}
}
b = rand()%8+1;
for(k=0;k<6;k++){
printf("%.2d ",num[k]);
}
printf("\n%d\n",b);

system("pause");*/
