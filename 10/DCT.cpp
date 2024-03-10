#define _CRT_SECURE_NO_WARNINGS  //���֐���warning�o�͖���

#include "header.h"

//------------------------------------------------------------------------------
//
// �摜�̗��U�R�T�C���ϊ��T���v���v���O����
//
//
//------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
     FILE *infile,*outfile;
	 char infname1[512];
     char outfname1[512];
//	 int palette[1024];			//�p���b�g�p�z��
	 int head[54];				//�w�b�_�f�[�^�z��

	 int size;
	 int height,width;
	 int k,j;

	 IMG_YUV *img, *img_out;        //8bit-YUV�f�[�^�̃|�C���^
	 IMG_RGB *img_rgb, *img_rgb2;   //8bit-RGB�f�[�^�̃|�C���^

	 int QP, qscaleDC, qscaleAC;
	 double snr_Y, snr_U, snr_V;

	 uchar array_line[176][16];
	 int student_number;
	 uchar qp_line[104][16];

     /* �p�����[�^�`�F�b�N */
     if (argc < 5) {    // (�ǂݍ��݃p�����[�^��+1)��菬�����Ă͂����Ȃ�
		 fprintf(stderr, "Usage: %s infname1 outfname1 QP student_number \n", argv[0]);
	     printf("        infname1: input file name \n");
	     printf("        outfname1: output file name \n");
		 printf("        QP:  quantization parameter( 1 <= QP <= 1024 ) \n");
		 printf("        student number:  student number (7-digit) \n");
	     exit(-1);
     }

	 /* �p�����[�^�ݒ� */
	 strcpy(infname1, argv[1]);      /* ���͉摜�t�@�C���� */
     strcpy(outfname1, argv[2]);     /* �o�͉摜�t�@�C���� */
	 QP = atoi(argv[3]);         /* DCT�W���ʎq���p�����[�^ */
	 student_number = atoi(argv[4]);         /* �w���ԍ�(�V�P�^�j */

	 qscaleDC = 8;         /* DCT�W���ʎq���p�����[�^�ݒ�(�����W���j */
	 qscaleAC = QP;         /* DCT�W���ʎq���p�����[�^�ݒ�(�𗬌W���j */

	 printf(" -Input parameters --------\n");
	 printf("   -- Input file= %s \n", infname1);
     printf("   -- Output file= %s \n", outfname1);
     printf("   -- QP= %d \n", QP);
//	 printf("   -- qscaleDC= %d \n", qscaleDC);
//	 printf("   -- qscaleAC= %d \n", qscaleAC);

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

    //  �p���b�g�f�[�^��ǂݍ��ށi���̃f�[�^�͖{�v���O�����ł͎g��Ȃ��j
//	for(int c=0; c<1024; c++){
//			palette[c] = getc(infile);
//  }

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

	printf("  DCT - Quantization - IDCT processing  \n");

	 int jj, kk;
	 int block_dataY[8][8], block_dataY_out[8][8];
	 int block_dataU[8][8], block_dataU_out[8][8];
	 int block_dataV[8][8], block_dataV_out[8][8];
	 double block_DCT[8][8], block_DCTQ[8][8];

	 copy_image(img, img_out, 1);

	 for(k=0;k<height-7; k=k+8){           /* �摜��8�̔{���łȂ��ꍇ�ɂ́C�摜�E������щ摜�����̒[���͏�������Ȃ��i���摜�̂܂܁j */
		 for(j=0; j<width-7; j=j+8){

			 /* �摜�z�񂩂�8�~8�u���b�N�̉�f�f�[�^�𒊏o */
			 for (kk=0; kk<8; kk++){
				 for(jj=0; jj<8; jj++){
					 block_dataY[jj][kk]=img->Y[ j+jj + (k+kk)*width ];
					 block_dataU[jj][kk]=img->U[ j+jj + (k+kk)*width ];
					 block_dataV[jj][kk]=img->V[ j+jj + (k+kk)*width ];
				 }
			 }

//          ��������8�~8�u���b�N���Ƃ�DCT���Z�Ɨʎq���ƋtDCT���s���i���ꂼ��̏����͊֐��ƂȂ��Ă���j
			 
			 /* �P�x(Y)�M���̏��� */
			 dct88(block_dataY, block_DCT);                                                  /* DCT */
			 quantization88_new(block_DCT, block_DCTQ, qscaleDC, qscaleAC, 0);               /* DCT�W���̗ʎq�� */
	         idct88(block_DCTQ, block_dataY_out);                                            /* �tDCT */

			 /* �F��(U)�M���̏��� */
	         dct88(block_dataU, block_DCT);                                                  /* DCT */
			 quantization88_new(block_DCT, block_DCTQ, qscaleDC, qscaleAC, 0);               /* DCT�W���̗ʎq�� */
	         idct88(block_DCTQ, block_dataU_out);                                            /* �tDCT */

			 /* �F��(V)�M���̏��� */
	         dct88(block_dataV, block_DCT);                                                  /* DCT */
			 quantization88_new(block_DCT, block_DCTQ, qscaleDC, qscaleAC, 0);               /* DCT�W���̗ʎq�� */
	         idct88(block_DCTQ, block_dataV_out);                                            /* �tDCT */

			 /* �������ꂽ8�~8�u���b�N�̉�f�l���摜�z��ɑ�� */
			 for (kk=0; kk<8; kk++){
			 	 for(jj=0; jj<8; jj++){
					 img_out->Y[ j+jj + (k+kk)*width ] = rounding_integer(block_dataY_out[jj][kk]);
					 img_out->U[ j+jj + (k+kk)*width ] = rounding_integer(block_dataU_out[jj][kk]);
					 img_out->V[ j+jj + (k+kk)*width ] = rounding_integer(block_dataV_out[jj][kk]);
				 }
			 }

		}
	  }

//---�����܂ł̏����ŁC�t�@�C���o�͂������摜��img_out�ɓ���Ă�������---
//--------------------------�����͂����܂�------------------------------------------------

	printf(" <==== Image processing end \n");
 
	/* SN�v�Z */
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

	//�r�b�g�}�b�v�摜�̏�������
	    // img_out�ɏ������ނׂ�YUV�`���̉摜�f�[�^�������Ă��� //
	    // img_rgb2�͏����̂��߂̍�Ɨp������ //
	write_bmp(img_out, img_rgb2, outfile);

	//�t�@�C���̃N���[�Y�ƃ��������
	fclose(infile);
	fclose(outfile);
    free_IMG_YUV(img);
    free_IMG_YUV(img_out);
    free_IMG_RGB(img_rgb);
    free_IMG_RGB(img_rgb2);

	return 0;
}