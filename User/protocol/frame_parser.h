#ifndef USER_PROTOCOL_FRAME_PARSER_H_
#define USER_PROTOCOL_FRAME_PARSER_H_

#include <stdint.h>
#include <stdbool.h>

#include <debug.h>

void parseFrame(const uint8_t* inData, uint32_t inLenData, uint8_t* outData, uint32_t* outLenData);
void getFdkFramePayload(uint8_t* data_ptr, uint16_t* dataLen_ptr);
void getRdyFramePayload(uint8_t* data_ptr, uint16_t* dataLen_ptr);


#endif /* USER_PROTOCOL_FRAME_PARSER_H_ */
