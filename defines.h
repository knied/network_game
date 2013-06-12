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

// Tile IDs, used for visualization
#define TILE_GUY			0x00
#define TILE_ARROW_UP		0xC0
#define TILE_CHECKER		0x1E
#define TILE_STAIRS			0x1F
#define TILE_WAVE			0x2E
#define TILE_HATCHING		0x4E

// Networking
#define PORT                42001
#define BUFFER_SIZE         512

// Debugging
#define DEBUG

#endif
