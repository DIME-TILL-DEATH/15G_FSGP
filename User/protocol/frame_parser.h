#ifndef USER_PROTOCOL_FRAME_PARSER_H_
#define USER_PROTOCOL_FRAME_PARSER_H_

#include <stdint.h>
#include <debug.h>

#define NUM_PROTOCOL_BYTES 112

void parseFrame(const uint8_t* inData, uint32_t inLenData, uint8_t* outData, uint32_t* outLenData);
void getFdkPayload(uint8_t* data_ptr, uint16_t* dataLen_ptr);

extern uint8_t framesCounter;

#endif /* USER_PROTOCOL_FRAME_PARSER_H_ */
