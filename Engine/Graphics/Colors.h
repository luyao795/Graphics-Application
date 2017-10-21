/*
This file defines a struct for Color to be used for clearing back buffer
*/

// Include Files
//==============

#include "Color.h"

// Interface
//==========

#ifndef EAE6320_COLORS_H
#define EAE6320_COLORS_H

namespace eae6320
{
	namespace Graphics
	{
		class Colors
		{

		public:
			// These color components will range [0, 1]
			static const Color Red;
			static const Color Green;
			static const Color Blue;
			static const Color White;
			static const Color Black;
			static const Color Yellow;
			static const Color Cyan;
			static const Color Magenta;
			static const Color Gray;

			// These color components will range [0, 255]
			static const RawColor RawRed;
			static const RawColor RawGreen;
			static const RawColor RawBlue;
			static const RawColor RawWhite;
			static const RawColor RawBlack;
			static const RawColor RawYellow;
			static const RawColor RawCyan;
			static const RawColor RawMagenta;
			static const RawColor RawGray;
		};
	}
}

#endif
