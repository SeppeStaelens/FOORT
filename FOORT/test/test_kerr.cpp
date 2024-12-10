#include <gtest/gtest.h>

#include "../src/Geometry.h" // basic tensor objects
#include "Metric.h"          // Metrics
#include "Diagnostics.h"     // Diagnostics
#include "Terminations.h"    // Termination conditions
#include "Geodesic.h"        // Geodesics (and Sources)
#include "Integrators.h"     // Integrator functions
#include "InputOutput.h"     // Output to screen and files

#include <iostream> // needed to open and load configuration file, if working in CONFIGURATION_MODE; also cout/cerr

TEST(Metric, bound_geodesic)
{
    //! This test computes the bound geodesic at r=3M in the Scwarzschild spacetime, and checks that it remains sufficiently
    //! bound, i.e. does not deviate significantly over the course of the integration.
    SetOutputLevel(OutputLevel::Level_0_WARNING);

    std::unique_ptr<Metric> theM = std::unique_ptr<Metric>(new KerrMetric(0., false, 1.));
    std::unique_ptr<Source> theS = std::unique_ptr<Source>(new NoSource(theM.get()));

    DiagBitflag AllDiags = Diag_FourColorScreen;
    DiagBitflag ValDiag = Diag_FourColorScreen;

    GeodesicPositionDiagnostic::DiagOptions =
        std::unique_ptr<GeodesicPositionOptions>(new GeodesicPositionOptions{5000, UpdateFrequency{1, false, false}});

    TermBitflag AllTerms = Term_BoundarySphere | Term_Horizon | Term_TimeOut;

    HorizonTermination::TermOptions =
        std::unique_ptr<HorizonTermOptions>(new HorizonTermOptions{
            dynamic_cast<SphericalHorizonMetric *>(theM.get())->getHorizonRadius(), false, 0.01, 1});
    BoundarySphereTermination::TermOptions =
        std::unique_ptr<BoundarySphereTermOptions>(new BoundarySphereTermOptions{10, false, 1});
    TimeOutTermination::TermOptions =
        std::unique_ptr<TimeOutTermOptions>(new TimeOutTermOptions{10, 1});

    GeodesicIntegratorFunc theIntegrator = Integrators::IntegrateGeodesicStep_RK4; // IntegrateGeodesicStep_RK4 or IntegrateGeodesicStep_Verlet
    Integrators::IntegratorDescription = "RK4";
    Integrators::epsilon = 0.03; // base step size that is used (is adapted dynamically)

    // Create one Geodesic instance per thread to work with
    Geodesic theGeod(theM.get(), theS.get(), // Metric and Source (non-owner pointers!)
                     AllDiags, ValDiag,      // Bitflags for Diagnostics
                     AllTerms,               // Bitflag for Terminations
                     theIntegrator);         // Function to use to integrate geodesic equation

    // Set up initial conditions for a geodesic
    Point initpos = {0.0, 3.0, pi / 2.0, 0.0};

    TwoIndex gdd = theM->getMetric_dd(initpos);
    OneIndex initvel = {-1.0 / sqrt(-gdd[0][0]), 0.0, 0.0, 1.0 / sqrt(gdd[3][3])};
    ScreenIndex scrindex = {0, 0};

    // Set the Geodesic to the current screen index and initial position/velocity
    theGeod.Reset(scrindex, initpos, initvel);

    // Loop integrating the geodesic step by step until finished
    while (theGeod.getTermCondition() == Term::Continue)
    {
        theGeod.Update();
    }

    Point current_pos = theGeod.getCurrentPos();
    double current_r = current_pos[1];
    EXPECT_NEAR(current_r, 3.0, 0.01);
    std::cout << "Final radial position: " << current_r << std::endl;
}
