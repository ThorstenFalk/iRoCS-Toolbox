/**************************************************************************
 *
 * Copyright (C) 2015 Thorsten Falk
 *
 *        Image Analysis Lab, University of Freiburg, Germany
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 *
 **************************************************************************/

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

#include <libcmdline/CmdLine.hh>
#include <libcmdline/ArgvIter.hh>

#include <libIRoCS/AttachIRoCSWorker.hh>

#include <libProgressReporter/ProgressReporterStream.hh>

class CmdLineVersionError: public CmdLineError {};
class CmdLineLicenseError: public CmdLineError {};

int main(int argc, char **argv)
{
  CmdArgThrow<CmdLineVersionError> versionArg(
      0, "version", "Display version information.");
  CmdArgThrow<CmdLineLicenseError> licenseArg(
      0, "license", "Display licensing information.");

  CmdArgType<std::string> inFileName(
      "<hdf5 file>",
      "The name of the hdf5 file containing the nuclei to fit the axis to",
      CmdArg::isREQ);
  CmdArgType<std::string> annotationName(
      'n', "nuclei", "<hdf5 group>", "The nucleus hdf5 group");
  annotationName.setDefaultValue("/annotation/detector");
  CmdArgType<std::string> qcName(
      'q', "qc", "<hdf5 group>", "The hdf5 group containing the QC marker");
  qcName.setDefaultValue("/annotation/qc");
  CmdArgType<std::string> axisName(
      'a', "axis", "<hdf5 group>", "The hdf5 group to store the axis to");
  axisName.setDefaultValue("/annotation/axis");
  CmdArgType<int> nIterations(
      'i', "nIterations", "<positive integer>",
      "The maximum number of iterations till a forced reparametrization is "
      "applied. Normally the fitting is run till convergence and then the "
      "reparametrization is automatically applied so large values here are "
      "usually a good choice.");
  nIterations.setDefaultValue(1000000);
  CmdArgType<double> kappa(
      'k', "kappa", "<positive double>",
      "The data term weighting factor. For extreme values of lambda and mu "
      "below you may need to adapt kappa as well to stay numerically stable. "
      "Usually you should keep it at its default value of 1.0");
  kappa.setDefaultValue(1.0);
  CmdArgType<double> lambda(
      'l', "lambda", "<positive double>",
      "The axis stiffness weighting factor. If the axis oscillates around "
      "your expectated axis increase this value. Usually very small values "
      "(0 - 0.1) are appropriate.");
  lambda.setDefaultValue(0.0);
  CmdArgType<double> mu(
      'm', "mu", "<positive double>",
      "The thickness constancy weighting factor. If the thickness oscillates "
      "a lot increase this value. Usually very small values (0 - 0.1) are "
      "appropriate.");
  mu.setDefaultValue(0.0);
  CmdArgType<double> tau(
      't', "tau", "<positive double>",
      "During Gradient descent optimization tau is the length of the step "
      "along the gradient to update the energy functional. The time-step is "
      "adapted automatically to avoid oscillations, but a sensible initial "
      "timestep must be given here to avoid immediate divergence. If the "
      "optimization oscillates reduce the time step.");
  tau.setDefaultValue(0.1);
  CmdArgType<double> searchRadiusUm(
      'r', "searchRadius_um", "<positive double>",
      "The threshold in micrometers for energy saturation. Points beyond that "
      "distance from the currrent model do not influence the model evolution. "
      "This parameter has to be chosen according to the surface point density "
      "and the distance between adjacent structures. If set to zero or a "
      "negative value it is determined automatically to 20% of the long axis "
      "of the point cloud's covariance.");
  searchRadiusUm.setDefaultValue(-1.0);
  CmdArgType<std::string> outFileName(
      'o', "outfile", "<hdf5 file>", "If given, the axis will be written "
      "to a file of the given name. If omitted the axis will be written "
      "to the input file");

  CmdLine cmd(argv[0], "B-Spline axis fit");
  cmd.description("Fit the medial axis to the given markers");
  
  try
  {
    cmd.append(&versionArg);
    cmd.append(&licenseArg);
    cmd.append(&inFileName);
    cmd.append(&annotationName);
    cmd.append(&qcName);
    cmd.append(&axisName);
    cmd.append(&kappa);
    cmd.append(&lambda);
    cmd.append(&mu);
    cmd.append(&nIterations);
    cmd.append(&tau);
    cmd.append(&searchRadiusUm);
    cmd.append(&outFileName);
    
    ArgvIter argvIter(--argc, ++argv);
    cmd.parse(argvIter);

    iRoCS::ProgressReporterStream pr(std::cout, 0, 0, 100, "\r ");

    /*---------------------------------------------------------------------
     *  Load annotation channel
     *---------------------------------------------------------------------*/ 
    pr.updateProgressMessage(
        "Loading '" + inFileName.value() + ":" + annotationName.value() + "'");
    std::vector<atb::Nucleus> nuclei;
    atb::Nucleus::loadList(nuclei, inFileName.value(), annotationName.value());
        
    /*---------------------------------------------------------------------
     *  Load QC position
     *---------------------------------------------------------------------*/ 
    pr.updateProgressMessage(
        "Loading '" + inFileName.value() + ":" + qcName.value() + "'");
    std::vector<atb::Nucleus> qcMarkers;
    atb::Nucleus::loadList(qcMarkers, inFileName.value(), qcName.value());
    if (qcMarkers.size() == 0)
    {
      std::cerr << "Did not find QC annotation in '" << inFileName.value()
                << ":" << qcName.value() << "'" << std::endl;
      return -1;
    }
    blitz::TinyVector<double,3> qcPositionUm(qcMarkers[0].positionUm());
    
    /*---------------------------------------------------------------------
     *  Prepare iRoCS
     *---------------------------------------------------------------------*/
    atb::IRoCS rct(&pr);

    /*---------------------------------------------------------------------
     *  Run attachIRoCS
     *---------------------------------------------------------------------*/
    iRoCS::attachIRoCS(
        rct, nuclei, qcPositionUm, kappa.value(), lambda.value(),
        mu.value(), searchRadiusUm.value(), nIterations.value(), tau.value(),
        &pr);
    if (pr.isAborted()) return -1;
    
    /*---------------------------------------------------------------------
     *  Save iRoCS channel
     *---------------------------------------------------------------------*/
    std::string outFile = (outFileName.given()) ?
        outFileName.value() : inFileName.value();
    pr.updateProgressMessage(
        "Saving '" + outFile + ":" + axisName.value() + "'");
    rct.save(outFile, axisName.value());

    /*---------------------------------------------------------------------
     *  Save annotation channel
     *---------------------------------------------------------------------*/
    pr.updateProgressMessage(
        "Saving '" + inFileName.value() + ":" + annotationName.value() + "'");
    atb::Nucleus::saveList(nuclei, inFileName.value(), annotationName.value());
  }
  catch (CmdLineUsageError &e)
  {
    cmd.usage();
    exit(-1);
  }
  catch (CmdLineVersionError e)
  {
    std::cout << PACKAGE_STRING << std::endl;
    exit(0);
  }
  catch (CmdLineLicenseError e)
  {
    std::cout << PACKAGE_STRING << std::endl << std::endl
              << "URL: " << PACKAGE_URL << std::endl << std::endl
              << "Copyright (C) 2012-2015 Thorsten Falk (" << PACKAGE_BUGREPORT
              << ")" << std::endl << std::endl
              << "Address:" << std::endl
              << "   Image Analysis Lab" << std::endl
              << "   Albert-Ludwigs-Universitaet" << std::endl
              << "   Georges-Koehler-Allee Geb. 52" << std::endl
              << "   79110 Freiburg" << std::endl
              << "   Germany" << std::endl << std::endl
              << "This program is free software: you can redistribute it and/or"
              << std::endl
              << "modify it under the terms of the GNU General Public License"
              << std::endl
              << "Version 3 as published by the Free Software Foundation."
              << std::endl << std::endl
              << "This program is distributed in the hope that it will be "
              << "useful," << std::endl
              << "but WITHOUT ANY WARRANTY; without even the implied warranty "
              << "of " << std::endl
              << "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the "
              << std::endl
              << "GNU General Public License for more details."
              << std::endl << std::endl
              << "You should have received a copy of the GNU General Public "
              << "License" << std::endl
              << "along with this program. If not, see " << std::endl
              << "<http://www.gnu.org/licenses/>." << std::endl;
    exit(0);
  }
  catch (CmdLineUsageHTMLError &e)
  {
    cmd.usageHTML(std::cout);
    exit(-1);
  }
  catch (CmdLineUsageXMLError &e)
  {
    cmd.usageXML(std::cout);
    exit(-1);
  }
  catch (CmdLineSyntaxError &e)
  {
    cmd.error() << e.str() << std::endl;
    cmd.usage(std::cerr);
    exit(-2);
  }    
}
