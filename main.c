#include <stdio.h>
#include <math.h>
#include <conio.h>
#include <stdlib.h>

#include "model.h"
#include "tga.h"
#define _CRT_SECURE_NO_WARNINGS
void swap(int *a, int *b);
int iabs(int a);

/*
* Using Digital Differential Analyzer algorihm
* to draw interval connecting (x0, y0) with (x1, y1)
* on image using color
*/
void line (tgaImage *image, 
           int x0, int y0,
           int x1, int y1,
           tgaColor color);

void sortCoordsByY(int coords[3][2]) {
    int curMinInd = 0, curMaxInd = 0;
    for (int i = 1; i < 3; i++) {
        if (coords[i][1] < coords[curMinInd][1]) {
            curMinInd = i;
        }
        if (coords[i][1] > coords[curMaxInd][1]) {
            curMaxInd = i;
        }
    }
    if (curMinInd == 0 && curMaxInd == 0) {
        curMaxInd = 2;
    }
    if (curMinInd != 0) {
        int coordsTmp[2];
        coordsTmp[0] = coords[0][0];
        coordsTmp[1] = coords[0][1];
        coords[0][0] = coords[curMinInd][0];
        coords[0][1] = coords[curMinInd][1];
        coords[curMinInd][0] = coordsTmp[0];
        coords[curMinInd][1] = coordsTmp[1];
        if (curMaxInd == 0) {
            curMaxInd = curMinInd;
        }
    }
    if (curMaxInd != 0) {
        int coordsTmp[2];
        coordsTmp[0] = coords[2][0];
        coordsTmp[1] = coords[2][1];
        coords[2][0] = coords[curMaxInd][0];
        coords[2][1] = coords[curMaxInd][1];
        coords[curMaxInd][0] = coordsTmp[0];
        coords[curMaxInd][1] = coordsTmp[1];
    }
}

int getXOfIntersection(int y, int x0, int y0, int x1, int y1) {
    int yDiff = y1 - y0;
    if (yDiff == 0) {
        return x0;
    }
    double k = (y - y0) / (double)yDiff;
    return x0 + k*(x1 - x0);
}

void triangle(int coords[3][2], tgaColor color, tgaImage *image) {
    // ...
    // screen_coords[j][0], screen_coords[j][1] ��� j �� 0 �� 2 - ��� �������� ���������� ������ ���. �����

    // ���������� screen_coords � ������� ����������� "y"
    sortCoordsByY(coords);

    for (int y = coords[0][1]; y <= coords[2][1]; y++) {
        int xA = y > coords[1][1]
            ? getXOfIntersection(y, coords[2][0], coords[2][1], coords[1][0], coords[1][1])
            : getXOfIntersection(y, coords[0][0], coords[0][1], coords[1][0], coords[1][1]);
        int xB = y > coords[2][1]
            ? getXOfIntersection(y, coords[2][0], coords[2][1], coords[1][0], coords[1][1])
            : getXOfIntersection(y, coords[0][0], coords[0][1], coords[2][0], coords[2][1]);
        for (int x = min(xA, xB); x <= max(xA, xB); x++) {
            tgaSetPixel(image, (unsigned int)x, (unsigned int)y, color);
        }
    }
}

void meshgrid(tgaImage *image, Model *model) {
    int i, j;
    for (i = 0; i < model->nface; ++i) {
        int screen_coords[3][2]; // ��������� � �������� ����������
        for (j = 0; j < 3; ++j) {
            Vec3 *v = &(model->vertices[model->faces[i][3 * j]]);
            screen_coords[j][0] = ((*v)[0] + 1) * image->width / 2;
            screen_coords[j][1] = (1 - (*v)[1]) * image->height / 2;
        }

        tgaColor color = tgaRGB(rand() % 256, rand() % 256, rand() % 256);

        triangle(screen_coords, color, image);

        /*
        for (j = 0; j < 3; ++j) {
            line(image, screen_coords[j][0], screen_coords[j][1],
            screen_coords[(j + 1) % 3][0], screen_coords[(j + 1) % 3][1], white);
        }
        */
    }	
}

int main()
{
    srand((unsigned)time(NULL));
	Model *model = loadFromObj("C:\\Users\\nasty\\source\\repos\\Project1\\Project1\\obj\\cat.obj");
  
	int height = 800;
    int width = 800;
    tgaImage * image = tgaNewImage(height, width, RGB);

	meshgrid(image, model);
	tgaSaveToFile(image, "out.tga");
	//tgaFreeImage(image);
	//freeModel(model);
    int i;
    //tgaColor white = tgaRGB(255, 255, 255);
    //tgaColor red = tgaRGB(255, 0, 0);
    //tgaColor blue = tgaRGB(0, 0, 255);

    tgaFreeImage(image);    
	freeModel(model);
    _getch();
    return 0;
}



void line(tgaImage *image, int x0, int y0, int x1, int y1, tgaColor color) {
	char steep = 0;
	if (abs(x0 - x1) < abs(y0 - y1)) {
		swap(&x0, &y0);
		swap(&x1, &y1);
		steep = 1;
	}
	if (x0 > x1) {
		swap(&x0, &x1);
		swap(&y0, &y1);
	}

	int errorAccumulation = 0;
	int deltaError = 2 * abs(y1 - y0);
	int y = y0;
	for (int x = x0; x <= x1; x++) {
		if (steep == 1) {
			tgaSetPixel(image, (unsigned int)y, (unsigned int)x, color);
		}
		else {
			tgaSetPixel(image, (unsigned int)x, (unsigned int)y, color);
		}
		
		errorAccumulation += deltaError;

		if (errorAccumulation > (x1 - x0)) {
			y += (y1 > y0 ? 1 : -1);
			errorAccumulation -= 2 * (x1 - x0);
		}
	}
}

void swap(int *a, int *b) {
    int t = *a;
    *a = *b;
    *b = t;
}

int iabs(int a) {
    return (a >= 0) ? a : -a;
}
