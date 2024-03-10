#define _CRT_SECURE_NO_WARNINGS  //旧関数のwarning出力無視

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "header.h"

typedef	unsigned char	uchar;

double c1_4=0.70710678, c1_8=0.92387953, c3_8=0.38268343, c1_16=0.98078528, c3_16=0.83146961, c5_16=0.55557023, c7_16=0.19509032;

// JPEG Quantization matrix ---
int q_matrix_y[8][8]={16, 11, 10, 16, 24, 40, 51, 61,
                      12, 12, 14, 19, 26, 58, 60, 55,
					  14, 13, 16, 24, 40, 57, 69, 56,
					  14, 17, 22, 29, 51, 87, 80, 62,
					  18, 22, 37, 56, 68,109,103, 77,
					  24, 35, 55, 64, 81,104,113, 92,
					  49, 64, 78, 87,103,121,120,101,
					  72, 92, 95, 98,112,100,103, 99};

int q_matrix_c[8][8]={17,18,24,47,99,99,99,99,
                      18,21,26,66,99,99,99,99,
					  24,26,56,99,99,99,99,99,
					  47,66,99,99,99,99,99,99,
					  99,99,99,99,99,99,99,99,
					  99,99,99,99,99,99,99,99,
					  99,99,99,99,99,99,99,99,
					  99,99,99,99,99,99,99,99}; 


//----------------------------------------------------------------
//  Function-ID	rounding(double dv)　ダブル型値を0-255の範囲に丸める
//----------------------------------------------------------------
unsigned char rounding(double dv)
{
    unsigned char ucv; 
	int iv = (int)(dv + 0.5);
	if (iv < 0) {
		iv=0;
	}
	else if (iv > 255) {
		iv=255;
	}
    ucv = (unsigned char)iv;
    return ucv;
}

//----------------------------------------------------------------
//  Function-ID	rounding_integer(int idv) 整数値を0-255の範囲に丸める
//----------------------------------------------------------------
unsigned char rounding_integer(int idv)
{
    unsigned char ucv; 
	int iv;
	iv=idv;
	if (idv < 0){
		iv=0;
	}
	else if (idv > 255){
		iv=255;
	}
    ucv = (unsigned char)iv;
    return ucv;
}

//------------------------------------------------------------------------------
//	Function-ID	m_alloc
//	Function-Name	メモリ確保
//	Abstract	異常終了時にエラーメッセージを返すmalloc関数
//	Argument	int	size	= 確保するメモリサイズ
//	Return-Value	void	*a	= 確保したメモリのアドレス
//	Special_Desc
//------------------------------------------------------------------------------
void *
m_alloc(int	size)
{
    void	*a;

    if ((a = malloc(size)) == NULL) {
	fprintf(stderr, "m_alloc: memory allocation error.\n");
	exit(-1);
    }

    return (a);
}



//----------------------------------------------------------------
//	Function-ID	alloc_IMG_YUV
//	Function-Name	IMGデータ領域確保
//	Abstract	IMG_YUVデータのメモリを確保する
//	Argument	int	width	= 画像横サイズ
//				int	height	= 画像縦サイズ
//	Return-Value	IMG_YUV *		= 確保した領域のポインタ
//	Special_Desc	
//----------------------------------------------------------------
IMG_YUV *
alloc_IMG_YUV(int	width,  int	height)
{
    IMG_YUV	*img;

    img		= (IMG_YUV   *) m_alloc(sizeof (IMG_YUV));
    img->width	= width;
    img->height	= height;
    img->pixel	= img->width * img->height;
    img->Y	= (uchar *) m_alloc(sizeof (uchar) * img->pixel);
	img->U	= (uchar *) m_alloc(sizeof (uchar) * img->pixel);
	img->V	= (uchar *) m_alloc(sizeof (uchar) * img->pixel);

    return (img);
}

//----------------------------------------------------------------
//	Function-ID	alloc_IMG_RGB
//	Function-Name	IMGデータ領域確保
//	Abstract	IMG_RGBデータのメモリを確保する
//	Argument	int	width	= 画像横サイズ
//				int	height	= 画像縦サイズ
//	Return-Value	IMG_RGB *		= 確保した領域のポインタ
//	Special_Desc	
//----------------------------------------------------------------
IMG_RGB *
alloc_IMG_RGB(int	width,  int	height)
{
    IMG_RGB	*img;

    img		= (IMG_RGB   *) m_alloc(sizeof (IMG_RGB));
    img->width	= width;
    img->height	= height;
    img->pixel	= img->width * img->height;
    img->R	= (uchar *) m_alloc(sizeof (uchar) * img->pixel);
	img->G	= (uchar *) m_alloc(sizeof (uchar) * img->pixel);
	img->B	= (uchar *) m_alloc(sizeof (uchar) * img->pixel);

    return (img);
}

//------------------------------------------------------------------------------
//	Function-ID	free_IMG_YUV
//	Function-Name	IMG_YUVデータメモリ解放
//	Abstract	IMG_YUVデータの領域を解放する
//	Argument	IMG_YUV	*img	= 画像構造体
//	Return-Value	Nothing
//	Special_Desc	
//------------------------------------------------------------------------------
void
free_IMG_YUV(IMG_YUV	*img)
{
    if (img->Y)		free(img->Y);
    if (img->U)		free(img->U);
    if (img->V)		free(img->V);
    free(img);
}

//------------------------------------------------------------------------------
//	Function-ID	free_IMG_RGB
//	Function-Name	IMG_RGBデータメモリ解放
//	Abstract	IMG_RGBデータの領域を解放する
//	Argument	IMG_RGB	*img	= 画像構造体
//	Return-Value	Nothing
//	Special_Desc	
//------------------------------------------------------------------------------
void
free_IMG_RGB(IMG_RGB	*img)
{
    if (img->R)		free(img->R);
    if (img->G)		free(img->G);
    if (img->B)		free(img->B);
    free(img);
}

//----------------------------------------------------------------
//	Function-ID	read_bmp
//	Function-Name	ビットマップ読み込み
//	Abstract	img_rgbに読み込み、 YUV変換してimgに格納
//	Argument	IMG_YUV *img:	YUV信号を格納する領域
//              IMG_RGB *img_rgb:	読み込んだBMPを格納する領域
//              FILE *infile:       読み込み先ポインタ
//	Special_Desc	24ビットのみ対応
//
//            +---------------------------------
//               Coded by Takashi Yoshino
//               Revised by Y.Yashima 2012.09.15
//----------------------------------------------------------------
void read_bmp(IMG_YUV *img,IMG_RGB *img_rgb, FILE *infile){
	int width, height;
	int size;

	width = img->width;
    height = img->height;
	size = width * height;
	int k,j;
	double y,u,v;
//	double r,g,b;

	//画像データを読み込む
	for( k=height-1; k>=0; k--){
		for( j=0; j<width; j++){
			img_rgb->B[j+k*width] = getc(infile);
			img_rgb->G[j+k*width] = getc(infile);
			img_rgb->R[j+k*width] = getc(infile);
		}
	}

	for(int k=0;k<height;k++){
		for(int j=0;j<width;j++){
				y = 0.2126 * img_rgb->R[ j+k*width ] + 0.7152 * img_rgb->G[ j+k*width ] + 0.0722 * img_rgb->B[ j+k*width ];
				u = -0.1146 * img_rgb->R[ j+k*width ] - 0.3854 * img_rgb->G[ j+k*width ] + 0.5 * img_rgb->B[ j+k*width ];
				v = 0.5 * img_rgb->R[ j+k*width ] - 0.4542 * img_rgb->G[ j+k*width ] - 0.0458 * img_rgb->B[ j+k*width ];
				img->Y[ j + k*width ] = rounding(y);       /* Y conponent */
				img->U[ j + k*width ] = rounding(u+128);   /* U conponent */
				img->V[ j + k*width ] = rounding(v+128);   /* V conponent */
		}
	}
}
//----------------------------------------------------------------
//	Function-ID	write_bmp
//	Function-Name	ビットマップ書き込み
//	Abstract	YUVをRGB変換してimg_rbg2に格納
//	Argument	IMG_YUV *img:	YUV信号が格納されている画像データ
//              IMG_RGB *img_rgb:       色変換後の画像格納領域（作業用配列）   
//              FILE *outfile:       書き込み先ポインタ
//	Special_Desc	24ビットのみ対応
//
//            +---------------------------------
//               Coded by Takashi Yoshino
//               Revised by Y.Yashima 2012.09.15
//----------------------------------------------------------------
void write_bmp(IMG_YUV *img, IMG_RGB *img_rgb, FILE *outfile){
    int width, height;
	int size;

	width = img->width;
    height = img->height;
	size = width * height;
	int k,j;
	double r,g,b;

	  	//YUV-RGB変換と並べ替え
		for(k=0;k<height;k++){
			for(j=0; j<width; j++){
			  	r = img->Y[ j + k*width ] + 1.5748 * (img->V[ j + k*width ]-128);
				g = img->Y[ j + k*width ] - 0.1873 * (img->U[ j + k*width ]-128) - 0.4681 * (img->V[ j + k*width ]-128);
				b = img->Y[ j + k*width ] + 1.8556 * (img->U[ j + k*width ]-128) ;
				img_rgb->R[ j + (height-k-1)*width ] = rounding(r);
				img_rgb->G[ j + (height-k-1)*width ] = rounding(g);
				img_rgb->B[ j + (height-k-1)*width ] = rounding(b);
			}
		}

		 //ヘッダ書き込み
	  uchar header_buf[54];
	  unsigned int file_size;
	  unsigned int offset_to_data;
	  unsigned long info_header_size;
	  unsigned int planes;
	  unsigned int color;
	  unsigned long compress;
	  unsigned long data_size;
	  long xppm;
	  long yppm;

	  file_size=size+54;
	  offset_to_data=54;
	  info_header_size=40;
	  planes = 1;
	  color = 24;
	  compress = 0;
	  data_size = size;
	  xppm = 1;
	  yppm = 1;

	  header_buf[0]='B';
	  header_buf[1]='M';
	  memcpy(header_buf+2,&file_size,sizeof(file_size));
	  header_buf[6]=0;
	  header_buf[7]=0;
	  header_buf[8]=0;
	  header_buf[9]=0;
	  memcpy(header_buf + 10, &offset_to_data, sizeof(file_size));
	  header_buf[11]=0;
	  header_buf[12]=0;
	  header_buf[13]=0;
	  memcpy(header_buf + 14, &info_header_size, sizeof(info_header_size));
	  header_buf[15] = 0;
	  header_buf[16] = 0;
	  header_buf[17] = 0;
	  memcpy(header_buf + 18, &width, sizeof(width));
	  memcpy(header_buf + 22, &height, sizeof(height));
	  memcpy(header_buf + 26, &planes, sizeof(planes));
	  memcpy(header_buf + 28, &color, sizeof(color));
	  memcpy(header_buf + 30, &compress, sizeof(compress));
	  memcpy(header_buf + 34, &data_size, sizeof(data_size));
	  memcpy(header_buf + 38, &xppm, sizeof(xppm));
	  memcpy(header_buf + 42, &yppm, sizeof(yppm));
	  header_buf[46] = 0;
	  header_buf[47] = 0;
	  header_buf[48] = 0;
	  header_buf[49] = 0;
	  header_buf[50] = 0;
	  header_buf[51] = 0;
	  header_buf[52] = 0;
	  header_buf[53] = 0;

	  fwrite(header_buf, sizeof(unsigned char), 54, outfile);

	  for(int kk=0;kk<size;kk++){
		  fputc(img_rgb->B[kk],outfile);		//Bを出力
		  fputc(img_rgb->G[kk],outfile);		//G
		  fputc(img_rgb->R[kk],outfile);		//R
	  }
}

//----------------------------------------------------------------
//	Function-ID	copy_image
//	Function-Name	
//	Abstract	IMG_YUV型構造体の画像データをコピーする（構造体ポインタを引き渡す）
//
//	Argument	IMG_YUV *img1: copy元画像データ
//		        IMG_YUV *img2: copy先画像データ
//		        int param : param=1(0以外)のときYUVすべてをコピー（カラーのままコピー）
//                          param=0のときYだけコピーしてUとVには128を入れる（モノクロ化）
//			
//	Return-Value	
//	Special_Desc
//
//            +---------------------------------
//               Coded by Y.Yashima
//
//----------------------------------------------------------------
void  copy_image(IMG_YUV *img1, IMG_YUV *img2, int param)
{

	 int j, k; 
	 int width, height;

	 width = img1->width;
     height = img1->height;

	 if(param==0) {
		 for(k=0;k<height;k++){
			 for(j=0; j<width; j++){
				 img2->Y[ j + k*width ] = img1->Y[ j + k*width ];
				 img2->U[ j + k*width ] = 128;
				 img2->V[ j + k*width ] = 128;
			 }
		 }
	 }
	 else{
		 for(k=0;k<height;k++){
			 for(j=0; j<width; j++){
				 img2->Y[ j + k*width ] = img1->Y[ j + k*width ];
				 img2->U[ j + k*width ] = img1->U[ j + k*width ];
				 img2->V[ j + k*width ] = img1->V[ j + k*width ];
			 }
		 }
	 }

}

//----------------------------------------------------------------
//	Function-ID	setvalue_image
//	Function-Name	
//	Abstract	IMG_YUV型構造体の画像データの各コンポーネントに定められた値をセットする
//
//	Argument	IMG_YUV *img1: 画像データ
//		        int Yvalue : 輝度信号にセットする値（0-255）
//		        int Uvalue : 輝度信号にセットする値（0-255）
//		        int Vvalue : 輝度信号にセットする値（0-255）
//			
//	Return-Value	
//	Special_Desc	
//----------------------------------------------------------------
void  setvalue_image(IMG_YUV *img1, int Yvalue, int Uvalue, int Vvalue)
{

	int j, k; 
	int width, height;
	unsigned char Ydata, Udata, Vdata;

	width = img1->width;
    height = img1->height;

	if(Yvalue<0) {
		Ydata = 0;
	}
	else if(Yvalue>255){
		Ydata = 255;
	}
	else{
		Ydata = (unsigned char) Yvalue;
	}

	if(Uvalue<0) {
		Udata = 0;
	}
	else if(Uvalue>255){
		Udata = 255;
	}
	else{
		Udata = (unsigned char) Uvalue;
	}

	if(Vvalue<0) {
		Vdata = 0;
	}
	else if(Vvalue>255){
		Vdata = 255;
	}
	else{
		Vdata = (unsigned char) Vvalue;
	}

	for(k=0;k<height;k++){
		for(j=0; j<width; j++){
			img1->Y[ j + k*width ] = Ydata;
	    }
	}

	for(k=0;k<height;k++){
		for(j=0; j<width; j++){
			img1->U[ j + k*width ] = Udata;
		    img1->V[ j + k*width ] = Vdata;
		}
	}

}

//----------------------------------------------------------------
//	Function-ID	snr_image
//	Function-Name	
//	Abstract	IMG_YUV型構造体の画像データのSNRを求める
//
//
//	Argument	IMG_YUV *img1: 原画像データ(input)
//		        IMG_YUV *img2: 処理画像データ(input)
//		        *snr_Y : 輝度信号YのSN比
//		        *snr_U : 色差信号UのSN比
//		        *snr_V : 色差信号VのSN比
//			
//	Return-Value	
//	Special_Desc	
//----------------------------------------------------------------
void  snr_image(IMG_YUV *img1, IMG_YUV *img2, double *snr_Y, double *snr_U, double *snr_V)
{

	int j, k;
	int width, height, size;
	int errorY, errorY2, errorU, errorU2, errorV, errorV2;
	double rmse_errorY2, rmse_errorU2, rmse_errorV2;

	width = img1->width;
	height = img1->height;
	size = width * height;

	errorY2 = 0;
	for (k = 0; k<height; k++){
		for (j = 0; j<width; j++){
			errorY = img1->Y[j + k*width] - img2->Y[j + k*width];
			errorY2 = errorY2 + errorY * errorY;
		}
	}
	rmse_errorY2 = sqrt((double)errorY2 / (double)(size));
	if (rmse_errorY2 == 0){
		*snr_Y = 1000;
	}
	else{
		*snr_Y = 20 * log10(255 / rmse_errorY2);
	}

	errorU2 = 0;
	errorV2 = 0;
	for (k = 0; k<height; k++){
		for (j = 0; j<width; j++){
			errorU = img1->U[j + k*width] - img2->U[j + k*width];
			errorU2 = errorU2 + errorU * errorU;
			errorV = img1->V[j + k*width] - img2->V[j + k*width];
			errorV2 = errorV2 + errorV * errorV;
		}
	}
	rmse_errorU2 = sqrt((double)errorU2 / (double)(size));
	if (rmse_errorU2 == 0){
		*snr_U = 1000;
	}
	else{
		*snr_U = 20 * log10(255 / rmse_errorU2);
	}

	rmse_errorV2 = sqrt((double)errorV2 / (double)(size));
	if (rmse_errorV2 == 0){
		*snr_V = 1000;
	}
	else{
		*snr_V = 20 * log10(255 / rmse_errorV2);
	}

}

//------------------------------------------------------------------------------
//	Function-ID	dct88
//	Abstract	8x8 DCTを行う
//	Argument	(*xx)[8]: 8x8入力画像マトリクス
//              (*yy)[8]: 8x8出力DCT係数マトリクス
//	Return-Value	Nothing
//	Special_Desc　Tis function calls subfunction 'dct8'(Chen's fast algorithm)
//------------------------------------------------------------------------------
void dct88(int (*xx)[8], double (*yy)[8])

{
     double x[8], y[8];
 	 double zz[8][8];
	 int jj,kk;

	 for (kk=0; kk<8; kk++){
        for (jj=0; jj<8; jj++){
	 	    x[jj]=xx[jj][kk];
		}
		dct8(x, y);
		for (jj=0; jj<8; jj++){
	 	    zz[jj][kk]=y[jj];
		}
	 }

//	 for (kk=0; kk<8; kk++){
//      for (jj=0; jj<8; jj++){
//	 	    printf(" %7.3f ", zz[jj][kk]);
//		}
//		printf("\n");
//	 }
//	 printf("\n");


	 for (jj=0; jj<8; jj++){
        for (kk=0; kk<8; kk++){
	 	    x[kk]=zz[jj][kk];
		}
		dct8(x, y);
		for (kk=0; kk<8; kk++){
	 	    yy[jj][kk]=y[kk]/4;
		}
	 }

}


void dct8(double x[], double y[])
{

	 double a[8], b[8], c[8];

	 a[0]= x[0]+x[7];
	 a[1]= x[1]+x[6];
	 a[2]= x[2]+x[5];
	 a[3]= x[3]+x[4];
	 a[4]=-x[4]+x[3];
	 a[5]=-x[5]+x[2];
	 a[6]=-x[6]+x[1];
	 a[7]=-x[7]+x[0];

	 b[0]= a[0]+a[3];
	 b[1]= a[1]+a[2];
	 b[2]=-a[2]+a[1];
	 b[3]=-a[3]+a[0];
	 b[4]= a[4];
	 b[5]= c1_4 * (-a[5]+a[6]);
	 b[6]= c1_4 * ( a[6]+a[5]);
	 b[7]= a[7];
     
	 c[0]= c1_4 * ( b[0]+b[1]);
	 c[1]= c1_4 * (-b[1]+b[0]);
	 c[2]= (c3_8 * b[2] + c1_8 * b[3]);
	 c[3]= (c3_8 * b[3] - c1_8 * b[2]);
	 c[4]= b[4]+b[5];
	 c[5]=-b[5]+b[4];
	 c[6]=-b[6]+b[7];
	 c[7]= b[7]+b[6];

     y[0]=c[0];
     y[4]=c[1];
     y[2]=c[2];
     y[6]=c[3];
     y[1]= ( c7_16 * c[4] + c1_16 * c[7] );
     y[5]= ( c3_16 * c[5] + c5_16 * c[6] );
     y[3]= ( c3_16 * c[6] - c5_16 * c[5] );
     y[7]= ( c7_16 * c[7] - c1_16 * c[4] );

}

//------------------------------------------------------------------------------
//	Function-ID	idct88
//	Abstract	8x8 逆DCTを行う
//	Argument	(*yy2)[8]: 8x8入力DCT係数マトリクス
//              (*xx2)[8]: 8x8出力画像データマトリクス
//	Return-Value	Nothing
//	Special_Desc　Tis function calls subfunction 'idct8'(Chen's fast algorithm)
//------------------------------------------------------------------------------
void idct88(double (*yy2)[8], int (*xx2)[8])

{
     double x[8], y[8];
 	 double zz[8][8];
	 int jj,kk;
	 int temp;

	 for (kk=0; kk<8; kk++){
        for (jj=0; jj<8; jj++){
	 	    x[jj]=yy2[jj][kk];
		}
		idct8(x, y);
		for (jj=0; jj<8; jj++){
	 	    zz[jj][kk]=y[jj];
		}
	 }

//	 for (kk=0; kk<8; kk++){
//      for (jj=0; jj<8; jj++){
//	 	    printf(" %7.3f ", zz[jj][kk]);
//		}
//		printf("\n");
//	 }
//	 printf("\n");


	 for (jj=0; jj<8; jj++){
        for (kk=0; kk<8; kk++){
	 	    x[kk]=zz[jj][kk];
		}
		idct8(x, y);
		for (kk=0; kk<8; kk++){
			temp=(int) (y[kk]/4+0.5);
			if(temp<0) temp=0;
			if(temp>255) temp=255;
			xx2[jj][kk]=temp;
		}
	 }

}


void idct8(double y2[], double x2[])
{

	 double a[8], b[8], c[8];

     c[0]=y2[0];
     c[1]=y2[4];
     c[2]=y2[2];
     c[3]=y2[6];
     c[4]= ( c7_16 * y2[1] - c1_16 * y2[7] );
     c[5]= ( c3_16 * y2[5] - c5_16 * y2[3] );
     c[6]= ( c3_16 * y2[3] + c5_16 * y2[5] );
     c[7]= ( c7_16 * y2[7] + c1_16 * y2[1] );

	 b[0]= c1_4 * ( c[0]+c[1]);
	 b[1]= c1_4 * (-c[1]+c[0]);
	 b[2]= (c3_8 * c[2] - c1_8 * c[3]);
	 b[3]= (c3_8 * c[3] + c1_8 * c[2]);
	 b[4]= c[4]+c[5];
	 b[5]=-c[5]+c[4];
	 b[6]=-c[6]+c[7];
	 b[7]= c[7]+c[6];

	 a[0]= b[0]+b[3];
	 a[1]= b[1]+b[2];
	 a[2]=-b[2]+b[1];
	 a[3]=-b[3]+b[0];
	 a[4]= b[4];
	 a[5]= c1_4 * (-b[5]+b[6]);
	 a[6]= c1_4 * ( b[6]+b[5]);
	 a[7]= b[7];

	 x2[0]= a[0]+a[7];
	 x2[1]= a[1]+a[6];
	 x2[2]= a[2]+a[5];
	 x2[3]= a[3]+a[4];
	 x2[4]=-a[4]+a[3];
	 x2[5]=-a[5]+a[2];
	 x2[6]=-a[6]+a[1];
	 x2[7]=-a[7]+a[0];

}

//------------------------------------------------------------------------------
//	Function-ID	quantization88_new
//	Abstract	8x8 DCT係数の量子化を行う．スケールファクタとして実数値を許容するバージョン．
//	Argument	(*xx)[8]: 8x8入力DCT係数マトリクス
//              (*outvalue)[8]: 8x8出力量子化代表値マトリクス
//              int qscaleDC: 直流係数のスケールファクタ（qscaleDC=16のときデフォールトのマトリクスが量子化ステップ）
//              int qscaleAC: 交流係数のスケールファクタ（qscaleAC=16のときデフォールトのマトリクスが量子化ステップ）
//              color: 0: 輝度信号，1:色差信号 （量子化マトリクスが異なる）
//	Return-Value	Nothing
//	Special_Desc　量子化マトリクスq_matrix_y[jj][kk]をグローバルで定義していること
//                量子化ステップは， qs= qscale/16 * Qmatrix　となる． 
//                qscaleDC>2, qscaleAC>4
//------------------------------------------------------------------------------
void quantization88_new(double (*xx)[8], double (*outvalue)[8], int qscaleDC, int qscaleAC, int color)

{
	 int jj,kk;
	 int qstep, qlevel, qvalue;
	 double qsDC, qsAC;

	 qsDC=(double) qscaleDC/16;
	 qsAC=(double) qscaleAC/16;

	 if(color==0){                        //輝度信号
		 for (kk=0; kk<8; kk++){
			 for (jj=0; jj<8; jj++){
				 if((jj+kk)==0) { 
					 qstep= (int) (q_matrix_y[kk][jj] * qsDC);
					 if(qstep<1) qstep=1;
					 qlevel= (int) (xx[jj][kk]/(double)qstep+0.5);
					 qvalue= qlevel * qstep;
				 }
				 else{
					 qstep=(int) (q_matrix_y[kk][jj] * qsAC);
					 if(qstep<1) qstep=1;
					 qlevel= (int) (fabs(xx[jj][kk])/(double)qstep+0.5);
					 qvalue= qlevel * qstep;
					 if(xx[jj][kk]<0) {qlevel=-qlevel; qvalue=-qvalue; }
				 }
				 outvalue[jj][kk]=qvalue;
			 }
		 }
	 }
	 else{                                   //色差信号
		 for (kk=0; kk<8; kk++){
			 for (jj=0; jj<8; jj++){
				 if((jj+kk)==0) { 
					 qstep=(int) (q_matrix_c[kk][jj] * qsDC);
					 if(qstep<1) qstep=1;
					 qlevel= (int) (xx[jj][kk]/(double)qstep+0.5);
					 qvalue= qlevel * qstep;
				 }
				 else{
					 qstep=(int) (q_matrix_c[kk][jj] * qsAC);
					 if(qstep<1) qstep=1;
					 qlevel= (int) (fabs(xx[jj][kk])/(double)qstep+0.5);
					 qvalue= qlevel * qstep;
					 if(xx[jj][kk]<0) {qlevel=-qlevel; qvalue=-qvalue; }
				 }
				 outvalue[jj][kk]=qvalue;
			 }
		 }
	 }

}

void create_qp_line(uchar (*qp_line)[16], int qp)
{
	int number, number1;
	int jj,kk,jst;
	uchar numarray[16][16];

	for (kk=0; kk<16; kk++){
		for (jj=0; jj<104; jj++){
			qp_line[jj][kk] = 0;
		}
	}

	jst = 8;
	number = qp/1000;
	create_number(numarray, number);
	for (kk=0; kk<16; kk++){
		for (jj=0; jj<16; jj++){
			qp_line[jst+jj][kk] = numarray[jj][kk];
		}
	}

	jst = jst+24;
	number1 = qp - number*1000;
	number = number1/100;
	create_number(numarray, number);
	for (kk=0; kk<16; kk++){
		for (jj=0; jj<16; jj++){
			qp_line[jst+jj][kk] = numarray[jj][kk];
		}
	}

	jst = jst+24;
	number1 = number1 - number*100;
	number = number1/10;
	create_number(numarray, number);
	for (kk=0; kk<16; kk++){
		for (jj=0; jj<16; jj++){
			qp_line[jst+jj][kk] = numarray[jj][kk];
		}
	}

	jst = jst+24;
	number1 = number1 - number*10;
	number = number1;
	create_number(numarray, number);
	for (kk=0; kk<16; kk++){
		for (jj=0; jj<16; jj++){
			qp_line[jst+jj][kk] = numarray[jj][kk];
		}
	}

}

void create_number_line(uchar (*array_line)[16], int student_number)
{
	int number, number1;
	int jj,kk,jst;
	uchar numarray[16][16];

	for (kk=0; kk<16; kk++){
		for (jj=0; jj<176; jj++){
			array_line[jj][kk] = 0;
		}
	}

	jst = 8;
	number = student_number/1000000;
	create_number(numarray, number);
	for (kk=0; kk<16; kk++){
		for (jj=0; jj<16; jj++){
			array_line[jst+jj][kk] = numarray[jj][kk];
		}
	}

	jst = jst+24;
	number1 = student_number - number*1000000;
	number = number1/100000;
	create_number(numarray, number);
	for (kk=0; kk<16; kk++){
		for (jj=0; jj<16; jj++){
			array_line[jst+jj][kk] = numarray[jj][kk];
		}
	}

	jst = jst+24;
	number1 = number1 - number*100000;
	number = number1/10000;
	create_number(numarray, number);
	for (kk=0; kk<16; kk++){
		for (jj=0; jj<16; jj++){
			array_line[jst+jj][kk] = numarray[jj][kk];
		}
	}

	jst = jst+24;
	number1 = number1 - number*10000;
	number = number1/1000;
	create_number(numarray, number);
	for (kk=0; kk<16; kk++){
		for (jj=0; jj<16; jj++){
			array_line[jst+jj][kk] = numarray[jj][kk];
		}
	}

	jst = jst+24;
	number1 = number1 - number*1000;
	number = number1/100;
	create_number(numarray, number);
	for (kk=0; kk<16; kk++){
		for (jj=0; jj<16; jj++){
			array_line[jst+jj][kk] = numarray[jj][kk];
		}
	}

	jst = jst+24;
	number1 = number1 - number*100;
	number = number1/10;
	create_number(numarray, number);
	for (kk=0; kk<16; kk++){
		for (jj=0; jj<16; jj++){
			array_line[jst+jj][kk] = numarray[jj][kk];
		}
	}

	jst = jst+24;
	number1 = number1 - number*10;
	number = number1;
	create_number(numarray, number);
	for (kk=0; kk<16; kk++){
		for (jj=0; jj<16; jj++){
			array_line[jst+jj][kk] = numarray[jj][kk];
		}
	}

}

void create_number( uchar (*numarray)[16], int number)
{
	int jj,kk;

	for (kk=0; kk<16; kk++){
		for (jj=0; jj<16; jj++){
			numarray[jj][kk] = 0;
		}
	}

	if(number == 0 || number == 2 || number == 3 || number == 5 || number == 6 || number == 7 || number == 8 || number == 9){  
		/* pettern-1 */
		kk = 0;
		for (jj=0; jj<16; jj++){
			numarray[jj][kk] = 255;
		}
	}

	if(number == 0 || number == 4 || number == 5 || number == 6 || number == 8 || number == 9){  
		/* pettern-2 */
		jj = 0;
		for (kk=0; kk<8; kk++){
			numarray[jj][kk] = 255;
		}
	}

	if(number == 0 || number == 1 || number == 2 || number == 3 || number == 4 || number == 7 || number == 8 || number == 9){  
		/* pettern-3 */
		jj = 15;
		for (kk=0; kk<8; kk++){
			numarray[jj][kk] = 255;
		}
	}

	if(number == 2 || number == 3 || number == 4 || number == 5 || number == 6 || number == 8 || number == 9){  
		/* pettern-4 */
		kk = 7;
		for (jj=0; jj<16; jj++){
			numarray[jj][kk] = 255;
		}
	}

	if(number == 0 || number == 2 || number == 6 || number == 8){  
		/* pettern-5 */
		jj = 0;
		for (kk=8; kk<15; kk++){
			numarray[jj][kk] = 255;
		}
	}

	if(number == 0 || number == 1 || number == 3 || number == 4 || number == 5 || number == 6 || number == 7 || number == 8 || number == 9){  
		/* pettern-6 */
		jj = 15;
		for (kk=8; kk<15; kk++){
			numarray[jj][kk] = 255;
		}
	}

	if(number == 0 || number == 2 || number == 3 || number == 5 || number == 6 || number == 8 || number == 9){  
		/* pettern-7 */
		kk = 15;
		for (jj=0; jj<16; jj++){
			numarray[jj][kk] = 255;
		}
	}

}
