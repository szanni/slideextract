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
#include <stdlib.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

using namespace cv;

static bool selected_roi = 0;
static Point point1;
static Point point2;

static void
_se_mouseHandler(int event, int x, int y, int flags, void *param)
{
	Mat clone;
	Mat frame = *(Mat*)param;

	if (event == EVENT_LBUTTONDOWN && (flags & EVENT_FLAG_LBUTTON)) {
		point1 = Point (x, y);
		point2 = Point (x, y);
	}

	if (!(flags & EVENT_FLAG_LBUTTON))
		return;

	if (event != EVENT_MOUSEMOVE && event != EVENT_LBUTTONUP)
		return;

	point2 = Point(x, y);
	selected_roi = (point1.x != point2.x && point1.y != point2.y);

	clone = frame.clone();
	rectangle(clone, point1, point2, CV_RGB(0, 255, 0), 1, 8, 0);
	imshow("frame", clone);
}

int
se_select_roi(const char *file, struct roi *roi)
{
	VideoCapture capture = VideoCapture(file, CAP_ANY);

	if (!capture.isOpened())
		return -1;

	int key = -1;
	Mat frame;
	Mat clone;
	namedWindow("frame", WINDOW_AUTOSIZE);

	while (key == -1)
	{
		capture.read(frame);
		if (frame.empty())
			break;

		setMouseCallback("frame", _se_mouseHandler, &frame);

		if (selected_roi)
		{
			clone = frame.clone();
			rectangle(clone, point1, point2, CV_RGB(0, 255, 0), 1, 8, 0);
			imshow("frame", clone);
		}
		else
		{
			imshow("frame", frame);
		}

		key = waitKey (10);
	}

	startWindowThread();

	destroyAllWindows();

	if (selected_roi)
	{
		roi->x = (point1.x < point2.x) ? point1.x : point2.x;
		roi->y = (point1.y < point2.y) ? point1.y : point2.y;
		roi->width = abs(point2.x - point1.x);
		roi->height = abs(point2.y - point1.y);
		return 0;
	}

	return -1;
}

static double
_se_compare_image(Mat &img1, Mat &img2, struct roi *roi)
{
	double correlation;
	Mat result = Mat(1, 1, CV_32F);
	Mat i1, i2;

	if (roi != NULL) {
		Rect r = Rect(roi->x, roi->y, roi->width, roi->height);
		i1 = img1.clone()(r);
		i2 = img2.clone()(r);
	}
	else {
		i1 = img1;
		i2 = img2;
	}

	matchTemplate(i1, i2, result, TM_CCORR_NORMED);
	minMaxLoc(result, NULL, &correlation);

	return correlation;
}

int
se_extract_slides(const char *file, const char *outprefix, struct roi *roi)
{
	int num = 0;
	char str[64];

	std::vector<int> image_properties;
	image_properties.push_back(IMWRITE_PNG_COMPRESSION);
	image_properties.push_back(9);
	image_properties.push_back(0);

	Mat current;
	Mat last;

	VideoCapture capture = VideoCapture(file, CAP_ANY);
	if (!capture.isOpened())
		return -1;

	do {
		capture.read(current);

		bool is_final_frame = current.empty();
		bool is_changed_frame = !current.empty() && !last.empty()
			&& _se_compare_image (last, current, roi) <= 0.999;
                
		if (is_final_frame || is_changed_frame) {
			snprintf (str, sizeof(str)/sizeof(*str), "%s%d.png", outprefix, num++);
			imwrite(str, last, image_properties);
		}

		if (is_final_frame)
			break;

		last = current.clone();
	}
	while (true);

	return 0;
}

