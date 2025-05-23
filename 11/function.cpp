#define _CRT_SECURE_NO_WARNINGS  //旧関数のwarning出力無視

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "header.h"

typedef	unsigned char	uchar;


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
//	Function-ID	cut_and_paste
//	Function-Name	
//	Abstract	IMG_YUV型構造体データ（444タイプ）の部分領域（矩形）を別の画像の定められた領域に貼り付ける（構造体ポインタを引き渡し）
//
//	Argument	IMG_YUV *img1: 入力画像データ（貼り付け先画像） 画像サイズ：width*height
//	         	IMG_YUV *img2: 入力画像データ（貼り付け元画像） 画像サイズ：width2*height2
//	         	IMG_YUV *img3: 出力画像データ（貼り付け後の画像）（img2から切り出してimg1に貼り付けたもの）画像サイズ：width*height
//              int cut_start_address_H : 切り出す領域の水平方向始まり座標(if 420, EVEN value)
//              int cut_start_address_V : 切り出す領域の垂直方向始まり座標(if 420, EVEN value)
//              int cut_region_size_H : 切り出す領域の水平方向幅（画素数）(if 420, EVEN value)
//              int cut_region_size_V : 切り出す領域の垂直方向幅（画素数）(if 420, EVEN value)
//              int paste_start_address_H : 貼り付け先領域の水平方向始まり座標(if 420, EVEN value)
//              int paste_start_address_V : 貼り付け先領域の垂直方向始まり座標(if 420, EVEN value)
//			
//	Return-Value	
//	Special_Desc	
//----------------------------------------------------------------
void  cut_and_paste(IMG_YUV *img1, IMG_YUV *img2, IMG_YUV *img3,
	int cut_start_address_H, int cut_start_address_V, int cut_region_size_H, int cut_region_size_V,
	int paste_start_address_H, int paste_start_address_V)
{

	int j, k;
	int jc, kc, jsize, ksize, jp, kp, jcp, kcp;
	int width, height;
	int widthc, heightc;
	int width2, height2;
	int widthc2, heightc2;

	width = img1->width;
	height = img1->height;
	widthc = width;
	heightc = height;

	width2 = img2->width;
	height2 = img2->height;
	widthc2 = width2;
	heightc2 = height2;

	jc = cut_start_address_H;
	jsize = cut_region_size_H;
	kc = cut_start_address_V;
	ksize = cut_region_size_V;
	jp = paste_start_address_H;
	kp = paste_start_address_V;

	for (k = 0; k<height; k++){
		for (j = 0; j<width; j++){
			img3->Y[j + k*width] = img1->Y[j + k*width];
		}
	}
	for (k = 0; k<heightc; k++){
		for (j = 0; j<widthc; j++){
			img3->U[j + k*widthc] = img1->U[j + k*widthc];
			img3->V[j + k*widthc] = img1->V[j + k*widthc];
		}
	}

	kcp = kc - 1;
	for (k = kp; k<kp + ksize; k++){
		kcp = kcp + 1;
		jcp = jc - 1;
		for (j = jp; j<jp + jsize; j++){
			jcp = jcp + 1;
			img3->Y[j + k*width] = img2->Y[jcp + kcp*width2];
			img3->U[j + k*widthc] = img2->U[jcp + kcp*widthc2];
			img3->V[j + k*widthc] = img2->V[jcp + kcp*widthc2];
		}
	}

}

void  draw_rectangle(IMG_YUV *img, int js, int ks, int jsize, int ksize)
{

	int j, k;
	int width, height;
	unsigned char Ydata = 128, Udata = 255, Vdata = 0;  /* blue */

	width = img->width;
	height = img->height;

	for (j = js; j < js+jsize; j++){
		img->Y[j + ks*width] = Ydata;
		img->U[j + ks*width] = Udata;
		img->V[j + ks*width] = Vdata;
		img->Y[j + (ks + ksize - 1)*width] = Ydata;
		img->U[j + (ks + ksize - 1)*width] = Udata;
		img->V[j + (ks + ksize - 1)*width] = Vdata;
	}

	for (k = ks; k < ks + ksize; k++){
		img->Y[js + k*width] = Ydata;
		img->U[js + k*width] = Udata;
		img->V[js + k*width] = Vdata;
		img->Y[js + jsize - 1 + k*width] = Ydata;
		img->U[js + jsize - 1 + k*width] = Udata;
		img->V[js + jsize - 1 + k*width] = Vdata;
	}

}

void create_number_line(uchar(*array_line)[16], int student_number)
{
	int number, number1;
	int jj, kk, jst;
	uchar numarray[16][16];

	for (kk = 0; kk<16; kk++){
		for (jj = 0; jj<176; jj++){
			array_line[jj][kk] = 0;
		}
	}

	jst = 8;
	number = student_number / 1000000;
	create_number(numarray, number);
	for (kk = 0; kk<16; kk++){
		for (jj = 0; jj<16; jj++){
			array_line[jst + jj][kk] = numarray[jj][kk];
		}
	}

	jst = jst + 24;
	number1 = student_number - number * 1000000;
	number = number1 / 100000;
	create_number(numarray, number);
	for (kk = 0; kk<16; kk++){
		for (jj = 0; jj<16; jj++){
			array_line[jst + jj][kk] = numarray[jj][kk];
		}
	}

	jst = jst + 24;
	number1 = number1 - number * 100000;
	number = number1 / 10000;
	create_number(numarray, number);
	for (kk = 0; kk<16; kk++){
		for (jj = 0; jj<16; jj++){
			array_line[jst + jj][kk] = numarray[jj][kk];
		}
	}

	jst = jst + 24;
	number1 = number1 - number * 10000;
	number = number1 / 1000;
	create_number(numarray, number);
	for (kk = 0; kk<16; kk++){
		for (jj = 0; jj<16; jj++){
			array_line[jst + jj][kk] = numarray[jj][kk];
		}
	}

	jst = jst + 24;
	number1 = number1 - number * 1000;
	number = number1 / 100;
	create_number(numarray, number);
	for (kk = 0; kk<16; kk++){
		for (jj = 0; jj<16; jj++){
			array_line[jst + jj][kk] = numarray[jj][kk];
		}
	}

	jst = jst + 24;
	number1 = number1 - number * 100;
	number = number1 / 10;
	create_number(numarray, number);
	for (kk = 0; kk<16; kk++){
		for (jj = 0; jj<16; jj++){
			array_line[jst + jj][kk] = numarray[jj][kk];
		}
	}

	jst = jst + 24;
	number1 = number1 - number * 10;
	number = number1;
	create_number(numarray, number);
	for (kk = 0; kk<16; kk++){
		for (jj = 0; jj<16; jj++){
			array_line[jst + jj][kk] = numarray[jj][kk];
		}
	}

}

void create_number(uchar(*numarray)[16], int number)
{
	int jj, kk;

	for (kk = 0; kk<16; kk++){
		for (jj = 0; jj<16; jj++){
			numarray[jj][kk] = 0;
		}
	}

	if (number == 0 || number == 2 || number == 3 || number == 5 || number == 6 || number == 7 || number == 8 || number == 9){
		/* pettern-1 */
		kk = 0;
		for (jj = 0; jj<16; jj++){
			numarray[jj][kk] = 255;
		}
	}

	if (number == 0 || number == 4 || number == 5 || number == 6 || number == 8 || number == 9){
		/* pettern-2 */
		jj = 0;
		for (kk = 0; kk<8; kk++){
			numarray[jj][kk] = 255;
		}
	}

	if (number == 0 || number == 1 || number == 2 || number == 3 || number == 4 || number == 7 || number == 8 || number == 9){
		/* pettern-3 */
		jj = 15;
		for (kk = 0; kk<8; kk++){
			numarray[jj][kk] = 255;
		}
	}

	if (number == 2 || number == 3 || number == 4 || number == 5 || number == 6 || number == 8 || number == 9){
		/* pettern-4 */
		kk = 7;
		for (jj = 0; jj<16; jj++){
			numarray[jj][kk] = 255;
		}
	}

	if (number == 0 || number == 2 || number == 6 || number == 8){
		/* pettern-5 */
		jj = 0;
		for (kk = 8; kk<15; kk++){
			numarray[jj][kk] = 255;
		}
	}

	if (number == 0 || number == 1 || number == 3 || number == 4 || number == 5 || number == 6 || number == 7 || number == 8 || number == 9){
		/* pettern-6 */
		jj = 15;
		for (kk = 8; kk<15; kk++){
			numarray[jj][kk] = 255;
		}
	}

	if (number == 0 || number == 2 || number == 3 || number == 5 || number == 6 || number == 8 || number == 9){
		/* pettern-7 */
		kk = 15;
		for (jj = 0; jj<16; jj++){
			numarray[jj][kk] = 255;
		}
	}

}

