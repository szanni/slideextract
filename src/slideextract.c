/* slideextract -- implementation
 *
 * Copyright (c) 2013, Angelo Haller <angelo@szanni.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "slideextract.h"
#include <stdio.h>
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/imgcodecs/imgcodecs_c.h>
#include <opencv2/videoio/videoio_c.h>

static _Bool selected_roi = 0;
static CvPoint point1;
static CvPoint point2;

static void
_se_mouseHandler (int event, int x, int y, int flags, void *param)
{
  IplImage *clone = NULL;
  IplImage *frame = param;

  if (event == CV_EVENT_LBUTTONDOWN && !(flags & CV_EVENT_FLAG_LBUTTON))
    {
      point1 = cvPoint (x, y);
      point2 = cvPoint (x, y);
    }

  if (!(flags & CV_EVENT_FLAG_LBUTTON))
    return;

  if (event != CV_EVENT_MOUSEMOVE && event != CV_EVENT_LBUTTONUP)
    return;

  point2 = cvPoint (x, y);
  selected_roi = (point1.x != point2.x && point1.y != point2.y);

  clone = cvCloneImage (frame);
  cvRectangle (clone, point1, point2, CV_RGB(0, 255, 0), 1, 8, 0);
  cvShowImage ("frame", clone);
  cvReleaseImage (&clone);
}

int
se_select_roi (const char *file, CvRect *roi)
{
  CvCapture *capture = cvCaptureFromFile (file);

  if (capture == NULL)
    return -1;

  int key = -1;
  IplImage *frame = NULL;
  IplImage *clone = NULL;
  cvNamedWindow ("frame", CV_WINDOW_AUTOSIZE);

  while (key == -1)
    {
      frame = cvQueryFrame (capture);
      if (frame == NULL)
        break;

      cvSetMouseCallback ("frame", _se_mouseHandler, frame);

      if (selected_roi)
        {
          clone = cvCloneImage (frame);
          cvRectangle (clone, point1, point2, CV_RGB(0, 255, 0), 1, 8, 0);
          cvShowImage ("frame", clone);
          cvReleaseImage (&clone);
        }
      else
        {
          cvShowImage ("frame", frame);
        }

      key = cvWaitKey (10);
    }

  cvDestroyWindow ("frame");
  cvReleaseCapture (&capture);
  cvStartWindowThread (); //memory leak

  if (selected_roi)
    {
      *roi = cvRect(point1.x, point1.y, point2.x - point1.x, point2.y - point1.y);
      return 0;
    }

  return -1;
}

static double
_se_compare_image (IplImage *img1, IplImage *img2, CvRect *roi)
{
  double correlation;
  CvMat *result = cvCreateMat (1, 1, CV_32F);

  if (roi != NULL)
    {
      cvSetImageROI (img1, *roi);
      cvSetImageROI (img2, *roi);
    }

  cvMatchTemplate (img1, img2, result, CV_TM_CCORR_NORMED);
  cvMinMaxLoc (result, NULL, &correlation, NULL, NULL, NULL);
  cvReleaseMat (&result);

  if (roi != NULL)
    {
      cvResetImageROI (img1);
      cvResetImageROI (img2);
    }

  return correlation;
}

int
se_extract_slides (const char *file, const char *outprefix, CvRect *roi)
{
  int num = 0;
  char str[64];
  int image_properties[3] = {CV_IMWRITE_PNG_COMPRESSION, 9, 0};
  IplImage *current = NULL;
  IplImage *last = NULL;
  CvCapture *capture = cvCaptureFromFile (file);

  if (capture == NULL)
    return -1;

  do
    {
      current = cvQueryFrame (capture);
      if (current == NULL)
        break;

      if (last != NULL)
        {
          double cmp = _se_compare_image (last, current, roi);
          if (cmp <= 0.999)
            {
              snprintf (str, sizeof(str)/sizeof(*str), "%s%d.png", outprefix, num++);
              cvSaveImage (str, last, image_properties);
            }
          cvReleaseImage (&last);
        }
      last = cvCloneImage (current);
    }
  while (current != NULL);

  cvReleaseImage (&last);
  cvReleaseCapture (&capture);

  return 0;
}

