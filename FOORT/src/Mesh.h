#ifndef _FOORT_MESH_H
#define _FOORT_MESH_H

///////////////////////////////////////////////////////////////////////////////////////
////// MESH.H
////// Declarations of abstract base Mesh class and all its descendants.
////// All definitions in Mesh.cpp
///////////////////////////////////////////////////////////////////////////////////////

#include "Geometry.h"	 // needed for basic tensor objects
#include "Diagnostics.h" // needed for Diagnostic "value" and "distance" functions
#include "InputOutput.h" // needed for ScreenOutput

#include <cmath>		// needed for sqrt (only on Linux)
#include <utility>		// std::move
#include <forward_list> // std::forward_list
#include <memory>		// std::unique_ptr
#include <vector>		// std::vector
#include <array>		// std::array
#include <string>		// for strings

// Abstract Mesh base class
class Mesh
{
public:
	// Basic constructor constructs Diagnostic that is used for determinines "values" and "distances" between values
	Mesh(DiagBitflag valdiag)
		// Calling CreateDiagnosticVector in this way will create a vector with exactly one element in it,
		// i.e. the Diagnostic we need!
		: m_DistanceDiagnostic{std::move((CreateDiagnosticVector(valdiag, valdiag, nullptr))[0])}
	{
	}

	// virtual destructor to ensure correct destruction of descendants
	virtual ~Mesh() = default;

	// Getter for how many geodesics the Mesh currently wants to integrate in this iteration
	virtual largecounter getCurNrGeodesics() const = 0;

	// This sets a new initial conditions (in the form of a ScreenPoint and ScreenIndex)
	// for a next pixel to be integrated in the current iteration
	virtual void getNewInitConds(largecounter index, ScreenPoint &newunitpoint, ScreenIndex &newscreenindex) const = 0;

	// When a geodesic is finished integrating, it tells the Mesh and passes on its final "value"
	// NOTE: despite being a non-const member function, this must be designed to be thread-safe!
	virtual void GeodesicFinished(largecounter index, std::vector<real> finalValues) = 0;

	// This is called when the current iteration is finished. The Mesh can now evaluate whether to continue or not
	virtual void EndCurrentLoop() = 0;

	// Returns false if the Mesh wants another iteration of pixels to integrate
	virtual bool IsFinished() const = 0;

	// Returns a string description of the Mesh (spaces allowed), describing its options
	virtual std::string getFullDescriptionStr() const;

protected:
	// The Diagnostic (a const pointer to a const Diagnostic object) that is used to calculate
	// distances (using FinalDataValDistance()) between the "values" that are assigned to Geodesics
	const std::unique_ptr<const Diagnostic> m_DistanceDiagnostic;
};

// A simple square mesh that will integrate a square of evenly spaced pixels
class SimpleSquareMesh final : public Mesh
{
public:
	// Default constructor not possible
	SimpleSquareMesh() = delete;
	// Constructor initializes total number of pixels and passes valdiag to base constructor
	// Note that we static_cast the sqrt() to round off the row/column size to an integer number
	SimpleSquareMesh(largecounter totalPixels, DiagBitflag valdiag)
		: m_TotalPixels{static_cast<pixelcoord>(sqrt(totalPixels)) * static_cast<pixelcoord>(sqrt(totalPixels))},
		  m_RowColumnSize{static_cast<pixelcoord>(sqrt(totalPixels))},
		  Mesh(valdiag)
	{
		if constexpr (dimension != 4)
			ScreenOutput("SimpleSquareMesh only defined in 4D!", OutputLevel::Level_0_WARNING);
	}

	// Declarations of overriding virtual functions

	largecounter getCurNrGeodesics() const final;

	void getNewInitConds(largecounter index, ScreenPoint &newunitpoint, ScreenIndex &newscreenindex) const final;

	void GeodesicFinished(largecounter index, std::vector<real> finalValues) final;

	void EndCurrentLoop() final;

	bool IsFinished() const final;

	// Description string getter
	std::string getFullDescriptionStr() const final;

private:
	// Total amount of pixels in grid (is the square of m_RowColumnSize)
	const largecounter m_TotalPixels;
	// Amount of pixels per row or column (square grid)
	const pixelcoord m_RowColumnSize;
	// Are we done integrating or not?
	bool m_Finished{false};
};

// Mesh which integrates only certain user-inputted pixels
class InputCertainPixelsMesh : public Mesh
{
public:
	// Default constructor not possible
	InputCertainPixelsMesh() = delete;
	// Copy constructor not possible
	InputCertainPixelsMesh(const InputCertainPixelsMesh &) = delete;
	// Constructor given in Mesh.cpp file; constructor asks for input of pixels
	InputCertainPixelsMesh(largecounter totalPixels, DiagBitflag valdiag);

	// Declarations of overriding virtual functions

	largecounter getCurNrGeodesics() const final;

	void getNewInitConds(largecounter index, ScreenPoint &newunitpoint, ScreenIndex &newscreenindex) const final;

	void GeodesicFinished(largecounter index, std::vector<real> finalValues) final;

	void EndCurrentLoop() final;

	bool IsFinished() const final;

	// Description string getter
	std::string getFullDescriptionStr() const final;

private:
	// Total pixel size of the screen (square grid)
	const pixelcoord m_RowColumnSize;

	// How many pixels have been inputted in total, i.e. need integrating
	largecounter m_TotalPixels{0};
	// All pixels' location
	std::vector<ScreenIndex> m_PixelsToIntegrate{};
	// Are we finished integrating?
	bool m_Finished{false};
};

// Adaptive subdivision Mesh: starts with evenly spaced, square Mesh,
// then decides to subdivide certain squares of pixels into smaller squares,
// based on which pixels have a bigger "weight", which is defined as the maximum
// "distance" (using the Diagnostic value distance) between the upper-left
// vertex of the square with the other three vertices of the square.
class SquareSubdivisionMesh : public Mesh
{
public:
	// default constructor not possible
	SquareSubdivisionMesh() = delete;
	// Constructor must be called with arguments:
	// - maxPixels: max. nr of pixels that can be integrated in TOTAL, over all iterations (if 0, then this is infinite,
	// i.e. we keep integrating until all squares are maximally subdivided or have weight 0)
	// - initialPixels: initial number of pixels to integrate (spaced equally over the screen)
	// - maxSubdivide: maximum number of times that we can subdivide squares (note: 1 denotes the
	// initial grid, so 2 would be subdividing the squares once)
	// - iterationPixels: maximum number of pixels to subdivide in each integration iteration
	// (max number of pixels that will be integrates is then 5*iterationPixels)
	// - initialSubToFinal: once we decide to subdivide a square, do we automatically keep subdividing it
	// until we reach maxSubdivision?
	// - valdiag: the "value" and "distance" Diagnostic to use
	SquareSubdivisionMesh(largecounter maxPixels, largecounter initialPixels, int maxSubdivide, largecounter iterationPixels, bool initialSubToFinal,
						  DiagBitflag valdiag)
		: m_InitialPixels{static_cast<pixelcoord>(sqrt(initialPixels)) * static_cast<pixelcoord>(sqrt(initialPixels))},
		  m_MaxSubdivide{maxSubdivide},
		  m_RowColumnSize{static_cast<pixelcoord>((sqrt(initialPixels) - 1) * ExpInt(2, maxSubdivide - 1) + 1)},
		  m_PixelsLeft{maxPixels}, m_MaxPixels{maxPixels}, m_InfinitePixels{maxPixels == 0}, m_IterationPixels{iterationPixels},
		  m_InitialSubDividideToFinal{initialSubToFinal}, Mesh(valdiag)
	{
		if constexpr (dimension != 4)
			ScreenOutput("SquareSubdivisionMesh only defined in 4D!", OutputLevel::Level_0_WARNING);

		// DEBUG message for constructor (can delete)
		ScreenOutput("SquareSubdivisionMesh constructed: maxPixels: " + (m_InfinitePixels ? "infinite" : std::to_string(maxPixels)) + "; m_InitialPixels: " + std::to_string(m_InitialPixels) + "; m_RowColumnSize: " + std::to_string(m_RowColumnSize), OutputLevel::Level_4_DEBUG);

		// Initialize the initial square, equally spaced grid
		InitializeFirstGrid();
	}

	// Declarations of overriding virtual functions

	largecounter getCurNrGeodesics() const final;

	void getNewInitConds(largecounter index, ScreenPoint &newunitpoint, ScreenIndex &newscreenindex) const final;

	void GeodesicFinished(largecounter index, std::vector<real> finalValues) final;

	void EndCurrentLoop() final;

	bool IsFinished() const final;

	// Description string getter
	std::string getFullDescriptionStr() const final;

private:
	// How many initial pixels (spread uniformly over the grid) do we integrate?
	const largecounter m_InitialPixels;
	// How many times are we allowed  to subdivide a square? Note: the initial grid is already at 1
	const int m_MaxSubdivide;
	// The total size in pixels of a row or column (square grid)
	const pixelcoord m_RowColumnSize;
	// How many pixels per iteration can we subdivide?
	const largecounter m_IterationPixels;
	// How many pixels can we integrate in total over all iterations?
	const largecounter m_MaxPixels;
	// If we decide to subdivide a square, do we automatically subdivide it further to the max level?
	const bool m_InitialSubDividideToFinal;
	// Are we allowed to integrate as many pixels as we want? (m_MaxPixels == 0)
	const bool m_InfinitePixels;

	// How many pixels are we still allowed to integrate (if !m_InfinitePixels)?
	largecounter m_PixelsLeft;

	// A struct the Mesh uses to keep all information about a given pixel
	struct PixelInfo
	{
		// Constructor with its ScreenIndex and current subdivision level
		PixelInfo(ScreenIndex ind, int subdiv) : Index{ind}, SubdivideLevel{subdiv} {}

		// The pixel's screenindex
		ScreenIndex Index{};

		// The level at which the pixel has been subdivided
		// Note: initial grid pixels are at 1; pixels at 0 are pixels that cannot be subdivided
		// (for example, at the right or lower edges)
		int SubdivideLevel{};

		// Weight of the pixel: if negative, this signifies that it needs to be updated/calculated!
		// The weight is determined as the max of the distance (as calculated by the value Diagnostic)
		// between its values and those of its right, lower, and right-lower neighbors.
		real Weight{-1};

		// The values associated to this pixel (as calculated by the value Diagnostic)
		std::vector<real> DiagValue{};

		// Where its lower and right neighbors are located in m_AllPixels
		// Note: the pixel with index 0 is (0,0) and can never be the lower or right neighbor of any other pixel!
		largecounter LowerNbrIndex{0};
		largecounter RightNbrIndex{0};
	};
	// The current queue of pixels to be integrated
	std::vector<PixelInfo> m_CurrentPixelQueue{};
	// A bool for every pixel in the current queue: gets set to true when the pixel is done integrating and gets its values returned
	std::vector<bool> m_CurrentPixelQueueDone{};
	// All pixels that have been integrated already (so does not include the pixels in the current queue)
	std::vector<PixelInfo> m_AllPixels{};

	// Initializes the first nxn screen in m_CurrentPixelQueue
	void InitializeFirstGrid();

	// Updates the neighbors of all pixels (that should have neighbors and don't yet) in m_AllPixels
	void UpdateAllNeighbors();

	// Updates all weights of the pixels in m_AllPixels with weight < 0 and subdiv > 0 and subdiv < m_MaxSubdivide
	// Assumes all squares have neighbors assigned correctly
	void UpdateAllWeights();

	// This will take the pixel m_AllPixels[ind] and subdivide it,
	// adding up to <=5 pixels to the CurrentPixelQueue
	void SubdivideAndQueue(largecounter ind);

	// Helper function to exponentiate an int to an int
	// Note: the result can be larger than fits in an int, but the base is always 2 and the exp is always
	// a number <=m_MaxSubdivide (which is an int)
	pixelcoord ExpInt(int base, int exp);
};

// Adaptive subdivision Mesh: starts with evenly spaced, square Mesh,
// then decides to subdivide certain squares of pixels into smaller squares,
// based on which pixels have a bigger "weight", which is defined as the maximum
// "distance" (using the Diagnostic value distance) between the upper-left
// vertex of the square with the other three vertices of the square.
// V2: new way of dealing with neighbors and looping over pixels
class SquareSubdivisionMeshV2 : public Mesh
{
public:
	// default constructor not possible
	SquareSubdivisionMeshV2() = delete;
	// Constructor must be called with arguments:
	// - maxPixels: max. nr of pixels that can be integrated in TOTAL, over all iterations (if 0, then this is infinite,
	// i.e. we keep integrating until all squares are maximally subdivided or have weight 0)
	// - initialPixels: initial number of pixels to integrate (spaced equally over the screen)
	// - maxSubdivide: maximum number of times that we can subdivide squares (note: 1 denotes the
	// initial grid, so 2 would be subdividing the squares once)
	// - iterationPixels: maximum number of pixels to subdivide in each integration iteration
	// (max number of pixels that will be integrates is then 5*iterationPixels)
	// - initialSubToFinal: once we decide to subdivide a square, do we automatically keep subdividing it
	// until we reach maxSubdivision?
	// - valdiag: the "value" and "distance" Diagnostic to use
	SquareSubdivisionMeshV2(largecounter maxPixels, largecounter initialPixels, int maxSubdivide, largecounter iterationPixels, bool initialSubToFinal,
							DiagBitflag valdiag)
		: m_InitialPixels{static_cast<pixelcoord>(sqrt(initialPixels)) * static_cast<pixelcoord>(sqrt(initialPixels))},
		  m_MaxSubdivide{maxSubdivide},
		  m_RowColumnSize{static_cast<pixelcoord>((sqrt(initialPixels) - 1) * ExpInt(2, maxSubdivide - 1) + 1)},
		  m_PixelsLeft{maxPixels}, m_MaxPixels{maxPixels}, m_InfinitePixels{maxPixels == 0}, m_IterationPixels{iterationPixels},
		  m_InitialSubDividideToFinal{initialSubToFinal}, Mesh(valdiag)
	{
		if constexpr (dimension != 4)
			ScreenOutput("SquareSubdivisionMeshV2 only defined in 4D!", OutputLevel::Level_0_WARNING);

		// Initialize the initial square, equally spaced grid
		InitializeFirstGrid();
	}

	// Declarations of overriding virtual functions

	largecounter getCurNrGeodesics() const final;

	void getNewInitConds(largecounter index, ScreenPoint &newunitpoint, ScreenIndex &newscreenindex) const final;

	void GeodesicFinished(largecounter index, std::vector<real> finalValues) final;

	void EndCurrentLoop() final;

	bool IsFinished() const final;

	// Description string getter
	std::string getFullDescriptionStr() const final;

private:
	// How many initial pixels (spread uniformly over the grid) do we integrate?
	const largecounter m_InitialPixels;
	// How many times are we allowed  to subdivide a square? Note: the initial grid is already at 1
	const int m_MaxSubdivide;
	// The total size in pixels of a row or column (square grid)
	const pixelcoord m_RowColumnSize;
	// How many pixels per iteration can we subdivide?
	const largecounter m_IterationPixels;
	// How many pixels can we integrate in total over all iterations?
	const largecounter m_MaxPixels;
	// If we decide to subdivide a square, do we automatically subdivide it further to the max level?
	const bool m_InitialSubDividideToFinal;
	// Are we allowed to integrate as many pixels as we want? (m_MaxPixels == 0)
	const bool m_InfinitePixels;

	// How many pixels are we still allowed to integrate (if !m_InfinitePixels)?
	largecounter m_PixelsLeft;

	// How many pixels we have integrated so far
	largecounter m_PixelsIntegrated{0};

	// A struct the Mesh uses to keep all information about a given pixel
	struct PixelInfo
	{
		// Constructor with its ScreenIndex and current subdivision level
		PixelInfo(ScreenIndex ind, int subdiv) : Index{ind}, SubdivideLevel{subdiv} {}

		// The pixel's screenindex: this gets set by the constructor and cannot change anymore
		const ScreenIndex Index{};

		// The level at which the pixel has been subdivided
		// Note: initial grid pixels are at 1; pixels at 0 are pixels that cannot be subdivided
		// (for example, at the right or lower edges)
		int SubdivideLevel{};

		// Weight of the pixel: if negative, this signifies that it needs to be updated/calculated!
		// The weight is determined as the max of the distance (as calculated by the value Diagnostic)
		// between its values and those of its right, lower, and right-lower neighbors.
		real Weight{-1};

		// The values associated to this pixel (as calculated by the value Diagnostic)
		std::vector<real> DiagValue{};

		// Pointers to its neighbors
		PixelInfo *LeftNbr{nullptr};
		PixelInfo *RightNbr{nullptr};
		PixelInfo *UpNbr{nullptr};
		PixelInfo *DownNbr{nullptr};
		PixelInfo *SEdiagNbr{nullptr};
	};

	// Master list of all pixels
	// std::forward_list is more space-efficient than std::list, bidirectional iteration is not needed, no random access supported
	// This list is only used to store the owner pointers (and thus the objects) of all pixels.
	// The other pixel vectors are used to iterate through (and need random access)
	std::forward_list<std::unique_ptr<PixelInfo>> m_AllPixels{};

	// List of active pixels, i.e. those that can be subdivided and have non-zero weight
	std::vector<PixelInfo *> m_ActivePixels{};
	// List of current queue of pixels to be sent to be integrated
	std::vector<PixelInfo *> m_CurrentPixelQueue{};
	// A bool for every pixel in the current queue: gets set to true when the pixel is done integrating and gets its values returned
	std::vector<bool> m_CurrentPixelQueueDone{};
	// List of pixels that are already integrated but need updating weights after current queue is all integrated
	std::vector<PixelInfo *> m_CurrentPixelUpdating{};

	// Initializes the first nxn screen and puts them in m_CurrentPixelQueue
	void InitializeFirstGrid();

	// Updates all weights of the pixels in m_CurrentPixelUpdating;
	// these should have subdiv > 0 and subdiv < m_MaxSubdivide, and all their neigbors assigned correctly
	// All pixels with weight > 0 will be added to m_ActivePixels
	void UpdateAllWeights();

	// Helper functions that return the appropriate neighbor of p, ONLY if this neighbor exists at the subdivision level specified
	// Returns nullptr otherwise; also return nullptr if p==nullptr
	PixelInfo *GetUp(PixelInfo *p, int subdiv) const;
	PixelInfo *GetDown(PixelInfo *p, int subdiv) const;
	PixelInfo *GetRight(PixelInfo *p, int subdiv) const;
	PixelInfo *GetLeft(PixelInfo *p, int subdiv) const;

	// This will take the pixel m_AllPixels[ind] and subdivide it,
	// adding up to <=5 pixels to the CurrentPixelQueue
	void SubdivideAndQueue(largecounter ind);

	// Helper function to exponentiate an int to an int
	// Note: the result can be larger than fits in an int, but the base is always 2 and the exp is always
	// a number <=m_MaxSubdivide (which is an int)
	pixelcoord ExpInt(int base, int exp) const;
};

#endif
