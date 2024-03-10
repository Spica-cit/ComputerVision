#define _CRT_SECURE_NO_WARNINGS  //旧関数のwarning出力無視

#include "header.h"

//------------------------------------------------------------------------------
//
// 画像の離散コサイン変換サンプルプログラム
//
//
//------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
     FILE *infile,*outfile;
	 char infname1[512];
     char outfname1[512];
//	 int palette[1024];			//パレット用配列
	 int head[54];				//ヘッダデータ配列

	 int size;
	 int height,width;
	 int k,j;

	 IMG_YUV *img, *img_out;        //8bit-YUVデータのポインタ
	 IMG_RGB *img_rgb, *img_rgb2;   //8bit-RGBデータのポインタ

	 int QP, qscaleDC, qscaleAC;
	 double snr_Y, snr_U, snr_V;

	 uchar array_line[176][16];
	 int student_number;
	 uchar qp_line[104][16];

     /* パラメータチェック */
     if (argc < 5) {    // (読み込みパラメータ数+1)より小さくてはいけない
		 fprintf(stderr, "Usage: %s infname1 outfname1 QP student_number \n", argv[0]);
	     printf("        infname1: input file name \n");
	     printf("        outfname1: output file name \n");
		 printf("        QP:  quantization parameter( 1 <= QP <= 1024 ) \n");
		 printf("        student number:  student number (7-digit) \n");
	     exit(-1);
     }

	 /* パラメータ設定 */
	 strcpy(infname1, argv[1]);      /* 入力画像ファイル名 */
     strcpy(outfname1, argv[2]);     /* 出力画像ファイル名 */
	 QP = atoi(argv[3]);         /* DCT係数量子化パラメータ */
	 student_number = atoi(argv[4]);         /* 学生番号(７ケタ） */

	 qscaleDC = 8;         /* DCT係数量子化パラメータ設定(直流係数） */
	 qscaleAC = QP;         /* DCT係数量子化パラメータ設定(交流係数） */

	 printf(" -Input parameters --------\n");
	 printf("   -- Input file= %s \n", infname1);
     printf("   -- Output file= %s \n", outfname1);
     printf("   -- QP= %d \n", QP);
//	 printf("   -- qscaleDC= %d \n", qscaleDC);
//	 printf("   -- qscaleAC= %d \n", qscaleAC);

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

    //  パレットデータを読み込む（このデータは本プログラムでは使わない）
//	for(int c=0; c<1024; c++){
//			palette[c] = getc(infile);
//  }

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

	printf("  DCT - Quantization - IDCT processing  \n");

	 int jj, kk;
	 int block_dataY[8][8], block_dataY_out[8][8];
	 int block_dataU[8][8], block_dataU_out[8][8];
	 int block_dataV[8][8], block_dataV_out[8][8];
	 double block_DCT[8][8], block_DCTQ[8][8];

	 copy_image(img, img_out, 1);

	 for(k=0;k<height-7; k=k+8){           /* 画像が8の倍数でない場合には，画像右側および画像下側の端数は処理されない（原画像のまま） */
		 for(j=0; j<width-7; j=j+8){

			 /* 画像配列から8×8ブロックの画素データを抽出 */
			 for (kk=0; kk<8; kk++){
				 for(jj=0; jj<8; jj++){
					 block_dataY[jj][kk]=img->Y[ j+jj + (k+kk)*width ];
					 block_dataU[jj][kk]=img->U[ j+jj + (k+kk)*width ];
					 block_dataV[jj][kk]=img->V[ j+jj + (k+kk)*width ];
				 }
			 }

//          ここから8×8ブロックごとにDCT演算と量子化と逆DCTを行う（それぞれの処理は関数となっている）
			 
			 /* 輝度(Y)信号の処理 */
			 dct88(block_dataY, block_DCT);                                                  /* DCT */
			 quantization88_new(block_DCT, block_DCTQ, qscaleDC, qscaleAC, 0);               /* DCT係数の量子化 */
	         idct88(block_DCTQ, block_dataY_out);                                            /* 逆DCT */

			 /* 色差(U)信号の処理 */
	         dct88(block_dataU, block_DCT);                                                  /* DCT */
			 quantization88_new(block_DCT, block_DCTQ, qscaleDC, qscaleAC, 0);               /* DCT係数の量子化 */
	         idct88(block_DCTQ, block_dataU_out);                                            /* 逆DCT */

			 /* 色差(V)信号の処理 */
	         dct88(block_dataV, block_DCT);                                                  /* DCT */
			 quantization88_new(block_DCT, block_DCTQ, qscaleDC, qscaleAC, 0);               /* DCT係数の量子化 */
	         idct88(block_DCTQ, block_dataV_out);                                            /* 逆DCT */

			 /* 復号された8×8ブロックの画素値を画像配列に代入 */
			 for (kk=0; kk<8; kk++){
			 	 for(jj=0; jj<8; jj++){
					 img_out->Y[ j+jj + (k+kk)*width ] = rounding_integer(block_dataY_out[jj][kk]);
					 img_out->U[ j+jj + (k+kk)*width ] = rounding_integer(block_dataU_out[jj][kk]);
					 img_out->V[ j+jj + (k+kk)*width ] = rounding_integer(block_dataV_out[jj][kk]);
				 }
			 }

		}
	  }

//---ここまでの処理で，ファイル出力したい画像をimg_outに入れておくこと---
//--------------------------処理はここまで------------------------------------------------

	printf(" <==== Image processing end \n");
 
	/* SN計算 */
	snr_image(img, img_out, &snr_Y, &snr_U, &snr_V);
	printf(" ====  Picture quality =====\n");
	printf("  SNR_Y(dB)   = %f \n", snr_Y);
	printf("  SNR_U(dB)   = %f \n", snr_U);
	printf("  SNR_V(dB)   = %f \n", snr_V);

	create_number_line(array_line, student_number);
	create_qp_line(qp_line, QP);

	for(k=0;k<16;k++){
		for(j=0; j<176; j++){
			img_out->Y[ j + (k+4)*width ] = array_line[j][k];
			img_out->U[ j + (k+4)*width ] = 128;
			img_out->V[ j + (k+4)*width ] = 128;
		}
	}
	for(k=0;k<16;k++){
		for(j=0; j<104; j++){
			img_out->Y[ j+184 + (k+4)*width ] = qp_line[j][k];
			img_out->U[ j+184 + (k+4)*width ] = 128;
			img_out->V[ j+184 + (k+4)*width ] = 128;
		}
	}

	//ビットマップ画像の書き込み
	    // img_outに書き込むべきYUV形式の画像データが入っている //
	    // img_rgb2は処理のための作業用メモリ //
	write_bmp(img_out, img_rgb2, outfile);

	//ファイルのクローズとメモリ解放
	fclose(infile);
	fclose(outfile);
    free_IMG_YUV(img);
    free_IMG_YUV(img_out);
    free_IMG_RGB(img_rgb);
    free_IMG_RGB(img_rgb2);

	return 0;
}