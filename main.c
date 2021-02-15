/* slideextract -- extract slides from video
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
#include <cargs.h>
#include <stdbool.h>

//! Print version and exit program
static void
version ()
{
	puts("slideextract 0.1.0 \n\
Copyright (c) 2013 Angelo Haller <angelo@szanni.org>\n\
All rights reserved.\n\
\n\
Redistribution and use in source and binary forms, with or without\n\
modification, are permitted provided that the following conditions are met:\n\
\n\
1. Redistributions of source code must retain the above copyright notice,\n\
   this list of conditions and the following disclaimer.\n\
2. Redistributions in binary form must reproduce the above copyright notice,\n\
   this list of conditions and the following disclaimer in the documentation\n\
   and/or other materials provided with the distribution.\n\
\n\
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\"\n\
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE\n\
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE\n\
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE\n\
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR\n\
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF\n\
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS\n\
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN\n\
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)\n\
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE\n\
POSSIBILITY OF SUCH DAMAGE.");

	exit (0);
}


/**
 * This is the main configuration of all options available.
 */
static struct cag_option options[] = {
	{.identifier = 'g',
	 .access_letters = "g",
	 .access_name = "gui",
	 .value_name = NULL,
	 .description = "Launch gui to set comparison region, press any key when done"},

	{.identifier = 'r',
	 .access_letters = "r",
	 .access_name = "region",
	 .value_name = "X1.Y1:X2.Y2",
	 .description = "Manually set comparison region (e.g. slide number) for faster and more accurate extraction."},

	{.identifier = 'V',
	 .access_letters = "V",
	 .access_name = "version",
	 .value_name = NULL,
	 .description = "Show version"},

	{.identifier = '?',
	 .access_letters = "h",
	 .access_name = "help",
	 .value_name = NULL,
	 .description = "Display this help and exit"}
};

//! Print help and exit program
static void
help()
{
	puts("Usage: slideextract [OPTION] infile outprefix\n Extract slides from video.");
	cag_option_print(options, CAG_ARRAY_SIZE(options), stdout);
	exit (0);
}

int
main (int argc, char **argv)
{
	struct roi roi;
	bool gflag = 0;
	bool rflag = 0;
	int ret;
	cag_option_context context;

	cag_option_prepare(&context, options, CAG_ARRAY_SIZE(options), argc, argv);
	while (cag_option_fetch(&context)) {
		switch (cag_option_get(&context)) {
			case 'g':
				gflag = 1;
				break;

			case 'r':
				rflag = 1;
				if (sscanf(cag_option_get_value(&context), "%d.%d:%d.%d", &roi.x, &roi.y, &roi.width, &roi.height) != 4)
					help();
				break;

			case 'V':
				version();
				break;

			case '?':
			default:
				help();
		}
	}
	int optind = cag_option_get_index(&context);
	argc -= optind;
	argv += optind;

	if (argc != 2 || (gflag && rflag))
		help();

	const char *file = *argv++;
	const char *outprefix = *argv;

	if (rflag)
		return se_extract_slides(file, outprefix, &roi);

	if (gflag)
	{
		if ((ret = se_select_roi(file, &roi)))
			return ret;

		printf("Selected ROI: %d.%d:%d.%d\n", roi.x, roi.y, roi.width, roi.height);
		return se_extract_slides(file, outprefix, &roi);
	}

	return se_extract_slides(file, outprefix, NULL);
}
