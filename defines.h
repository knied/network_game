//
//  defines.h
//  ascii_art
//
//  Created by Administrator on 14.05.13.
//  Copyright (c) 2013 Kristof Niederholtmeyer. All rights reserved.
//

#ifndef ascii_art_defines_h
#define ascii_art_defines_h

// World Tiles
#define TILE_MASK		0x1F

#define TILE_VOID 		0x00
#define TILE_WATER 		0x01
#define TILE_DEEP_WATER 0x02 // for visual effects only
#define TILE_DIRT 		0x03
#define TILE_SAND 		0x04
#define TILE_STONE 		0x05
#define TILE_GRASS0 	0x06
#define TILE_GRASS1 	0x07
#define TILE_GRASS2 	0x08
#define TILE_GRASS3 	0x09
#define TILE_LADDER 	0x0A
//						0x0B
//						0x0C
//						0x0D
//						0x0E
//						0x0F

//						0x10
//						0x11
//						0x12
//						0x13
//						0x14
//						0x15
//						0x16
//						0x17
//						0x18
//						0x19
//						0x1A
//						0x1B
//						0x1C
//						0x1D
//						0x1E
//						0x1F

// Navigation Layer
#define NAV_MASK		0xE0

#define NAV_LEVEL		0x00
#define NAV_UP 			0x20
#define NAV_DOWN 		0x40
#define NAV_FALL 		0x60
#define NAV_BLOCKED 	0x80
#define NAV_UNKNOWN 	0xA0
//						0xC0
//						0xE0

#endif
