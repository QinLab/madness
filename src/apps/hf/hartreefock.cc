#define WORLD_INSTANTIATE_STATIC_TEMPLATES  
#include "hartreefock.h"

namespace madness
{

  //***************************************************************************
  HartreeFock::HartreeFock(World& world, funcT V, std::vector<funcT> phis,
    std::vector<double> eigs, bool bCoulomb, bool bExchange, double thresh) :
    _V(V), _phis(phis), _eigs(eigs), _bCoulomb(bCoulomb),
    _bExchange(bExchange), _world(world), _thresh(thresh)
  {
    ones = functorT(new OnesFunctor());
    zeros = functorT(new ZerosFunctor());
  }
  //***************************************************************************
  
  //***************************************************************************
  HartreeFock::HartreeFock(World& world, funcT V, funcT phi, double eig, 
    bool bCoulomb, bool bExchange, double thresh) : _V(V),
    _bCoulomb(bCoulomb), _bExchange(bExchange), _world(world), _thresh(thresh)
  {
    ones = functorT(new OnesFunctor());
    zeros = functorT(new ZerosFunctor());
    _phis.push_back(phi);
    _eigs.push_back(eig);
  }
  //***************************************************************************
  
  //***************************************************************************
  HartreeFock::~HartreeFock()
  {
  }
  //***************************************************************************
  
  //***************************************************************************
  void HartreeFock::hartree_fock(int maxits)
  {
    for (int it = 0; it < maxits; it++)
    {
      printf("//************* iteration #%d *************//\n\n", it);
      printf("thresh = %.4e\n\n", _thresh);
      for (int pi = 0; pi < _phis.size(); pi++)
      {
        // Get psi from collection
        funcT psi = _phis[pi];
        // Calculate nuclear contribution
	//madness::print("THIS IS V");
        //_V.print_tree();
	//madness::print("THIS IS PSI");
        //psi.print_tree();
        printf("iteration #%d: calc nuclear ...\n\n", it);
        printf("iteration #%d: psi.norm2() = %.5f\n\n", it, 
          psi.norm2());
        funcT pnuclear = _V*psi;
        pnuclear.truncate(_thresh);
        printf("iteration #%d: pnuclear.norm2() = %.5f\n\n", it, 
          pnuclear.norm2());
        // Calculate the Coulomb contribution to the Fock operator (J)
        printf("iteration #%d: calc coulomb ...\n\n", it);
        funcT pcoulomb = calculate_coulomb(psi);
        // Calculate the Exchange contribution to the Fock operator (K)
        printf("iteration #%d: calc exchange ...\n\n", it);
        funcT pexchange = calculate_exchange(psi);
        // Get new wavefunction
        funcT pfunc = pnuclear + 2.0 * pcoulomb - pexchange;
        pfunc.scale(-2.0).truncate(_thresh);
        printf("iteration #%d: pfunc.norm2() = %.5f\n\n", it, 
          pfunc.norm2());
        // Create the free-particle Green's function operator
        printf("iteration #%d: _eigs[%d] = %.5f\n\n", it, pi, 
          _eigs[pi]);
        SeparatedConvolution<double,3> op = 
          BSHOperator<double,3>(_world, sqrt(-2.0*_eigs[pi]), 
              FunctionDefaults<3>::k, 1e-3, _thresh);      
        // Apply the Green's function operator (stubbed)
        printf("iteration #%d: apply BSH ...\n\n", it);
        funcT tmp = apply(op, pfunc);
        tmp.truncate(_thresh);
        printf("iteration #%d (after BSH): tmp.norm2() = %.5f\n\n", it, 
          tmp.norm2());
        // (Not sure whether we have to do this mask thing or not!)
        printf("iteration #%d: doing gram-schmidt ...\n\n", it);
        for (int pj = 0; pj < pi; ++pj)
        {
          // Project out the lower states
          // Make sure that pi != pj
          if (pi != pj)
          {
            // Get other wavefunction
            funcT psij = _phis[pj];
            double overlap = inner(tmp, psij);
            tmp -= overlap*psij;
          }
        }
        // Update e
        tmp.truncate(_thresh);
        funcT r = tmp - psi;
        double norm = tmp.norm2();
        double eps_old = _eigs[pi];
        printf("Updating wavefunction on iteration #%d ...\n\n", it);
        double ecorrection = -0.5*inner(pfunc, r) / (norm*norm);
        _eigs[pi] += ecorrection;
        _phis[pi] = tmp.scale(1.0/tmp.norm2());
        printf("iteration #%d: tmp(/psi).norm2() = %.5f\n\n", it, 
          tmp.norm2());
      }
      // Display energies
    }
  }
  //***************************************************************************

  //***************************************************************************
  funcT HartreeFock::calculate_coulomb(funcT psi)
  {
    if (include_coulomb())
    {
      // Electron density
      funcT density = FunctionFactory<double,3>(_world).functor(zeros);
      // Create Coulomb operator
      SeparatedConvolution<double,3> op = 
        CoulombOperator<double,3>(_world, FunctionDefaults<3>::k, 1e-4, _thresh);      
      for (std::vector<funcT>::iterator pj = _phis.begin(); pj != _phis.end(); ++pj)
      {
        // Get phi(j) from iterator
        funcT& phij = (*pj);
        // Compute the j-th density
        funcT prod = phij*phij;
        prod.truncate(_thresh);
        density += prod;
      }
      // Transform Coulomb operator into a function (stubbed)
      printf("density.norm2() = %.5f\n\n", density.norm2()); 
      funcT Vc = apply(op, density);
      // Note that we are not using psi
      // The density is built from all of the wavefunctions. The contribution
      // psi will be subtracted out later during the exchange.
      funcT rfunc = Vc*psi;
      printf("Vc.norm2() = %.5f\n\n", Vc.norm2()); 
      printf("pcoulomb.norm2() = %.5f\n\n", rfunc.norm2()); 
      return rfunc;
    }
    return FunctionFactory<double,3>(_world).functor(zeros);
  }
  //***************************************************************************

  //***************************************************************************
  funcT HartreeFock::calculate_exchange(funcT psi)
  {
    // Return value
    funcT rfunc = FunctionFactory<double,3>(_world).functor(zeros);
    if (include_exchange())
    {
      // Create Coulomb operator
      SeparatedConvolution<double,3> op = 
        CoulombOperator<double,3>(_world, FunctionDefaults<3>::k, 1e-4, _thresh);      
      // Use the psi and pj wavefunctions to build a product so that the K 
      // operator can be applied to the wavefunction indexed by pj, NOT PSI.
      for (std::vector<funcT>::iterator pi = _phis.begin(); pi != _phis.end(); ++pi)
      {
        for (std::vector<funcT>::iterator pj = _phis.begin(); pj != _phis.end(); ++pj)
        {
          // Get phi(j) from iterator
          funcT& phij = (*pj);
          // NOTE that psi is involved in this calculation
          funcT prod = phij*psi;
          printf("prod.norm2() = %.5f\n\n", prod.norm2()); 
          // Transform Coulomb operator into a function (stubbed)
          funcT Vex = apply(op, prod);
          // NOTE that the index is j.
          rfunc += Vex*phij;
        }
      }
    }
    return rfunc;
  }
  //***************************************************************************

  //***************************************************************************
  double HartreeFock::calculate_ke_sp(funcT psi)
  {
    double kenergy = 0.0;
    for (int axis = 0; axis < 3; axis++)
    {
      funcT dpsi = diff(psi, axis);
      kenergy += 0.5 * inner(dpsi, dpsi);
    }
    return kenergy;
  }
  //***************************************************************************

  //***************************************************************************
  double HartreeFock::calculate_pe_sp(funcT psi)
  {
    funcT vpsi = _V*psi;
    vpsi.truncate(_thresh);
    return vpsi.inner(psi);
  }
  //***************************************************************************

  //***************************************************************************
  double HartreeFock::calculate_coulomb_energy(funcT psi)
  {
    if (include_coulomb())
    {
      // Electron density
      funcT density = FunctionFactory<double,3>(_world).functor(zeros);
      // Create Coulomb operator
      SeparatedConvolution<double,3> op = 
        CoulombOperator<double,3>(_world, FunctionDefaults<3>::k, 1e-4, _thresh);      
      for (std::vector<funcT>::iterator pj = _phis.begin(); pj != _phis.end(); ++pj)
      {
        // Get phi(j) from iterator
        funcT& phij = (*pj);
        // Compute the j-th density
        funcT prod = phij*phij;
        prod.truncate(_thresh);
        density += prod;
      }
      // Transform Coulomb operator into a function (stubbed)
      funcT Vc = apply(op, density);
      // Note that we are not using psi
      // The density is built from all of the wavefunctions. The contribution
      // psi will be subtracted out later during the exchange.
      funcT vpsi = Vc*psi;
      return inner(vpsi, psi);
    }
    return 0.0;
  }
  //***************************************************************************

  //***************************************************************************
  double HartreeFock::calculate_exchange_energy(funcT psi)
  {
    // Return value
    funcT rfunc = FunctionFactory<double,3>(_world).functor(zeros);
    if (include_exchange())
    {
      // Create Coulomb operator
      SeparatedConvolution<double,3> op = 
        CoulombOperator<double,3>(_world, FunctionDefaults<3>::k, 1e-4, _thresh);      
      // Use the psi and pj wavefunctions to build a product so that the K 
      // operator can be applied to the wavefunction indexed by pj, NOT PSI.
      for (std::vector<funcT>::iterator pi = _phis.begin(); pi != _phis.end(); ++pi)
      {
        for (std::vector<funcT>::iterator pj = _phis.begin(); pj != _phis.end(); ++pj)
        {
          // Get phi(j) from iterator
          funcT& phij = (*pj);
          // NOTE that psi is involved in this calculation
          funcT prod = phij*psi;
          // Transform Coulomb operator into a function (stubbed)
          funcT Vex = apply(op, prod);
          // NOTE that the index is j.
          rfunc += Vex*phij;
        }
      }
    }
    return inner(rfunc, psi);
  }
  //***************************************************************************

  //***************************************************************************
  double HartreeFock::calculate_tot_ke_sp()
  {
    double tot_ke = 0.0;
    for (int pi = 0; pi < _phis.size(); pi++)
    {
      // Get psi from collection
      funcT psi = _phis[pi];
      // Calculate kinetic energy contribution from psi
      tot_ke += calculate_ke_sp(psi);
    }
    return tot_ke;
  }
  //***************************************************************************
  
  //***************************************************************************
  double HartreeFock::calculate_tot_pe_sp()
  {
    double tot_pe = 0.0;
    for (int pi = 0; pi < _phis.size(); pi++)
    {
      // Get psi from collection
      funcT psi = _phis[pi];
      // Calculate potential energy contribution from psi
      tot_pe += calculate_pe_sp(psi);
    }
    return tot_pe;
  }
  //***************************************************************************
  
  //***************************************************************************
  double HartreeFock::calculate_tot_coulomb_energy()
  {
    double tot_ce = 0.0;
    for (int pi = 0; pi < _phis.size(); pi++)
    {
      // Get psi from collection
      funcT psi = _phis[pi];
      // Calculate coulomb energy contribution from psi
      tot_ce += calculate_coulomb_energy(psi);
    }
    return tot_ce;
  }
  //***************************************************************************
  
  //***************************************************************************
  double HartreeFock::calculate_tot_exchange_energy()
  {
    double tot_ee = 0.0;
    for (int pi = 0; pi < _phis.size(); pi++)
    {
      // Get psi from collection
      funcT psi = _phis[pi];
      // Calculate exchange energy contribution from psi
      tot_ee += calculate_exchange_energy(psi);
    }
    return tot_ee;
  }
  //***************************************************************************
}
//*****************************************************************************

