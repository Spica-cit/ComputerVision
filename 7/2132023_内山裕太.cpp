#define _CRT_SECURE_NO_WARNINGS  //���֐���warning�o�͖���

#include "header.h"

//----------------------------------------------------------------
//  ��l�摜�̖c���������s���i�W�ߖT�c���j�C���̓r�b�g�}�b�v�t�@�C����24�r�b�g�̂ݑΉ�
//
//   - �{�v���O������8�ߖT�c���ɑΉ�
//   - ���͂̓�l�摜�́C����f=255�C����f=0�C�Ɛݒ肳��Ă��邱��
//
//  �v���O�����̓���������
//     binary_image_processing.exe input_filename output_filename student_number
//
//----------------------------------------------------------------

int main(int argc, char* argv[])
{
     FILE *infile,*outfile;
	 char infname1[512];
     char outfname1[512];
//	 int palette[1024];			//�p���b�g�p�z��
	 int head[54];				//�w�b�_�f�[�^�z��

	 int size,height,width;
	 int k,j,number;

	 IMG_YUV *img, *img_out;        //8bit-YUV�f�[�^�̃|�C���^
	 IMG_RGB *img_rgb, *img_rgb2;   //8bit-RGB�f�[�^�̃|�C���^
	 IMG_YUV *img_work;

     /* �p�����[�^�`�F�b�N */
     if (argc < 4) {    // (�ǂݍ��݃p�����[�^��+1)��菬�����Ă͂����Ȃ�
		 fprintf(stderr, "Usage: %s infname1 outfname1 number \n", argv[0]);
	     printf("        infname1: input file name \n");
	     printf("        outfname1: output file name \n");
		 printf("        number: student number \n");
		 exit(-1);
     }

	 /* �p�����[�^�ݒ� */
	 strcpy(infname1, argv[1]);      /* ���͉摜�t�@�C���� */
     strcpy(outfname1, argv[2]);     /* �o�͉摜�t�@�C���� */
	 number = atoi(argv[3]);      /* �w���ԍ�(7���j */

	 printf(" -Input parameters --------\n");
	 printf("   -- Input file= %s \n", infname1);
     printf("   -- Output file= %s \n", outfname1);
	 printf("   -- Student number= %d \n", number);

	 /* �摜�t�@�C���̃I�[�v�� */
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

	 //�r�b�g�}�b�v�t�@�C���̃w�b�_����ǂݍ���
	 for(int c=0; c<54; c++){
		 head[c] = fgetc(infile);
	 }
	 
	 //�w�b�_����̕��ƍ������Z�o
	 width  = head[18] | head[19]<<8 | head[20]<<16 | head[21]<<24;
	 height = head[22] | head[23]<<8 | head[24]<<16 | head[25]<<24;
	 size = width*height;
	 printf(" Image size : width=%d height=%d\n",width,height);

	 // �摜�������m��
	 img = alloc_IMG_YUV(width, height);
	 img_out = alloc_IMG_YUV(width, height);
	 img_rgb = alloc_IMG_RGB(width, height);
	 img_rgb2 = alloc_IMG_RGB(width, height);

	 img_work = alloc_IMG_YUV(width, height);

	 //�r�b�g�}�b�v�t�@�C���̉摜�f�[�^���ǂݍ���
	 read_bmp(img, img_rgb, infile);

	    // img�ɂ�YUV�`��(444�t�H�[�}�b�g�j�̉摜�f�[�^������ //
	    // img_rgb�ɂ�RGB�`���i444�t�H�[�}�b�g�j�̉摜�f�[�^������ //
	 
	 printf(" Image processing start =====> \n");
	 setvalue_image(img_out, 128, 128, 128);
	 setvalue_image(img_work, 128, 128, 128);

//--------------------------�����͂�����---------------------------------

//   �� �����̒��ŁC������Ɨp�̉摜�z�񂪕K�v�Ȃ�Cimg_work->U[j+k*width] �̂悤��
//     �uimg_work�v���g�����Ƃ��ł��܂��i�錾����у������m�ۍς݁j

     /* 8�ߖT�c��(Dilation)���� */
	 int value;
	 for(k=1; k<height-1; k++){          // ��ʂ̒[�͏������Ȃ��悤�ɂ��Ă���
		 for(j=1; j<width-1; j++){
			 // ����8��f�̍��v�v�Z
			 value = img->Y[j-1+(k-1)*width] + img->Y[j+(k-1)*width] + img->Y[j+1+(k-1)*width]
			      + img->Y[j-1 + k  *width]                         + img->Y[j+1 + k*width]
			      + img->Y[j-1+(k+1)*width] + img->Y[j+(k+1)*width] + img->Y[j+1+(k+1)*width];	 

			 if(value>0){    // ���͂Ɉ�ł�����f����������C����f�𔒂ɂ���
				 img_work->U[j + k*width] = 255;                           
			 }
			 else{   // �����łȂ���Ό��摜�̂܂�
				 img_work->U[j + k*width] = img->Y[ j + k*width];   
			 }
		 }
	 }

	 for(k=1; k<height-1; k++){          // ��ʂ̒[�͏������Ȃ��悤�ɂ��Ă���
		 for (j=1; j<width-1; j++) {
			 // ����8��f�̍��v�v�Z
			 value = img_work->U[j-1 + (k-1)*width] + img_work->U[j + (k-1)*width] + img_work->U[j+1 + (k-1)*width]
				 + img_work->U[j-1 + k*width]								+ img_work->U[j+1 + k*width]
				 + img_work->U[j-1 + (k+1)*width] + img_work->U[j + (k+1)*width] + img_work->U[j+1 + (k+1)*width];

			 if(value<2040){    // ���͂Ɉ�ł�����f����������C����f�����ɂ���
				 img->Y[j + k*width] = 0;
			 }
			 else{   // �����łȂ���Ό��摜�̂܂�
				 img->Y[j + k*width] = img_work->U[j + k*width];
			 }
		 }
	 }

	 for(k=1; k<height-1; k++) {          // ��ʂ̒[�͏������Ȃ��悤�ɂ��Ă���
		 for(j=1; j<width-1; j++) {
			 // ����8��f�̍��v�v�Z
			 value = img->Y[j-1 + (k-1)*width] + img->Y[j + (k-1)*width] + img->Y[j+1 + (k-1)*width]
				 + img->Y[j-1 + k*width] + img->Y[j+1 + k*width]
				 + img->Y[j-1 + (k+1)*width] + img->Y[j + (k+1)*width] + img->Y[j+1 + (k+1)*width];

			 if(value<2040) {    // ���͂Ɉ�ł�����f����������C����f�����ɂ���
				 img_work->U[j + k*width] = 0;
			 }
			 else{   // �����łȂ���Ό��摜�̂܂�
				 img_work->U[j + k*width] = img->Y[j + k*width];
			 }
		 }
	 }

	 for(k=1; k<height-1; k++) {          // ��ʂ̒[�͏������Ȃ��悤�ɂ��Ă���
		 for(j=1; j<width-1; j++) {
			 // ����8��f�̍��v�v�Z
			 value = img_work->U[j-1 + (k-1)*width] + img_work->U[j + (k-1)*width] + img_work->U[j+1 + (k-1)*width]
				 + img_work->U[j-1 + k*width] + img_work->U[j+1 + k*width]
				 + img_work->U[j-1 + (k+1)*width] + img_work->U[j + (k+1)*width] + img_work->U[j+1 + (k+1)*width];

			 if (value>0) {    // ���͂Ɉ�ł�����f����������C����f�𔒂ɂ���
				 img_out->Y[j + k*width] = 255;
			 }
			 else {   // �����łȂ���Ό��摜�̂܂�
				 img_out->Y[j + k*width] = img_work->U[j + k*width];
			 }
		 }
	 }

//---�����܂ł̏����ŁC�t�@�C���o�͂������摜��img_out�ɓ���Ă�������---
//--------------------------�����͂����܂�-------------------------------
	 
	 number = number - (number / 1000) * 1000;
	 img_out->Y[0] = rounding_integer(number);
	 printf("  === File output: number = %d \n", number);

 	 //�r�b�g�}�b�v�摜�̏�������
	    // img_out�ɏ������ނׂ�YUV�`���̉摜�f�[�^�������Ă��� //
	    // img_rgb2�͏����̂��߂̍�Ɨp������ //
	 write_bmp(img_out, img_rgb2, outfile);
	 printf(" <==== Image processing end \n");
	 
	 //�t�@�C���̃N���[�Y�ƃ��������
	 fclose(infile);
	 fclose(outfile);
	 free_IMG_YUV(img);
	 free_IMG_YUV(img_out);
	 free_IMG_RGB(img_rgb);
	 free_IMG_RGB(img_rgb2);
	 free_IMG_YUV(img_work);
	 
	 return 0;
}