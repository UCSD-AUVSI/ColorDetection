#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <queue>
#include <cstdlib>
#include <limits>

using namespace cv;

struct Pix {
    Pix() {count = 0;}
    Pix(Vec3b col) {color = col; count = 1;}
    Vec3b color;
    int count;
};

struct Color {
    int red;
    int green;
    int blue;
    std::string name;
};

double mmax(double d1, double d2, double d3) {
    if(d1 > d2 && d1 > d3) return d1;
    if(d2 > d1 && d2 > d3) return d2;
    if(d3 > d1 && d3 > d2) return d3;
    if(d3 == d1 && d3 == d2) return d3;
}

double mmin(double d1, double d2, double d3) {
    if(d1 < d2 && d1 < d3) return d1;
    if(d2 < d1 && d2 < d3) return d2;
    if(d3 < d1 && d3 < d2) return d3;
    if(d3 == d1 && d3 == d2) return d3;
}

void toHSV(double red, double green, double blue, double& hue, double& sat, double& light) {
    double min, max, delta;
    
    min = mmin(red, green, blue);
    max = mmax(red, green, blue);
    delta = max - min;

    light = (max + min)/2;

    if(delta != 0)
        sat = delta/max;
    else {
        sat = 0;
        hue = -1;
        return;
    }

    if(red == max)
        hue = (green-blue)/delta;
    else if(green == max)
        hue = 2+(blue-red)/delta;
    else 
        hue = 4+(red-green)/delta;

    hue *= 60;
    if(hue < 0)
        hue += 360;
}

void setColorDict(std::vector<Color>& colorDict) {
    Color c;

    c.red = 255; c.green = 255; c.blue = 255; c.name = "white";
    colorDict.push_back(c);
    c.red = 255; c.green = 0; c.blue = 0; c.name = "red";
    colorDict.push_back(c);
    c.red = 0; c.green = 255; c.blue = 0; c.name = "lime";
    colorDict.push_back(c);
    c.red = 0; c.green = 0; c.blue = 255; c.name = "blue";
    colorDict.push_back(c);
    c.red = 0; c.green = 0; c.blue = 0; c.name = "black";
    colorDict.push_back(c);
    c.red = 255; c.green = 255; c.blue = 0; c.name = "yellow";
    colorDict.push_back(c);
    c.red = 0; c.green = 255; c.blue = 255; c.name = "cyan";
    colorDict.push_back(c);
    c.red = 255; c.green = 0; c.blue = 255; c.name = "magenta";
    colorDict.push_back(c);
    c.red = 192; c.green = 192; c.blue = 192; c.name = "silver";
    colorDict.push_back(c);
    c.red = 128; c.green = 128; c.blue = 128; c.name = "gray";
    colorDict.push_back(c);
    c.red = 128; c.green = 0; c.blue = 0; c.name = "maroon";
    colorDict.push_back(c);
    c.red = 128; c.green = 128; c.blue = 0; c.name = "olive";
    colorDict.push_back(c);
    c.red = 0; c.green = 128; c.blue = 0; c.name = "green";
    colorDict.push_back(c);
    c.red = 128; c.green = 0; c.blue = 128; c.name = "purple";
    colorDict.push_back(c);
    c.red = 0; c.green = 128; c.blue = 128; c.name = "teal";
    colorDict.push_back(c);
    c.red = 0; c.green = 0; c.blue = 128; c.name = "navy";
    colorDict.push_back(c);
    c.red = 255; c.green = 165; c.blue = 0; c.name = "orange";
    colorDict.push_back(c);
}

int main(int argc, char** argv) {
    Mat image = imread(argv[1], 1);

    if(argc != 2 || !image.data) {
        printf("No image Pix \n");
        return -1;
    }

    std::vector<Color> colorDict;
    setColorDict(colorDict);

    std::cout << "there are " << colorDict.size() << " colors in the dictionary" << std::endl;

    vector<Pix> colors;
    bool found = false;
    for(int i = 0; i < image.size().height; i++) {
        for(int j = 0; j < image.size().width; j++) {
            Vec3b current = image.at<Vec3b>(i, j);

            for(int k = 0; k < colors.size(); k++) {
                if(colors[k].color == current) {
                    colors[k].count++;
                    found = true;
                }
            }

            if(!found) {
                colors.push_back(Pix(current));
            }

            found = false;
        }
    }

    unsigned int bsum = 0;
    unsigned int rsum = 0;
    unsigned int gsum = 0;
    int count = 0;
    // skip the first element since it is the background
    // don't want to include background color in average
    for(int i = 1; i < colors.size(); i++) {
        bsum += colors[i].color.val[0];
        gsum += colors[i].color.val[1];
        rsum += colors[i].color.val[2];
        ++count;
    }

    double average_red = rsum/count;
    double average_green = gsum/count;
    double average_blue = bsum/count;
    std::cout << "average: " << average_red << " " << average_green << " " << average_blue << std::endl;

    double ahue, asat, alight;
    toHSV(average_red, average_green, average_blue, ahue, asat, alight);
    double avgA = (ahue, asat, alight)/3;
    double temp = std::numeric_limits<double>::max();
    int index;

    for(int i = 0; i < colorDict.size(); i++) {
        double hue, sat, light;
        int red  = colorDict[i].red;
        int green = colorDict[i].green;
        int blue = colorDict[i].blue;
        toHSV(red, green, blue, hue, sat, light);

        double temp2 = abs(hue-ahue) + abs(sat-asat) + abs(light-alight);

        if(temp2 < temp) {
            temp = temp2;
            index = i;
        }
    }

    std::cout << colorDict[index].name << std::endl;

    namedWindow("Display Image", CV_WINDOW_AUTOSIZE);
    imshow("Display Image", image);

    waitKey(0);

    return 0;
}

