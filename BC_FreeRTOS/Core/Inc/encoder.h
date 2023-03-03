//
// Created by A9201 on 2023/2/28.
//

#ifndef BC_FREERTOS_ENCODER_H
#define BC_FREERTOS_ENCODER_H

typedef struct {
    int encoder_left;
    int encoder_right;
}encoder;

void EncoderInit(void);
int GetTim2Encoder(void);
int GetTim3Encoder(void);

#endif //BC_FREERTOS_ENCODER_H
