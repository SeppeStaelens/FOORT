#include <gtest/gtest.h>
#include <iostream>
#include <fstream>

#include "Config.h"
#include "Geometry.h"
#include "Metric.h"       // Metrics
#include "Diagnostics.h"  // Diagnostics
#include "Terminations.h" // Termination conditions
#include "Geodesic.h"     // Geodesics (and Sources)
#include "Integrators.h"  // Integrator functions
#include "InputOutput.h"  // Output to screen and files

TEST(Metric, bound_geodesic)
{
    Point x = {1, 0, 0, 0};
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
        std::unique_ptr<TimeOutTermOptions>(new TimeOutTermOptions{1000, 1});

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

    std::ofstream r_phi_file;
    r_phi_file.open("r_phi_bound_geodesic.txt");
    if (!r_phi_file.is_open())
    {
        std::cerr << "Error opening file r_phi_bound_geodesic.txt" << std::endl;
        return;
    }
    else
    {
        std::cout << "success" << std::endl;
    }
    Point current_pos;
    double current_r, current_phi;
    // Loop integrating the geodesic step by step until finished
    while (theGeod.getTermCondition() == Term::Continue)
    {
        theGeod.Update();
        Point current_pos = theGeod.getCurrentPos();
        current_r = current_pos[1];
        current_phi = current_pos[3];
        r_phi_file << std::setprecision(15) << current_r << " " << current_phi << std::endl;
        //  << std::setprecision(15) << "Current radial position: " << current_r << " phi: " << current_phi << std::endl;
    }

    r_phi_file.close();
    current_pos = theGeod.getCurrentPos();
    current_r = current_pos[1];
    EXPECT_NEAR(current_r, 3.0, 0.1) << "Final radial position: " << current_r << std::endl;
}
