#pragma once

#ifndef COVIDSIM_COVIDSIM_H_INCLUDED_
#define COVIDSIM_COVIDSIM_H_INCLUDED_

#include "MachineDefines.h"

#define KMP_LIBRARY throughput

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <math.h>
#include <string.h>
#include <signal.h>
#include <time.h>

#include "Country.h"
#include "Constants.h"
#include "Param.h"
#include "Bitmap.h"
#include "Model.h"

/*
	#define HOST_TREATED(x) (0)
	#define HOST_TO_BE_TREATED(x) (0)
	#define PLACE_TREATED(x,y) (0)
  */

//// place type codes
#define PlaceType_PrimarySchool 0
#define PlaceType_SecondarySchool 1
#define PlaceType_University 2
#define PlaceType_Office 3

/* int NumPC,NumPCD; */
#define MAXINTFILE 10

void InitModel(int); //adding run number as a parameter for event log: ggilani - 15/10/2014
int RunModel(int); //adding run number as a parameter for event log: ggilani - 15/10/2014
void LoadSnapshot(void);

extern int InterruptRun;
extern param P;
extern char OutFile[1024], OutFileBase[1024], OutDensFile[1024], SnapshotLoadFile[1024], SnapshotSaveFile[1024], AdunitFile[1024];
extern void SaveEvents(void); //added this function to save infection events from all realisations: ggilani - 15/10/14
extern void SaveResults(void);
extern void ReadParams(char*, char*);
extern void ReadAirTravel(char* AirTravelFile);
extern void ReadInterventions(char* IntFile);
extern adminunit AdUnits[MAX_ADUNITS];
extern void CalcOriginDestMatrix_adunit(void); //added function to calculate origin destination matrix: ggilani 28/01/15
extern void SaveOriginDestMatrix(void); //added function to save origin destination matrix so it can be done separately to the main results: ggilani - 13/02/15
extern void SaveSummaryResults(void);
extern void SaveRandomSeeds(void); //added this function to save random seeds for each run: ggilani - 09/03/17

///// ***** ///// ***** ///// ***** ///// ***** ///// ***** ///// ***** ///// ***** ///// ***** ///// ***** ///// ***** ///// ***** ///// ***** /////
///// ***** ///// ***** ///// ***** ///// ***** ///// ***** GLOBAL VARIABLES (some structures in CovidSim.h file and some containers) - memory allocated later.
///// ***** ///// ***** ///// ***** ///// ***** ///// ***** ///// ***** ///// ***** ///// ***** ///// ***** ///// ***** ///// ***** ///// ***** /////

extern person* Hosts;
extern household* Households;
extern popvar State, StateT[MAX_NUM_THREADS];
extern cell* Cells; // Cells[i] is the i'th cell
extern cell ** CellLookup; // CellLookup[i] is a pointer to the i'th populated cell
extern microcell* Mcells, ** McellLookup;
extern place** Places;

/**
 * @brief Airport state.
 *
 * Not used for COVID-19 right now. Might be more relevant for USA and
 * other countries that have lots of internal flights. Slows the simulation.
 */

//// Time Series defs:
//// TimeSeries is an array of type results, used to store (unsurprisingly) a time series of every quantity in results. Mostly used in RecordSample.
//// TSMeanNE and TSVarNE are the mean and variance of non-extinct time series. TSMeanE and TSVarE are the mean and variance of extinct time series. TSMean and TSVar are pointers that point to either extinct or non-extinct.
extern results* TimeSeries, * TSMean, * TSVar, * TSMeanNE, * TSVarNE, * TSMeanE, * TSVarE; //// TimeSeries used in RecordSample, RecordInfTypes, SaveResults. TSMean and TSVar
extern airport* Airports;
extern bitmap_header* bmh;
//added declaration of pointer to events log: ggilani - 10/10/2014
extern events* InfEventLog;
extern int* nEvents;

// These allow up to about 2 billion people per pixel, which should be ample.
extern int32_t *bmPopulation; // The population in each bitmap pixel. Special value -1 means "country boundary"
extern int32_t *bmInfected; // The number of infected people in each bitmap pixel.
extern int32_t *bmRecovered; // The number of recovered people in each bitmap pixel.
extern int32_t *bmTreated; // The number of treated people in each bitmap pixel.

extern int ns, DoInitUpdateProbs;
extern int PlaceDistDistrib[NUM_PLACE_TYPES][MAX_DIST], PlaceSizeDistrib[NUM_PLACE_TYPES][MAX_PLACE_SIZE];


#endif // COVIDSIM_COVIDSIM_H_INCLUDED_
