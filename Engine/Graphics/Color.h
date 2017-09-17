/*
	This file defines a struct for Color to be used for clearing back buffer
*/

// Include Files
//==============

// Interface
//==========

#ifndef EAE6320_COLOR_H
#define EAE6320_COLOR_H

namespace eae6320
{
	namespace Graphics
	{
		class Color
		{
		public:
			// This constructor is for when you want to call predefined functions
			// below without defining the initial Color data.
			Color()
			{
				colorR = 0.0f;
				colorG = 0.0f;
				colorB = 0.0f;
				colorA = 0.0f;
			}

			// This constructor is for when you know which exact color you want
			// with corresponding RGBA values.
			Color(const float r, const float g, const float b, const float a)
			{
				if (isInRange(r))
					colorR = r;
				else
					colorR = 0.0f;

				if (isInRange(g))
					colorG = g;
				else
					colorG = 0.0f;

				if (isInRange(b))
					colorB = b;
				else
					colorB = 0.0f;

				if (isInRange(a))
					colorA = a;
				else
					colorA = 0.0f;
			}

			// Those functions below return individual components of the current color
			float R() const
			{
				return colorR;
			}

			float G() const
			{
				return colorG;
			}

			float B() const
			{
				return colorB;
			}

			float A() const
			{
				return colorA;
			}

			// Those functions below set individual components to given values
			void R(const float r)
			{
				if (isInRange(r))
					colorR = r;
				else
					colorR = 0.0f;
			}

			void G(const float g)
			{
				if (isInRange(g))
					colorG = g;
				else
					colorG = 0.0f;
			}

			void B(const float b)
			{
				if (isInRange(b))
					colorB = b;
				else
					colorB = 0.0f;
			}

			void A(const float a)
			{
				if (isInRange(a))
					colorA = a;
				else
					colorA = 0.0f;
			}

			// Those functions below return Color instances that represent most common colors
			Color Black() const
			{
				return Color(0.0f, 0.0f, 0.0f, 1.0f);
			}

			Color White() const
			{
				return Color(1.0f, 1.0f, 1.0f, 1.0f);
			}

			Color Red() const
			{
				return Color(1.0f, 0.0f, 0.0f, 1.0f);
			}

			Color Green() const
			{
				return Color(0.0f, 1.0f, 0.0f, 1.0f);
			}

			Color Blue() const
			{
				return Color(0.0f, 0.0f, 1.0f, 1.0f);
			}

			Color Yellow() const
			{
				return Color(1.0f, 1.0f, 0.0f, 1.0f);
			}

			Color Cyan() const
			{
				return Color(0.0f, 1.0f, 1.0f, 1.0f);
			}

			Color Magenta() const
			{
				return Color(1.0f, 0.0f, 1.0f, 1.0f);
			}

			bool isInRange(const float component) const
			{
				return ((component >= 0.0f) && (component <= 1.0f));
			}

		private:
			float colorR, colorG, colorB, colorA;
		};
	}
}

#endif
