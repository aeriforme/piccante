/* Copyright 2014 - Andrea Sgattoni, Luca Fedeli, Stefano Sinigardi */

/*******************************************************************************
This file is part of piccante.

piccante is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

piccante is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with piccante.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

#ifndef __PARTICLE_SPECIES_H__
#define __PARTICLE_SPECIES_H__

#define _USE_MATH_DEFINES

#include <mpi.h>
#include "commons.h"
#include "structures.h"
#include "grid.h"
#include "current.h"
#include "em_field.h"

#define _VERY_SMALL_MOMENTUM 1.0e-5

class SPECIE{
public:
  static const int allocsize = 1000;
  particlesType type;
  bool allocated;
  int allocatedNp, Np, Ncomp;

  double Z, A;                   //atomic number, mass number
  double chargeSign, coupling;  // charge/mass*(speed_of_light^2)
  double Numerical2Physical_particles; // physical particles corresponding to one num. particle
  double mass;
  double minima[7], maxima[7];   //components minima and maxima
  double totalMomentum[3], totalEnergy;
  std::string name;
  bool isTestSpecies;

  ACCESSO accesso;
  PLASMA  plasma;
  //  double (*initial_profile)(double , double , double , PLASMA*);
  SPECIEspectrum spectrum;
  GRID *mygrid;

  SPECIE();
  SPECIE(GRID *grid);
  ~SPECIE();

  void allocate_species();
  void erase();
  void reallocate_species();
  SPECIE operator = (SPECIE &destro);
  void creation();
  void creationFromFile1D(std::string name);
  void move_window();
  void addMarker();
  bool amIWithMarker();
  void output(std::ofstream &ff);
  static const int myWidth = 12;
  static const int myNarrowWidth = 6;

  std::string getName();

  void init_output_diag(std::ofstream &ff);
  void output_diag(int istep, std::ofstream &ff);
  void init_output_extrems(std::ofstream &ff);
  void output_extrems(int istep, std::ofstream &ff);
  void init_output_stat(std::ofstream &fdiag, std::ofstream &fextrem);
  void output_stat(int istep, std::ofstream &fdiag, std::ofstream &fextrem, std::ofstream &fspectrum);
  void position_advance();
  void position_pbc();
  void position_parallel_pbc();
  void position_obc();
  void momenta_advance(EM_FIELD *ebfield);
  void momentaStretchedAdvance(EM_FIELD *ebfield);
  void momenta_advance_with_friction(EM_FIELD *ebfield, double lambda);
  void current_deposition(CURRENT *current);
  void add_momenta(double uxin, double uyin, double uzin);
  void add_momenta(gsl_rng* ext_rng, double uxin, double uyin, double uzin, tempDistrib distribution);
  void current_deposition_standard(CURRENT *current);
  void currentStretchedDepositionStandard(CURRENT *current);
  void density_deposition_standard(CURRENT *current);
  void densityStretchedDepositionStandard(CURRENT *current);
  void setParticlesPerCellXYZ(int numX, int numY, int numZ);
  void setName(std::string iname);
  double getKineticEnergy();
  void computeKineticEnergyWExtrems();
  void outputSpectrum(std::ofstream &fspectrum);

  void dump(std::ofstream &f);
  void reloadDump(std::ifstream &f);

  bool areEnergyExtremesAvailable();

  void printParticleNumber();


  //PUBLIC INLINE FUNCTIONS
#ifdef _ACC_SINGLE_POINTER
  inline double &ru(int c, int np) { return val[c + np*Ncomp]; }
  inline double &r0(int np) { return val[np*Ncomp + 0]; }
  inline double &r1(int np) { return val[np*Ncomp + 1]; }
  inline double &r2(int np) { return val[np*Ncomp + 2]; }
  inline double &u0(int np) { return val[np*Ncomp + 3]; }
  inline double &u1(int np) { return val[np*Ncomp + 4]; }
  inline double &u2(int np) { return val[np*Ncomp + 5]; }
  inline double &w(int np) { return val[np*Ncomp + 6]; }
  inline long int &marker(int np) { return *((long int*)(val+(np*Ncomp + 7))); }
  //inline long int &marker(int np) { return *((long int*)(&dummy)); }
#else
  inline double &ru(int c, int np) { return val[c][np]; }
  inline double &r0(int np) { return val[0][np]; }
  inline double &r1(int np) { return val[1][np]; }
  inline double &r2(int np) { return val[2][np]; }
  inline double &u0(int np) { return val[3][np]; }
  inline double &u1(int np) { return val[4][np]; }
  inline double &u2(int np) { return val[5][np]; }
  inline double &w(int np) { return val[6][np]; }
  inline long int &marker(int np) { return *((long int*)(&val[7][np])); }
  //inline long int &marker(int np) { return *((long int*)(&dummy)); }
#endif



private:
#ifdef _ACC_SINGLE_POINTER
  double *val;
#else
  double **val;
#endif
  double dummy;
  int valSize;
  int particlePerCell;
  int particlePerCellXYZ[3];
  long long lastParticle;
  double savedExtrema[14];
  double savedEnergy;
  bool energyExtremesFlag;
  bool flagWithMarker;
  void callWaterbag(gsl_rng* ext_rng, double p0_x, double p0_y, double p0_z, double uxin, double uyin, double uzin);
  void callUnifSphere(gsl_rng* ext_rng, double p0, double uxin, double uyin, double uzin);
  void callSupergaussian(gsl_rng* ext_rng, double p0, double alpha, double uxin, double uyin, double uzin);
  void callMaxwell(gsl_rng* ext_rng, double temp, double uxin, double uyin, double uzin);
  void callJuttner(gsl_rng* ext_rng, double a, double uxin, double uyin, double uzin);
  void callSpecial(gsl_rng* ext_rng, double Ta);
  void computeParticleMassChargeCoupling();
  void setNumberOfParticlePerCell();
  void setLocalPlasmaMinimaAndMaxima(double *plasmarmin, double *plasmarmax);
  long long getSumNewParticlesOfAllPreviousProcessors(int number);
  int getNumberOfParticlesWithin(double plasmarmin[3], double plasmarmax[3]);
  int getNumberOfParticlesWithinFromFile1D(double plasmarmin[3], double plasmarmax[3], std::string name);
  void createParticlesWithinFrom(double plasmarmin[3], double plasmarmax[3], int oldNumberOfParticles, long long disp);
  void createStretchedParticlesWithinFrom(double plasmarmin[3], double plasmarmax[3], int oldNumberOfParticles, long long disp);
  void createParticlesWithinFromButFromFile1D(double plasmarmin[3], double plasmarmax[3], int oldNumberOfParticles, long long disp, std::string name);
  void createStretchedParticlesWithinFromButFromFile1D(double plasmarmin[3], double plasmarmax[3], int oldNumberOfParticles, long long disp, std::string name);


  void computeLorentzMatrix(double ux, double uy, double uz, double matr[16]);

  void debug_warning_particle_outside_boundaries(double x, double y, double z, int nump);
};



#endif /* _PARTICLE_SPECIES_H_ */

