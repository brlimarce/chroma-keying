# This ImageLab version will compile in Linux. 
#
# The main program should have an extension .cpp
# To compile a main program, for example gray1.cpp, and create the output file gray1.out
#       make gray1.out  <Enter>
#
# To run the output file
#       ./gray1.out  <Enter>
#
# The functions called in the main programs can be found in the .h files
#      image.h, binary.h, color.h, filter.h, tools.h and jpegio.h
#

# the compiler
CC = g++

# the object files
OBJS = binary.o color.o filter.o tools.o jpegio.o

.SUFFIXES : .c .cpp .o .out

#  This is the Linux version of the JPEG library
JPEGLIB = libjpeg.a

# this will compile the object files (*.o) from the C files (*.c)
.c.o :
	$(CC) -c $<

# this will compile the main program (.out) from the main program file (*.cpp)
.cpp.out : $(OBJS)
	make $(OBJS)
	$(CC) -o $@ $< $(OBJS) $(JPEGLIB)

# clean everything
clean:
	rm -f *.out
	rm -f *.o

forms15: $(OBJS)
	$(CC) -c forms.c
	$(CC) -o $@ form15.cpp $(OBJS) forms.o $(JPEGLIB)

#	cp forms15 /media/DATA-394GB/ovci-set
#	chmod 777 /media/DATA-394GB/ovci-set/forms15 

runseeds1:
	./seed4.out images/seed1/caloy2.jpg seed-measure.csv

runseeds2:
	mv seed-measure.csv seed-measure.old
	./seed4.out images/grc/cropped/IRGC-8595.jpg seed-measure.csv
	./seed4.out images/grc/cropped/IRGC-6533.jpg seed-measure.csv
	./seed4.out images/grc/cropped/IRGC-3959.jpg seed-measure.csv
	./seed4.out images/grc/cropped/IRGC-2340.jpg seed-measure.csv
	./seed4.out images/grc/cropped/IRGC-12434.jpg seed-measure.csv
	./seed4.out images/grc/cropped/IRGC-12485.jpg seed-measure.csv
	./seed4.out images/grc/cropped/IRGC-3905.jpg seed-measure.csv
	./seed4.out images/grc/cropped/IRGC-615.jpg seed-measure.csv
	./seed4.out images/grc/cropped/IRGC-3122.jpg seed-measure.csv
	./seed4.out images/grc/cropped/IRGC-75750.jpg seed-measure.csv
	./seed4.out images/grc/cropped/IRGC-75275.jpg seed-measure.csv
	./seed4.out images/grc/cropped/IRGC-74916.jpg seed-measure.csv
	./seed4.out images/grc/cropped/IRGC-81691.jpg seed-measure.csv
	./seed4.out images/grc/cropped/IRGC-87383.jpg seed-measure.csv
	./seed4.out images/grc/cropped/IRGC-86929.jpg seed-measure.csv
	./seed4.out images/grc/cropped/IRGC-86937.jpg seed-measure.csv
	./seed4.out images/grc/cropped/IRGC-89910.jpg seed-measure.csv
	./seed4.out images/grc/cropped/IRGC-87772.jpg seed-measure.csv

runfolder:
	./processfolder1.out images/grc/cropped
	mv images/grc/cropped/*.jpg.annotated.jpg images/grc/annotated

