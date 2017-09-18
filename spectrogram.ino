/*
Author: Roberto Del Valle Rodriguez
Date: August 12, 2017
Description: Arduino Spectrogram for Arduino UNO
             based on project by Chris McClanahan
             http://mcclanahoochie.com/blog/portfolio/music-visualization-with-an-arduino
*/

#include <fix_fft.h>
#include <openGLCD.h>
#include "Arial14.h"
#include "SystemFont5x7.h"

char imaginary_array[128], real_array[128], previousRun[64];
char offset = 30, heightLimit = 60;
int k = 0, datapoint = 0, j = 0;
char toggle = 0;
const int max_columns = 12;
const int max_rows = 64;
const int width = max_columns;
const int height = max_rows;
int column = 0;
int spectrogram_edge = 0;
int spectrogram[max_columns][max_rows];
int threshold = 2;

void setup() {
    GLCD.Init(NON_INVERTED);
    analogReference(DEFAULT);     
    GLCD.SelectFont(System5x7);   
    GLCD.ClearScreen();
    countdown(4);
    GLCD.ClearScreen();
    for (int l = 0; l < 64; l++) {
        previousRun[l] = 0;
    };
};

void countdown(int counter) {
    while (counter-- > 0) {
        GLCD.CursorTo(2, 2);
        GLCD.PutChar(counter + '0');
        delay(1000);
    }
}

void loop() {
    if (toggle == 0) { GLCD.ClearScreen(); toggle = 1; }
    else { toggle = toggle - 1; }

    for (k = 0; k < 128; k++) {
        datapoint = analogRead(5);
        real_array[k] = datapoint / 4 - 128;
        imaginary_array[k] = 0;
    };

    fix_fft(real_array, imaginary_array, 7, 0);

    char absolute_value;
    for (k = 1; k < max_rows; k++) {
        absolute_value = sqrt(real_array[k] * real_array[k] + imaginary_array[k] * imaginary_array[k]) * 1.33;
        if(absolute_value < threshold>>1) absolute_value = 0;
        if(absolute_value >= heightLimit) real_array[k] = heightLimit - 1;
        GLCD.DrawLine(k + offset, previousRun[k], k + offset, heightLimit, BLACK);
        GLCD.DrawLine(k + offset, heightLimit, k + offset, heightLimit - absolute_value, BLACK);
        real_array[k] = absolute_value;
        previousRun[k] = heightLimit - absolute_value;
    };

    for (k = 0; k < max_rows; k++) {
        spectrogram[column][k] = real_array[k];
    };

    column = column + 1;
    if (column == max_columns) { column = 0; }

    for (k = 0; k < max_columns - spectrogram_edge; k++) {
        for (j = 0; j < max_rows; ++j) {
            if (spectrogram[k + spectrogram_edge][j] > threshold) {
                GLCD.SetDot(k, height - j, BLACK);
            }
        }
    }

    for (k = 0; k < spectrogram_edge; k++) {
        for (j = 0; j < max_rows; ++j) {
            if (spectrogram[k][j] > threshold) {
                GLCD.SetDot(k + max_columns - spectrogram_edge, height - j, BLACK);
            }
        }
    }

    spectrogram_edge = spectrogram_edge + 1;
    if (spectrogram_edge == max_columns) { spectrogram_edge = 0; }

};