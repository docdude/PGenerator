/*
 * Copyright (c) 2021-2022 Juan Francisco Loya
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * See the File README and COPYING for more detail about License
 *
*/


#pragma once

class RGB
{
public:
	int R;
	int G;
	int B;

	RGB(int r, int g, int b)
	{
		R = r;// * 0.8588235 + 16.0;
		G = g;// * 0.8588235 + 16.0;
		B = b;// * 0.8588235 + 16.0;
//		R = r * 0.856305 + 16.0 << (bits - 8);
//		G = g * 0.856305 + 16.0 << (bits - 8);
//		B = b * 0.856305 + 16.0 << (bits - 8);
	}

	bool Equals(RGB rgb)
	{
		return (R == rgb.R) && (G == rgb.G) && (B == rgb.B);
	}
};

class YCbCr
{
public:
	float Y;
	float Cb;
	float Cr;


	YCbCr(float y, float cb, float cr)
	{
		Y = y;
		Cb = cb;
		Cr = cr;
	}

	bool Equals(YCbCr ycbcr)
	{
		return (Y == ycbcr.Y) && (Cb == ycbcr.Cb) && (Cr == ycbcr.Cr);
	}
};

static YCbCr RGB2YCbCr(RGB rgb, int bits, int colorimetry) {
	float primaries[2][3] =
	{
	{ 0.2126, 0.7152, 0.0722}, 
	{ 0.2627, 0.6780, 0.0593}, 
	};
	int offset1 = 224 << (bits - 8);
	int offset2 = 219 << (bits - 8);
	int offset3 = 128 << (bits - 8);
	int R, G, B;
	int idx;
	float d, e;
	
	R = rgb.R * ((pow(2,(8+(bits-8))) - 1) / (pow(2,8) - 1));  //x10 = (2^10 - 1) / (2^8 - 1) * x8, where x8 and x10 are 8 and 10 bit values respectively.
	G = rgb.G * ((pow(2,(8+(bits-8))) - 1) / (pow(2,8) - 1));
	B = rgb.B * ((pow(2,(8+(bits-8))) - 1) / (pow(2,8) - 1));	
//	if (bits == 8) {
//		R = R * 0.85588235 + 16;
//		G = G * 0.85588235 + 16;
//		B = B * 0.85588235 + 16;
//	} 
//	if (bits == 10) {
//		R = R * 0.856305 + 64;
//		G = G * 0.856305 + 64;
//		B = B * 0.856305 + 64;
//	}	
	if (colorimetry == 2) {
		idx = 0;
		d = 1.8556;
		e = 1.5748;
	}	
	if (colorimetry == 9) {
		idx = 1;
		d = 1.8814;
		e = 1.4746;
	}
	int Y = std::round((primaries[idx][0] * R + primaries[idx][1]* G + primaries[idx][2] * B));
	int Cb = std::round(((-primaries[idx][0]/d) * R - (primaries[idx][1]/d) * G + ((d/2)/d) * B)*offset1/offset2 + offset3); // Chrominance Blue
	int Cr = std::round((((e/2)/e) * R - (primaries[idx][1]/e) * G - (primaries[idx][2]/e) * B)*offset1/offset2 + offset3); // Chrominance Red

	return YCbCr(Y, Cb, Cr); 
}

static YCbCr RGB2YCbCr_2020(RGB rgb, int bits) {
	int offset1 = 224 << (bits - 8);
	int offset2 = 219 << (bits - 8);
	int offset3 = 128 << (bits - 8);
	int Y = (0.2627 * rgb.R + 0.6780 * rgb.G + 0.0593 * rgb.B);
	int Cb = ((-0.2627/1.8814) * rgb.R - (0.6780/1.8814) * rgb.G + (0.9278/1.814) * rgb.B)*offset1/offset2 + offset3; // Chrominance Blue
	int Cr = ((0.7373/1.4746) * rgb.R - (0.6780/1.4746) * rgb.G - (0.0593/1.4746) * rgb.B)*offset1/offset2 + offset3; // Chrominance Red

	return YCbCr(Y, Cb, Cr); 
}
