#ifndef USER_PROTOCOL_FRAME_PARSER_H_
#define USER_PROTOCOL_FRAME_PARSER_H_

#include <stdint.h>
#include <debug.h>

void parseFrame(uint8_t* inData, uint32_t inLenData, uint8_t* outData, uint32_t* outLenData);

#endif /* USER_PROTOCOL_FRAME_PARSER_H_ */
