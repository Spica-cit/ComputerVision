#define _CRT_SECURE_NO_WARNINGS  //旧関数のwarning出力無視

#include "header.h"

int main(int argc, char* argv[])
{
	/* 変数の宣言 */
     FILE *infile,*outfile;
	 FILE *fp;

	 char infname[512];
     char outfname[512], csvfname[512];
//	 int palette[1024];			//パレット用配列
	 int head[54];				//ヘッダデータ配列

	 int size;
	 int height,width;
	 int k,j;
	 int value;

	 IMG_YUV *img, *img_out;        //8bit-YUV画像データのポインタ
	 IMG_RGB *img_rgb, *img_rgb2;   //8bit-RGB画像データのポインタ

     /* パラメータチェック */
     if (argc < 4) {    // (読み込みパラメータ数+1)より小さくてはいけない
		 fprintf(stderr, "Usage: %s infname1 outfname1 csvfname \n", argv[0]);
	     printf("        infname1: input file name \n");
	     printf("        outfname1: output file name \n");
		 printf("        csvfname: output csv file name \n");
		 exit(-1);
     }

	 /* パラメータ設定 */
	 strcpy(infname, argv[1]);      /* 入力画像ファイル名 */
     strcpy(outfname, argv[2]);     /* 出力画像ファイル名 */
	 strcpy(csvfname, argv[3]);    /* ヒストグラム出力csvファイル名 */

	 printf(" -Input parameters --------\n");
	 printf("   -- Input image file= %s \n", infname);
     printf("   -- Output image file= %s \n", outfname);
	 printf("   -- Output csv file= %s \n", csvfname);

	 /* 画像ファイルのオープン */
     if( !strcmp(infname,"-") ){
		 infile = stdin;
     }
     else if ((infile = fopen(infname, "rb")) == NULL) {
		 fprintf(stderr, "File open error = %s\n", infname);
		 exit(-1);
     }

	 sprintf(outfname, "%s", argv[2]);
     if ((outfile = fopen(outfname, "wb")) == NULL) {
		 fprintf(stderr, "OutPut File open error = %s\n", outfname);
		 exit(-1);
     }

	 fp = fopen(csvfname, "w");
	 if (fp == NULL) {
		 printf("CSV File open error = %s\n", csvfname);
		 return -1;
	 }

	//ビットマップファイルのヘッダ部を読み込み
	for(int c=0; c<54; c++){
		head[c] = fgetc(infile);
	}

	//ヘッダからの幅と高さを算出
	width  = head[18] | head[19]<<8 | head[20]<<16 | head[21]<<24;
	height = head[22] | head[23]<<8 | head[24]<<16 | head[25]<<24;
	size = width*height;
	printf(" Image size : width=%d height=%d\n",width,height);

	// 画像メモリ確保
	 img = alloc_IMG_YUV(width, height);
	 img_out = alloc_IMG_YUV(width, height);
	 img_rgb = alloc_IMG_RGB(width, height);
	 img_rgb2 = alloc_IMG_RGB(width, height);

	//ビットマップファイルの画像データ部読み込み
	 read_bmp(img, img_rgb, infile);

	    // imgにはYUV形式(444フォーマット）の画像データが入る //
	    // img_rgbにはRGB形式（444フォーマット）の画像データが入る //

	printf(" Image processing start =====> \n");

//--------------------------処理はここへ--------------------------------------------------
	int hist[256];
	int i,n;
	for (i = 0; i < 256; i++) {
		hist[i] = 0;
	}

	for(k=0; k<height; k++){
		for(j=0; j<width; j++){
			value = img->Y[j + k*width];	  // 輝度値
			hist[value]++;
		}
	}
	n = width*height;
	for (i = 0; i < 256; i++) {
		fprintf(fp, "%d,%f \n", i, (double)hist[i]/n);       // ヒストグラムのファイル出力
	}

	copy_image(img, img_out, 0); // imgをimg_outにコピー．ただし，色差は128に置き換える（モノクロ化）

//---ここまでの処理で，ファイル出力したい画像をimg_outに入れておくこと---
//--------------------------処理はここまで------------------------------------------------

	printf(" <==== Image processing end \n");
 
	//ビットマップ画像の書き込み
	    // img_outに書き込むべきYUV形式の画像データが入っている
	    // img_rgb2は処理のための作業用メモリ
	write_bmp(img_out, img_rgb2, outfile);

	//ファイルのクローズとメモリ解放
	fclose(infile);
	fclose(outfile);
	fclose(fp);
    free_IMG_YUV(img);
    free_IMG_YUV(img_out);
    free_IMG_RGB(img_rgb);
    free_IMG_RGB(img_rgb2);

	return 0;
}