///
/// @file
/// @brief Contains the Field Cooling program
///
/// @details Performs a time series with decreasing temperature
///
/// @section License
/// Use of this code, either in source or compiled form, is subject to license from the authors.
/// Copyright \htmlonly &copy \endhtmlonly Richard Evans, 2009-2010. All Rights Reserved.
///
/// @section info File Information
/// @author  Richard Evans, richard.evans@york.ac.uk
/// @version 1.0
/// @date    30/03/2011
/// @internal
///	Created:		30/03/2011
///	Revision:	--
///=====================================================================================
///

// Standard Libraries
#include <iostream>

// Vampire Header files
#include "atoms.hpp"
#include "errors.hpp"
#include "material.hpp"
#include "program.hpp"
#include "random.hpp"
#include "sim.hpp"
#include "stats.hpp"
#include "vio.hpp"
#include "vmath.hpp"
#include "vmpi.hpp"

namespace program{

/// @brief Function to calculate the a field cooled magnetisation
///
/// @section License
/// Use of this code, either in source or compiled form, is subject to license from the authors.
/// Copyright \htmlonly &copy \endhtmlonly Richard Evans, 2009-2010. All Rights Reserved.
///
/// @section Information
/// @author  Richard Evans, richard.evans@york.ac.uk
/// @version 1.0
/// @date    30/03/2011
///
/// @internal
///	Created:		30/03/2011
///	Revision:	--
///=====================================================================================
///
void field_cool(){

	// check calling of routine if error checking is activated
	if(err::check==true){std::cout << "program::field_cool has been called" << std::endl;}

	// Perform several runs if desired
	for(int run=0;run<sim::runs; run++){

		// Set equilibration temperature and field
		sim::temperature=sim::Teq;
		
		// Equilibrate system
		while(sim::time<sim::equilibration_time){
			
			sim::integrate(sim::partial_time);
			
			// Calculate magnetisation statistics
			stats::mag_m();
			
			// Output data
			vout::data();
		}
		
		int start_time=sim::time;
		
		// Perform Field Cooling
		while(sim::time<sim::total_time+start_time){

			// Calculate Temperature using desired cooling function
			double time_from_start=mp::dt_SI*double(sim::time-start_time);
			switch(sim::cooling_function_flag){
				case 0:{ // exponential
					sim::temperature = sim::Tmin + (sim::Tmax-sim::Tmin)*exp(-time_from_start/sim::cooling_time);
				}
				break;
				case 1:{ // gaussian
					sim::temperature = sim::Tmin + (sim::Tmax-sim::Tmin)*exp(-(time_from_start)*(time_from_start)/((sim::cooling_time)*(sim::cooling_time)));
				}
				break;
				case 2:{ // double-gaussian
					double centre_time = 3.0*sim::cooling_time;
					double time_from_centre=mp::dt_SI*double(sim::time-start_time)-centre_time;
					sim::temperature = sim::Tmin + (sim::Tmax-sim::Tmin)*exp(-(time_from_centre)*(time_from_centre)/((sim::cooling_time)*(sim::cooling_time)));
				}
				break;
				case 3:{ // linear
					double Tlinear = sim::Tmax-(sim::Tmax-sim::Tmin)*(time_from_start/sim::cooling_time);
					if(Tlinear>=sim::Tmin) sim::temperature = Tlinear;
					else sim::temperature= sim::Tmin;
				}
				break;
			}
			
			// Integrate system
			sim::integrate(sim::partial_time);
			
			// Calculate magnetisation statistics
			stats::mag_m();

			// Output data
			vout::data();

		}
		
	} // end of run loop
	
} // end of field_cool()

}//end of namespace program