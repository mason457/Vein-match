#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "image.h"

//將R填入Ri影像中.
void resizeR(IMG *R,IMG *RI)
{
int ch = R->h / 2, cw = R->w / 2;
int ch1=RI->h/2, cw1=RI->w/2; 
register int h,w;
int dh=ch1-ch, dw=cw1-cw; 

	memset(RI->base, 0, RI->h*RI->w);
	for(h=0;h<R->h;h++) for(w=0;w<R->w;w++) RI->me[h+dh][w+dw]=R->me[h][w];
//	save_tif("RI.tif",RI);
}

//vcount是兩邊都是靜脈match的點數..
//mcount是兩邊都是手指match的點數
//miscount是 兩邊不吻合的點數  
//rate=(mcount+10*vcount)/( 總點數), 總點數=mcount+10*vcount+miscount
//vcount 乘上10, 讓weight變大.
float matchrate(int th,int tw,IMG *R,IMG *RI,IMG *T, int *mc, int *vc, int *misc)
{
	float fcount, rate;
	int ch = R->h / 2, cw = R->w / 2;
	int ch1=RI->h/2, cw1=RI->w/2; 
	register int h,w;
	int dh=ch1-ch, dw=cw1-cw; 
	int h1,w1,h2,w2, g1, g2;
	int miscount, mcount, vcount;


	//	memset(V->base, 0, V->h*V->w); 
	miscount=mcount=vcount=0;

	for(h=0;h<R->h;h++){
		h1=h+dh; h2=h1+th;  
		if(h2<0 || h2>=T->h) continue; 
		for(w=0;w<R->w;w++){
			w1=w+dw; w2=w1+tw;
			if(w2<0 || w2>=T->w) continue; 
			g1=RI->me[h1][w1];  if(g1==0) continue;			
			g2=T->me[h2][w2];   if(g2==0) continue;		
			if(g1==g2){
				if(g1==255) {
					vcount ++;
					//V->me[h2][w2]=255;
				}
				else {
					mcount ++;
					//	V->me[h2][w2]=200;
				}
			}
			else {
				miscount ++;
				//				V->me[h2][w2]=100;
			}
		}
	}
	*vc=vcount; *mc=mcount; *misc=miscount;
	fcount=0.01*mcount+100*vcount;
	rate=fcount/(100*vcount+miscount+0.01*mcount);
	//save_tif("vv.tif",V); 


	return rate;
}

//T以mh,mw平移 與RI進行比對..畫出影像..存入filename tif中.
float savematch(int th,int tw,IMG *R,IMG *RI,IMG *T, IMG *V, char *filename)
{
	int ch = R->h / 2, cw = R->w / 2;
	int ch1=RI->h/2, cw1=RI->w/2; 
	register int h,w;
	int dh=ch1-ch, dw=cw1-cw; 
	int h1,w1,h2,w2, g1, g2;
//	int count,miscount, mcount, vcount;

	memset(V->r, 0, V->h*V->w); 
	memset(V->g, 0, V->h*V->w); 
	memset(V->b, 0, V->h*V->w);  

	//miscount=mcount=vcount=0;
	for(h=0;h<R->h;h++){
		h1=h+dh; h2=h1+th;  
		if(h2<0 || h2>=T->h) continue; 
		for(w=0;w<R->w;w++){
			w1=w+dw; w2=w1+tw;
			if(w2<0 || w2>=T->w) continue; 
			g1=RI->me[h1][w1]; if(g1==0) continue;			
			g2=T->me[h2][w2];   if(g2==0) continue;	
			if(g1 !=g2) {
				V->rme[h][w]=V->gme[h][w]=V->bme[h][w]=50;
			}
			else{
				V->rme[h][w]=g1; 
				V->gme[h][w]=g2;
			}
		}
	}

	//save_tif("vv.tif",V); 
	
	save_tif(filename,V);  //將V影像存入filename檔中.
	return 0;
}

//將I旋轉後的影像為 T  以T進行對Ri影像比對..
void rotate(int k, IMG *I,IMG *T)
{
	register int h,w;
	int H, W, H2, W2;
	int h1, w1, h3,w3;
	double th;
	int cth=T->h/2, ctw=T->w/2;  //T 影像中心點
	int ch=I->h/2, cw=I->w/2;  //I 影像中心點
	unsigned char **tme=T->me; 
	unsigned char **ime=I->me;

	memset(T->base, 0, T->h*T->w);

	//400 x 360
	H=I->h; W=I->w; H2=H-2; W2=W-2;
	th=k/180.0 * M_PI;
	for(h=0;h<T->h;h++) {
		h1=h-cth; 
		for(w=0;w<T->w;w++){
			w1=w-ctw;
			h3=ch + cos(th) *h1 - sin(th) * w1;
			w3=cw + sin(th) *h1 +cos(th) * w1;

			if(h3<0 || w3<0 || h3>H2 || w3>W2) tme[h][w]=0;
			else tme[h][w]=ime[h3][w3];
		}
	}

}

int match(unsigned char **work,IMG *T, IMG *R, int *shifth,int *shiftw)
{
	int H = R->h; //400
	int W = R->w; //360
	register int h, w;
	unsigned char **me = R->me;
	int ch = R->h / 2,  h1 = ch - 45;
	int cw = R->w / 2,  w1 = cw - 60;

	int count = 0;
	int min = 1000000;
	int th, tw;
	int h2, w2, mh=0, mw=0;

	for (th = -10; th <= 10; th++){
		for (tw = -10; tw <= 10; tw++){
			h2 = h1 + th; w2 = w1 + tw;
			count = 0;
			for (h = 0; h < 90; h++){
				for (w = 0; w < 120; w++){
					if (work[h][w] != me[h + h2][w + w2]) count++;					
				}
			}
			if (min > count) {
				min = count;
				mh = th; 
				mw = tw;
			}

		}
	}
	*shifth=mh;  *shiftw=mw;

	int vcount=0;
	int mcount=0;
	int miscount = 0;
	int g1, g2;
	for(h=0;h<H; h++) {
		h1=h+mh; 
		if(h1<0 || h1>=H) continue;
		for(w=0;w<W;w++){
			w1=w+mw;
			if(w1<0 || w>=W) continue; 
			g1=T->me[h][w]; 
			if(g1==0) continue;
			g2=R->me[h1][w1]; 			
			if(g2==0) continue; 
			if(g1==g2){
				if(g1==255) vcount ++;
				else mcount ++;
			}
			else miscount ++;
		}
	}
	printf(" %d %d %d %d %f\n", vcount, mcount, vcount+mcount, miscount, 
		(float)(vcount+mcount)/(miscount + vcount + mcount) );

	return vcount+mcount;
}


void 	plotTR1(int mh, int mw,IMG *T,IMG *R)
{
	register int h,w;
	int h1, w1;
	int vcount=0;
	int mcount=0;
	int miscount = 0;
	int g1, g2;
	IMG *U;
	int H=T->h, W=T->w;

	U=get_img(3, T->h, T->w); 

	for(h=0;h<H; h++) {
		h1=h+mh; 
		if(h1<0 || h1>=T->h) continue;
		for(w=0;w<W;w++){
			w1=w+mw;
			if(w1<0 || w>=T->w) continue; 
			g1=T->me[h1][w1]; 
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
		}
	}
	save_tif("RGB.tif",U);
	free_img(U);
}

