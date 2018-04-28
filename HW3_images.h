#ifndef __HW3_IMAGES_H__
#define __HW3_IMAGES_H__

#include <stdint.h>
#include <stdio.h>


#define       PLANE_HEIGHT 	41	// height of plane image in pixels
#define       PLANE_WIDTH  	24	// width of plane image in pixels
#define				grade_HEIGHT	11	// height of grade image in pixels
#define				grade_WIDTH		8		// width of grade image in pixels

#define				grade_ERASE_HEIGHT	11	// height of grade image in pixels
#define				grade_ERASE_WIDTH		16		// width of grade image in pixels

#define				pointer_HEIGHT	11	// height of grade image in pixels
#define				pointer_WIDTH		8		// width of grade image in pixels


// ==========================================================================
// structure definition
// ==========================================================================

// This structure describes a single character's display information
typedef struct
{
	const uint8_t widthBits;					// width, in bits (or pixels), of the character
	const uint16_t offset;					// offset of the character's bitmap, in bytes, into the the FONT_INFO's data array
	
} FONT_CHAR_INFO;	

// Describes a single font
typedef struct
{
	const uint8_t 			heightPages;	// height, in pages (8 pixels), of the font's characters
	const uint8_t 			startChar;		// the first character in the font (e.g. in charInfo and data)
	const uint8_t 			endChar;		// the last character in the font
	const uint8_t			spacePixels;	// number of pixels that a space character takes up
	const FONT_CHAR_INFO*	charInfo;		// pointer to array of char information
	const uint8_t*			data;			// pointer to generated array of character visual representation
		
} FONT_INFO;	


extern const uint8_t planeBitmap[];
extern const uint8_t planeBitmap2[];
extern const uint8_t gradeBitmap[];
extern const uint8_t gradeBitmapA[];
extern const uint8_t gradeBitmapF[];
extern const uint8_t triangleBitmap[];
extern const uint8_t triangleClear[];

extern const uint8_t gradeErase[];

extern const uint8_t microsoftSansSerif_8ptBitmaps[];
extern const FONT_INFO microsoftSansSerif_8ptFontInfo;
extern const FONT_CHAR_INFO microsoftSansSerif_8ptDescriptors[];


#endif