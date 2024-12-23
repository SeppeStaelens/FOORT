#ifndef _FOORT_TERMINATIONS_H
#define _FOORT_TERMINATIONS_H

///////////////////////////////////////////////////////////////////////////////////////
////// TERMINATIONS.H
////// Declarations of abstract base Termination class and all its descendants.
////// All definitions in Terminations.cpp
///////////////////////////////////////////////////////////////////////////////////////

#include "Geometry.h" // for basic tensor objects

#include <cstdint> // for std::uint16_t
#include <memory>  // for std::unique_ptr
#include <vector>  // for std::vector
#include <utility> // for std::pair

// Forward declaration of Geodesic class needed here, since Diagnostics are passed a pointer to their owner Geodesic
// (note "Geodesic.h" is NOT included to avoid header loop, and we do not need Geodesic member functions here!)
class Geodesic;

/////////////////////////
//// TERMINATION BITFLAGS
// Used for constructing vector of Terminations
// Note that this means every Termination is either "on" or "off";
// it is not possible to have a Termination "on" more than once
using TermBitflag = std::uint16_t;

// Define a bitflag per existing Termination
constexpr TermBitflag Term_None{0b0000'0000'0000'0000};
constexpr TermBitflag Term_BoundarySphere{0b0000'0000'0000'0001};
constexpr TermBitflag Term_TimeOut{0b0000'0000'0000'0010};
constexpr TermBitflag Term_Horizon{0b0000'0000'0000'0100};
constexpr TermBitflag Term_ThetaSingularity{0b0000'0000'0000'1000};
constexpr TermBitflag Term_NaN{0b0000'0000'0001'0000};
constexpr TermBitflag Term_GeneralSingularity{0b0000'0000'0010'0000};

//// TERMINATION ADD POINT B1 ////
// Add a TermBitflag for your new Termination. Make sure you use a bitflag that has not been used before!
// Sample code:
/*
constexpr TermBitflag Term_MyTerm				{ 0b0000'0000'0000'1000 };
*/
//// END TERMINATION ADD POINT B1 ////

////////////////////////////
//// TERMINATION CONDITIONS

// Possible termination conditions that can be set by Terminations
enum class Term
{
	Uninitialized = -1, // Geodesic has not been properly initialized yet with initial position/velocity
	Continue = 0,		// All is right, continue integrating geodesic
	Horizon,			// STOP, encountered horizon (set by HorizonTermination)
	BoundarySphere,		// STOP, encountered boundary sphere (set by BoundarySphereTermination)
	TimeOut,			// STOP, taken too many steps (set by TimeOutTermination)
	ThetaSingularity,	// STOP, too close to polar coordinate singularity (theta = 0 or theta = pi/2)
	NaN,				// STOP, NaN encountered in geodesic position or velocity
	GeneralSingularity, // STOP, singularity encountered (of any codimension)

	//// TERMINATION ADD POINT B2 ////
	// Add a new Termination condition that your new Termination can set
	// Sample code:
	/*
	MyTermCond,			// STOP, encountered (...)
	*/
	//// END TERMINATION ADD POINT B2 ////

	Maxterms // Number of termination conditions that exist
};

//////////////////////////////////////////////////////////////
//// GENERAL DECLARATIONS OF AND WITH ABSTRACT BASE CLASS ////

// Abstract base class for all Terminations
class Termination
{
public:
	// Constructor must initialize the pointer to its owner Geodesic
	Termination() = delete;
	Termination(Geodesic *const theGeodesic) : m_OwnerGeodesic{theGeodesic}
	{
	}

	// Resets Termination object. This is called when the owner Geodesic is reset in order to start integrating
	// a new geodesic.
	// The base class implementation only resets m_StepsSinceUpdated
	// Descendants can override this if they need to reset additional internal variables
	virtual void Reset();

	// virtual destructor to ensure correct destruction of descendants
	virtual ~Termination() = default;

	// Function that is called to determine whether Termination wants to
	// terminate the Geodesic. Returns Term::Continue if no termination wanted,
	// otherwise it returns the appropriate Term condition
	virtual Term CheckTermination() = 0;

	// This returns the full description of the Termination
	virtual std::string getFullDescriptionStr() const = 0;

protected:
	// The geodesic that owns the Termination (a const pointer to the Geodesic)
	Geodesic *const m_OwnerGeodesic;

	// Helper function to decide if the Termination should indeed update its status, based on
	// UpdateNSteps (which is set to 0 if we always update)
	bool DecideUpdate(largecounter UpdateNSteps);

	// The termination is itself in charge of keeping track of how many steps it has been since it has been updated
	// The Termination's TerminationOptions struct tells it how many steps it needs to wait between updates
	largecounter m_StepsSinceUpdated{};
};

// The owner vector of derived Termination classes
using TerminationUniqueVector = std::vector<std::unique_ptr<Termination>>;

// Helper to create a new vector of Termination options, based on the bitflag
TerminationUniqueVector CreateTerminationVector(TermBitflag termflags, Geodesic *const theGeodesic);

///////////////////////////////////////////////////////
//// DECLARATIONS FOR DERIVED TERMINATION CLASSES  ////

// Forward declaration needed before Termination
struct HorizonTermOptions;
// Horizon termination: terminate geodesics if they get too close to the horizon (returns Term::Horizon)
class HorizonTermination final : public Termination
{
public:
	// Basic constructor only passes on Geodesic pointer to base class constructor
	HorizonTermination(Geodesic *const theGeodesic) : Termination(theGeodesic) {}

	// Check if we are too close to the horizon
	Term CheckTermination() final;

	// Description string
	std::string getFullDescriptionStr() const final;

	// Options (contains horizon radius, if we are using logarithmic r coordinate, and distance allowed from the horizon)
	static std::unique_ptr<HorizonTermOptions> TermOptions;
};

// Forward declaration needed before Termination
struct BoundarySphereTermOptions;
// The Boundary Sphere: this terminates the geodesic (and returns Term::BoundarySphere) if
// the geodesic reaches outside of the boundary sphere
class BoundarySphereTermination final : public Termination
{
public:
	// Basic constructor only passes on Geodesic pointer to base class constructor
	BoundarySphereTermination(Geodesic *const theGeodesic) : Termination(theGeodesic) {}

	// Check if we have passed the boundary sphere
	Term CheckTermination() final;

	// Description string
	std::string getFullDescriptionStr() const final;

	// The options that the BoundarySphereTermination keeps (contains the radius of the boundary sphere)
	static std::unique_ptr<BoundarySphereTermOptions> TermOptions;
};

// Forward declaration needed before Termination
struct TimeOutTermOptions;
// The Time Out: this terminates the geodesic if too many steps have been
// taken in its integration (and returns Term::TimeOut)
class TimeOutTermination final : public Termination
{
public:
	// Basic constructor only passes on Geodesic pointer to base class constructor
	TimeOutTermination(Geodesic *const theGeodesic) : Termination(theGeodesic) {}

	// This descendant needs to override Reset in order to also reset m_CurNrSteps
	void Reset() final;

	// Check if we have already taken too many steps
	Term CheckTermination() final;

	// Description string
	std::string getFullDescriptionStr() const final;

	// The options that the TimeOutTermination keeps (contains max number of steps allowed)
	static std::unique_ptr<TimeOutTermOptions> TermOptions;

private:
	// Keep track of the number of steps that the geodesic has taken so far
	largecounter m_CurNrSteps{0};
};

// Forward declaration needed before Termination
struct ThetaSingularityTermOptions;
class ThetaSingularityTermination final : public Termination
{
public:
	ThetaSingularityTermination(Geodesic *const theGeodesic) : Termination(theGeodesic) {}

	// No override of Reset() necessary

	// Check the specific termination condition
	Term CheckTermination() final;

	// Description string
	std::string getFullDescriptionStr() const final;

	// The options that the Termination keeps (will probably be a descendant struct instead, which specifies
	// any additional options the Termination needs)
	static std::unique_ptr<ThetaSingularityTermOptions> TermOptions;
};

// Forward declaration needed before Termination
struct NaNTermOptions;
// NaN termination: terminate geodesics if position or velocity contains a nan
class NaNTermination final : public Termination
{
public:
	// Basic constructor only passes on Geodesic pointer to base class constructor
	NaNTermination(Geodesic *const theGeodesic) : Termination(theGeodesic) {}

	// Check if we are too close to the horizon
	Term CheckTermination() final;

	// Description string
	std::string getFullDescriptionStr() const final;

	// Options (contains horizon radius, if we are using logarithmic r coordinate, and distance allowed from the horizon)
	static std::unique_ptr<NaNTermOptions> TermOptions;
};

// Forward declaration needed before Termination
struct GeneralSingularityTermOptions;
// General singularity: terminate geodesic if it comes too close to one of a given number of (arbitrary codimension) singularities
class GeneralSingularityTermination final : public Termination
{
public:
	// Basic constructor only passes on Geodesic pointer to base class constructor
	GeneralSingularityTermination(Geodesic *const theGeodesic) : Termination(theGeodesic) {}

	// Check if we are too close to the horizon
	Term CheckTermination() final;

	// Description string
	std::string getFullDescriptionStr() const final;

	// Options (contains horizon radius, if we are using logarithmic r coordinate, and distance allowed from the horizon)
	static std::unique_ptr<GeneralSingularityTermOptions> TermOptions;

private:
	std::string SingularityToString(int singnr) const;
};

//// TERMINATION ADD POINT A1 /////
// Declare your Termination class here, inheriting from Termination.
// Sample code:
/*
// Forward declaration needed before Termination
struct TerminationOptions; // possibly will instead need to declare descendant options struct
class MyTermination final : public Termination // good practice to make the class final unless descendant classes are possible
{
public:
	// Constructor must at least pass on Geodesic pointer to base class constructor
	MyTermination(Geodesic* const theGeodesic) : Termination(theGeodesic) {}

	// Do you need to reset any internal variables specific to MyTermination? If so, override Reset() (This is not mandatory)
	// Note: make sure to call the base class implementation Termination::Reset()
	// from within your implementation of MyTermination::Reset(), so that the base class internal variable is also reset!
	void Reset() final;

	// Check the specific termination condition
	Term CheckTermination() final;

	// Description string
	std::string getFullDescriptionStr() const final;

	// The options that the Termination keeps (will probably be a descendant struct instead, which specifies
	// any additional options the Termination needs)
	static std::unique_ptr<TerminationOptions> TermOptions;

private:
	// any private member variables that are needed to keep track of things
};
*/
//// END TERMINATION ADD POINT A1 ////

////////////////////////////////////////
//// ALL TERMINATIONOPTIONS STRUCTS ////

// Base class for TerminationOptions. Other Terminations can inherit from here if they require more options.
struct TerminationOptions
{
public:
	// Basic constructor only sets the number of steps between updates
	TerminationOptions(largecounter Nsteps) : UpdateEveryNSteps{Nsteps}
	{
	}

	// virtual destructor to ensure correct destruction of descendants
	virtual ~TerminationOptions() = default;

	const largecounter UpdateEveryNSteps;
};

// Options class for HorizonTermination; keeps track of location of horizon radius and the epsilon to terminate away from the horizon
struct HorizonTermOptions : public TerminationOptions
{
public:
	HorizonTermOptions(real theHorizonRadius, bool therLogScale, real theAtHorizonEps, largecounter Nsteps) : HorizonRadius{theHorizonRadius}, AtHorizonEps{theAtHorizonEps}, rLogScale{therLogScale}, TerminationOptions(Nsteps)
	{
	}

	const real HorizonRadius;
	const real AtHorizonEps;
	const bool rLogScale;
};

// Options class for BoundarySphere; has to keep track of the BoundarySphere's radius
struct BoundarySphereTermOptions : public TerminationOptions
{
public:
	BoundarySphereTermOptions(real theRadius, bool therLogScale, largecounter Nsteps) : SphereRadius{theRadius}, rLogScale{therLogScale},
																						TerminationOptions(Nsteps)
	{
	}

	const real SphereRadius;
	const bool rLogScale;
};

// Options class for TimeOut; has to keep track of the max. number of integration steps allowed
struct TimeOutTermOptions : public TerminationOptions
{
public:
	TimeOutTermOptions(largecounter MaxStepsAllowed, largecounter Nsteps) : MaxSteps{MaxStepsAllowed}, TerminationOptions(Nsteps)
	{
	}

	const largecounter MaxSteps;
};

// Options class for ThetaSingularityTermination
struct ThetaSingularityTermOptions : public TerminationOptions
{
public:
	ThetaSingularityTermOptions(real epsilon, largecounter Nsteps) : ThetaSingEpsilon{epsilon}, TerminationOptions(Nsteps)
	{
	}

	const real ThetaSingEpsilon;
};

// Options class for TimeOut; has to keep track of the max. number of integration steps allowed
struct NaNTermOptions : public TerminationOptions
{
public:
	NaNTermOptions(bool consoleoutputon, largecounter Nsteps) : OutputToConsole{consoleoutputon}, TerminationOptions(Nsteps)
	{
	}

	const bool OutputToConsole;
};

struct GeneralSingularityTermOptions : public TerminationOptions
{
public:
	GeneralSingularityTermOptions(std::vector<Singularity> sings,
								  real eps, bool consoleoutputon, bool therlogscale, largecounter Nsteps)
		: Singularities{std::move(sings)}, Epsilon{eps}, OutputToConsole{consoleoutputon},
		  rLogScale{therlogscale},
		  TerminationOptions(Nsteps) {}

	const std::vector<Singularity> Singularities;
	const real Epsilon;
	const bool OutputToConsole;
	const bool rLogScale;
};

//// TERMINATION ADD POINT A2 ////
// Add your new TerminationOptions struct here, inheriting from TerminationOptions (if needed)
// Sample code:
/*
struct MyTermOptions : public TerminationOptions
{
public:
	MyTermOptions(..., largecounter Nsteps) : TerminationOptions(Nsteps) //, other initialization
	{}

	// member variables (const!) here
};
*/
//// END TERMINATION ADD POINT A2 ////

#endif
