#ifndef _FOORT_METRIC_H
#define _FOORT_METRIC_H

#include "Geometry.h" // Needed for basic tensor objects etc.

#include "Spline.h"
#include <string> // for strings
#include <vector> // needed for the (non-fixed size) vector of symmetries in the metric

///////////////////////////////////////////////////////////////////////////////////////
////// METRIC.H
////// Declarations of abstract base Metric class and all its descendants.
////// All definitions in Metric.cpp
///////////////////////////////////////////////////////////////////////////////////////

// The abstract base class for all Metrics.
class Metric
{
public:
	// Virtual destructor to ensure correct destruction of descendants
	virtual ~Metric() = default;

	Metric(bool rlogscale = false);

	// Basic functions that return the metric with indices down or up:
	// pure virtual as they must be defined in the descendant classes.
	//
	// Get the metric at Point p, indices down
	virtual TwoIndex getMetric_dd(const Point &p) const = 0;
	// Get the metric at Point p, indices up
	virtual TwoIndex getMetric_uu(const Point &p) const = 0;

	// The following functions return the Christoffel and other derivative quantities of the metric.
	// They are implemented for this base class, BUT are left as virtual functions to allow for
	// other metrics to implement their own (more efficient)
	// way of calculating them, if so desired.
	//
	// Get the Christoffel symbol, indices up-down-down
	virtual ThreeIndex getChristoffel_udd(const Point &p) const;
	// Get the Riemann tensor, indices up-down-down-down
	virtual FourIndex getRiemann_uddd(const Point &p) const;
	// Get the Kretschmann scalar
	virtual real getKretschmann(const Point &p) const;

	// Function to get the description of the metric
	// (used for outputting to the screen while running and possibly to the output files)
	// There is a base class implementation of this function returning an undescriptive string
	virtual std::string getFullDescriptionStr() const;

	bool getrLogScale() const;

protected:
	// The symmetries (coordinate Killing vectors) of the metric. Should be set by descendant constructor.
	std::vector<int> m_Symmetries{};
	// Are we using a logarithmic r coordinate?
	const bool m_rLogScale;
};

// Abstract base class for a metric that has a spherical horizon (i.e. horizon at constant radius r)
class SphericalHorizonMetric : public Metric
{
public:
	// No default construction allowed, must specify horizon radius
	SphericalHorizonMetric() = delete;
	// Constructor that initializes horizon radius
	SphericalHorizonMetric(real HorizonRadius, bool rLogScale);

	// Getter functions for the two member variables
	real getHorizonRadius() const;

protected:
	// Radius of the horizon
	const real m_HorizonRadius;
};

// The Kerr metric (normalized so that M = 1)
class KerrMetric final : public SphericalHorizonMetric
{
private:
	// Mass-rescaled rotation parameter for Kerr
	// Note that this should be between -1 and 1.
	const real m_aParam;

	// Mass parameter for Kerr. Default is 1.
	const real m_mParam;

public:
	// No default constructor allowed, must specify a
	KerrMetric() = delete;

	// Constructor setting parameter a
	KerrMetric(real aParam, bool rLogScale = false, real mParam = 1.);

	// The override of the basic metric getter functions
	TwoIndex getMetric_dd(const Point &p) const final;
	TwoIndex getMetric_uu(const Point &p) const final;

	// The override of the description string getter
	std::string getFullDescriptionStr() const final;
};

// Flat space (4D)
class FlatSpaceMetric final : public Metric
{
public:
	// Simple (default) constructor is all that is needed
	FlatSpaceMetric(bool rlogscale = false);

	// The override of the basic metric getter functions
	TwoIndex getMetric_dd(const Point &p) const final;
	TwoIndex getMetric_uu(const Point &p) const final;

	// The override of the description string getter
	std::string getFullDescriptionStr() const final;
};

// Rasheed-Larsen black hole
class RasheedLarsenMetric final : public SphericalHorizonMetric
{
private:
	// Rasheed-Larsen is specified by four parameters
	const real m_aParam;
	const real m_mParam;
	const real m_pParam;
	const real m_qParam;

public:
	// No default constructor allowed, must specify parameters
	RasheedLarsenMetric() = delete;

	// Constructor setting parameter a
	RasheedLarsenMetric(real mParam, real aParam, real pParam, real qParam, bool rLogScale = false);

	// The override of the basic metric getter functions
	TwoIndex getMetric_dd(const Point &p) const final;
	TwoIndex getMetric_uu(const Point &p) const final;

	// The override of the description string getter
	std::string getFullDescriptionStr() const final;
};

// Johanssen black hole metric (implementation by Seppe Staelens)
class JohannsenMetric final : public SphericalHorizonMetric
{
private:
	// Johannsen up to first order in deviation function is specified by five parameters (if M=1)
	const real m_aParam;
	const real m_alpha13Param;
	const real m_alpha22Param;
	const real m_alpha52Param;
	const real m_eps3Param;

public:
	// No default constructor allowed, must specify parameters
	JohannsenMetric() = delete;

	// Constructor setting parameter a
	JohannsenMetric(real aParam, real alpha13Param, real alpha22Param, real alpha52Param, real eps3Param, bool rLogScale = false);

	// The override of the basic metric getter functions
	TwoIndex getMetric_dd(const Point &p) const final;
	TwoIndex getMetric_uu(const Point &p) const final;

	// The override of the description string getter
	std::string getFullDescriptionStr() const final;
};

// Manko-Novikov metric (with angular momentum and M3 parameter turned on) (implementation by Seppe Staelens)
class MankoNovikovMetric final : public SphericalHorizonMetric
{
private:
	// Manko-Novikov metric with only alpha3 as symmetry breaking parameter
	const real m_aParam;
	const real m_alpha3Param;

	// These are convenient derived quantities from a
	const real m_alphaParam;
	const real m_kParam;

public:
	// No default constructor allowed, must specify parameters
	MankoNovikovMetric() = delete;

	// Constructor setting parameter a and alpha3
	MankoNovikovMetric(real aParam, real alpha3Param, bool rLogScale = false);

	// The override of the basic metric getter functions
	TwoIndex getMetric_dd(const Point &p) const final;
	TwoIndex getMetric_uu(const Point &p) const final;

	// The override of the description string getter
	std::string getFullDescriptionStr() const final;
};

// The Kerr metric in Kerr-Schild coordinates (normalized so that M = 1)
class KerrSchildMetric final : public SphericalHorizonMetric
{
private:
	// Rotation parameter for Kerr
	// Note that this should be between -1 and 1 since M=1
	const real m_aParam;

public:
	// No default constructor allowed, must specify a
	KerrSchildMetric() = delete;

	// Constructor setting parameter a
	KerrSchildMetric(real aParam, bool rLogScale = false);

	// The override of the basic metric getter functions
	TwoIndex getMetric_dd(const Point &p) const final;
	TwoIndex getMetric_uu(const Point &p) const final;

	// The override of the description string getter
	std::string getFullDescriptionStr() const final;
};

// Abstract base class for a metric with an arbitrary number of singularities (of arbitrary codimension)
class SingularityMetric : public Metric
{
public:
	// Constructor that initializes singularities
	SingularityMetric(std::vector<Singularity> thesings, bool rLogScale);

	// Getter functions for the two member variables
	std::vector<Singularity> getSingularities() const;

protected:
	// All singularities of the metric
	const std::vector<Singularity> m_AllSingularities;
};

// Ring fuzzball (implementation Lies Van Dael)
class ST3CrMetric final : public SingularityMetric
{
public:
	// Constructor which will be called to initialize all parameters of the metric
	ST3CrMetric(real P, real q0, real lambda, bool rlogscale = false);

	// The basic getter functions
	TwoIndex getMetric_dd(const Point &p) const final;
	TwoIndex getMetric_uu(const Point &p) const final;

	// The description string getter
	std::string getFullDescriptionStr() const final;

private:
	const real m_P;
	const real m_q0;
	const real m_lambda;

	real get_omega(real r, real theta, real l) const;
	real f_phi(real phi, real r, real theta, real l, real R) const;
	real f_om_phi(real phi, real r, real theta, real l, real R) const;
};

// Boson star with solitonic potential (sigma = 0.06, phi_c = 0.044) (implementation by Seppe Staelens)
class BosonStarMetric final : public Metric
{
public:
	// Simple (default) constructor is all that is needed
	BosonStarMetric(bool rLogScale = false);
	// The override of the basic metric getter functions
	TwoIndex getMetric_dd(const Point &p) const final;
	TwoIndex getMetric_uu(const Point &p) const final;
	// The override of the description string getter
	std::string getFullDescriptionStr() const final;

protected:
	// The spline interpolator for Phi
	tk::spline m_PhiSpline;
	// The spline interpolator for m
	tk::spline m_mSpline;
};

//// METRIC ADD POINT A ////
// Declare your new Metric class here, publically inheriting from the base class Metric
// (or SphericalHorizonMetric if your Metric has a horizon, or SingularityMetric if your Metric has other, arbitrary singularities)
// Give definitions (implementation) of these functions in Metric.cpp (or other source code file)
// Don't forget to set m_Symmetries appropriately (in the constructor),
// if your metric has any symmetry (e.g. stationarity, axisymmetry)!
// Sample code:
/*
class MyMetric final : public Metric // good practice to make the class final unless descendant classes are possible
{
public:
	// Constructor which will be called to initialize all parameters of the metric
	MyMetric(args...);

	// The basic getter functions
	// These MUST be implemented
	TwoIndex getMetric_dd(const Point& p) const final;
	TwoIndex getMetric_uu(const Point& p) const final;

	// The description string getter
	// This is optional (but recommended) to implement; if not implemented,
	// the base class Metric::getFullDescriptionStr() will be called instead
	std::string getFullDescriptionStr() const final;

private:
	// good practice to have all const params (initialized in the constructor)
	// since the metric cannot change after initialization
	// const params...;

};
*/
//// END METRIC ADD POINT A ////

#endif
