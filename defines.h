//
//  defines.h
//  ascii_art
//
//  Created by Administrator on 14.05.13.
//  Copyright (c) 2013 Kristof Niederholtmeyer. All rights reserved.
//

#ifndef ascii_art_defines_h
#define ascii_art_defines_h

// Block IDs, used in the world representation
#define BLOCK_VOID      	0x00
#define BLOCK_WATER     	0x01
#define BLOCK_DIRT      	0x02
#define BLOCK_SAND      	0x03
#define BLOCK_STONE     	0x04
#define BLOCK_GRASS     	0x05
#define BLOCK_LADDER    	0x06

// Items
#define ITEM_NONE           0x00
#define ITEM_SWORD          0x01
#define ITEM_POTION         0x02

// Color codes
#define COLOR_BLACK     	0x00
#define COLOR_WHITE     	0x01
#define COLOR_DIRT      	0x02
#define COLOR_SAND      	0x03
#define COLOR_STONE     	0x04
#define COLOR_GRASS0     	0x05
#define COLOR_GRASS1     	0x06
#define COLOR_GRASS2     	0x07
#define COLOR_GRASS3     	0x08
#define COLOR_LADDER    	0x09
#define COLOR_WATER     	0x0A
#define COLOR_DEEP_WATER	0x0B
#define COLOR_LIGHT_GRAY    0x0C
#define COLOR_DARK_GRAY     0x0D
#define COLOR_PLAYER0		0x0E
#define COLOR_RED           0x0F
#define COLOR_CHEST         0x10

// Tile IDs, used for visualization
#define TILE_GUY			0x00
#define TILE_GUY_SIDE       0x02
#define TILE_GUY_BACK		0x01
#define TILE_MONSTER		0x20
#define TILE_MONSTER_SIDE   0x22
#define TILE_MONSTER_BACK   0x21
#define TILE_ARROW_UP		0xC0
#define TILE_CHECKER		0x1E
#define TILE_STAIRS			0x1F
#define TILE_WAVE			0x2E
#define TILE_HATCHING		0x4E
#define TILE_NONE           0x03
#define TILE_FULL_HEART     0x0E
#define TILE_EMPTY_HEART    0x0F
#define TILE_CHEST          0x58
#define TILE_UP             0xC2
#define TILE_RIGHT          0xC3

#define TILE_A              0xE0
#define TILE_B              0xE1
#define TILE_C              0xE2
#define TILE_D              0xE3
#define TILE_E              0xE4
#define TILE_F              0xE5
#define TILE_G              0xE6
#define TILE_H              0xE7
#define TILE_I              0xE8
#define TILE_J              0xE9
#define TILE_K              0xEA
#define TILE_L              0xEB
#define TILE_M              0xEC
#define TILE_N              0xED
#define TILE_O              0xEE
#define TILE_P              0xEF
#define TILE_Q              0xF0
#define TILE_R              0xF1
#define TILE_S              0xF2
#define TILE_T              0xF3
#define TILE_U              0xF4
#define TILE_V              0xF5
#define TILE_W              0xF6
#define TILE_X              0xF7
#define TILE_Y              0xF8
#define TILE_Z              0xF9

#define TILE_0              0xD0
#define TILE_1              0xD1
#define TILE_2              0xD2
#define TILE_3              0xD3
#define TILE_4              0xD4
#define TILE_5              0xD5
#define TILE_6              0xD6
#define TILE_7              0xD7
#define TILE_8              0xD8
#define TILE_9              0xD9

#define TILE_PERIOD         0xDA
#define TILE_HYPHEN         0xFB
#define TILE_COLON          0xFF
#define TILE_PLUS           0xFA

// Frame Tiles
#define TILE_FRAME_LR       0x30
#define TILE_FRAME_TB       0x31
#define TILE_FRAME_LRTB     0x32
#define TILE_FRAME_LRB      0x33
#define TILE_FRAME_LTB      0x34
#define TILE_FRAME_RB       0x35
#define TILE_FRAME_L        0x36
#define TILE_FRAME_T        0x37
#define TILE_FRAME_         0x38

// Keyboard Input
#define KEY_UP				0x00
#define KEY_DOWN			0x01
#define KEY_LEFT			0x02
#define KEY_RIGHT			0x03
#define KEY_W 				0x04
#define KEY_S				0x05
#define KEY_A 				0x06
#define KEY_D 				0x07
#define KEY_PERIOD 			0x08
#define KEY_COMMA 			0x09
#define KEY_I               0x0A
#define KEY_SPACE           0x0B

// Networking
#define NET_PORT                42000
#define NET_BUFFER_SIZE         512

// Virtual Connection
#define NET_PROTOCOL_ID      0x42424242
// 4Byte PROTOCOL_ID, 2Byte SEQ, 2Byte ACK, 4Byte ACK bitfield
#define NET_HEADER_SIZE      (4 + 2 + 2 + 4)
#define NET_MAX_PACKET_SIZE  (NET_HEADER_SIZE + NET_BUFFER_SIZE)
// Maxmium distance between two sequence numbers
#define NET_MAX_SEQ_DISTANCE 0xFFFE // = 2^16-1 = 65.534

// Debugging
#define DEBUG

#endif
