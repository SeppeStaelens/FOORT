#ifndef _FOORT_METRIC_H
#define _FOORT_METRIC_H

#include<vector>

#include"Geometry.h"
#include"InputOutput.h"

// The abstract base class for all Metrics.
class Metric
{
public:
	virtual ~Metric() = default;
	// Get the metric, indices down
	virtual TwoIndex getMetric_dd(const Point& p) const = 0;	
	// Get the metric, indices up
	virtual TwoIndex getMetric_uu(const Point& p) const = 0;	

	// These return the Christoffel and other derivative quantities of the metric. They are computed for any
	// metric, BUT are left as virtual functions to allow for other metrics to implement their own (more efficient)
	// way of calculating them, if so desired.
	// 
	// Get the Christoffel symbol, indices up-down-down
	virtual ThreeIndex getChristoffel_udd(const Point& p) const;
	// Get the Riemann tensor, indices up-down-down-down
	virtual FourIndex getRiemann_uddd(const Point& p) const;
	// Get the Kretschmann scalar
	virtual real getKretschmann(const Point& p) const;
protected:
	std::vector<int> m_Symmetries{};
};


// The type of metric that has a spherical horizon
class SphericalHorizonMetric : public Metric
{
protected:
	// Radius of the horizon
	const real m_HorizonRadius;
	// Are we using a logarithmic r coordinate?
	const bool m_rLogScale;
public:
	// No default construction allowed, must specify horizon radius
	SphericalHorizonMetric() = delete;
	// Constructor that initializes radius and distance to horizon necessary for termination
	SphericalHorizonMetric(real HorizonRadius, bool rLogScale);

	real getHorizonRadius() const;
	bool getrLogScale() const;
};



// The Kerr metric
class KerrMetric final : public SphericalHorizonMetric
{
private:
	const real m_aParam;

public:

	// No constructor without passing parameters, and no copies allowed
	KerrMetric() = delete;
	KerrMetric(const KerrMetric& other) = delete;
	KerrMetric& operator=(const KerrMetric& other) = delete;

	// Constructor setting paramater a
	KerrMetric(real aParam=0.0, bool rLogScale=false);

	TwoIndex getMetric_dd(const Point& p) const final;
	TwoIndex getMetric_uu(const Point& p) const final;
};

// Flat space in spherical coordinates (4D)
class FlatSpaceMetric final : public Metric
{
public:
	// Simple (default) constructor is all that is needed
	FlatSpaceMetric();

	TwoIndex getMetric_dd(const Point& p) const final;
	TwoIndex getMetric_uu(const Point& p) const final;
};



#endif