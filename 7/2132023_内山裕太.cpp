#define _CRT_SECURE_NO_WARNINGS  //旧関数のwarning出力無視

#include "header.h"

//----------------------------------------------------------------
//  二値画像の膨張処理を行う（８近傍膨張），入力ビットマップファイルは24ビットのみ対応
//
//   - 本プログラムは8近傍膨張に対応
//   - 入力の二値画像は，白画素=255，黒画素=0，と設定されていること
//
//  プログラムの動かしかた
//     binary_image_processing.exe input_filename output_filename student_number
//
//----------------------------------------------------------------

int main(int argc, char* argv[])
{
     FILE *infile,*outfile;
	 char infname1[512];
     char outfname1[512];
//	 int palette[1024];			//パレット用配列
	 int head[54];				//ヘッダデータ配列

	 int size,height,width;
	 int k,j,number;

	 IMG_YUV *img, *img_out;        //8bit-YUVデータのポインタ
	 IMG_RGB *img_rgb, *img_rgb2;   //8bit-RGBデータのポインタ
	 IMG_YUV *img_work;

     /* パラメータチェック */
     if (argc < 4) {    // (読み込みパラメータ数+1)より小さくてはいけない
		 fprintf(stderr, "Usage: %s infname1 outfname1 number \n", argv[0]);
	     printf("        infname1: input file name \n");
	     printf("        outfname1: output file name \n");
		 printf("        number: student number \n");
		 exit(-1);
     }

	 /* パラメータ設定 */
	 strcpy(infname1, argv[1]);      /* 入力画像ファイル名 */
     strcpy(outfname1, argv[2]);     /* 出力画像ファイル名 */
	 number = atoi(argv[3]);      /* 学生番号(7桁） */

	 printf(" -Input parameters --------\n");
	 printf("   -- Input file= %s \n", infname1);
     printf("   -- Output file= %s \n", outfname1);
	 printf("   -- Student number= %d \n", number);

	 /* 画像ファイルのオープン */
     if( !strcmp(infname1,"-") ){
		 infile = stdin;
     }
     else if ((infile = fopen(infname1, "rb")) == NULL) {
		 fprintf(stderr, "File open error = %s\n", infname1);
		 exit(-1);
     }

	 sprintf(outfname1, "%s", argv[2]);
     if ((outfile = fopen(outfname1, "wb")) == NULL) {
		 fprintf(stderr, "OutPut File open error = %s\n", outfname1);
		 exit(-1);
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

	 img_work = alloc_IMG_YUV(width, height);

	 //ビットマップファイルの画像データ部読み込み
	 read_bmp(img, img_rgb, infile);

	    // imgにはYUV形式(444フォーマット）の画像データが入る //
	    // img_rgbにはRGB形式（444フォーマット）の画像データが入る //
	 
	 printf(" Image processing start =====> \n");
	 setvalue_image(img_out, 128, 128, 128);
	 setvalue_image(img_work, 128, 128, 128);

//--------------------------処理はここへ---------------------------------

//   ※ 処理の中で，もし作業用の画像配列が必要なら，img_work->U[j+k*width] のように
//     「img_work」を使うことができます（宣言およびメモリ確保済み）

     /* 8近傍膨張(Dilation)処理 */
	 int value;
	 for(k=1; k<height-1; k++){          // 画面の端は処理しないようにしている
		 for(j=1; j<width-1; j++){
			 // 周囲8画素の合計計算
			 value = img->Y[j-1+(k-1)*width] + img->Y[j+(k-1)*width] + img->Y[j+1+(k-1)*width]
			      + img->Y[j-1 + k  *width]                         + img->Y[j+1 + k*width]
			      + img->Y[j-1+(k+1)*width] + img->Y[j+(k+1)*width] + img->Y[j+1+(k+1)*width];	 

			 if(value>0){    // 周囲に一つでも白画素があったら，原画素を白にする
				 img_work->U[j + k*width] = 255;                           
			 }
			 else{   // そうでなければ原画像のまま
				 img_work->U[j + k*width] = img->Y[ j + k*width];   
			 }
		 }
	 }

	 for(k=1; k<height-1; k++){          // 画面の端は処理しないようにしている
		 for (j=1; j<width-1; j++) {
			 // 周囲8画素の合計計算
			 value = img_work->U[j-1 + (k-1)*width] + img_work->U[j + (k-1)*width] + img_work->U[j+1 + (k-1)*width]
				 + img_work->U[j-1 + k*width]								+ img_work->U[j+1 + k*width]
				 + img_work->U[j-1 + (k+1)*width] + img_work->U[j + (k+1)*width] + img_work->U[j+1 + (k+1)*width];

			 if(value<2040){    // 周囲に一つでも黒画素があったら，原画素を黒にする
				 img->Y[j + k*width] = 0;
			 }
			 else{   // そうでなければ原画像のまま
				 img->Y[j + k*width] = img_work->U[j + k*width];
			 }
		 }
	 }

	 for(k=1; k<height-1; k++) {          // 画面の端は処理しないようにしている
		 for(j=1; j<width-1; j++) {
			 // 周囲8画素の合計計算
			 value = img->Y[j-1 + (k-1)*width] + img->Y[j + (k-1)*width] + img->Y[j+1 + (k-1)*width]
				 + img->Y[j-1 + k*width] + img->Y[j+1 + k*width]
				 + img->Y[j-1 + (k+1)*width] + img->Y[j + (k+1)*width] + img->Y[j+1 + (k+1)*width];

			 if(value<2040) {    // 周囲に一つでも黒画素があったら，原画素を黒にする
				 img_work->U[j + k*width] = 0;
			 }
			 else{   // そうでなければ原画像のまま
				 img_work->U[j + k*width] = img->Y[j + k*width];
			 }
		 }
	 }

	 for(k=1; k<height-1; k++) {          // 画面の端は処理しないようにしている
		 for(j=1; j<width-1; j++) {
			 // 周囲8画素の合計計算
			 value = img_work->U[j-1 + (k-1)*width] + img_work->U[j + (k-1)*width] + img_work->U[j+1 + (k-1)*width]
				 + img_work->U[j-1 + k*width] + img_work->U[j+1 + k*width]
				 + img_work->U[j-1 + (k+1)*width] + img_work->U[j + (k+1)*width] + img_work->U[j+1 + (k+1)*width];

			 if (value>0) {    // 周囲に一つでも白画素があったら，原画素を白にする
				 img_out->Y[j + k*width] = 255;
			 }
			 else {   // そうでなければ原画像のまま
				 img_out->Y[j + k*width] = img_work->U[j + k*width];
			 }
		 }
	 }

//---ここまでの処理で，ファイル出力したい画像をimg_outに入れておくこと---
//--------------------------処理はここまで-------------------------------
	 
	 number = number - (number / 1000) * 1000;
	 img_out->Y[0] = rounding_integer(number);
	 printf("  === File output: number = %d \n", number);

 	 //ビットマップ画像の書き込み
	    // img_outに書き込むべきYUV形式の画像データが入っている //
	    // img_rgb2は処理のための作業用メモリ //
	 write_bmp(img_out, img_rgb2, outfile);
	 printf(" <==== Image processing end \n");
	 
	 //ファイルのクローズとメモリ解放
	 fclose(infile);
	 fclose(outfile);
	 free_IMG_YUV(img);
	 free_IMG_YUV(img_out);
	 free_IMG_RGB(img_rgb);
	 free_IMG_RGB(img_rgb2);
	 free_IMG_YUV(img_work);
	 
	 return 0;
}