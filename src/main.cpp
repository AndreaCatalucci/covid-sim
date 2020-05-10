
#ifdef _OPENMP
#include <omp.h>
#endif // _OPENMP

#include "CovidSim.h"
#include "Bitmap.h"
#include "Error.h"
#include "SetupModel.h"
#include "Rand.h"

int main(int argc, char *argv[])
{
  char ParamFile[1024]{}, DensityFile[1024]{}, NetworkFile[1024]{}, AirTravelFile[1024]{}, SchoolFile[1024]{}, RegDemogFile[1024]{}, InterventionFile[MAXINTFILE][1024]{}, PreParamFile[1024]{}, buf[2048]{}, *sep;
  int i, GotP, GotPP, GotO, GotL, GotS, GotAP, GotScF, Perr, cl;

  ///// Flags to ensure various parameters have been read; set to false as default.
  GotP = GotO = GotL = GotS = GotAP = GotScF = GotPP = 0;

  Perr = 0;
  fprintf(stderr, "sizeof(int)=%i sizeof(long)=%i sizeof(float)=%i sizeof(double)=%i sizeof(unsigned short int)=%i sizeof(int *)=%i\n", (int)sizeof(int), (int)sizeof(long), (int)sizeof(float), (int)sizeof(double), (int)sizeof(unsigned short int), (int)sizeof(int *));
  cl = clock();

  ///// Read in command line arguments - lots of things, e.g. random number seeds; (pre)parameter files; binary files; population data; output directory? etc.

  if (argc < 7)
    Perr = 1;
  else
  {
    ///// Get seeds.
    i = argc - 4;
    sscanf(argv[i], "%li", &P.setupSeed1);
    sscanf(argv[i + 1], "%li", &P.setupSeed2);
    sscanf(argv[i + 2], "%li", &P.runSeed1);
    sscanf(argv[i + 3], "%li", &P.runSeed2);

    ///// Set parameter defaults - read them in after
    P.PlaceCloseIndepThresh = P.LoadSaveNetwork = P.DoHeteroDensity = P.DoPeriodicBoundaries = P.DoSchoolFile = P.DoAdunitDemog = P.OutputDensFile = P.MaxNumThreads = P.DoInterventionFile = 0;
    P.PreControlClusterIdCaseThreshold = 0;
    P.R0scale = 1.0;
    P.KernelOffsetScale = P.KernelPowerScale = 1.0; //added this so that kernel parameters are only changed if input from the command line: ggilani - 15/10/2014
    P.DoSaveSnapshot = P.DoLoadSnapshot = 0;

    //// scroll through command line arguments, anticipating what they can be using various if statements.
    for (i = 1; i < argc - 4; i++)
    {
      if ((argv[i][0] != '/') && ((argv[i][2] != ':') && (argv[i][3] != ':')))
        Perr = 1;
      if (argv[i][1] == 'P' && argv[i][2] == ':')
      {
        GotP = 1;
        sscanf(&argv[i][3], "%s", ParamFile);
      }
      else if (argv[i][1] == 'O' && argv[i][2] == ':')
      {
        GotO = 1;
        sscanf(&argv[i][3], "%s", OutFileBase);
      }
      else if (argv[i][1] == 'D' && argv[i][2] == ':')
      {
        sscanf(&argv[i][3], "%s", DensityFile);
        P.DoHeteroDensity = 1;
        P.DoPeriodicBoundaries = 0;
      }
      else if (argv[i][1] == 'A' && argv[i][2] == ':')
      {
        sscanf(&argv[i][3], "%s", AdunitFile);
      }
      else if (argv[i][1] == 'L' && argv[i][2] == ':')
      {
        GotL = 1;
        P.LoadSaveNetwork = 1;
        sscanf(&argv[i][3], "%s", NetworkFile);
      }
      else if (argv[i][1] == 'S' && argv[i][2] == ':')
      {
        P.LoadSaveNetwork = 2;
        GotS = 1;
        sscanf(&argv[i][3], "%s", NetworkFile);
      }
      else if (argv[i][1] == 'R' && argv[i][2] == ':')
      {
        sscanf(&argv[i][3], "%lf", &P.R0scale);
      }
      else if (argv[i][1] == 'K' && argv[i][2] == 'P' && argv[i][3] == ':') //added Kernel Power and Offset scaling so that it can easily be altered from the command line in order to vary the kernel quickly: ggilani - 15/10/14
      {
        sscanf(&argv[i][4], "%lf", &P.KernelPowerScale);
      }
      else if (argv[i][1] == 'K' && argv[i][2] == 'O' && argv[i][3] == ':')
      {
        sscanf(&argv[i][4], "%lf", &P.KernelOffsetScale);
      }
      else if (argv[i][1] == 'C' && argv[i][2] == 'L' && argv[i][3] == 'P' && argv[i][4] == '1' && argv[i][5] == ':') // generic command line specified param - matched to #1 in param file
      {
        sscanf(&argv[i][6], "%lf", &P.clP1);
      }
      else if (argv[i][1] == 'C' && argv[i][2] == 'L' && argv[i][3] == 'P' && argv[i][4] == '2' && argv[i][5] == ':') // generic command line specified param - matched to #2 in param file
      {
        sscanf(&argv[i][6], "%lf", &P.clP2);
      }
      else if (argv[i][1] == 'C' && argv[i][2] == 'L' && argv[i][3] == 'P' && argv[i][4] == '3' && argv[i][5] == ':') // generic command line specified param - matched to #3 in param file
      {
        sscanf(&argv[i][6], "%lf", &P.clP3);
      }
      else if (argv[i][1] == 'C' && argv[i][2] == 'L' && argv[i][3] == 'P' && argv[i][4] == '4' && argv[i][5] == ':') // generic command line specified param - matched to #4 in param file
      {
        sscanf(&argv[i][6], "%lf", &P.clP4);
      }
      else if (argv[i][1] == 'C' && argv[i][2] == 'L' && argv[i][3] == 'P' && argv[i][4] == '5' && argv[i][5] == ':') // generic command line specified param - matched to #5 in param file
      {
        sscanf(&argv[i][6], "%lf", &P.clP5);
      }
      else if (argv[i][1] == 'C' && argv[i][2] == 'L' && argv[i][3] == 'P' && argv[i][4] == '6' && argv[i][5] == ':') // generic command line specified param - matched to #6 in param file
      {
        sscanf(&argv[i][6], "%lf", &P.clP6);
      }
      else if (argv[i][1] == 'A' && argv[i][2] == 'P' && argv[i][3] == ':')
      {
        GotAP = 1;
        sscanf(&argv[i][3], "%s", AirTravelFile);
      }
      else if (argv[i][1] == 's' && argv[i][2] == ':')
      {
        GotScF = 1;
        sscanf(&argv[i][3], "%s", SchoolFile);
      }
      else if (argv[i][1] == 'T' && argv[i][2] == ':')
      {
        sscanf(&argv[i][3], "%i", &P.PreControlClusterIdCaseThreshold);
      }
      else if (argv[i][1] == 'C' && argv[i][2] == ':')
      {
        sscanf(&argv[i][3], "%i", &P.PlaceCloseIndepThresh);
      }
      else if (argv[i][1] == 'd' && argv[i][2] == ':')
      {
        P.DoAdunitDemog = 1;
        sscanf(&argv[i][3], "%s", RegDemogFile);
      }
      else if (argv[i][1] == 'c' && argv[i][2] == ':')
      {
        sscanf(&argv[i][3], "%i", &P.MaxNumThreads);
      }
      else if (argv[i][1] == 'M' && argv[i][2] == ':')
      {
        P.OutputDensFile = 1;
        sscanf(&argv[i][3], "%s", OutDensFile);
      }
      else if (argv[i][1] == 'I' && argv[i][2] == ':')
      {
        sscanf(&argv[i][3], "%s", InterventionFile[P.DoInterventionFile]);
        P.DoInterventionFile++;
      }
      else if (argv[i][1] == 'L' && argv[i][2] == 'S' && argv[i][3] == ':')
      {
        sscanf(&argv[i][4], "%s", SnapshotLoadFile);
        P.DoLoadSnapshot = 1;
      }
      else if (argv[i][1] == 'P' && argv[i][2] == 'P' && argv[i][3] == ':')
      {
        sscanf(&argv[i][4], "%s", PreParamFile);
        GotPP = 1;
      }
      else if (argv[i][1] == 'S' && argv[i][2] == 'S' && argv[i][3] == ':')
      {
        sscanf(&argv[i][4], "%s", buf);
        fprintf(stderr, "### %s\n", buf);
        sep = strchr(buf, ',');
        if (!sep)
          Perr = 1;
        else
        {
          P.DoSaveSnapshot = 1;
          *sep = ' ';
          sscanf(buf, "%lf %s", &(P.SnapshotSaveTime), SnapshotSaveFile);
        }
      }
    }
    if (((GotS) && (GotL)) || (!GotP) || (!GotO))
      Perr = 1;
  }

  ///// END Read in command line arguments

  sprintf(OutFile, "%s", OutFileBase);

  fprintf(stderr, "Param=%s\nOut=%s\nDens=%s\n", ParamFile, OutFile, DensityFile);
  if (Perr)
    ERR_CRITICAL_FMT("Syntax:\n%s /P:ParamFile /O:OutputFile [/AP:AirTravelFile] [/s:SchoolFile] [/D:DensityFile] [/L:NetworkFileToLoad | /S:NetworkFileToSave] [/R:R0scaling] SetupSeed1 SetupSeed2 RunSeed1 RunSeed2\n", argv[0]);

    //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// ****
    //// **** SET UP OMP / THREADS
    //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// ****

#ifdef _OPENMP
  P.NumThreads = omp_get_max_threads();
  if ((P.MaxNumThreads > 0) && (P.MaxNumThreads < P.NumThreads))
    P.NumThreads = P.MaxNumThreads;
  if (P.NumThreads > MAX_NUM_THREADS)
  {
    fprintf(stderr, "Assigned number of threads (%d) > MAX_NUM_THREADS (%d)\n", P.NumThreads, MAX_NUM_THREADS);
    P.NumThreads = MAX_NUM_THREADS;
  }
  fprintf(stderr, "Using %d threads\n", P.NumThreads);
  omp_set_num_threads(P.NumThreads);

#pragma omp parallel default(shared)
  {
    fprintf(stderr, "Thread %i initialised\n", omp_get_thread_num());
  }
  /* fprintf(stderr,"int=%i\tfloat=%i\tdouble=%i\tint *=%i\n",(int) sizeof(int),(int) sizeof(float),(int) sizeof(double),(int) sizeof(int *));	*/
#else
  P.NumThreads = 1;
#endif
  if (!GotPP)
  {
    sprintf(PreParamFile, ".." DIRECTORY_SEPARATOR "Pre_%s", ParamFile);
  }

  //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// ****
  //// **** READ IN PARAMETERS, DATA ETC.
  //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// ****

  ReadParams(ParamFile, PreParamFile);
  if (GotScF)
    P.DoSchoolFile = 1;
  if (P.DoAirports)
  {
    if (!GotAP)
      ERR_CRITICAL_FMT("Syntax:\n%s /P:ParamFile /O:OutputFile /AP:AirTravelFile [/s:SchoolFile] [/D:DensityFile] [/L:NetworkFileToLoad | /S:NetworkFileToSave] [/R:R0scaling] SetupSeed1 SetupSeed2 RunSeed1 RunSeed2\n", argv[0]);
    ReadAirTravel(AirTravelFile);
  }

  //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// ****
  //// **** INITIALIZE
  //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// ****

  ///// initialize model (for all realisations).
  SetupModel(DensityFile, NetworkFile, SchoolFile, RegDemogFile);

  for (i = 0; i < MAX_ADUNITS; i++)
    AdUnits[i].NI = 0;
  if (P.DoInterventionFile > 0)
    for (i = 0; i < P.DoInterventionFile; i++)
      ReadInterventions(InterventionFile[i]);

  fprintf(stderr, "Model setup in %lf seconds\n", ((double)(clock() - cl)) / CLOCKS_PER_SEC);

  //print out number of calls to random number generator

  //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// ****
  //// **** RUN MODEL
  //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// **** //// ****

  P.NRactE = P.NRactNE = 0;
  for (i = 0; (i < P.NumRealisations) && (P.NRactNE < P.NumNonExtinctRealisations) && (!InterruptRun); i++)
  {
    if (P.NumRealisations > 1)
    {
      sprintf(OutFile, "%s.%i", OutFileBase, i);
      fprintf(stderr, "Realisation %i   (time=%lf nr_ne=%i)\n", i + 1, ((double)(clock() - cl)) / CLOCKS_PER_SEC, P.NRactNE);
    }

    ///// Set and save seeds
    if (i == 0 || (P.ResetSeeds && P.KeepSameSeeds))
    {
      P.nextRunSeed1 = P.runSeed1;
      P.nextRunSeed2 = P.runSeed2;
    }
    if (P.ResetSeeds)
    {
      //save these seeds to file
      SaveRandomSeeds();
    }
    // Now that we have set P.nextRunSeed* ready for the run, we need to save the values in case
    // we need to reinitialise the RNG after the run is interrupted.
    long thisRunSeed1 = P.nextRunSeed1;
    long thisRunSeed2 = P.nextRunSeed2;
    if (i == 0 || P.ResetSeeds)
    {
      setall(&P.nextRunSeed1, &P.nextRunSeed2);
      //fprintf(stderr, "%i, %i\n", P.newseed1,P.newseed2);
      //fprintf(stderr, "%f\n", ranf());
    }

    ///// initialize model (for this realisation).
    InitModel(i); //passing run number into RunModel so we can save run number in the infection event log: ggilani - 15/10/2014
    if (P.DoLoadSnapshot)
      LoadSnapshot();
    while (RunModel(i))
    { // has been interrupted to reset holiday time. Note that this currently only happens in the first run, regardless of how many realisations are being run.
      long tmp1 = thisRunSeed1;
      long tmp2 = thisRunSeed2;
      setall(&tmp1, &tmp2); // reset random number seeds to generate same run again after calibration.
      InitModel(i);
    }
    if (P.OutputNonSummaryResults)
    {
      if (((!TimeSeries[P.NumSamples - 1].extinct) || (!P.OutputOnlyNonExtinct)) && (P.OutputEveryRealisation))
      {
        SaveResults();
      }
    }
    if ((P.DoRecordInfEvents) && (P.RecordInfEventsPerRun == 1))
    {
      SaveEvents();
    }
  }
  sprintf(OutFile, "%s", OutFileBase);

  //Calculate origin destination matrix if needed
  if ((P.DoAdUnits) && (P.DoOriginDestinationMatrix))
  {
    CalcOriginDestMatrix_adunit();
    SaveOriginDestMatrix();
  }

  P.NRactual = P.NRactNE;
  TSMean = TSMeanNE;
  TSVar = TSVarNE;
  if ((P.DoRecordInfEvents) && (P.RecordInfEventsPerRun == 0))
  {
    SaveEvents();
  }
  sprintf(OutFile, "%s.avNE", OutFileBase);
  SaveSummaryResults();
  P.NRactual = P.NRactE;
  TSMean = TSMeanE;
  TSVar = TSVarE;
  sprintf(OutFile, "%s.avE", OutFileBase);
  //SaveSummaryResults();

#ifdef WIN32_BM
  Gdiplus::GdiplusShutdown(m_gdiplusToken);
#endif
  fprintf(stderr, "Extinction in %i out of %i runs\n", P.NRactE, P.NRactNE + P.NRactE);
  fprintf(stderr, "Model ran in %lf seconds\n", ((double)(clock() - cl)) / CLOCKS_PER_SEC);
  fprintf(stderr, "Model finished\n");
}
