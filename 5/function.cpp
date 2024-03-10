#define _CRT_SECURE_NO_WARNINGS  //���֐���warning�o�͖���

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "header.h"

typedef	unsigned char	uchar;


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

//----------------------------------------------------------------
//  Function-ID	rounding_off(double x) �����̏����_�ȉ����l�̌ܓ�����
//----------------------------------------------------------------
int rounding_off(double x)
{
	return (int)(x < 0.0 ? x - 0.5 : x + 0.5);
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

//------------------------------------------------------------------------------
//	Function-ID	bicubic_sinc_function
//	Function-Name	
//	Abstract	�o�C�L���[�r�b�N�psinc�֐�
//	Argument	double t	= ���S����̋���
//	Return-Value	double	value = h(t)
//	Special_Desc
//------------------------------------------------------------------------------
double bicubic_sinc_function(double	t)
{
	double	value, at;

	at = fabs(t);
	if (at <= 1){
		value = 1 - 2 * t*t + t*t*t;
	}
	else if (at>2){
		value = 0;
	}
	else{
		value = 4 - 8 * t + 5 * t*t - t*t*t;
	}

	return (value);
}

//----------------------------------------------------------------
//	Function-ID	affine4_image
//	Function-Name	
//	Abstract	�摜�̃A�t�B���ϊ����s���D
//              6�p�����[�^�̍s��ɂ��ϊ��D
//
//	Argument	IMG_YUV *img: �����Ώۉ摜�f�[�^
//		        IMG_YUV *imgout: �A�t�B���ϊ���̉摜�f�[�^
//                  �摜�̒��S�iwidth/2, height/2)��(0,0)�Ƃ݂Ȃ��Ċ􉽕ϊ����s��
//
//              double a,b,c,d,e,f: �A�t�B���ϊ��p�����[�^
//
//                 X = ax+by+e
//                 Y = cx+dy+f
//			                    
//	Return-Value	
//	Special_Desc	This function calls "bicubic_sinc_function"
//----------------------------------------------------------------
void  affine4_image(IMG_YUV *img, IMG_YUV *imgout, double a, double b, double c, double d, double e, double f)
{

	int j, k;
	int jj, kk;
	int width, height, widthc, heightc;
	int w2, h2, w2c, h2c;
	double Det;
	double jf, kf;
	int j0, k0;
	int loop, loopj, loopk, loopw;
	double d0x[4], d0y[4];
	int fY[4][4], fU[4][4], fV[4][4];
	double hy[4], hx[4];
	double sum, work[4];
	double sumU, sumV, workU[4], workV[4];

	width = img->width;
	height = img->height;
	widthc = width;
	heightc = height;
	w2 = width / 2;
	h2 = height / 2;
	w2c = widthc / 2;
	h2c = heightc / 2;

	/* === Y-signal processing ========================= */
	//	printf("affine_image: Y-signal processing \n");
	for (k = 0; k<height; k++){       /* �ϊ���̉摜��ŃX�L���� */
		for (j = 0; j<width; j++){    /* �ϊ���̉摜��ŃX�L���� */

			/* ��]�O�̍��W�l�ɕϊ� (in case of 0<r<90) */

			Det = a*d - b*c;
			jf = (d*(j - w2 - e) - b*(k - h2 - f)) / Det + w2;
			kf = (-c*(j - w2 - e) + a*(k - h2 - f)) / Det + h2;

			jf = rounding_off(jf*100000.0) / 100000.0;   /* ���Z�덷�C���̂��߂̎l�̌ܓ� */
			kf = rounding_off(kf*100000.0) / 100000.0;   /* ���Z�덷�C���̂��߂̎l�̌ܓ� */

			if (jf<0 || jf>(width - 1) || kf<0 || kf>(height - 1)) continue; /* �����Ώۗ̈�O */

			/* --- by bicubic interpolation ----------------------*/

			k0 = (int)kf;   // ��ԍ��W�ɑ΂��č���ɑ��݂�����͉摜�̉�f�ʒu���W
			d0y[1] = kf - k0; d0y[0] = 1 + kf - k0; d0y[2] = 1 - (kf - k0); d0y[3] = 2 - (kf - k0);   // 4�̎��݉�f�ʒu���W�܂ł̋����i�����j
			for (loop = 0; loop <= 3; loop++){
				hy[loop] = bicubic_sinc_function(d0y[loop]);
			}

			j0 = (int)jf;   // ��ԍ��W�ɑ΂��č���ɑ��݂�����͉摜�̉�f�ʒu���W
			d0x[1] = jf - j0; d0x[0] = 1 + jf - j0; d0x[2] = 1 - (jf - j0); d0x[3] = 2 - (jf - j0);   // 4�̎��݉�f�ʒu���W�܂ł̋����i�����j
			for (loop = 0; loop <= 3; loop++){
				hx[loop] = bicubic_sinc_function(d0x[loop]);
			}

			for (loopk = -1; loopk <= 2; loopk++){
				kk = k0 + loopk;
				if (kk<0) { kk = 0; }
				else if (kk>(height - 1)) { kk = height - 1; }
				for (loopj = -1; loopj <= 2; loopj++){
					jj = j0 + loopj;
					if (jj<0) { jj = 0; }
					else if (jj>(width - 1)) { jj = width - 1; }
					fY[loopj + 1][loopk + 1] = img->Y[jj + kk*width];    /* ��ԂɎg����f��4�~4�s��fY[4][4]�ɑ�� */
				}
			}
			for (loopw = 0; loopw <= 3; loopw++){
				sum = 0;
				for (loop = 0; loop <= 3; loop++){
					sum = sum + hy[loop] * fY[loopw][loop];
				}
				work[loopw] = sum;
			}
			sum = 0;
			for (loop = 0; loop <= 3; loop++){
				sum = sum + work[loop] * hx[loop];
			}
			imgout->Y[j + k*width] = rounding(sum);
		}
	}

	/* === U,V-signal processing ========================= */
	//	printf("affine_image: UV-signal processing \n");

	for (k = 0; k<heightc; k++){       /* �ϊ���̉摜��ŃX�L���� */
		for (j = 0; j<widthc; j++){    /* �ϊ���̉摜��ŃX�L���� */

			/* �ϊ��O�̍��W�l�ɕϊ� */

			Det = a*d - b*c;
			jf = (d*(j - w2c - e) - b*(k - h2c - f)) / Det + w2c;
			kf = (-c*(j - w2c - e) + a*(k - h2c - f)) / Det + h2c;

			jf = rounding_off(jf*100000.0) / 100000.0;   /* ���Z�덷�C���̂��߂̎l�̌ܓ� */
			kf = rounding_off(kf*100000.0) / 100000.0;   /* ���Z�덷�C���̂��߂̎l�̌ܓ� */

			if (jf<0 || jf>(widthc - 1) || kf<0 || kf>(heightc - 1)) continue; /* �����Ώۗ̈�O */

			/* --- by bicubic interpolation ----------------------*/

			k0 = (int)kf;   // ��ԍ��W�ɑ΂��č���ɑ��݂�����͉摜�̉�f�ʒu���W
			d0y[1] = kf - k0; d0y[0] = 1 + kf - k0; d0y[2] = 1 - (kf - k0); d0y[3] = 2 - (kf - k0);   // 4�̎��݉�f�ʒu���W�܂ł̋����i�����j
			for (loop = 0; loop <= 3; loop++){
				hy[loop] = bicubic_sinc_function(d0y[loop]);
			}

			j0 = (int)jf;   // ��ԍ��W�ɑ΂��č���ɑ��݂�����͉摜�̉�f�ʒu���W
			d0x[1] = jf - j0; d0x[0] = 1 + jf - j0; d0x[2] = 1 - (jf - j0); d0x[3] = 2 - (jf - j0);   // 4�̎��݉�f�ʒu���W�܂ł̋����i�����j
			for (loop = 0; loop <= 3; loop++){
				hx[loop] = bicubic_sinc_function(d0x[loop]);
			}

			for (loopk = -1; loopk <= 2; loopk++){
				kk = k0 + loopk;
				if (kk<0) { kk = 0; }
				else if (kk>(heightc - 1)) { kk = heightc - 1; }
				for (loopj = -1; loopj <= 2; loopj++){
					jj = j0 + loopj;
					if (jj<0) { jj = 0; }
					else if (jj>(widthc - 1)) { jj = widthc - 1; }
					fU[loopj + 1][loopk + 1] = img->U[jj + kk*widthc];    /* ��ԂɎg����f��4�~4�s��f[4][4]�ɑ�� */
					fV[loopj + 1][loopk + 1] = img->V[jj + kk*widthc];    /* ��ԂɎg����f��4�~4�s��f[4][4]�ɑ�� */
				}
			}
			for (loopw = 0; loopw <= 3; loopw++){
				sumU = 0;
				sumV = 0;
				for (loop = 0; loop <= 3; loop++){
					sumU = sumU + hy[loop] * fU[loopw][loop];
					sumV = sumV + hy[loop] * fV[loopw][loop];
				}
				workU[loopw] = sumU;
				workV[loopw] = sumV;
			}
			sumU = 0;
			sumV = 0;
			for (loop = 0; loop <= 3; loop++){
				sumU = sumU + workU[loop] * hx[loop];
				sumV = sumV + workV[loop] * hx[loop];
			}
			imgout->U[j + k*widthc] = rounding(sumU);
			imgout->V[j + k*widthc] = rounding(sumV);

		}
	}

}

//-----------------------------------------------------------------------------
//	Function-ID	draw_line
//	Function-Name	
//	Abstract	�����i��������ѐ����j��`�悷��
//
//	Argument	IMG_YUV *img: �`�悷��摜�f�[�^�i���o�́C444�`���ł��邱�Ɓj
//		        int xs : �n�_�������W
//		        int ys : �n�_�������W
//		        int xe : �I�_�������W
//		        int ye : �I�_�������W
//		        int color : �`�悳�����̐F
//                            �i-1:���C0:���C1:���΁C2:�C3:�ԁC4:�s���N�C5:���j
//			
//	Return-Value	
//	Special_Desc	
//
//-----------------------------------------------------------------------------
void  draw_line(IMG_YUV *img, int xs, int ys, int xe, int ye, int color)
{

	int j, k;
	int width, height;
	int widthc, heightc;
	int dataY, dataU, dataV;

	width = img->width;
	height = img->height;
	widthc = width;
	heightc = height;

	if (color == -1) { dataY = 0; dataU = 128; dataV = 128; }   /* �� */
	if (color == 0) { dataY = 255; dataU = 128; dataV = 128; }   /* �� */
	if (color == 1) { dataY = 128; dataU = 0; dataV = 0; }   /* ���� */
	if (color == 2) { dataY = 128; dataU = 255; dataV = 0; }   /* �� */
	if (color == 3) { dataY = 128; dataU = 0; dataV = 255; }   /* �� */
	if (color == 4) { dataY = 128; dataU = 255; dataV = 255; }   /* �s���N */
	if (color == 5) { dataY = 255; dataU = 0; dataV = 128; }   /* �� */

	for (j = xs; j <= xe; j++){
		img->Y[j + ys*width] = dataY;
		img->U[j + ys*width] = dataU;
		img->V[j + ys*width] = dataV;
		img->Y[j + ye*width] = dataY;
		img->U[j + ye*width] = dataU;
		img->V[j + ye*width] = dataV;
	}
	for (k = ys; k <= ye; k++){
		img->Y[xs + k*width] = dataY;
		img->U[xs + k*width] = dataU;
		img->V[xs + k*width] = dataV;
		img->Y[xe + k*width] = dataY;
		img->U[xe + k*width] = dataU;
		img->V[xe + k*width] = dataV;
	}

}

//-----------------------------------------------------------------------------
//	Function-ID	draw_axis
//	Function-Name	
//	Abstract	���W����`�悷��
//
//	Argument	IMG_YUV *img: �`�悷��摜�f�[�^�i���o�́C444�`���ł��邱�Ɓj
//		        int xs : �n�_�������W
//		        int ys : �n�_�������W
//		        int xe : �I�_�������W
//		        int ye : �I�_�������W
//		        int color : �`�悳�����̐F
//                            �i-1:���C0:���C1:���΁C2:�C3:�ԁC4:�s���N�C5:���j
//			
//	Return-Value	
//	Special_Desc	
//
//-----------------------------------------------------------------------------
void  draw_axix(IMG_YUV *img, int jc, int kc)
{

	int width, height;
	int color;
	int x, y, xs, ys, xe, ye;

	width = img->width;
	height = img->height;

	/* ���W���̕`�� */
	color = 2;
	ys = 0;
	ye = height - 1;
	for (x = jc; x<width; x = x + 100){
		xs = x;
		xe = x;
		draw_line(img, xs, ys, xe, ye, color);
	}
	for (x = jc; 0 <= x; x = x - 100){
		xs = x;
		xe = x;
		draw_line(img, xs, ys, xe, ye, color);
	}
	xs = 0;
	xe = width - 1;
	for (y = kc; y<height; y = y + 100){
		ys = y;
		ye = y;
		draw_line(img, xs, ys, xe, ye, color);
	}
	for (y = kc; 0 <= y; y = y - 100){
		ys = y;
		ye = y;
		draw_line(img, xs, ys, xe, ye, color);
	}

	color = 3;
	draw_line(img, jc, 0, jc, height - 1, color);
	draw_line(img, jc - 1, 0, jc - 1, height - 1, color);
	draw_line(img, jc + 1, 0, jc + 1, height - 1, color);
	draw_line(img, 0, kc, width - 1, kc, color);
	draw_line(img, 0, kc - 1, width - 1, kc - 1, color);
	draw_line(img, 0, kc + 1, width - 1, kc + 1, color);

}
