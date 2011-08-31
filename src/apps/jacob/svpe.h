/*
  This file is part of MADNESS.

  Copyright (C) 2007,2010 Oak Ridge National Laboratory

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

  For more information please contact:

  Robert J. Harrison
  Oak Ridge National Laboratory
  One Bethel Valley Road
  P.O. Box 2008, MS-6367

  email: harrisonrj@ornl.gov
  tel:   865-241-3937
  fax:   865-572-0680


  $Id$
*/

/// \file svpe.h
#ifndef MADNESS_SVPE_H
#define MADNESS_SVPE_H
#include <mra/mra.h>
#include <mra/lbdeux.h>
#include <misc/ran.h>
#include <linalg/solvers.h>
#include <ctime>
#include <list>
#include <mra/sdf_shape_3D.h>
#include <mra/funcplot.h> //jacob added
#include <constants.h> //jacob added
#include <vector>  //jacob added

typedef real_function_3d realfunc;
typedef real_functor_3d realfunct;
//The screen solver, This is the solver being used. It is faster than VolumeSolventPotential
class ScreenSolventPotential {
private:
  World& world;
  double& sigma;
  double& epsilon_1;
  double& epsilon_2;
  int& maxiter;
  std::vector<double>& atomic_radii;
  std::vector< madness::Vector<double,3> > atomic_coords;
  vector_real_function_3d dlog;
  realfunc rdielectric;
  realfunc surface;
public:
  //constructor                                                                                                                                           
 ScreenSolventPotential(World& world,
			double& sigma,
			double& epsilon_1,
			double& epsilon_2,
			int& maxiter,
			std::vector<double>& atomic_radii,
			std::vector< madness::Vector<double,3> > atomic_coords)
   :world(world)
    ,sigma(sigma)
    ,epsilon_1(epsilon_1)
    ,epsilon_2(epsilon_2)
    ,maxiter(maxiter)
    ,atomic_radii(atomic_radii)
    ,atomic_coords(atomic_coords)
    ,dlog(3) {
   // Functors for mask related quantities                                                                                                             
   realfunct rdielectric_functor(new MolecularVolumeExponentialSwitchReciprocal(sigma, epsilon_1, epsilon_2, atomic_radii, atomic_coords));
   realfunct gradx_functor(new MolecularVolumeExponentialSwitchLogGrad(sigma, epsilon_1, epsilon_2, atomic_radii, atomic_coords,0));
   realfunct grady_functor(new MolecularVolumeExponentialSwitchLogGrad(sigma, epsilon_1, epsilon_2, atomic_radii, atomic_coords,1));
   realfunct gradz_functor(new MolecularVolumeExponentialSwitchLogGrad(sigma, epsilon_1, epsilon_2, atomic_radii, atomic_coords,2));
   realfunct surface_functor(new MolecularVolumeMask(sigma, atomic_radii, atomic_coords));
   // Make the actual functions                                                                                                                        
   const double rfourpi = 1.0/(4.0*constants::pi);
   rdielectric = real_factory_3d(world).functor(rdielectric_functor).nofence();
   dlog[0] = real_factory_3d(world).functor(gradx_functor).nofence();
   dlog[1] = real_factory_3d(world).functor(grady_functor).nofence();
   dlog[2] = real_factory_3d(world).functor(gradz_functor); // FENCE                                                                                   
   scale(world, dlog, rfourpi);
   rdielectric.truncate(false);
   truncate(world, dlog);
   surface = real_factory_3d(world).functor(surface_functor);
 }
  //make surface charge. life is good!!!uuh!
  /// Given the full Coulomb potential computes the surface charge                                                                                        
  realfunc make_surface_charge(const realfunc& u) const {
    real_derivative_3d Dx = free_space_derivative<double,3>(u.world(), 0);
    real_derivative_3d Dy = free_space_derivative<double,3>(u.world(), 1);
    real_derivative_3d Dz = free_space_derivative<double,3>(u.world(), 2);
    return (dlog[0]*Dx(u) + dlog[1]*Dy(u) + dlog[2]*Dz(u)).truncate();
  }
  //compute the cavitation energy (surface_tension*solvent accessible surface)
  double make_cav_energy(const double& surface_tension) const {
    //surface tension should be in Newton/meter
    double convfact = 6.423049507*std::pow(10,-4); // 1N/m = 6.423049507e−4a.u
    return surface.trace()*surface_tension*convfact;
  }
  //Compute the reaction potential                                                                              
  realfunc ScreenReactionPotential(World& world,int maxiter, const realfunc rhot, bool solventplot)const {
    const bool USE_SOLVER = true;
    double tol = std::max(1e-3,FunctionDefaults<3>::get_thresh());
    real_convolution_3d op = madness::CoulombOperator(world, tol*10.0, tol*0.1);
    realfunc charge = rdielectric*rhot;
    realfunc U0 = op(charge);
    realfunc U = op(rhot);
    //    realfunc U = Uguess.is_initialized()? Uguess : U0;
    double unorm = U.norm2();
    if (USE_SOLVER) {
      madness::NonlinearSolver solver;
      // This section employs a non-linear equation solver from solvers.h                                                                                  
      //  http://onlinelibrary.wiley.com/doi/10.1002/jcc.10108/abstract                                                                                    
      realfunc uvec, rvec;
      if (world.rank() == 0){
	print("\n\n");//for formating output
	
	madness::print("    Computing the solute-solvent potential   ");
	madness::print("           ______________________           \n ");
	
	madness::print("iteration ","    "," residue norm2\n");
      }
      for (int iter=0; iter<maxiter; iter++) {
        uvec = U;
        realfunc surface_charge = make_surface_charge(U);
        rvec = (U -U0 - op(surface_charge)).truncate() ;
	realfunc U_new = solver.update(uvec,rvec);
        double err = rvec.norm2();
	//	madness::print("iter ", iter, " error ", err, "soln(2.4566)", U(coord_3d(2.4566)));
	if (world.rank()==0)
	  madness::print("  ", iter,"             " , err);
	
        if (err >0.3*unorm){ U = 0.5*U + 0.5*U_new;
        }
        else
          U = U_new;
	if (err < 10.0*tol) break;
      }
    }
    if (world.rank()==0)
      print("\n\n"); //formating output
    //plot the potentials
    coord_3d lo(0.0),hi(0.0);
    lo[0] = -20.0;
    hi[0] = 20.0;
   
    realfunc Ufree = op(rhot);
    realfunc Ureaction = U - Ufree;
    if(solventplot){
      plot_line("solvation_pot.dat",401,lo,hi,U);
      plot_line("freespace_pot.dat",401,lo,hi,Ufree);
      plot_line("reaction_pot.dat",401,lo,hi,Ureaction);
      plotdx(U,"solvation_pot.dx");
      plotdx(Ufree,"freespace_pot.dx");
      plotdx(Ureaction,"reaction_pot.dx");
    }
    return Ureaction;
  }
};

#endif