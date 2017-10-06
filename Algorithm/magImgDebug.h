/*
 * magImgDebug.h
 *
 *  Created on: 2017-6-11
 *      Author: KangSong
 */

#ifndef MAG_IMG_DEBUG_H_
#define MAG_IMG_DEBUG_H_

extern  int g_FileNum;

void drawMagSensor(IplImage *pSrc,IplImage *pImg,struct tagMagSensorZone *pMagSensorZone);

void drawMagSensor_Foreign(IplImage *pSrc, IplImage *pImg, struct tagMagSensorZone_Foreign *pMagSensorZone);

void drawMagZone(IplImage *pImg,imgPoint *pCorner);
void drawMagZoneAf(IplImage *pImg,imgPoint *pCorner);
void drawMagImg(IplImage *pImg);
#endif
