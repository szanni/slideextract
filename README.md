slideextract
============

slideextract is a program to extract slides from videos.
It outputs an image for each detected slide. Slide detection is done by
comparing consecutive frames for differences within a specific threshold,
to account for video encoding artifacts.

For better results and faster extraction you should specify a comparison
region, such as the slide number. From a series of unchanging comparison
regions slideextract will pick the last frame in hopes of capturing the
final state of the entire slide (animated bullet points, annotations,
etc...).

    # Select a comparision region via gui
    slideextract -g video.mp4 outprefix

Once extraction is completed you can view the extracted slides with your
favorite image viewer and delete possible duplicates. This often happens
when the video jumps back and forth between slides.

You can then combine the slides to a pdf with a tool like convert:

    convert `ls -v outprefix*.png` outprefix.pdf


Build Instructions
------------------

    # build directory
    mkdir build

    # Install dependencies
    cd build
    conan install .. --build=missing
    cd ..

    # Configure build
    cmake -S . -B ./build

    # Build
    cmake --build ./build


License
-------

BSD 2-Clause license, see LICENSE for more details.
