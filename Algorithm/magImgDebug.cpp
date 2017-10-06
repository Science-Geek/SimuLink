/*
 * magImgDebug.cpp
 *
 *  Created on: 2017-6-11
 *      Author: KangSong
 */
//#include <stdafx.h>
#include "string.h"
#include "math.h"
#include "magImg.h"
#include "magImgType.h"


extern struct tagMagSensorZone_Foreign gMagSensorZone_Foreign[TDN_MAX];

#if X86_X64_DEBUG
void drawMagSensor(IplImage *pSrc, IplImage *pImg, struct tagMagSensorZone *pMagSensorZone)
{
	struct tagMagSensorZone *pSensorZone = NULL;
	uint8_t *imgData,*srcData;
	int r,g,b,thred;
	imgData = (uint8_t *)pImg->imageData;
	srcData = (uint8_t *)pSrc->imageData;
	for(pSensorZone = pMagSensorZone;pSensorZone != NULL;pSensorZone = pSensorZone->next)
	{
		if(pSensorZone->type == TDN_IRLC || pSensorZone->type == TDN_IRRC )
		{
			r = -1;
			g = -1;
			b = -1;
			thred = 50;
		}
		//else if(pSensorZone->type == MM0 || pSensorZone->type == MM1 ||
		//	pSensorZone->type == MM2 || pSensorZone->type == MM3 ||
		//	pSensorZone->type == MM4 || pSensorZone->type == MM5)
		//{
		//	r = 200;
		//	g = 0;
		//	b = 0;
		//	thred = 50;
		//}
		else
		{
			r = 0;
			g = 200;
			b = 0;
			thred = 50;
		}

		for(int i=0;i<pImg->height;i++)
		{
			//for(int j=pSensorZone->position;j<pSensorZone->position+pSensorZone->range;j++)
			//{
			//	if(srcData[i*pSrc->widthStep+j] <thred)
			//	{
			//		if(b >= 0)
			//			imgData[i*pImg->widthStep+j*3] = b;
			//		if(g >= 0)
			//			imgData[i*pImg->widthStep+j*3+1] = g;
			//		if(r >= 0)
			//			imgData[i*pImg->widthStep+j*3+2] = r;
			//	}
			//}
			if(b >= 0)
				imgData[i*pImg->widthStep+pSensorZone->position*3] = b;
			if(g >= 0)
				imgData[i*pImg->widthStep+pSensorZone->position*3+1] = g;
			if(r >= 0)
				imgData[i*pImg->widthStep+pSensorZone->position*3+2] = r;

			if(b >= 0)
				imgData[i*pImg->widthStep+(pSensorZone->position+pSensorZone->range)*3] = b;
			if(g >= 0)
				imgData[i*pImg->widthStep+(pSensorZone->position+pSensorZone->range)*3+1] = g;
			if(r >= 0)
				imgData[i*pImg->widthStep+(pSensorZone->position+pSensorZone->range)*3+2] = r;
		}

	}
}

void drawMagSensor_Foreign(IplImage *pSrc, IplImage *pImg, struct tagMagSensorZone_Foreign *pMagSensorZone)
{
	struct tagMagSensorZone_Foreign *pSensorZone = NULL;
	uint8_t *imgData, *srcData;
	int r, g, b, thred;
	imgData = (uint8_t *)pImg->imageData;
	srcData = (uint8_t *)pSrc->imageData;
	for (pSensorZone = pMagSensorZone; pSensorZone != NULL; pSensorZone = pSensorZone->next)
	{
		if (pSensorZone->type == TDN_IRLC || pSensorZone->type == TDN_IRRC)
		{
			r = -1;
			g = -1;
			b = -1;
			thred = 50;
		}
		//else if(pSensorZone->type == MM0 || pSensorZone->type == MM1 ||
		//	pSensorZone->type == MM2 || pSensorZone->type == MM3 ||
		//	pSensorZone->type == MM4 || pSensorZone->type == MM5)
		//{
		//	r = 200;
		//	g = 0;
		//	b = 0;
		//	thred = 50;
		//}
		else
		{
			r = 0;
			g = 200;
			b = 0;
			thred = 50;
		}

		for (int i = 0; i<pImg->height; i++)
		{
			//for(int j=pSensorZone->position;j<pSensorZone->position+pSensorZone->range;j++)
			//{
			//	if(srcData[i*pSrc->widthStep+j] <thred)
			//	{
			//		if(b >= 0)
			//			imgData[i*pImg->widthStep+j*3] = b;
			//		if(g >= 0)
			//			imgData[i*pImg->widthStep+j*3+1] = g;
			//		if(r >= 0)
			//			imgData[i*pImg->widthStep+j*3+2] = r;
			//	}
			//}
			if (b >= 0)
				imgData[i*pImg->widthStep + pSensorZone->position * 3] = b;
			if (g >= 0)
				imgData[i*pImg->widthStep + pSensorZone->position * 3 + 1] = g;
			if (r >= 0)
				imgData[i*pImg->widthStep + pSensorZone->position * 3 + 2] = r;

			if (b >= 0)
				imgData[i*pImg->widthStep + (pSensorZone->position + pSensorZone->range) * 3] = b;
			if (g >= 0)
				imgData[i*pImg->widthStep + (pSensorZone->position + pSensorZone->range) * 3 + 1] = g;
			if (r >= 0)
				imgData[i*pImg->widthStep + (pSensorZone->position + pSensorZone->range) * 3 + 2] = r;
		}

	}
}

void drawMagZone(IplImage *pImg,imgPoint *pCorner)
{
	float k,b;
	int xStart[256],xEnd[256];
	uint8_t *imgData;
	if(pImg == NULL)
		return;
	imgData = (uint8_t *)pImg->imageData;
	if(pCorner[0].y <= pCorner[1].y)
	{
		if(pCorner[3].y - pCorner[0].y == 0)
			b = 0;
		else
			b = (pCorner[3].x - pCorner[0].x)/(float)(pCorner[3].y - pCorner[0].y);

		for(int i=0;i<=pCorner[3].y - pCorner[0].y;i++)
		{
			xStart[i] = (pCorner[0].x + i*b)+0.5;
		}

		if(pCorner[1].y - pCorner[0].y == 0)
			b= pCorner[1].x - pCorner[0].x;
		else
			b = (pCorner[1].x - pCorner[0].x)/(float)(pCorner[1].y - pCorner[0].y);
		for(int i=0;i<=pCorner[1].y - pCorner[0].y;i++)
		{
			xEnd[i] = (pCorner[0].x + i*b)+0.5;
		}

		if(pCorner[2].y - pCorner[3].y == 0)
			b = 0;
		else
			b = (pCorner[2].x - pCorner[3].x)/(float)(pCorner[2].y - pCorner[3].y);

		for(int i=0;i<=pCorner[2].y - pCorner[3].y;i++)
		{
			xStart[i+pCorner[3].y-pCorner[0].y] = (pCorner[3].x + i*b)+0.5;
		}

		if(pCorner[2].y - pCorner[1].y == 0)
			b= pCorner[2].x - pCorner[1].x;
		else
			b = (pCorner[2].x - pCorner[1].x)/(float)(pCorner[2].y - pCorner[1].y);

		for(int i=0;i<=pCorner[2].y - pCorner[1].y;i++)
		{
			xEnd[i+pCorner[1].y-pCorner[0].y] = (pCorner[1].x + i*b)+0.5;
		}
		//k= (pCorner[0].y - pCorner[3].y)/(float)(pCorner[0].x - pCorner[3].x);
		//b = ((pCorner[0].y-k*pCorner[0].x)+(pCorner[3].y-k*pCorner[3].x))/2;
		//for(int i=0;i<=pCorner[3].y - pCorner[0].y;i++)
		//{
		//	xStart[i] = (pCorner[0].y+i - b)/k;
		//}

		//k= (pCorner[0].y - pCorner[1].y)/(float)(pCorner[0].x - pCorner[1].x);
		//b = ((pCorner[0].y-k*pCorner[0].x)+(pCorner[1].y-k*pCorner[1].x))/2;
		//for(int i=0;i<=pCorner[1].y - pCorner[0].y;i++)
		//{
		//	xEnd[i] = (pCorner[0].y+i - b)/k;
		//}

		//k= (pCorner[3].y - pCorner[2].y)/(float)(pCorner[3].x - pCorner[2].x);
		//b = ((pCorner[3].y-k*pCorner[3].x)+(pCorner[2].y-k*pCorner[2].x))/2;
		//for(int i=0;i<=pCorner[2].y - pCorner[3].y;i++)
		//{
		//	xStart[i+pCorner[3].y-pCorner[0].y] = (pCorner[3].y+i - b)/k;
		//}

		//k= (pCorner[1].y - pCorner[2].y)/(float)(pCorner[1].x - pCorner[2].x);
		//b = ((pCorner[1].y-k*pCorner[1].x)+(pCorner[2].y-k*pCorner[2].x))/2;
		//for(int i=0;i<=pCorner[2].y - pCorner[1].y;i++)
		//{
		//	xEnd[i+pCorner[1].y-pCorner[0].y] = (pCorner[1].y+i - b)/k;
		//}

		for(int i=pCorner[0].y ;i<=pCorner[2].y;i++)
		{
			for(int j=xStart[i-pCorner[0].y];j<=xEnd[i-pCorner[0].y];j++)
			{
				if(i>=0 && i<pImg->height && j>=0 && j<pImg->width)
					//imgData[(pImg->height - i)*pImg->widthStep + j * 3 + 2] = 0;
					imgData[i*pImg->widthStep + j * 3 + 2] = 0;
			}
		}
	}
	else
	{
		if(pCorner[0].y - pCorner[1].y == 0)
			b = 0;
		else
			b = (pCorner[0].x - pCorner[1].x)/(float)(pCorner[0].y - pCorner[1].y);

		for(int i=0;i<=pCorner[0].y - pCorner[1].y;i++)
		{
			xStart[i] = (pCorner[1].x + i*b)+0.5;
		}

		if(pCorner[2].y - pCorner[1].y == 0)
			b= pCorner[2].x - pCorner[1].x;
		else
			b = (pCorner[2].x - pCorner[1].x)/(float)(pCorner[2].y - pCorner[1].y);
		for(int i=0;i<=pCorner[2].y - pCorner[1].y;i++)
		{
			xEnd[i] = (pCorner[1].x + i*b)+0.5;
		}

		if(pCorner[3].y - pCorner[0].y == 0)
			b = 0;
		else
			b = (pCorner[3].x - pCorner[0].x)/(float)(pCorner[3].y - pCorner[0].y);

		for(int i=0;i<=pCorner[3].y - pCorner[0].y;i++)
		{
			xStart[i+pCorner[0].y-pCorner[1].y] = (pCorner[0].x + i*b)+0.5;
		}

		if(pCorner[3].y - pCorner[2].y == 0)
			b= pCorner[3].x - pCorner[2].x;
		else
			b = (pCorner[3].x - pCorner[2].x)/(float)(pCorner[3].y - pCorner[2].y);

		for(int i=0;i<=pCorner[3].y - pCorner[2].y;i++)
		{
			xEnd[i+pCorner[2].y-pCorner[1].y] = (pCorner[2].x + i*b)+0.5;
		}

		for(int i=pCorner[1].y ;i<=pCorner[3].y;i++)
		{
			for(int j=xStart[i-pCorner[1].y];j<=xEnd[i-pCorner[1].y];j++)
			{
				if(i>=0 && i<pImg->height && j>=0 && j<pImg->width)
					imgData[i*pImg->widthStep+j*3+2] = 0;
			}
		}
	}

}

void drawMagZoneAf(IplImage *pImg,imgPoint *pCorner)
{
	float k,b;
	int xStart[512] = { 0 }, xEnd[512] = { 0 };
	uint8_t *imgData;
	if(pImg == NULL || pCorner[3].y - pCorner[0].y<0 || pCorner[1].y - pCorner[0].y<0 || 
		pCorner[0].y - pCorner[1].y<0 || pCorner[2].y - pCorner[1].y<0)
		return;
	imgData = (uint8_t *)pImg->imageData;
	if(pCorner[0].y <= pCorner[1].y)
	{
		if(pCorner[3].y - pCorner[0].y == 0)
			b = 0;
		else
			b = (pCorner[3].x - pCorner[0].x)/(float)(pCorner[3].y - pCorner[0].y);

		for(int i=0;i<=pCorner[3].y - pCorner[0].y;i++)
		{
			xStart[i] = (pCorner[0].x + i*b)+0.5;
		}

		if(pCorner[1].y - pCorner[0].y == 0)
			b= pCorner[1].x - pCorner[0].x;
		else
			b = (pCorner[1].x - pCorner[0].x)/(float)(pCorner[1].y - pCorner[0].y);
		for(int i=0;i<=pCorner[1].y - pCorner[0].y;i++)
		{
			xEnd[i] = (pCorner[0].x + i*b)+0.5;
		}

		if(pCorner[2].y - pCorner[3].y == 0)
			b = 0;
		else
			b = (pCorner[2].x - pCorner[3].x)/(float)(pCorner[2].y - pCorner[3].y);

		for(int i=0;i<=pCorner[2].y - pCorner[3].y;i++)
		{
			xStart[i+pCorner[3].y-pCorner[0].y] = (pCorner[3].x + i*b)+0.5;
		}

		if(pCorner[2].y - pCorner[1].y == 0)
			b= pCorner[2].x - pCorner[1].x;
		else
			b = (pCorner[2].x - pCorner[1].x)/(float)(pCorner[2].y - pCorner[1].y);

		for(int i=0;i<=pCorner[2].y - pCorner[1].y;i++)
		{
			xEnd[i+pCorner[1].y-pCorner[0].y] = (pCorner[1].x + i*b)+0.5;
		}

		for(int i=pCorner[0].y ;i<=pCorner[2].y;i++)
		{
			for(int j=xStart[i-pCorner[0].y];j<=xEnd[i-pCorner[0].y];j++)
			{
				if(i<pImg->height && j<pImg->width)
					imgData[i*pImg->widthStep+j*3] = 0;
			}
		}
	}
	else
	{
		if(pCorner[0].y - pCorner[1].y == 0)
			b = 0;
		else
			b = (pCorner[0].x - pCorner[1].x)/(float)(pCorner[0].y - pCorner[1].y);

		for(int i=0;i<=pCorner[0].y - pCorner[1].y;i++)
		{
			xStart[i] = (pCorner[1].x + i*b)+0.5;
		}

		if(pCorner[2].y - pCorner[1].y == 0)
			b= pCorner[2].x - pCorner[1].x;
		else
			b = (pCorner[2].x - pCorner[1].x)/(float)(pCorner[2].y - pCorner[1].y);
		for(int i=0;i<=pCorner[2].y - pCorner[1].y;i++)
		{
			xEnd[i] = (pCorner[1].x + i*b)+0.5;
		}

		if(pCorner[3].y - pCorner[0].y == 0)
			b = 0;
		else
			b = (pCorner[3].x - pCorner[0].x)/(float)(pCorner[3].y - pCorner[0].y);

		for(int i=0;i<=pCorner[3].y - pCorner[0].y;i++)
		{
			xStart[i+pCorner[0].y-pCorner[1].y] = (pCorner[0].x + i*b)+0.5;
		}

		if(pCorner[3].y - pCorner[2].y == 0)
			b= pCorner[3].x - pCorner[2].x;
		else
			b = (pCorner[3].x - pCorner[2].x)/(float)(pCorner[3].y - pCorner[2].y);

		for(int i=0;i<=pCorner[3].y - pCorner[2].y;i++)
		{
			xEnd[i+pCorner[2].y-pCorner[1].y] = (pCorner[2].x + i*b)+0.5;
		}

		for(int i=pCorner[1].y ;i<=pCorner[3].y;i++)
		{
			for(int j=xStart[i-pCorner[1].y];j<=xEnd[i-pCorner[1].y];j++)
			{
				if(i<pImg->height && j<pImg->width)
					imgData[i*pImg->widthStep+j*3] = 0;
			}
		}
	}
}

void drawMagImg(IplImage *pImg)
{
	struct tagMagSensorZone_Foreign *pSensorZone;
	struct tagMagClass *pMagClass;
	imgPoint Corner[4];
	IplImage *pImgMagZone;

	//for(pTdnMagSensorRes = pMagFeatureRes->pTdnMagSensorRes;pTdnMagSensorRes!=NULL;pTdnMagSensorRes = pTdnMagSensorRes->next)
	//{
		for(pSensorZone = gMagSensorZone_Foreign;pSensorZone != NULL;pSensorZone = pSensorZone->next)
		{
		//	if(pSensorZone->type == pTdnMagSensorRes->type)
			{
				Corner[0].x = Corner[3].x = pSensorZone->position;
				Corner[1].x = Corner[2].x = pSensorZone->position + pSensorZone->range;
		//		break;
			}
			for(pMagClass = pSensorZone->pMagClass;pMagClass != NULL ;pMagClass = pMagClass->next)
			{
				Corner[0].y = Corner[1].y = pMagClass->startimg;
				Corner[2].y = Corner[3].y = pMagClass->endimg;
				drawMagZoneAf(pImg,Corner);
			}
		}

	//}
}
#endif
