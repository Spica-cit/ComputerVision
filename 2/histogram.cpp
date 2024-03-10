#define _CRT_SECURE_NO_WARNINGS  //���֐���warning�o�͖���

#include "header.h"

int main(int argc, char* argv[])
{
	/* �ϐ��̐錾 */
     FILE *infile,*outfile;
	 FILE *fp;

	 char infname[512];
     char outfname[512], csvfname[512];
//	 int palette[1024];			//�p���b�g�p�z��
	 int head[54];				//�w�b�_�f�[�^�z��

	 int size;
	 int height,width;
	 int k,j;
	 int value;

	 IMG_YUV *img, *img_out;        //8bit-YUV�摜�f�[�^�̃|�C���^
	 IMG_RGB *img_rgb, *img_rgb2;   //8bit-RGB�摜�f�[�^�̃|�C���^

     /* �p�����[�^�`�F�b�N */
     if (argc < 4) {    // (�ǂݍ��݃p�����[�^��+1)��菬�����Ă͂����Ȃ�
		 fprintf(stderr, "Usage: %s infname1 outfname1 csvfname \n", argv[0]);
	     printf("        infname1: input file name \n");
	     printf("        outfname1: output file name \n");
		 printf("        csvfname: output csv file name \n");
		 exit(-1);
     }

	 /* �p�����[�^�ݒ� */
	 strcpy(infname, argv[1]);      /* ���͉摜�t�@�C���� */
     strcpy(outfname, argv[2]);     /* �o�͉摜�t�@�C���� */
	 strcpy(csvfname, argv[3]);    /* �q�X�g�O�����o��csv�t�@�C���� */

	 printf(" -Input parameters --------\n");
	 printf("   -- Input image file= %s \n", infname);
     printf("   -- Output image file= %s \n", outfname);
	 printf("   -- Output csv file= %s \n", csvfname);

	 /* �摜�t�@�C���̃I�[�v�� */
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

	//�r�b�g�}�b�v�t�@�C���̉摜�f�[�^���ǂݍ���
	 read_bmp(img, img_rgb, infile);

	    // img�ɂ�YUV�`��(444�t�H�[�}�b�g�j�̉摜�f�[�^������ //
	    // img_rgb�ɂ�RGB�`���i444�t�H�[�}�b�g�j�̉摜�f�[�^������ //

	printf(" Image processing start =====> \n");

//--------------------------�����͂�����--------------------------------------------------
	int hist[256];
	int i,n;
	for (i = 0; i < 256; i++) {
		hist[i] = 0;
	}

	for(k=0; k<height; k++){
		for(j=0; j<width; j++){
			value = img->Y[j + k*width];	  // �P�x�l
			hist[value]++;
		}
	}
	n = width*height;
	for (i = 0; i < 256; i++) {
		fprintf(fp, "%d,%f \n", i, (double)hist[i]/n);       // �q�X�g�O�����̃t�@�C���o��
	}

	copy_image(img, img_out, 0); // img��img_out�ɃR�s�[�D�������C�F����128�ɒu��������i���m�N�����j

//---�����܂ł̏����ŁC�t�@�C���o�͂������摜��img_out�ɓ���Ă�������---
//--------------------------�����͂����܂�------------------------------------------------

	printf(" <==== Image processing end \n");
 
	//�r�b�g�}�b�v�摜�̏�������
	    // img_out�ɏ������ނׂ�YUV�`���̉摜�f�[�^�������Ă���
	    // img_rgb2�͏����̂��߂̍�Ɨp������
	write_bmp(img_out, img_rgb2, outfile);

	//�t�@�C���̃N���[�Y�ƃ��������
	fclose(infile);
	fclose(outfile);
	fclose(fp);
    free_IMG_YUV(img);
    free_IMG_YUV(img_out);
    free_IMG_RGB(img_rgb);
    free_IMG_RGB(img_rgb2);

	return 0;
}