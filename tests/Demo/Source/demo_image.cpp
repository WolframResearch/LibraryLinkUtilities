/* Include required header */
#include "WolframLibrary.h"
#include "WolframImageLibrary.h"

#include <algorithm>
#include <cmath>
#include <complex>
#include <cstdint>
#include <iterator>
#include <vector>

#include "MArgumentManager.h"
#include "Image.h"
#include "LibraryLinkError.h"
#include "LibraryLinkFunctionMacro.h"

using namespace LibraryLinkUtils;
using std::log2;
using std::pow;
using std::sqrt;

/* Return the version of Library Link */
EXTERN_C DLLEXPORT mint WolframLibrary_getVersion() {
	return WolframLibraryVersion;
}

/* Initialize Library */
EXTERN_C DLLEXPORT int WolframLibrary_initialize(WolframLibraryData libData) {
	MArgumentManager::setLibraryData(libData);
	return LIBRARY_NO_ERROR;
}

/* Uninitialize Library */
EXTERN_C DLLEXPORT void WolframLibrary_uninitialize(WolframLibraryData libData) {
	return;
}

template<typename T> static T maxValue() {
	return -1; // ERROR
}

template<> char maxValue<char>() {
	return 1;
}

template<> raw_t_ubit8 maxValue<raw_t_ubit8>() {
	return 255;
}

template<> raw_t_ubit16 maxValue<raw_t_ubit16>() {
	return 65535;
}

template<> raw_t_real32 maxValue<raw_t_real32>() {
	return 1.0f;
}

template<> raw_t_real64 maxValue<raw_t_real64>() {
	return 1.0;
}

/* Negate image colors */
LIBRARY_LINK_FUNCTION(color_negate) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);

		mngr.operateOnImage(0, [&](const auto& image_in) {
			auto image_out = image_in;
			std::transform(std::cbegin(image_in), std::cend(image_in), std::begin(image_out), [](auto channelValue) {
						return maxValue<decltype(channelValue)>() - channelValue;
					});
			mngr.setImage(image_out);
		});

	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

template<typename T>
static void irgb_to_gray(T* out, const T* in, mint rows, mint cols, mbool alphaQ) {
	if (alphaQ) {
		for (mint row = 0; row < rows; row++) {
			for (mint col = 0; col < cols; col++) {
				mint idx = row * cols + col;
				T r = in[4 * idx];
				T g = in[4 * idx + 1];
				T b = in[4 * idx + 2];
				out[2 * idx] = static_cast<T>(.299 * r + .587 * g + .114 * b);
				out[2 * idx + 1] = in[4 * idx + 3];
			}
		}
	}
	else {
		for (mint row = 0; row < rows; row++) {
			for (mint col = 0; col < cols; col++) {
				mint idx = row * cols + col;
				T r = in[3 * idx];
				T g = in[3 * idx + 1];
				T b = in[3 * idx + 2];
				out[idx] = static_cast<T>(.299 * r + .587 * g + .114 * b);
			}
		}
	}
}

struct RGBImageToGrayscale {
	template<typename T>
	void operator()(Image<T> imageIn, MArgumentManager& mngr) {
		if (imageIn.colorspace() != MImage_CS_RGB)
			ErrorManager::throwException("FunctionError");
		/* This function accepts only 2D images, but can be easily extended to work with Image3D.*/
		if (imageIn.is3D())
			ErrorManager::throwException("FunctionError");

		auto rows = imageIn.rows();
		auto columns = imageIn.columns();
		auto alphaQ = imageIn.alphaChannelQ();

		Image<T> imageOut(columns, rows, alphaQ ? 2 : 1, MImage_CS_Gray, true);
		irgb_to_gray(imageOut.data(), imageIn.data(), rows, columns, alphaQ);
		mngr.setImage(imageOut);
	}
};

template<>
inline void RGBImageToGrayscale::operator()(Image<std::int8_t>, MArgumentManager&) {
	ErrorManager::throwException("FunctionError");
}

/* Convert interleaved RGB image to grayscale */
EXTERN_C DLLEXPORT int rgb_to_gray(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument res) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, res);
		mngr.operateOnImage<RGBImageToGrayscale>(0, mngr);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

struct RGBHistogramEqualization {
	template<typename T>
	void operator()(Image<T> imageIn, MArgumentManager& mngr) {
		if (imageIn.colorspace() != MImage_CS_RGB)
			ErrorManager::throwException("FunctionError");
		/* This function accepts only 2D images, but can be easily extended to work with Image3D.*/
		if (imageIn.is3D())
			ErrorManager::throwException("FunctionError");

		auto rows = imageIn.rows();
		auto columns = imageIn.columns();
		auto alphaQ = imageIn.alphaChannelQ();

		Image<std::uint8_t> imageOut(imageIn, false);
		Image<std::uint8_t> imageGray(columns, rows, imageIn.channels(), MImage_CS_Gray, false);

		auto* dataGray = imageGray.data();
		irgb_to_gray(dataGray, imageOut.data(), rows, columns, alphaQ);

		std::vector<mint> histogram(256, 0);
		for (mint ii = 0; ii < rows; ii++) {
			for (mint jj = 0; jj < columns; jj++) {
				histogram[dataGray[ii * columns + jj]]++;
			}
		}

		mint accum = 0;
		std::vector<mint> cdf;
		std::transform(std::cbegin(histogram), std::cend(histogram), std::back_inserter(cdf), [&accum](const auto& histElem) {
			accum += histElem;
			return accum;
		});

		mint cdfmin = *std::min_element(std::cbegin(histogram), std::cend(histogram));

		for (auto& elem : imageOut) {
			elem = maxValue<raw_t_ubit8>() * (cdf[elem] - cdfmin) / (rows * columns - cdfmin) + 0.5;
		}
		mngr.setImage(imageOut);
	}
};

template<>
inline void RGBHistogramEqualization::operator()(Image<std::int8_t>, MArgumentManager&) {
	ErrorManager::throwException("FunctionError");
}

EXTERN_C DLLEXPORT int rgbHistogramEqualization(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument res) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, res);
		mngr.operateOnImage<RGBHistogramEqualization>(0, mngr);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

struct iUpsample {
	template<typename T>
	void operator()(Image<T> imageIn, MArgumentManager& mngr) {
		auto n = mngr.getInteger<mint>(1);

		auto width = imageIn.columns();
		auto height = imageIn.rows();
		auto channels = imageIn.channels();

		Image<T> imageOut(n * width, n * height, channels, imageIn.colorspace(), imageIn.interleavedQ());

		std::fill(std::begin(imageOut), std::end(imageOut), static_cast<T>(0));

		auto* inIter = imageIn.data();
		auto* outIter = imageOut.data();

		for (mint ii = 0; ii < height; ii++) {
			for (mint jj = 0; jj < width; jj++) {
				for (mint kk = 0; kk < channels; kk++) {
					*outIter++ = *inIter++;
				}
				for (mint kk = 0; kk < (n - 1) * channels; kk++) {
					*outIter++ = 0;
				}
			}
			for (mint jj = 0; jj < (n - 1) * n * width * channels; jj++) {
				*outIter++ = 0;
			}
		}

		mngr.setImage(imageOut);
	}
};

EXTERN_C DLLEXPORT int Upsample(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		mngr.operateOnImage<iUpsample>(0, mngr);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

/* TONE Mapping Operators */

/*
 references = {
 "http://www.mpi-inf.mpg.de/~reinhard/papers/jgt_reinhard.pdf",
 "http://www.cis.rit.edu/jaf/publications/sig02_paper.pdf"
 };
 luminance[{r_, g_, b_}] := 0.27*r + 0.67*g + 0.06*b
 delta = 0.00001
 ReinhardGlobal[img_] := Module[{data, lum, lWorld, pixelCount, a, ld, lwhite,
 chrominanceChannels, lmax, lmin},
 data = ImageData[img, "Real"];
 pixelCount = Times@@ImageDimensions[img];
 lum = Map[luminance, data, {2}];
 chrominanceChannels = MapThread[#1/(#2 + delta) &, {data, lum}, 2];
 lWorld = Exp[Total[Log[lum + delta], Infinity]/pixelCount];
 lmax = Max[lum];
 lmin = Min[lum];
 a = 0.18 * 4^((2*Log2[lWorld] - Log2[lmin] - Log2[lmax])/(Log2[lmax] - Log2[lmin]));
 lwhite = 1.5 * 2^(Log2[lmax] - Log2[lmin] - 5);
 lum = a*lum/lWorld;
 ld = lum*(1+lum/(lwhite^2))/(1+lum);
 data = ld*chrominanceChannels;
 Image[data, "Real"]
 ]
 */

template<typename T> static T clip(const T &x, const T &minVal, const T &maxVal) {
	return std::max(std::min(x, maxVal), minVal);
}

static raw_t_real32 luminance(raw_t_real32 r, raw_t_real32 g, raw_t_real32 b) {
	return 0.27f * r + 0.67f * g + 0.06f * b;
}

static void ireinhard_global(raw_t_real32 *out, raw_t_real32 *in, mint width, mint height, mint channels) {
	const raw_t_real32 delta = 0.00001f;
	mint pixelCount = width * height;

	raw_t_real32 lworld = 0;
	raw_t_real32 lmax = 0;
	raw_t_real32 lmin = 1;
	std::vector<raw_t_real32> lum(pixelCount);

	for (mint ii = 0; ii < height; ii++) {
		for (mint jj = 0; jj < width; jj++) {
			mint idx = (ii * width + jj);
			raw_t_real32 r = in[idx * channels];
			raw_t_real32 g = in[idx * channels + 1];
			raw_t_real32 b = in[idx * channels + 2];
			lum[idx] = luminance(r, g, b);
			out[idx * channels] = r / (lum[idx] + delta);
			out[idx * channels + 1] = g / (lum[idx] + delta);
			out[idx * channels + 2] = b / (lum[idx] + delta);
			for (mint kk = 3; kk < channels; kk++) {
				out[idx * channels + kk] = in[idx * channels + kk];
			}
			lworld += log(lum[idx] + delta);
			lmax = std::max(lum[idx] - delta, lmax);
			lmin = std::min(lum[idx] + delta, lmin);
		}
	}

	lworld = exp(lworld / pixelCount);

	raw_t_real32 a = 0.18f * pow(4.0f, ((2 * log2(lworld) - log2(lmin) - log2(lmax)) / (delta + log2(lmax) - log2(lmin))));
	raw_t_real32 lwhite = 1.5f * pow(2.0f, (log2(lmax) - log2(lmin) - 5));

	for (mint ii = 0; ii < height; ii++) {
		for (mint jj = 0; jj < width; jj++) {
			raw_t_real32 ld;
			mint idx = (ii * width + jj);
			lum[idx] = a * lum[idx] / lworld;
			ld = lum[idx] * (1 + lum[idx] / (lwhite * lwhite)) / (1 + lum[idx]);

			out[idx * channels] = clip(ld * out[idx * channels], 0.0f, 1.0f);
			out[idx * channels + 1] = clip(ld * out[idx * channels + 1], 0.0f, 1.0f);
			out[idx * channels + 2] = clip(ld * out[idx * channels + 2], 0.0f, 1.0f);
		}
	}
}

/* perform the reinhard_global tone mapping */
EXTERN_C DLLEXPORT int reinhard_global(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);

		auto imageIn = mngr.getImage<raw_t_real32>(0);
		if (imageIn.colorspace() != MImage_CS_RGB)
			ErrorManager::throwException("FunctionError");
		/* This function accepts only 2D images, but can be easily extended to work with Image3D.*/
		if (imageIn.is3D())
			ErrorManager::throwException("FunctionError");

		auto height = imageIn.rows();
		auto width = imageIn.columns();
		auto channels = imageIn.channels();

		Image<raw_t_real32> imageOut(width, height, channels, imageIn.colorspace(), imageIn.interleavedQ());

		ireinhard_global(imageOut.data(), imageIn.data(), width, height, channels);

		mngr.setImage(imageOut);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

/*
 references = {
 "http://www.mpi-inf.mpg.de/~reinhard/papers/jgt_reinhard.pdf",
 "http://www.cis.rit.edu/jaf/publications/sig02_paper.pdf"
 };
 luminance[{r_, g_, b_}] := 0.27*r + 0.67*g + 0.06*b
 delta = 0.00001
 Options[$Operator] = $Operator["Options"]
 ReinhardLocal[img_, opts:OptionsPattern[]] :=
 Module[{data, lum, lWorld, pixelCount, a, ld, lwhite,
 chrominanceChannels, lmax, lmin, gauss, phi, maxRadius, threshold, findgauss},
 data = ImageData[img, "Real"];
 pixelCount = Times@@ImageDimensions[img];
 lum = Map[luminance, data, {2}];
 chrominanceChannels = MapThread[#1/(#2 + delta) &, {data, lum},
 2];
 lWorld = Exp[Total[Log[lum + delta], Infinity]/pixelCount];
 lmax = Max[lum];
 lmin = Min[lum];
 a = 0.18 * 4^((2*Log2[lWorld] - Log2[lmin] - Log2[lmax])/(Log2[lmax] - Log2[lmin]));
 threshold = 0.05;
 lwhite = 1.5 * 2^(Log2[lmax] - Log2[lmin] - 5);
 lum = a*lum/lWorld;
 phi = 8.0;
 maxRadius = 8;
 gauss[n_] := gauss[n] = GaussianFilter[lum, {n, 1.6*n}];
 findgauss[n_, x_, y_] := Module[{v1, v2, val, denom, s},
 v1 = gauss[n][[x, y]];
 v2 = gauss[n+1][[x, y]];
 s = 1.6^(2*n);
 denom = 2^(phi)*a/s^2 + v1;
 val = v1 - v2;
 If[Abs[val/denom] >= threshold || n > maxRadius,
 v1,
 findgauss[n+1, x, y]
 ]
 ];
 ld = lum/(1+Table[findgauss[1, x, y], {x, Dimensions[lum][[1]]}, {y, Dimensions[lum][[2]]}]);
 data = ld*chrominanceChannels;
 ClearAll[gauss];
 Image[data, "Real"]
 ]
 */

/* GaussianMatrix[{1, 1.6}] */
static constexpr raw_t_real32 gauss_kernel_1[3][3] = { { 0.0920282f, 0.119305f, 0.0920282f }, { 0.119305f, 0.154667f, 0.119305f }, { 0.0920282f, 0.119305f, 0.0920282f } };

static raw_t_real32 guass_r_1(raw_t_real32 *img, mint xpos, mint ypos, mint width, mint height) {
	mint ii, jj, pos;
	raw_t_real32 accum = 0;
	mint idx = ypos * width + xpos;
	for (ii = -1; ii <= 1; ii++) {
		if (ypos + ii < 0 || ypos + ii >= height) {
			continue;
		}
		for (jj = -1; jj <= 1; jj++) {
			if (xpos + jj < 0 || xpos + jj >= width) {
				continue;
			}
			pos = idx + ii * width + jj;
			accum += img[pos] * gauss_kernel_1[ii + 1][jj + 1];
		}
	}
	return accum;
}

/* GaussianMatrix[{2, 1.6*2}] */
static constexpr raw_t_real32 gauss_kernel_2[5][5] = { { 0.0323465f, 0.0377221f, 0.0397141f, 0.0377221f, 0.0323465f }, { 0.0377221f, 0.043991f, 0.0463141f, 0.043991f, 0.0377221f },
	{ 0.0397141f, 0.0463141f, 0.0487599f, 0.0463141f, 0.0397141f }, { 0.0377221f, 0.043991f, 0.0463141f, 0.043991f, 0.0377221f }, { 0.0323465f, 0.0377221f, 0.0397141f, 0.0377221f,
		0.0323465f } };

static raw_t_real32 guass_r_2(raw_t_real32 *img, mint xpos, mint ypos, mint width, mint height) {
	mint ii, jj, pos;
	raw_t_real32 accum = 0;
	mint idx = ypos * width + xpos;
	for (ii = -2; ii <= 2; ii++) {
		if (ypos + ii < 0 || ypos + ii >= height) {
			continue;
		}
		for (jj = -2; jj <= 2; jj++) {
			if (xpos + jj < 0 || xpos + jj >= width) {
				continue;
			}
			pos = idx + ii * width + jj;
			accum += img[pos] * gauss_kernel_2[ii + 2][jj + 2];
		}
	}
	return accum;
}

static const raw_t_real32 gauss_kernel_3[7][7] = { /* GaussianMatrix[{3, 1.6*3}] */
{ 0.0162577f, 0.0181613f, 0.0194107f, 0.0198463f, 0.0194107f, 0.0181613f, 0.0162577f }, { 0.0181613f, 0.0202878f, 0.0216835f, 0.0221701f, 0.0216835f, 0.0202878f, 0.0181613f }, {
	0.0194107f, 0.0216835f, 0.0231752f, 0.0236952f, 0.0231752f, 0.0216835f, 0.0194107f }, { 0.0198463f, 0.0221701f, 0.0236952f, 0.024227f, 0.0236952f, 0.0221701f, 0.0198463f }, {
	0.0194107f, 0.0216835f, 0.0231752f, 0.0236952f, 0.0231752f, 0.0216835f, 0.0194107f }, { 0.0181613f, 0.0202878f, 0.0216835f, 0.0221701f, 0.0216835f, 0.0202878f, 0.0181613f }, {
	0.0162577f, 0.0181613f, 0.0194107f, 0.0198463f, 0.0194107f, 0.0181613f, 0.0162577f } };
static raw_t_real32 guass_r_3(raw_t_real32 *img, mint xpos, mint ypos, mint width, mint height) {
	mint ii, jj, pos;
	raw_t_real32 accum = 0;
	mint idx = ypos * width + xpos;
	for (ii = -3; ii <= 3; ii++) {
		if (ypos + ii < 0 || ypos + ii >= height) {
			continue;
		}
		for (jj = -3; jj <= 3; jj++) {
			if (xpos + jj < 0 || xpos + jj >= width) {
				continue;
			}
			pos = idx + ii * width + jj;
			accum += img[pos] * gauss_kernel_3[ii + 3][jj + 3];
		}
	}
	return accum;
}

static const raw_t_real32 gauss_kernel_4[9][9] = { /* GaussianMatrix[{4, 1.6*4}] */
{ 0.00975387f, 0.0106342f, 0.0113116f, 0.0117388f, 0.0118848f, 0.0117388f, 0.0113116f, 0.0106342f, 0.00975387f }, { 0.0106342f, 0.0115939f, 0.0123325f, 0.0127982f, 0.0129574f,
	0.0127982f, 0.0123325f, 0.0115939f, 0.0106342f }, { 0.0113116f, 0.0123325f, 0.0131181f, 0.0136135f, 0.0137829f, 0.0136135f, 0.0131181f, 0.0123325f, 0.0113116f }, { 0.0117388f,
	0.0127982f, 0.0136135f, 0.0141277f, 0.0143034f, 0.0141277f, 0.0136135f, 0.0127982f, 0.0117388f }, { 0.0118848f, 0.0129574f, 0.0137829f, 0.0143034f, 0.0144813f, 0.0143034f,
	0.0137829f, 0.0129574f, 0.0118848f }, { 0.0117388f, 0.0127982f, 0.0136135f, 0.0141277f, 0.0143034f, 0.0141277f, 0.0136135f, 0.0127982f, 0.0117388f }, { 0.0113116f, 0.0123325f,
	0.0131181f, 0.0136135f, 0.0137829f, 0.0136135f, 0.0131181f, 0.0123325f, 0.0113116f }, { 0.0106342f, 0.0115939f, 0.0123325f, 0.0127982f, 0.0129574f, 0.0127982f, 0.0123325f,
	0.0115939f, 0.0106342f }, { 0.00975387f, 0.0106342f, 0.0113116f, 0.0117388f, 0.0118848f, 0.0117388f, 0.0113116f, 0.0106342f, 0.00975387f } };
static raw_t_real32 guass_r_4(raw_t_real32 *img, mint xpos, mint ypos, mint width, mint height) {
	mint ii, jj, pos;
	raw_t_real32 accum = 0;
	mint idx = ypos * width + xpos;
	for (ii = -4; ii <= 4; ii++) {
		if (ypos + ii < 0 || ypos + ii >= height) {
			continue;
		}
		for (jj = -4; jj <= 4; jj++) {
			if (xpos + jj < 0 || xpos + jj >= width) {
				continue;
			}
			pos = idx + ii * width + jj;
			accum += img[pos] * gauss_kernel_4[ii + 4][jj + 4];
		}
	}
	return accum;
}

static const raw_t_real32 gauss_kernel_5[11][11] = { /* Map[ToString, GaussianMatrix[{5, 1.6*5}], {2}] /. x_String :> StringJoin[x, "f"] */
{ 0.0064955f, 0.00697212f, 0.00736701f, 0.00766278f, 0.00784594f, 0.00790796f, 0.00784594f, 0.00766278f, 0.00736701f, 0.00697212f, 0.0064955f }, { 0.00697212f, 0.00748371f,
	0.00790758f, 0.00822505f, 0.00842165f, 0.00848823f, 0.00842165f, 0.00822505f, 0.00790758f, 0.00748371f, 0.00697212f }, { 0.00736701f, 0.00790758f, 0.00835546f, 0.00869091f,
	0.00889864f, 0.00896899f, 0.00889864f, 0.00869091f, 0.00835546f, 0.00790758f, 0.00736701f }, { 0.00766278f, 0.00822505f, 0.00869091f, 0.00903982f, 0.0092559f, 0.00932907f,
	0.0092559f, 0.00903982f, 0.00869091f, 0.00822505f, 0.00766278f }, { 0.00784594f, 0.00842165f, 0.00889864f, 0.0092559f, 0.00947714f, 0.00955206f, 0.00947714f, 0.0092559f,
	0.00889864f, 0.00842165f, 0.00784594f }, { 0.00790796f, 0.00848823f, 0.00896899f, 0.00932907f, 0.00955206f, 0.00962757f, 0.00955206f, 0.00932907f, 0.00896899f, 0.00848823f,
	0.00790796f }, { 0.00784594f, 0.00842165f, 0.00889864f, 0.0092559f, 0.00947714f, 0.00955206f, 0.00947714f, 0.0092559f, 0.00889864f, 0.00842165f, 0.00784594f }, { 0.00766278f,
	0.00822505f, 0.00869091f, 0.00903982f, 0.0092559f, 0.00932907f, 0.0092559f, 0.00903982f, 0.00869091f, 0.00822505f, 0.00766278f }, { 0.00736701f, 0.00790758f, 0.00835546f,
	0.00869091f, 0.00889864f, 0.00896899f, 0.00889864f, 0.00869091f, 0.00835546f, 0.00790758f, 0.00736701f }, { 0.00697212f, 0.00748371f, 0.00790758f, 0.00822505f, 0.00842165f,
	0.00848823f, 0.00842165f, 0.00822505f, 0.00790758f, 0.00748371f, 0.00697212f }, { 0.0064955f, 0.00697212f, 0.00736701f, 0.00766278f, 0.00784594f, 0.00790796f, 0.00784594f,
	0.00766278f, 0.00736701f, 0.00697212f, 0.0064955f } };
static raw_t_real32 guass_r_5(raw_t_real32 *img, mint xpos, mint ypos, mint width, mint height) {
	mint ii, jj, pos;
	raw_t_real32 accum = 0;
	mint idx = ypos * width + xpos;
	for (ii = -5; ii <= 5; ii++) {
		if (ypos + ii < 0 || ypos + ii >= height) {
			continue;
		}
		for (jj = -5; jj <= 5; jj++) {
			if (xpos + jj < 0 || xpos + jj >= width) {
				continue;
			}
			pos = idx + ii * width + jj;
			accum += img[pos] * gauss_kernel_5[ii + 5][jj + 5];
		}
	}
	return accum;
}

static mbool find_gauss_aux(raw_t_real32 a, raw_t_real32 delta, mint n, raw_t_real32 v1, raw_t_real32 v2, raw_t_real32 *v) {
	const raw_t_real32 phi = 8.0f;
	const raw_t_real32 threshold = 0.05f;
	raw_t_real32 s = pow(1.6f, 2.0f * n);
	raw_t_real32 denom = pow(2.0f, phi) * a / (s * s) + v1 + delta;
	raw_t_real32 val = v1 - v2;
	if (fabs(val / denom) >= threshold) {
		*v = v1;
		return True;
	}
	return False;
}

static raw_t_real32 find_gauss(raw_t_real32 a, raw_t_real32 delta, raw_t_real32 *img, mint xpos, mint ypos, mint width, mint height) {
	raw_t_real32 res;
	raw_t_real32 v1 = guass_r_1(img, xpos, ypos, width, height);
	raw_t_real32 v2 = guass_r_2(img, xpos, ypos, width, height);
	if (find_gauss_aux(a, delta, 1, v1, v2, &res)) {
		return res;
	}
	v1 = v2;
	v2 = guass_r_3(img, xpos, ypos, width, height);
	if (find_gauss_aux(a, delta, 2, v1, v2, &res)) {
		return res;
	}
	v1 = v2;
	v2 = guass_r_4(img, xpos, ypos, width, height);
	if (find_gauss_aux(a, delta, 3, v1, v2, &res)) {
		return res;
	}
	v1 = v2;
	v2 = guass_r_5(img, xpos, ypos, width, height);
	if (find_gauss_aux(a, delta, 4, v1, v2, &res)) {
		return res;
	}
	return v1;
}

static void ireinhard_local(raw_t_real32 *out, raw_t_real32 *in, mint width, mint height, mint channels) {
	mint ii, jj, kk;
	const raw_t_real32 delta = 0.00001f;
	mint pixelCount = width * height;

	raw_t_real32 lworld = 0;
	raw_t_real32 lmax = 0;
	raw_t_real32 lmin = 1;
	std::vector<raw_t_real32> lum(pixelCount);

	for (ii = 0; ii < height; ii++) {
		for (jj = 0; jj < width; jj++) {
			mint idx = (ii * width + jj);
			raw_t_real32 r = in[idx * channels];
			raw_t_real32 g = in[idx * channels + 1];
			raw_t_real32 b = in[idx * channels + 2];
			lum[idx] = luminance(r, g, b);
			out[idx * channels] = r / (lum[idx] + delta);
			out[idx * channels + 1] = g / (lum[idx] + delta);
			out[idx * channels + 2] = b / (lum[idx] + delta);
			for (kk = 3; kk < channels; kk++) {
				out[idx * channels + kk] = in[idx * channels + kk];
			}
			lworld += log(lum[idx] + delta);
			lmax = std::max(lum[idx] - delta, lmax);
			lmin = std::min(lum[idx] + delta, lmin);
		}
	}

	lworld = exp(lworld / pixelCount);

	raw_t_real32 a = 0.18f * pow(4.0f, ((2 * log2(lworld) - log2(lmin) - log2(lmax)) / (delta + log2(lmax) - log2(lmin))));
	//raw_t_real32 lwhite = 1.5f * pow(2.0f, (log2(lmax) - log2(lmin) - 5));

	for (ii = 0; ii < height; ii++) {
		for (jj = 0; jj < width; jj++) {
			raw_t_real32 ld;
			mint idx = (ii * width + jj);
			lum[idx] = a * lum[idx] / lworld;
			ld = lum[idx] / (1 + find_gauss(a, delta, lum.data(), jj, ii, width, height));

			out[idx * channels] = clip(ld * out[idx * channels], 0.0f, 1.0f);
			out[idx * channels + 1] = clip(ld * out[idx * channels + 1], 0.0f, 1.0f);
			out[idx * channels + 2] = clip(ld * out[idx * channels + 2], 0.0f, 1.0f);
		}
	}
}

/* perform the reinhard_global tone mapping */
EXTERN_C DLLEXPORT int reinhard_local(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);

		auto imageIn = mngr.getImage<raw_t_real32>(0);
		if (imageIn.colorspace() != MImage_CS_RGB)
			ErrorManager::throwException("FunctionError");
		/* This function accepts only 2D images, but can be easily extended to work with Image3D.*/
		if (imageIn.is3D())
			ErrorManager::throwException("FunctionError");

		auto height = imageIn.rows();
		auto width = imageIn.columns();
		auto channels = imageIn.channels();

		Image<raw_t_real32> imageOut(width, height, channels, imageIn.colorspace(), imageIn.interleavedQ());

		ireinhard_local(imageOut.data(), imageIn.data(), width, height, channels);

		mngr.setImage(imageOut);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

/* Chromatic Adaptation */

/* Color conversion

 xyzToXY[xyz_] :=
 {xyz[[1]], xyz[[2]]}/Total[xyz]

 xyToXYZ[xy_, Y_] :=
 Module[{x = xy[[1]], y = xy[[2]]},
 {Y/y*x, Y, Y/y*(1 - x - y)}
 ]

 rgbToXYZ = {
 {0.4124564`, 0.3575761`, 0.1804375`},
 {0.2126729`, 0.7151522`, 0.072175`},
 {0.0193339`, 0.119192`, 0.9503041`}
 }

 */

static void xyzToXY(const raw_t_real32 &x, const raw_t_real32 &y, const raw_t_real32 &z, raw_t_real32 &X, raw_t_real32 &Y) {
	const raw_t_real32 sum = x + y + z;
	X = x / sum;
	Y = y / sum;
	return;
}

static void xyToXYZ(const raw_t_real32 &X, const raw_t_real32 &Y, const raw_t_real32 &C, raw_t_real32 &x, raw_t_real32 &y, raw_t_real32 &z) {
	x = C / (Y * X);
	y = C;
	z = C / y * (1 - x - y);
	return;
}

static void RGB_toXYZ(const raw_t_real32 &r, const raw_t_real32 &g, const raw_t_real32 &b, raw_t_real32 &x, raw_t_real32 &y, raw_t_real32 &z) {
	x = r * static_cast<raw_t_real32>(0.412387) + g * static_cast<raw_t_real32>(0.357591) + b * static_cast<raw_t_real32>(0.18045);
	y = r * static_cast<raw_t_real32>(0.212637) + g * static_cast<raw_t_real32>(0.715183) + b * static_cast<raw_t_real32>(0.0721802);
	z = r * static_cast<raw_t_real32>(0.0193306) + g * static_cast<raw_t_real32>(0.119197) + b * static_cast<raw_t_real32>(0.950373);
	return;
}

static void XYZToRGB(const raw_t_real32 &x, const raw_t_real32 &y, const raw_t_real32 &z, raw_t_real32 &r, raw_t_real32 &g, raw_t_real32 &b) {
	r = x * static_cast<raw_t_real32>(3.241) + y * static_cast<raw_t_real32>(-1.5374) + z * static_cast<raw_t_real32>(-0.498616);
	g = y * static_cast<raw_t_real32>(-0.969224) + y * static_cast<raw_t_real32>(1.87593) + z * static_cast<raw_t_real32>(0.0415542);
	b = z * static_cast<raw_t_real32>(0.0556394) + y * static_cast<raw_t_real32>(-0.204011) + z * static_cast<raw_t_real32>(1.05715);
	return;
}

/*

 (* Von Kries Algorithm *)
 resources = {
 "http://scien.stanford.edu/pages/labsite/2010/psych221/projects/2010/JasonSu/adaptation.html",
 "http://www.brucelindbloom.com/index.html?Eqn_ChromAdapt.html",
 "http://books.google.com/books?id=dN3sZ56Pu3cC&pg=PA43&lpg=PA43&dq=Chromatic+Adaptation+example+von+kries&source=bl&ots=azvrbvuHQO&sig=oSzTV24ACpwkU7DumP4sD_T1sX0&hl=en&sa=X&ei=2yhTT7e5Nc3yggfKyL28DQ&ved=0CFMQ6AEwBQ#v=onepage&q=Chromatic%20Adaptation%20example%20von%20kries&f=false"
 };


 model["vonKries"] = {{0.40024, 0.7076, -0.08081}, {-0.2263, 1.16532, 0.0457},
 {0, 0, 0.91822}}
 model["brandford"] = {{0.8951, 0.2664, -0.1614}, {-0.7502, 1.7135, 0.0367},
 {0.0389, -0.0685, 1.0296}}
 model["sharp"] = {{1.2694, -0.0988, -0.1706}, {-0.8364, 1.8006, 0.0357},
 {0.0297, -0.0315, 1.0018}}
 model["cmccat2000"] = {{0.7982, 0.3389, -0.1371}, {-0.5918, 1.5512, 0.0406},
 {0.0008, 0.239, 0.9753}}
 model["cat02"] = {{0.7328, 0.4296, -0.1624}, {-0.7036, 1.6975, 0.0061}, {0.003,
 0.0136, 0.9834}}

 vonKries[modelName_String, target_, est_] :=
 Module[{xfm = model[modelName], gain, mat},
 gain =
 First@Transpose[(xfm.Transpose[target])/(xfm.Transpose[est])];
 mat = Inverse[xfm].(DiagonalMatrix[gain].xfm);
 Inverse[rgbToXYZ].mat.rgbToXYZ
 ]

 target = {95.04, 100, 108.88};

 WhiteBalance[img_, model_:"vonKries"] :=
 Module[{gray, est, mat},
 gray = Mean[Mean[255*ImageData[img]]];
 est = xyzToXY[rgbToXYZ.gray];
 est = xyToXYZ[est, 100];
 mat = vonKries[model, {target}, {est}];
 ImageApply[(mat.#) &, img]
 ]
 */

void whiteBalance(raw_t_ubit8 *out, raw_t_ubit8 *in, mint width, mint height, mint channels, mint medX, mint medY, mint medZ) {
	for (mint ii = 0; ii < height; ii++) {
		for (mint jj = 0; jj < width; jj++) {
			for (mint kk = 0; kk < channels; kk++) {
				mint index = channels * (ii * width + jj);

				double r = in[index + 0] / 255.0;
				double g = in[index + 1] / 255.0;
				double b = in[index + 2] / 255.0;

				double x = medX * 0.412387 + medY * 0.357591 + medZ * 0.18045;
				double y = medX * 0.212637 + medY * 0.715183 + medZ * 0.0721802;
				double z = medX * 0.0193306 + medY * 0.119197 + medZ * 0.950373;

				double sum = x + y + z;
				double X = x / sum;
				double Y = y / sum;

				double estX = 100 / Y * X;
				double estY = 100;
				double estZ = 100 / Y * (1 - X - Y);

				double targetX = 95.04;
				double targetY = 100;
				double targetZ = 108.88;

				x = (0.40024 * targetX + 0.7076 * targetY + -0.08081 * targetZ) / (0.40024 * estX + 0.7076 * estY + -0.08081 * estZ);
				y = (-0.2263 * targetX + 1.16532 * targetY + 0.0457 * targetZ) / (-0.2263 * estX + 1.16532 * estY + 0.0457 * estZ);
				z = targetZ / estZ;

				double ta = 0.40024 * (0. + 1.8599363874558397 * x) + -0.2263 * (0. + -1.1293816185800916 * y);
				double tb = 0.7076 * (0. + 1.8599363874558397 * x) + 1.16532 * (0. + -1.1293816185800916 * y);
				double tc = -0.08081 * (0. + 1.8599363874558397 * x) + 0.0457 * (0. + -1.1293816185800916 * y) + 0.91822 * (0. + 0.2198974095961933 * z);
				double td = 0.40024 * (0. + 0.3611914362417675 * x) + -0.2263 * (0. + 0.6388124632850422 * y);
				double te = 0.7076 * (0. + 0.3611914362417675 * x) + 1.16532 * (0. + 0.6388124632850422 * y);
				double tf = -0.08081 * (0. + 0.3611914362417675 * x) + 0.0457 * (0. + 0.6388124632850422 * y) + 0.91822 * (0. + -6.3705968386499e-6 * z);
				double tg = 0;
				double th = 0;
				double ti = 0. + 0.91822 * (0. + 1.0890636230968613 * z);

				x = b
						* (0.584699569474613 * ta + 0.23387982778984517 * tb + 3.0794175165359725 * tc + -0.2773574912653834 * td + -0.11094299650615336 * te
								+ -1.4607493515217624 * tf + -0.08995378598808151 * tg + -0.03598151439523261 * th + -0.4737565729684595 * ti)
						+ g
								* (1.1587092024906749 * ta + 2.3174184049813498 * tb + 0.3862362928150638 * tc + -0.5496441151781635 * td + -1.099288230356327 * te
										+ -0.18321465382142613 * tf + -0.1782629662561986 * tg + -0.3565259325123972 * th + -0.05942097213434799 * ti)
						+ r
								* (1.3365463360279806 * ta + 0.6891569272956974 * tb + 0.06265062975415432 * tc + -0.6340027564134477 * td + -0.32690777695397993 * te
										+ -0.029718888813998177 * tf + -0.2056225271078049 * tg + -0.10602414981400575 * th + -0.009638559074000522 * ti);
				y = r
						* (-0.3997801258091086 * ta + -0.20613669400738588 * tb + -0.0187396994552169 * tc + 0.7737727140710301 * td + 0.39897668466862624 * te
								+ 0.03627060769714784 * tf + 0.01714007212281252 * tg + 0.008837852545305867 * th + 0.0008034411404823516 * ti)
						+ g
								* (-0.34658649555281573 * ta + -0.6931729911056315 * tb + -0.11552879954205893 * tc + 0.6708166714928755 * td + 1.341633342985751 * te
										+ 0.2236054946305942 * tf + 0.014859461856802371 * tg + 0.029718923713604743 * th + 0.004953152567064713 * ti)
						+ b
								* (-0.17489200422318826 * ta + -0.06995680168927532 * tb + -0.9210978242910322 * tc + 0.338502721973017 * td + 0.1354010887892068 * te
										+ 1.7827808773238276 * tf + 0.007498275608427907 * tg + 0.002999310243371163 * th + 0.039490915433981484 * ti);
				z = r
						* (0.02295048453364339 * ta + 0.011833847413144971 * tb + 0.0010758043102859064 * tc + -0.08415176935817942 * td + -0.04339077010208874 * te
								+ -0.003944615463826249 * tf + 0.43605928449681247 * tg + 0.22484314125289886 * th + 0.02044028556844535 * ti)
						+ g
								* (0.019896756972738263 * ta + 0.039793513945476526 * tb + 0.0066322504694654345 * tc + -0.07295476926821186 * td + -0.14590953853642372 * te
										+ -0.02431824962187548 * tf + 0.37803845041357265 * tg + 0.7560769008271453 * th + 0.12601278156368545 * ti)
						+ b
								* (0.010040159524835302 * ta + 0.004016063809934121 * tb + 0.05287816978790461 * tc + -0.036813915079427786 * td + -0.014725566031771114 * te
										+ -0.19388660581659606 * tf + 0.19076306525100253 * tg + 0.07630522610040101 * th + 1.0046854065069357 * ti);

				out[index + 0] = (raw_t_ubit8) (255 * x);
				out[index + 1] = (raw_t_ubit8) (255 * y);
				out[index + 2] = (raw_t_ubit8) (255 * z);

				for (int ii = 3; ii < channels; ii++) {
					out[index + ii] = in[index + ii];
				}
			}
		}
	}

	return;
}

/* Effects */

/*
 sepiaRed[{r_, g_, b_}] := r*0.393 + g*0.769 + b*0.189
 sepiaGreen[{r_, g_, b_}] := r*0.349 + g*0.686 + b*0.168
 sepiaBlue[{r_, g_, b_}] := r*0.272 + g*0.534 + b*0.131
 Sepia[img_] := ImageApply[Through[{sepiaRed, sepiaGreen, sepiaBlue}[#]] &, img]
 */
static void isepia(raw_t_real32 *out, raw_t_real32 *in, mint width, mint height, mint channels) {
	for (mint ii = 0; ii < height; ii++) {
		for (mint jj = 0; jj < width; jj++) {
			for (mint kk = 0; kk < channels; kk++) {
				mint index = channels * (ii * width + jj);
				raw_t_real32 r = in[index + 0];
				raw_t_real32 g = in[index + 1];
				raw_t_real32 b = in[index + 2];

				out[index + 0] = r * static_cast<raw_t_real32>(0.393) + g * static_cast<raw_t_real32>(0.769) + b * static_cast<raw_t_real32>(0.189);
				out[index + 1] = r * static_cast<raw_t_real32>(0.349) + g * static_cast<raw_t_real32>(0.686) + b * static_cast<raw_t_real32>(0.168);
				out[index + 2] = r * static_cast<raw_t_real32>(0.272) + g * static_cast<raw_t_real32>(0.534) + b * static_cast<raw_t_real32>(0.131);

				for (int ii = 3; ii < channels; ii++) {
					out[index + ii] = in[index + ii];
				}
			}
		}
	}
	return;
}

EXTERN_C DLLEXPORT int sepia(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);

		auto imageIn = mngr.getImage<raw_t_real32>(0);
		if (imageIn.colorspace() != MImage_CS_RGB)
			ErrorManager::throwException("FunctionError");
		/* This function accepts only 2D images, but can be easily extended to work with Image3D.*/
		if (imageIn.is3D())
			ErrorManager::throwException("FunctionError");

		auto height = imageIn.rows();
		auto width = imageIn.columns();
		auto channels = imageIn.channels();

		Image<raw_t_real32> imageOut(width, height, channels, imageIn.colorspace(), imageIn.interleavedQ());

		isepia(imageOut.data(), imageIn.data(), width, height, channels);

		mngr.setImage(imageOut);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

/*
 Lomography[image_, v_:4, c_:0.75] :=
 Module[{img = ColorConvert[image, "RGB"], vign, red, green, blue},
 vign = ColorNegate[
 ImageAdjust[
 DistanceTransform[Image[1 - BoxMatrix[1, Reverse[ImageDimensions[img]]], "Bit"]],
 {0, 0, v},
 {Automatic, Automatic}
 ]
 ];
 {red, green, blue} = ColorSeparate[ImageMultiply[img, vign]];
 ColorCombine[{ImageAdjust[red, c], ImageAdjust[green, c], blue}]
 ]
 */

static raw_t_real32 vignette(const mint xIndex, const mint yIndex, const mint width, const mint height) {
	raw_t_real32 midX = width / static_cast<raw_t_real32>(2);
	raw_t_real32 midY = height / static_cast<raw_t_real32>(2);

	raw_t_real32 dist = sqrt(
			pow(static_cast<raw_t_real32>(xIndex - midX) / midX, static_cast<raw_t_real32>(2.0))
					+ pow(static_cast<raw_t_real32>(yIndex - midY) / midY, static_cast<raw_t_real32>(2.0)));

	dist /= sqrt(static_cast<raw_t_real32>(2.0));

	return 1 - dist;
}

static raw_t_real32 xclamp(const raw_t_real32 val, const raw_t_real32 low, const raw_t_real32 high) {
	return val <= low ? low : (val >= high ? high : val);
}

static raw_t_real32 imageAdjust(const raw_t_real32 pixel, const raw_t_real32 lowOut, const raw_t_real32 highOut) {
	raw_t_real32 res, val;

	const raw_t_real32 lowIn = static_cast<raw_t_real32>(0);
	const raw_t_real32 highIn = static_cast<raw_t_real32>(1);
	const raw_t_real32 gamma = static_cast<raw_t_real32>(1);

	val = xclamp(pixel, lowIn, highIn);

	res = pow((val - lowIn) / (highIn - lowIn), gamma);
	res = res * (highOut - lowOut) - lowOut;

	return clip(res, 0.0f, 1.0f);
}

static void ilomography(raw_t_real32 *out, raw_t_real32 *in, mint width, mint height, mint channels) {
	const raw_t_real32 c = 0.75;
	for (mint ii = 0; ii < height; ii++) {
		for (mint jj = 0; jj < width; jj++) {
			for (mint kk = 0; kk < channels; kk++) {
				mint index = channels * (ii * width + jj);
				raw_t_real32 r = in[index + 0];
				raw_t_real32 g = in[index + 1];
				raw_t_real32 b = in[index + 2];

				raw_t_real32 vig = vignette(jj, ii, width, height);

				r *= vig;
				g *= vig;
				b *= vig;

				out[index + 0] = imageAdjust(r, 0.0f, c);
				out[index + 1] = imageAdjust(g, 0.0f, c);
				out[index + 2] = b;

				for (int ii = 3; ii < channels; ii++) {
					out[index + ii] = in[index + ii];
				}
			}
		}
	}

	return;
}

EXTERN_C DLLEXPORT int lomography(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);

		auto imageIn = mngr.getImage<raw_t_real32>(0);
		if (imageIn.colorspace() != MImage_CS_RGB)
			ErrorManager::throwException("FunctionError");
		/* This function accepts only 2D images, but can be easily extended to work with Image3D.*/
		if (imageIn.is3D())
			ErrorManager::throwException("FunctionError");

		auto height = imageIn.rows();
		auto width = imageIn.columns();
		auto channels = imageIn.channels();

		Image<raw_t_real32> imageOut(width, height, channels, imageIn.colorspace(), imageIn.interleavedQ());

		ilomography(imageOut.data(), imageIn.data(), width, height, channels);

		mngr.setImage(imageOut);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}
