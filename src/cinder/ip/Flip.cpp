/*
 Copyright (c) 2010, The Barbarian Group
 All rights reserved.

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and
	the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
	the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/

#include "cinder/ip/Flip.h"

#include <boost/preprocessor/seq.hpp>

using namespace std;

namespace cinder { namespace ip {

template<typename T>
void flipVertical( SurfaceT<T> *surface )
{
	const int32_t rowBytes = surface->getRowBytes();
	unique_ptr<uint8_t[]> buffer( new uint8_t[rowBytes] );
	
	const int32_t lastRow = surface->getHeight() - 1;
	const int32_t halfHeight = surface->getHeight() / 2;
	for( int32_t y = 0; y < halfHeight; ++y ) {
		memcpy( buffer.get(), surface->getData( Vec2i( 0, y ) ), rowBytes );
		memcpy( surface->getData( Vec2i( 0, y ) ), surface->getData( Vec2i( 0, lastRow - y ) ), rowBytes );
		memcpy( surface->getData( Vec2i( 0, lastRow - y ) ), buffer.get(), rowBytes );
	}	
}

namespace { // anonymous
template<typename T>
void flipVerticalRawSameChannelOrder( const SurfaceT<T> &srcSurface, SurfaceT<T> *destSurface, const Vec2i &size )
{
	const int32_t srcPixelInc = srcSurface.getPixelInc();
	const size_t copyBytes = size.x * srcPixelInc * sizeof(T);
	for( int32_t y = 0; y < size.y; ++y ) {
		const T *srcPtr = srcSurface.getData( Vec2i( 0, y ) );
		T *dstPtr = destSurface->getData( Vec2i( 0, size.y - y - 1 ) );
		memcpy( dstPtr, srcPtr, copyBytes );
	}
}

template<typename T>
void flipVerticalRawRgba( const SurfaceT<T> &srcSurface, SurfaceT<T> *destSurface, const Vec2i &size )
{
	const uint8_t srcRed = srcSurface.getChannelOrder().getRedOffset();
	const uint8_t srcGreen = srcSurface.getChannelOrder().getGreenOffset();
	const uint8_t srcBlue = srcSurface.getChannelOrder().getBlueOffset();
	const uint8_t srcAlpha = srcSurface.getChannelOrder().getAlphaOffset();
	
	const uint8_t dstRed = destSurface->getChannelOrder().getRedOffset();
	const uint8_t dstGreen = destSurface->getChannelOrder().getGreenOffset();
	const uint8_t dstBlue = destSurface->getChannelOrder().getBlueOffset();
	const uint8_t dstAlpha = destSurface->getChannelOrder().getAlphaOffset();
	
	for( int32_t y = 0; y < size.y; ++y ) {
		const T *src = srcSurface.getData( Vec2i( 0, y ) );
		T *dst = destSurface->getData( Vec2i( 0, size.y - y - 1 ) );
		for( int x = 0; x < size.x; ++x ) {
			dst[dstRed] = src[srcRed];
			dst[dstGreen] = src[srcGreen];
			dst[dstBlue] = src[srcBlue];
			dst[dstAlpha] = src[srcAlpha];
			src += 4;
			dst += 4;
		}
	}
}

template<typename T>
void flipVerticalRawRgbFullAlpha( const SurfaceT<T> &srcSurface, SurfaceT<T> *destSurface, const Vec2i &size )
{
	const uint8_t srcRed = srcSurface.getChannelOrder().getRedOffset();
	const uint8_t srcGreen = srcSurface.getChannelOrder().getGreenOffset();
	const uint8_t srcBlue = srcSurface.getChannelOrder().getBlueOffset();
	const T fullAlpha = CHANTRAIT<T>::max();
	const int8_t srcPixelInc = srcSurface.getPixelInc();
	
	const uint8_t dstRed = destSurface->getChannelOrder().getRedOffset();
	const uint8_t dstGreen = destSurface->getChannelOrder().getGreenOffset();
	const uint8_t dstBlue = destSurface->getChannelOrder().getBlueOffset();
	const uint8_t dstAlpha = destSurface->getChannelOrder().getAlphaOffset();
	
	for( int32_t y = 0; y < size.y; ++y ) {
		const T *src = srcSurface.getData( Vec2i( 0, y ) );
		T *dst = destSurface->getData( Vec2i( 0, size.y - y - 1 ) );
		for( int x = 0; x < size.x; ++x ) {
			dst[dstRed] = src[srcRed];
			dst[dstGreen] = src[srcGreen];
			dst[dstBlue] = src[srcBlue];
			dst[dstAlpha] = fullAlpha;
			src += srcPixelInc;
			dst += 4;
		}
	}
}

template<typename T>
void flipVerticalRawRgb( const SurfaceT<T> &srcSurface, SurfaceT<T> *destSurface, const Vec2i &size )
{
	const uint8_t srcRed = srcSurface.getChannelOrder().getRedOffset();
	const uint8_t srcGreen = srcSurface.getChannelOrder().getGreenOffset();
	const uint8_t srcBlue = srcSurface.getChannelOrder().getBlueOffset();
	const int8_t srcPixelInc = srcSurface.getPixelInc();
	
	const uint8_t dstRed = destSurface->getChannelOrder().getRedOffset();
	const uint8_t dstGreen = destSurface->getChannelOrder().getGreenOffset();
	const uint8_t dstBlue = destSurface->getChannelOrder().getBlueOffset();
	const int8_t dstPixelInc = destSurface->getPixelInc();
	
	for( int32_t y = 0; y < size.y; ++y ) {
		const T *src = srcSurface.getData( Vec2i( 0, y ) );
		T *dst = destSurface->getData( Vec2i( 0, size.y - y - 1 ) );
		for( int x = 0; x < size.x; ++x ) {
			dst[dstRed] = src[srcRed];
			dst[dstGreen] = src[srcGreen];
			dst[dstBlue] = src[srcBlue];
			src += srcPixelInc;
			dst += dstPixelInc;
		}
	}
}
} // anonymous namespace

template<typename T>
void flipVertical( const SurfaceT<T> &srcSurface, SurfaceT<T> *destSurface )
{
	std::pair<Area,Vec2i> srcDst = clippedSrcDst( srcSurface.getBounds(), destSurface->getBounds(), destSurface->getBounds(), Vec2i(0,0) );
	
	if( destSurface->getChannelOrder() == srcSurface.getChannelOrder() )
		flipVerticalRawSameChannelOrder( srcSurface, destSurface, srcDst.first.getSize() );
	else if( destSurface->hasAlpha() && srcSurface.hasAlpha() )
		flipVerticalRawRgba( srcSurface, destSurface, srcDst.first.getSize() );
	else if( destSurface->hasAlpha() && ( ! srcSurface.hasAlpha() ) )
		flipVerticalRawRgbFullAlpha( srcSurface, destSurface, srcDst.first.getSize() );
	else
		flipVerticalRawRgb( srcSurface, destSurface, srcDst.first.getSize() );
}

#define flip_PROTOTYPES(r,data,T)\
	template void flipVertical<T>( SurfaceT<T> *surface );\
	template void flipVertical<T>( const SurfaceT<T> &srcSurface, SurfaceT<T> *destSurface );

BOOST_PP_SEQ_FOR_EACH( flip_PROTOTYPES, ~, CHANNEL_TYPES )

} } // namespace cinder::ip
