#define _CRT_SECURE_NO_WARNINGS  //���֐���warning�o�͖���

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
//  Function-ID	rounding(double dv)�@�_�u���^�l��0-255�͈̔͂Ɋۂ߂�
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
//  Function-ID	rounding_integer(int idv) �����l��0-255�͈̔͂Ɋۂ߂�
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
//	Function-Name	�������m��
//	Abstract	�ُ�I�����ɃG���[���b�Z�[�W��Ԃ�malloc�֐�
//	Argument	int	size	= �m�ۂ��郁�����T�C�Y
//	Return-Value	void	*a	= �m�ۂ����������̃A�h���X
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
//	Function-Name	IMG�f�[�^�̈�m��
//	Abstract	IMG_YUV�f�[�^�̃��������m�ۂ���
//	Argument	int	width	= �摜���T�C�Y
//				int	height	= �摜�c�T�C�Y
//	Return-Value	IMG_YUV *		= �m�ۂ����̈�̃|�C���^
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
//	Function-Name	IMG�f�[�^�̈�m��
//	Abstract	IMG_RGB�f�[�^�̃��������m�ۂ���
//	Argument	int	width	= �摜���T�C�Y
//				int	height	= �摜�c�T�C�Y
//	Return-Value	IMG_RGB *		= �m�ۂ����̈�̃|�C���^
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
//	Function-Name	IMG_YUV�f�[�^���������
//	Abstract	IMG_YUV�f�[�^�̗̈���������
//	Argument	IMG_YUV	*img	= �摜�\����
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
//	Function-Name	IMG_RGB�f�[�^���������
//	Abstract	IMG_RGB�f�[�^�̗̈���������
//	Argument	IMG_RGB	*img	= �摜�\����
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
//	Function-Name	�r�b�g�}�b�v�ǂݍ���
//	Abstract	img_rgb�ɓǂݍ��݁A YUV�ϊ�����img�Ɋi�[
//	Argument	IMG_YUV *img:	YUV�M�����i�[����̈�
//              IMG_RGB *img_rgb:	�ǂݍ���BMP���i�[����̈�
//              FILE *infile:       �ǂݍ��ݐ�|�C���^
//	Special_Desc	24�r�b�g�̂ݑΉ�
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

	//�摜�f�[�^��ǂݍ���
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
//	Function-Name	�r�b�g�}�b�v��������
//	Abstract	YUV��RGB�ϊ�����img_rbg2�Ɋi�[
//	Argument	IMG_YUV *img:	YUV�M�����i�[����Ă���摜�f�[�^
//              IMG_RGB *img_rgb:       �F�ϊ���̉摜�i�[�̈�i��Ɨp�z��j   
//              FILE *outfile:       �������ݐ�|�C���^
//	Special_Desc	24�r�b�g�̂ݑΉ�
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

	  	//YUV-RGB�ϊ��ƕ��בւ�
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

		 //�w�b�_��������
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
		  fputc(img_rgb->B[kk],outfile);		//B���o��
		  fputc(img_rgb->G[kk],outfile);		//G
		  fputc(img_rgb->R[kk],outfile);		//R
	  }
}

//----------------------------------------------------------------
//	Function-ID	copy_image
//	Function-Name	
//	Abstract	IMG_YUV�^�\���̂̉摜�f�[�^���R�s�[����i�\���̃|�C���^�������n���j
//
//	Argument	IMG_YUV *img1: copy���摜�f�[�^
//		        IMG_YUV *img2: copy��摜�f�[�^
//		        int param : param=1(0�ȊO)�̂Ƃ�YUV���ׂĂ��R�s�[�i�J���[�̂܂܃R�s�[�j
//                          param=0�̂Ƃ�Y�����R�s�[����U��V�ɂ�128������i���m�N�����j
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
//	Abstract	IMG_YUV�^�\���̂̉摜�f�[�^�̊e�R���|�[�l���g�ɒ�߂�ꂽ�l���Z�b�g����
//
//	Argument	IMG_YUV *img1: �摜�f�[�^
//		        int Yvalue : �P�x�M���ɃZ�b�g����l�i0-255�j
//		        int Uvalue : �P�x�M���ɃZ�b�g����l�i0-255�j
//		        int Vvalue : �P�x�M���ɃZ�b�g����l�i0-255�j
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
//	Abstract	IMG_YUV�^�\���̂̉摜�f�[�^��SNR�����߂�
//
//
//	Argument	IMG_YUV *img1: ���摜�f�[�^(input)
//		        IMG_YUV *img2: �����摜�f�[�^(input)
//		        *snr_Y : �P�x�M��Y��SN��
//		        *snr_U : �F���M��U��SN��
//		        *snr_V : �F���M��V��SN��
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
//	Abstract	8x8 DCT���s��
//	Argument	(*xx)[8]: 8x8���͉摜�}�g���N�X
//              (*yy)[8]: 8x8�o��DCT�W���}�g���N�X
//	Return-Value	Nothing
//	Special_Desc�@Tis function calls subfunction 'dct8'(Chen's fast algorithm)
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
//	Abstract	8x8 �tDCT���s��
//	Argument	(*yy2)[8]: 8x8����DCT�W���}�g���N�X
//              (*xx2)[8]: 8x8�o�͉摜�f�[�^�}�g���N�X
//	Return-Value	Nothing
//	Special_Desc�@Tis function calls subfunction 'idct8'(Chen's fast algorithm)
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
//	Abstract	8x8 DCT�W���̗ʎq�����s���D�X�P�[���t�@�N�^�Ƃ��Ď����l�����e����o�[�W�����D
//	Argument	(*xx)[8]: 8x8����DCT�W���}�g���N�X
//              (*outvalue)[8]: 8x8�o�͗ʎq����\�l�}�g���N�X
//              int qscaleDC: �����W���̃X�P�[���t�@�N�^�iqscaleDC=16�̂Ƃ��f�t�H�[���g�̃}�g���N�X���ʎq���X�e�b�v�j
//              int qscaleAC: �𗬌W���̃X�P�[���t�@�N�^�iqscaleAC=16�̂Ƃ��f�t�H�[���g�̃}�g���N�X���ʎq���X�e�b�v�j
//              color: 0: �P�x�M���C1:�F���M�� �i�ʎq���}�g���N�X���قȂ�j
//	Return-Value	Nothing
//	Special_Desc�@�ʎq���}�g���N�Xq_matrix_y[jj][kk]���O���[�o���Œ�`���Ă��邱��
//                �ʎq���X�e�b�v�́C qs= qscale/16 * Qmatrix�@�ƂȂ�D 
//                qscaleDC>2, qscaleAC>4
//------------------------------------------------------------------------------
void quantization88_new(double (*xx)[8], double (*outvalue)[8], int qscaleDC, int qscaleAC, int color)

{
	 int jj,kk;
	 int qstep, qlevel, qvalue;
	 double qsDC, qsAC;

	 qsDC=(double) qscaleDC/16;
	 qsAC=(double) qscaleAC/16;

	 if(color==0){                        //�P�x�M��
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
	 else{                                   //�F���M��
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
