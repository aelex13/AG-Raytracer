// Template, major revision 6
// IGAD/NHTV - Jacco Bikker - 2006-2015

#pragma once

namespace Tmpl8 {

#include "emmintrin.h"

#define REDMASK	(0xff0000)
#define GREENMASK (0x00ff00)
#define BLUEMASK (0x0000ff)

#define PALETTE_LEVELS	32

typedef unsigned long Pixel;

inline Pixel AddBlend( Pixel a_Color1, Pixel a_Color2 )
{
	const unsigned int r = (a_Color1 & REDMASK) + (a_Color2 & REDMASK);
	const unsigned int g = (a_Color1 & GREENMASK) + (a_Color2 & GREENMASK);
	const unsigned int b = (a_Color1 & BLUEMASK) + (a_Color2 & BLUEMASK);
	const unsigned r1 = (r & REDMASK) | (REDMASK * (r >> 24));
	const unsigned g1 = (g & GREENMASK) | (GREENMASK * (g >> 16));
	const unsigned b1 = (b & BLUEMASK) | (BLUEMASK * (b >> 8));
	return (r1 + g1 + b1);
}

// subtractive blending
inline Pixel SubBlend( Pixel a_Color1, Pixel a_Color2 )
{
	int red = (a_Color1 & REDMASK) - (a_Color2 & REDMASK);
	int green = (a_Color1 & GREENMASK) - (a_Color2 & GREENMASK);
	int blue = (a_Color1 & BLUEMASK) - (a_Color2 & BLUEMASK);
	if (red < 0) red = 0;
	if (green < 0) green = 0;
	if (blue < 0) blue = 0;
	return (Pixel)(red + green + blue);
}

class Surface8
{
public:
	Surface8( char* a_File );
	~Surface8();
	unsigned char* GetBuffer() { return m_Buffer; }
	Pixel* GetPalette( int a_Idx ) { return palette[a_Idx]; }
	int GetWidth() { return m_Width; }
	int GetHeight() { return m_Height; }
	void LoadImage( char* a_File );
private:
	unsigned char* m_Buffer;
	Pixel* palette[PALETTE_LEVELS];
	int m_Width, m_Height, m_Pitch;	
};

class Surface
{
	enum
	{
		OWNER = 1
	};

public:
	// constructor / destructor
	Surface( int a_Width, int a_Height, Pixel* a_Buffer, int a_Pitch );
	Surface( int a_Width, int a_Height );
	Surface( char* a_File );
	~Surface();
	// member data access
	Pixel* GetBuffer() { return m_Buffer; }
	void SetBuffer( Pixel* a_Buffer ) { m_Buffer = a_Buffer; }
	int GetWidth() { return m_Width; }
	int GetHeight() { return m_Height; }
	int GetPitch() { return m_Pitch; }
	void SetPitch( int a_Pitch ) { m_Pitch = a_Pitch; }
	void ParseHeader( unsigned char* a_Header );
	// Special operations
	void InitCharset();
	void SetChar( int c, char* c1, char* c2, char* c3, char* c4, char* c5 );
	void Centre( char* a_String, int y1, Pixel color );
	void Print( char* a_String, int x1, int y1, Pixel color );
	void Clear( Pixel a_Color );
	void Line( float x1, float y1, float x2, float y2, Pixel color );
	void Plot( int x, int y, Pixel c );
	void LoadImage( char* a_File );
	void CopyTo( Surface* a_Dst, int a_X, int a_Y );
	void BlendCopyTo( Surface* a_Dst, int a_X, int a_Y );
	void ScaleColor( unsigned int a_Scale );
	void Box( int x1, int y1, int x2, int y2, Pixel color );
	void Bar( int x1, int y1, int x2, int y2, Pixel color );
	void Resize( Surface* a_Orig );
private:
	// Attributes
	Pixel* m_Buffer;	
	int m_Width, m_Height, m_Pitch;	
	// Static attributes for the buildin font
	char s_Font[51][5][5];	
	int s_Transl[256];		
};

class Sprite
{
public:
	// Sprite flags
	enum
	{
		FLARE		= (1<< 0),
		OPFLARE		= (1<< 1),	
		FLASH		= (1<< 4),	
		DISABLED	= (1<< 6),	
		GMUL		= (1<< 7),
		BLACKFLARE	= (1<< 8),	
		BRIGHTEST   = (1<< 9),
		RFLARE		= (1<<12),
		GFLARE		= (1<<13),
		NOCLIP		= (1<<14)
	};
	// Structors
	Sprite( Surface* a_Surface, unsigned int a_NumFrames );
	~Sprite();
	// Methods
	void Draw( int a_X, int a_Y, Surface* a_Target = 0 );
	void DrawScaled( int a_X, int a_Y, int a_Width, int a_Height, Surface* a_Target );
	void SetFlags( unsigned int a_Flags ) { m_Flags = a_Flags; }
	void SetFrame( unsigned int a_Index ) { m_CurrentFrame = a_Index; }
	unsigned int GetFlags() const { return m_Flags; }
	int GetWidth() { return m_Width; }
	int GetHeight() { return m_Height; }
	Pixel* GetBuffer() { return m_Surface->GetBuffer(); }	
	unsigned int Frames() { return m_NumFrames; }
	Surface* GetSurface() { return m_Surface; }
private:
	// Methods
	void InitializeStartData();
	// Attributes
	int m_Width, m_Height, m_Pitch;
	unsigned int m_NumFrames;          
	unsigned int m_CurrentFrame;       
	unsigned int m_Flags;
	unsigned int** m_Start;
	Surface* m_Surface;
};

}; // namespace Tmpl8