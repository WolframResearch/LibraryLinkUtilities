/* Include required header */
#include "WolframLibrary.h"

#include "LLU/MArgumentManager.h"
#include "LLU/LibraryLinkError.h"

using namespace LibraryLinkUtils;

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

EXTERN_C DLLEXPORT int parabola(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	MArgumentManager mngr(Argc, Args, Res);
	auto x = mngr.getReal(0);
	auto a = mngr.getReal(1);
	mngr.setReal(x * x - a);
	return 0;
}

static mint mandelbrot_max_iterations = 1000;

EXTERN_C DLLEXPORT int mandelbrot(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	MArgumentManager mngr(Argc, Args, Res);

	std::complex<double> z = 0;
	auto c = mngr.getComplex(0);

	mreal rr = z.real() * z.real();
	mreal ii = z.imag() * z.imag();
	mint n = 0;
	while ((n < mandelbrot_max_iterations) && (rr + ii < 4)) {
		z.imag(2. * z.real() * z.imag() + c.imag());
		z.real(rr - ii + c.real());
		rr = z.real() * z.real();
		ii = z.imag() * z.imag();
		n++;
	}

	if (n == mandelbrot_max_iterations)
		n = 0;

	mngr.setInteger(n);
	return LIBRARY_NO_ERROR;
}

EXTERN_C DLLEXPORT int refine(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	MArgumentManager mngr(Argc, Args, Res);
	auto dist = mngr.getReal(0);
	auto distsq = dist * dist * (1. + 1.e-8); /* Avoid adding extra points due to roundoff */

	Tensor<double> Targ = mngr.getTensor<double>(1);
	if (Targ.rank() != 2)
		return LIBRARY_RANK_ERROR;

	if (Targ.dimension(0) != 2)
		return LIBRARY_DIMENSION_ERROR;

	auto n = Targ.dimension(1);
	auto* x = Targ.data();
	auto* y = x + n;

	/* Do a pass to determine the length of the result */
	double x2 = x[0];
	double y2 = y[0];
	mint nr = n;
	for (mint i = 1; i < n; i++) {
		double x1 = x2;
		double y1 = y2;
		x2 = x[i];
		y2 = y[i];
		double dx = x2 - x1;
		double dy = y2 - y1;
		double d = dx * dx + dy * dy;
		if (d > distsq) {
			d = sqrt(d);
			mint k = ((mint) ceil(d / dist)) - 1;
			nr += k;
		}
	}

	Tensor<double> Tres(0., { 2, nr });
	auto* rx = Tres.data();
	auto* ry = rx + nr;

	x2 = x[0];
	y2 = y[0];
	rx[0] = x2;
	ry[0] = y2;
	for (mint j = 1, i = 1; i < n; i++, j++) {
		double x1 = x2;
		double y1 = y2;
		x2 = x[i];
		y2 = y[i];
		double dx = x2 - x1;
		double dy = y2 - y1;
		double d = dx * dx + dy * dy;
		if (d > distsq) {
			d = sqrt(d);
			mint k = ((mint) ceil(d / dist)) - 1;
			double ds = 1. / ((mreal) (k + 1));
			for (mint m = 1; m <= k; m++, j++) {
				double s = m * ds;
				rx[j] = x1 + s * dx;
				ry[j] = y1 + s * dy;
			}
		}
		rx[j] = x2;
		ry[j] = y2;
	}

	mngr.setTensor(Tres);
	return LIBRARY_NO_ERROR;
}

static mreal epsConstant = 0.3;
static mreal omegaConstant = 1.0;
static mreal gammaConstant = 0.15;

/* Overwrites x and y */
int duffing_rhs(mint n, mreal t, mreal *x, mreal *y) {
	for (mint i = 0; i < n; i++) {
		mreal xi = x[i];
		x[i] = y[i];
		y[i] = epsConstant * cos(omegaConstant * t) + xi * (1 - xi * xi) - gammaConstant * y[i];
	}
	return 0;
}

EXTERN_C DLLEXPORT int duffing_crk4(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	MArgumentManager mngr(Argc, Args, Res);
	auto Targ = mngr.getTensor<double>(0);
	auto t0 = mngr.getReal(1);
	auto t1 = mngr.getReal(2);

	auto h = t1 - t0;
	if (h < 0)
		return LIBRARY_FUNCTION_ERROR;

	auto n = Targ.size();
	auto nh = n / 2;
	if (n != nh * 2)
		return LIBRARY_DIMENSION_ERROR;
	auto* x = Targ.data();

	std::vector<mreal> k(4 * n);
	auto* k0 = k.data();

	for (mint i = 0; i < n; i++)
		k0[i] = x[i];

	if (duffing_rhs(nh, t0, k0, k0 + nh)) {
		return LIBRARY_FUNCTION_ERROR;
	}

	auto* k1 = k0 + n;
	for (mint i = 0; i < n; i++) {
		k0[i] *= h;
		k1[i] = x[i] + 0.5 * k0[i];
	}

	if (duffing_rhs(nh, t0 + h / 2, k1, k1 + nh))
		return LIBRARY_FUNCTION_ERROR;

	auto* k2 = k1 + n;
	for (mint i = 0; i < n; i++) {
		k1[i] *= h;
		k2[i] = x[i] + 0.5 * k1[i];
	}

	if (duffing_rhs(nh, t0 + h / 2, k2, k2 + nh))
		return LIBRARY_FUNCTION_ERROR;

	auto* k3 = k2 + n;
	for (mint i = 0; i < n; i++) {
		k2[i] *= h;
		k3[i] = x[i] + k2[i];
	}

	if (duffing_rhs(nh, t1, k3, k3 + nh))
		return LIBRARY_FUNCTION_ERROR;

	Tensor<mreal> Tres(0., Targ.dimensions());

	for (mint i = 0; i < n; i++)
		Tres[i] = x[i] + (k0[i] + 2. * (k1[i] + k2[i]) + h * k3[i]) / 6.;

	mngr.setTensor(Tres);
	return LIBRARY_NO_ERROR;
}

static mreal alpha = 0.02;

EXTERN_C DLLEXPORT int brusselator_pde_rhs(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	MArgumentManager mngr(Argc, Args, Res);
	auto Targ = mngr.getTensor<double>(1);

	if (Targ.rank() != 1)
		return LIBRARY_RANK_ERROR;
	auto nr = Targ.size();
	auto n = nr / 2;

	auto u = Targ.begin();
	auto v = u + n;

	Tensor<double> Tres(0., { nr });

	auto up = Tres.begin();
	auto vp = up + n;

	/* Decrement n so loop excludes boundaries */
	n--;
	auto dm = static_cast<mreal>(n);
	dm *= alpha * dm;

	/* Boundary conditions to converge to correct values */
	up[0] = 1. - u[0];
	up[n] = 1. - u[n];
	vp[0] = 3. - v[0];
	vp[n] = 3. - v[n];

	for (mint i = 1; i < n; i++) {
		mreal uuv = u[i];
		uuv *= uuv * v[i];
		up[i] = 1. + uuv - 4. * u[i] + dm * (u[i - 1] - 2. * u[i] + u[i + 1]);
		vp[i] = 3. * u[i] - uuv + dm * (v[i - 1] - 2. * v[i] + v[i + 1]);
	}

	mngr.setTensor(Tres);
	return 0;
}

EXTERN_C DLLEXPORT int brusselator_pde_jacobian_values(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	MArgumentManager mngr(Argc, Args, Res);
	auto Targ = mngr.getTensor<double>(1);

	if (Targ.rank() != 1)
		return LIBRARY_RANK_ERROR;
	auto nr = Targ.size();
	auto n = nr / 2;

	auto u = Targ.begin();
	auto v = u + n;

	Tensor<double> Tres(0., { 4 + 8 * (n - 2) });

	/* Decrement n so loop excludes boundaries */
	n--;
	auto dm = static_cast<mreal>(n);
	dm *= alpha * dm;

	mint k = 0;

	Tres[k++] = -1.; /* u[0] bc */
	for (mint i = 1; i < n; i++) {
		/* u equations */
		Tres[k++] = dm;
		Tres[k++] = 2. * u[i] * v[i] - 4. - 2. * dm;
		Tres[k++] = dm;
		Tres[k++] = u[i] * u[i];
	}
	Tres[k++] = -1.; /* u[n] bc */
	Tres[k++] = -1.; /* v[0] bc */
	for (mint i = 1; i < n; i++) {
		/* v equations */
		Tres[k++] = 3. - 2. * u[i] * v[i];
		Tres[k++] = dm;
		Tres[k++] = -u[i] * u[i] - 2. * dm;
		Tres[k++] = dm;
	}
	Tres[k++] = -1.; /* v[n] bc */

	mngr.setTensor(Tres);
	return 0;
}

EXTERN_C DLLEXPORT int brusselator_pde_jacobian_positions(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	MArgumentManager mngr(Argc, Args, Res);
	auto Targ = mngr.getTensor<double>(1);

	if (Targ.rank() != 1)
		return LIBRARY_RANK_ERROR;
	auto nr = Targ.size();
	auto n = nr / 2;


	Tensor<mint> Tres(1, { 4 + 8 * (n - 2), 2 });

	/* Decrement n so loop excludes boundaries */
	n--;

//	k = 0;
//	jpos[k++] = 1;
//	jpos[k++] = 1; /* u[0] bc */
	mint k = 2;
	for (mint i = 1; i < n; i++) {
		/* u equations */
		mint r = i + 1;
		Tres[k++] = r;
		Tres[k++] = i;
		Tres[k++] = r;
		Tres[k++] = i + 1;
		Tres[k++] = r;
		Tres[k++] = i + 2;
		Tres[k++] = r;
		Tres[k++] = i + n + 2;
	}
	Tres[k++] = n + 1;
	Tres[k++] = n + 1; /* u[n] bc */
	Tres[k++] = n + 2;
	Tres[k++] = n + 2; /* v[0] bc */
	for (mint i = 1; i < n; i++) {
		/* v equations */
		mint r = i + n + 2;
		Tres[k++] = r;
		Tres[k++] = i + 1;
		Tres[k++] = r;
		Tres[k++] = i + n + 1;
		Tres[k++] = r;
		Tres[k++] = i + n + 2;
		Tres[k++] = r;
		Tres[k++] = i + n + 3;
	}
	Tres[k++] = 2 * (n + 1);
	Tres[k++] = 2 * (n + 1); /* v[n] bc */

	mngr.setTensor(Tres);
	return 0;
}
