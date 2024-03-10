#define _CRT_SECURE_NO_WARNINGS  //旧関数のwarning出力無視

#include "header.h"

int main(int argc, char* argv[])
{
     FILE *infile1,*infile2,*outfile;
	 FILE *fp;

	 char infname1[512], infname2[512];
	 char outfname[512], csvfname[512];
//	 int palette[1024];			//パレット用配列
	 int head[54];				//ヘッダデータ配列

	 int size;
	 int height,width;
	 int j, k;
	 int number;
	 uchar array_line[176][16];

	 IMG_YUV *img1, *img2, *img_out;        //8bit-YUVデータのポインタ
	 IMG_RGB *img_rgb, *img_rgb2;   //8bit-RGBデータのポインタ

     /* パラメータチェック */
     if (argc < 5) {    // (読み込みパラメータ数+1)より小さくてはいけない
		 fprintf(stderr, "Usage: %s infname1 infname2 outfname csvfname student_number \n", argv[0]);
	     printf("        infname1: input file name 1 \n");
	     printf("        infname2: input file name 2 \n");
		 printf("        outfname: output image file name \n");
		 printf("        csvfname: output csv file name \n");
		 printf("        student_number: student number \n");
		 exit(-1);
     }

	 /* パラメータ設定 */
	 strcpy(infname1, argv[1]);    /* 入力画像ファイル名（現フレーム） */
	 strcpy(infname2, argv[2]);    /* 入力画像ファイル名（過去フレーム） */
     strcpy(outfname, argv[3]);    /* 出力画像ファイル名 */
	 strcpy(csvfname, argv[4]);    /* ヒストグラム出力csvファイル名 */
	 number = atoi(argv[5]);      /* 学生番号(7桁） */

	 printf(" -Input parameters --------\n");
	 printf("   -- Input image file1= %s \n", infname1);
	 printf("   -- Input image file2= %s \n", infname2);
	 printf("   -- Output image file= %s \n", outfname);
	 printf("   -- Output csv file = %s \n", csvfname);
	 printf("   -- Student number= %d \n", number);

	 /* 画像ファイルのオープン */
     if( !strcmp(infname1,"-") ){
		 infile1 = stdin;
     }
     else if ((infile1 = fopen(infname1, "rb")) == NULL) {
		 fprintf(stderr, "File open error = %s\n", infname1);
		 exit(-1);
     }
     if( !strcmp(infname2,"-") ){
		 infile2 = stdin;
     }
     else if ((infile2 = fopen(infname2, "rb")) == NULL) {
		 fprintf(stderr, "File open error = %s\n", infname2);
		 exit(-1);
     }

	 sprintf(outfname, "%s", argv[3]);
     if ((outfile = fopen(outfname, "wb")) == NULL) {
		 fprintf(stderr, "OutPut File open error = %s\n", outfname);
		 exit(-1);
     }
	 
	 fp = fopen( csvfname, "w" );
	 if( fp == NULL ){
		 printf("CSV File open error = %s\n", csvfname);
		 return -1;
	 }
	 
	 //ビットマップファイルのヘッダ部を読み込み
	 for(int c=0; c<54; c++){
		 head[c] = fgetc(infile1);
		 head[c] = fgetc(infile2);
	 }
	 
	 //ヘッダからの幅と高さを算出
	 width  = head[18] | head[19]<<8 | head[20]<<16 | head[21]<<24;
	 height = head[22] | head[23]<<8 | head[24]<<16 | head[25]<<24;
	 size = width*height;
	 printf(" Image size : width=%d height=%d\n",width,height);
	 
	 // 画像メモリ確保
	 img1 = alloc_IMG_YUV(width, height);
	 img2 = alloc_IMG_YUV(width, height);
	 img_out = alloc_IMG_YUV(width, height);
	 img_rgb = alloc_IMG_RGB(width, height);
	 img_rgb2 = alloc_IMG_RGB(width, height);
	 
	 //ビットマップファイルの画像データ部読み込み
	 read_bmp(img1, img_rgb, infile1);
	 read_bmp(img2, img_rgb, infile2);

	    // img1,img2にはYUV形式(444フォーマット）の画像データが入る //
	    // img_rgbにはRGB形式（444フォーマット）の画像データが入る //
	 
	 printf(" Image processing start =====> \n");
	 setvalue_image(img_out, 128, 128, 128);

//--------------------------処理はここへ--------------------------------------------------
//   img1には現在のフレームの画像データが入っている
//   img2には過去フレーム（1フレーム前）の画像データが入っている
	 
	 int value;
	 int hist[512];
	 
	 int i;
	 for(i=0; i<512; i++){
		 hist[i] = 0;
	 }
	 
	 for(k=0; k<height; k++){
		 for(j=0; j<width; j++){
			 
			 value = img1->Y[j + k*width] - img2->Y[j + k*width];  // フレーム間差分計算
			 hist[value + 256] = hist[value + 256] + 1;      // ヒストグラム計算
			 
			 // フレーム間差分の可視化画像作成（わかりやすくするため差分を2倍）
			 img_out->Y[j + k*width] = rounding_integer(abs(value)*2);
		 
		 }
	 }
	 
	 for(i=0; i<512; i++){
		 fprintf(fp, "%d,%d \n", i-256, hist[i]);       // ヒストグラム出力
	 }

//---ここまでの処理で，ファイル出力したい画像をimg_outに入れておくこと---
//--------------------------処理はここまで------------------------------------------------
	 
	 printf(" <==== Image processing end \n");
	 
	 create_number_line(array_line, number);

	 for (k = 0; k<16; k++){
		 for (j = 0; j<176; j++){
			 img_out->Y[j + (k + 4)*width] = array_line[j][k];
			 img_out->U[j + (k + 4)*width] = 128;
			 img_out->V[j + (k + 4)*width] = 128;
		 }
	 }

	 //ビットマップ画像の書き込み
	    // img_outに書き込むべきYUV形式の画像データが入っている //
	    // img_rgb2は処理のための作業用メモリ //
	 
	 write_bmp(img_out, img_rgb2, outfile);
	 
	 //ファイルのクローズとメモリ解放
	 fclose(infile1);
	 fclose(infile2);
	 fclose(outfile);
	 fclose(fp);
	 free_IMG_YUV(img1);
	 free_IMG_YUV(img2);
	 free_IMG_YUV(img_out);
	 free_IMG_RGB(img_rgb);
	 free_IMG_RGB(img_rgb2);
	 
	 return 0;
}