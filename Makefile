all: run convert

run:
	g++ -Dx_pos=5.0 -I include main.cpp -Ofast
	a.exe > image.ppm     

convert:
	magick image.ppm image.png