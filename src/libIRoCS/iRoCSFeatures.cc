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

#include "iRoCSFeatures.hh"

#include <libArrayToolbox/Random.hh>

#include <libsvmtl/StDataHdf5.hh>
#include <libsvmtl/MultiClassSVMOneVsOne.hh>
#include <libsvmtl/TwoClassSVMc.hh>
#include <libsvmtl/Kernel_RBF.hh>
#include <libsvmtl/Model.hh>

namespace iRoCS
{

  std::string Features::normalizationTypeToString(NormalizationType type)
  {
    switch (type)
    {
    case None :
      return "No normalization";
    case FeatureZeroMeanStddev :
      return "Feature-wise Zero Mean and Unit Standard deviation";
    case UnitNorm :
      return "Unit feature vector norm";
    case Combined :
      return "Unit vector norm + individual feature normalization";
    default :
      return "Invalid";
    }
    return "Invalid";
  }
  
  Features::NormalizationType Features::stringToNormalizationType(
      std::string const &normTypeString)
  {
    if (normTypeString == normalizationTypeToString(None))
        return None;
    else if (normTypeString == normalizationTypeToString(FeatureZeroMeanStddev))
        return FeatureZeroMeanStddev;
    else if (normTypeString == normalizationTypeToString(UnitNorm))
        return UnitNorm;
    else if (normTypeString == normalizationTypeToString(Combined))
        return Combined;
    return Invalid;
  }

  const int Features::PositiveMagnitude = 0x001;
  const int Features::NegativeMagnitude = 0x002;
  const int Features::PositiveRadius = 0x003;
  const int Features::NegativeRadius = 0x004;

  Features::Features(
      blitz::TinyVector<double,3> const &featureElementSizeUm,
      iRoCS::ProgressReporter *progress)
          : p_progress(progress)
  {
    std::cout << "Initializing iRoCS::Features... " << std::flush;
    _dataScaled.setElementSizeUm(featureElementSizeUm);
    _intrinsicCoordinates.setElementSizeUm(featureElementSizeUm);
    _houghDsNames[PositiveMagnitude] = "houghmap1";
    _houghDsNames[NegativeMagnitude] = "houghmap2";
    _houghDsNames[PositiveRadius] = "houghmapR1";
    _houghDsNames[NegativeRadius] = "houghmapR2";    
    std::cout << "OK" << std::endl;
  }

  Features::~Features()
  {}

  blitz::TinyVector<double,3> const &Features::elementSizeUm() const
  {
    return _dataScaled.elementSizeUm();
  }

  void Features::addFeatureToGroup(
      std::string const &groupName, std::string const &featureName)
  {
    size_t i = 0;
    for (; i < _featureGroups.size() && _featureGroups[i] != groupName; ++i);
    if (i == _featureGroups.size())
    {
      _featureGroups.push_back(groupName);
      _normalize.push_back(None);
      _featureNames.push_back(std::vector<std::string>());
    }
    _featureNames[i].push_back(featureName);
  }

  void Features::setGroupNormalization(
      std::string const &groupName, NormalizationType normType)
  {
    size_t i = 0;
    for (; i < _featureGroups.size() && _featureGroups[i] != groupName; ++i);
    if (i < _featureGroups.size()) _normalize[i] = normType;
  }

  std::string Features::sdFeatureName(atb::SDMagFeatureIndex const &index) const
  {
    std::stringstream dsStream;
    dsStream << "sigma" << std::setw(4) << std::setfill('0') << index.s
             << "_laplace" << std::setw(4) << std::setfill('0') << index.l
             << "_l" << std::setw(4) << std::setfill('0') << index.b;
    return dsStream.str();
  }

  std::string Features::houghFeatureName(const int state) const
  {
    return _houghDsNames.find(state)->second;
  }

  void Features::deleteFeature(atb::SDMagFeatureIndex const &index)
  {
    _sdFeatures.erase(index);
  }

  void Features::deleteFeature(const int state)
  {
    _houghFeatures.erase(state);
  }

  void Features::generateRandomSamples(
      std::vector< blitz::TinyVector<double,3> > &markers,
      blitz::TinyVector<double,3> const &upperBoundUm,
      int nInRootSamples, int nOutRootSamples)
  {
    if (p_progress != NULL && !p_progress->updateProgressMessage(
            "Computing distribution parameters"))
        return;
    blitz::TinyVector<double,3> mean = 0.0;
    blitz::TinyMatrix<double,3,3> cov;
    cov = 0.0;
    for (size_t i = 0; i < markers.size() &&
             (p_progress == NULL || !p_progress->isAborted()); ++i)
        mean += markers[i];
    mean /= static_cast<double>(markers.size());

    for (size_t i = 0; i < markers.size() &&
             (p_progress == NULL || !p_progress->isAborted()); ++i)
    {
      blitz::TinyVector<double,3> p = markers[i] - mean;
      cov = cov + atb::outerProduct(p, p);
    }
    cov = 1.0 / static_cast<double>(markers.size()) * cov;
  
    int sample = 0;
    while (sample < nInRootSamples) 
    {
      blitz::TinyVector<double,3> pos = atb::drawRandomVector(mean, cov);
      if (blitz::any(pos < 0 || pos >= upperBoundUm)) continue;

      size_t i = 0;
      for (; i < markers.size() &&
               (p_progress == NULL || !p_progress->isAborted()); ++i)
          if (blitz::dot(markers[i] - pos, markers[i] - pos) < 4.0) break;

      if (i == markers.size())
      {
        markers.push_back(pos);
        sample++;
        std::stringstream msg;
        msg << "Generating in-root samples (" << sample << " / "
            << nInRootSamples << ")";
        if (p_progress != NULL && !p_progress->updateProgressMessage(
                msg.str())) return;
      }
    }
  
    sample = 0;
    while (sample < nOutRootSamples) 
    {
      blitz::TinyVector<double,3> pos = atb::drawRandomVector(upperBoundUm);

      size_t i = 0;
      for (; i < markers.size() &&
               (p_progress == NULL || !p_progress->isAborted()); ++i)
          if (blitz::dot(markers[i] - pos, markers[i] - pos) < 4.0) break;

      if (i == markers.size())
      {
        markers.push_back(pos);
        sample++;
        std::stringstream msg;
        msg << "Generating out-of-root samples (" << sample << " / "
            << nOutRootSamples << ")";
        if (p_progress != NULL && !p_progress->updateProgressMessage(
                msg.str())) return;
      }
    }
  }

  void Features::normalizeFeatures(std::vector<svt::BasicFV> &samples)
  {
    size_t feaStart = 0;
    for (size_t i = 0; i < _featureGroups.size(); ++i) 
    {
      if ((int(_normalize[i]) & UnitNorm) == UnitNorm) 
      {
        std::string msg("Normalizing '" + _featureGroups[i] + "' to unit norm");
        if (p_progress != NULL)
        {
          if (!p_progress->updateProgressMessage(msg)) return;
        }
        else std::cout << msg << std::endl;
        
#ifdef _OPENMP
#pragma omp parallel for
#endif
        for (ptrdiff_t k = 0; k < static_cast<ptrdiff_t>(samples.size()); ++k)
        {
          if (p_progress != NULL && p_progress->isAborted()) continue;
          double norm = 0.0;
          for (size_t j = 0; j < _featureNames[i].size(); ++j)
              norm += samples[k][static_cast<int>(j + feaStart)] *
                  samples[k][static_cast<int>(j + feaStart)];
          norm = (norm > 0.0) ? 1.0 / std::sqrt(norm) : 0.0;
          for (size_t j = 0; j < _featureNames[i].size(); ++j)
              samples[k][static_cast<int>(j + feaStart)] *= norm;
        }
        if (p_progress != NULL && p_progress->isAborted()) return;
      }
      feaStart += _featureNames[i].size();
    }
    if (p_progress != NULL && p_progress->isAborted()) return;

    if (_means.size() == 0) 
    {
      if (p_progress != NULL && !p_progress->updateProgressMessage(
              "Computing feature means and standard deviations")) return;
      _means.resize(_featureGroups.size());
      _stddevs.resize(_featureGroups.size());
      size_t feaIdx = 0;
      for (size_t i = 0; i < _featureGroups.size(); ++i)
      {
        if (p_progress != NULL && p_progress->isAborted()) return;
        _means[i].resize(_featureNames[i].size(), 0.0);
        _stddevs[i].resize(_featureNames[i].size(), 0.0);

        for (ptrdiff_t j = 0;
             j < static_cast<ptrdiff_t>(_featureNames[i].size()); ++j) 
        {
          if (p_progress != NULL && p_progress->isAborted()) continue;
          for (std::vector<svt::BasicFV>::const_iterator it = samples.begin();
               it != samples.end(); ++it)
          {
            if (p_progress != NULL && p_progress->isAborted()) continue;
            _means[i][j] += (*it)[static_cast<int>(feaIdx)];
          }
          _means[i][j] /= static_cast<double>(samples.size());
          for (std::vector<svt::BasicFV>::const_iterator it = samples.begin();
               it != samples.end(); ++it)
          {
            if (p_progress != NULL && p_progress->isAborted()) continue;
            _stddevs[i][j] +=
                ((*it)[static_cast<int>(feaIdx)] - _means[i][j]) *
                ((*it)[static_cast<int>(feaIdx)] - _means[i][j]);
          }
          _stddevs[i][j] =
              std::sqrt(_stddevs[i][j] / static_cast<double>(samples.size()));
          ++feaIdx;
        }
        if (p_progress != NULL && p_progress->isAborted()) return;
      }
    }
  
    feaStart = 0;
    for (size_t i = 0; i < _featureGroups.size(); ++i) 
    {
      if ((int(_normalize[i]) & FeatureZeroMeanStddev) ==
          FeatureZeroMeanStddev) 
      {
        std::string msg(
            "Normalizing individual features of '" + _featureGroups[i] +
            "' to zero mean and unit standard deviation");
        if (p_progress != NULL)
        {
          if (!p_progress->updateProgressMessage(msg)) return;
        }
        else std::cout << msg << std::endl;

#ifdef _OPENMP
#pragma omp parallel for
#endif
        for (ptrdiff_t k = 0; k < static_cast<ptrdiff_t>(samples.size()); ++k)
        {
          if (p_progress != NULL && p_progress->isAborted()) continue;
          size_t feaIdx = feaStart;
          for (size_t j = 0; j < _featureNames[i].size(); ++j, ++feaIdx) 
          {
            samples[k][static_cast<int>(feaIdx)] =
                (samples[k][static_cast<int>(feaIdx)] - _means[i][j]) /
                _stddevs[i][j];
          }
        }
        if (p_progress != NULL && p_progress->isAborted()) return;
      }
      feaStart += _featureNames[i].size();
    }
  }

  void Features::saveNormalizationParameters(std::string const &modelFileName)
  {
    try
    {
      svt::StDataHdf5 modelMap(modelFileName.c_str(), H5F_ACC_RDWR);
      modelMap.setExceptionFlag(true);

      modelMap.setArray(
          "featureGroups", _featureGroups.begin(), _featureGroups.size());
      std::vector<int> normalize(_normalize.size());
      for (size_t i = 0; i < _normalize.size(); ++i)
          normalize[i] = int(_normalize[i]);
      modelMap.setArray("featureNormalization",
                        normalize.begin(), normalize.size());
      for (size_t i = 0; i < _featureGroups.size(); ++i) 
      {
        std::string groupName = h5GroupName(_featureGroups[i]);
        modelMap.setArray("featureNames_" + groupName,
                          _featureNames[i].begin(), _featureNames[i].size());
        modelMap.setArray("featureMeans_" + groupName,
                          _means[i].begin(), _means[i].size());
        modelMap.setArray("featureStddevs_" + groupName,
                          _stddevs[i].begin(), _stddevs[i].size());
      }
    }
    catch (std::exception &e)
    {
      if (p_progress != NULL)
          p_progress->abortWithError(
              std::string("Could not save normalization parameters to SVM "
                          "model: ") + e.what());
    }
  }

  void Features::loadNormalizationParameters(
      std::string const &modelFileName) 
  {
    try
    {
      svt::StDataHdf5 modelMap(modelFileName.c_str());
      modelMap.setExceptionFlag(true);

      int nFeatureGroups = modelMap.getArraySize("featureGroups");
      _featureGroups.resize(nFeatureGroups);
      modelMap.getArray(
          "featureGroups", _featureGroups.begin(), nFeatureGroups);
      _featureBaseGroup = _featureGroups.front().substr(
          0, _featureGroups.front().rfind("/"));
      _featureBaseGroup = _featureBaseGroup.substr(
          0, _featureBaseGroup.rfind("/"));
      _normalize.resize(nFeatureGroups);
      std::vector<int> normalize(nFeatureGroups);
      modelMap.getArray(
          "featureNormalization", normalize.begin(), nFeatureGroups);
      for (int i = 0; i < nFeatureGroups; ++i)
          _normalize[i] = static_cast<NormalizationType>(normalize[i]);
      _featureNames.resize(_featureGroups.size());
      _means.resize(_featureGroups.size());
      _stddevs.resize(_featureGroups.size());
      for (size_t i = 0; i < _featureGroups.size(); ++i) 
      {
        std::string groupName = h5GroupName(_featureGroups[i]);
        _featureNames[i].resize(
            modelMap.getArraySize("featureNames_" + groupName));
        int nFeatures = _featureNames[i].size();
        modelMap.getArray(
            "featureNames_" + groupName, _featureNames[i].begin(), nFeatures);
        _means[i].resize(nFeatures);
        modelMap.getArray(
            "featureMeans_" + groupName, _means[i].begin(), nFeatures);
        _stddevs[i].resize(nFeatures);
        modelMap.getArray(
            "featureStddevs_" + groupName, _stddevs[i].begin(), nFeatures);
      }
    }
    catch (std::exception &e)
    {
      if (p_progress != NULL) p_progress->abortWithError(
          std::string("Could not load normalization parameters from SVM "
                      "model:") + e.what());
    }
  }

  void Features::trainTwoClassSVM(
      std::vector<svt::BasicFV> &trainVectors,
      std::string const &modelFileName,
      float cost, float gamma) 
  {
    std::cout << "Training two-class SVM on " << trainVectors.size()
              << " training samples" << std::endl;
    
    svt::Model<svt::BasicFV> model;
    svt::TwoClassSVMc<svt::Kernel_RBF> svm;
    svm.kernel().setGamma(gamma);
    svm.setCost(cost); 
    svm.updateKernelCache(
        trainVectors.begin(), trainVectors.end(), svt::DirectAccessor());
    svm.train(trainVectors.begin(), trainVectors.end(), model);
    try
    {
      svt::StDataHdf5 modelMap(modelFileName.c_str(), H5F_ACC_RDWR);
      modelMap.setExceptionFlag(true);
      svm.saveParameters(modelMap);
      model.saveParameters(modelMap);
    }
    catch (std::exception& e) 
    {
      if (p_progress != NULL) p_progress->abortWithError(
          std::string("Could not save SVM model: ") + e.what());
    }
    std::cout << "Training finished" << std::endl;
  }

  void Features::classifyTwoClassSVM(
      std::vector<svt::BasicFV> &testVectors,
      std::string const &modelFileName)
  {
    std::cout << "Classifying " << testVectors.size() << " test samples"
              << std::endl;
    svt::Model<svt::BasicFV> model;
    svt::TwoClassSVMc<svt::Kernel_RBF> svm;

    try
    {
      svt::StDataHdf5 modelMap(modelFileName.c_str());
      modelMap.setExceptionFlag(true);
      model.loadParameters(modelMap);    
      svm.loadParameters(modelMap);
    }
    catch (std::exception &e)
    {
      if (p_progress != NULL) p_progress->abortWithError(
          "Could not load SVM model from '" + modelFileName + "': " + e.what());
      return;
    }
    
    svm.clearKernelCache();
    
    double progressStepPerClassification = 0.0;
    if (p_progress != NULL)
    {
      progressStepPerClassification =
          static_cast<double>(
              p_progress->taskProgressMax() - p_progress->taskProgressMin()) /
          static_cast<double>(testVectors.size());
      p_progress->updateProgressMessage("Classifying...");
    }

    int nClassified = 0;
#ifdef _OPENMP
#pragma omp parallel for
#endif
    for (ptrdiff_t i = 0; i < static_cast<ptrdiff_t>(testVectors.size()); ++i)
    {
      if (p_progress != NULL)
      {
        if (p_progress->isAborted()) continue;
#ifdef _OPENMP
#pragma omp critical
#endif
        {
          nClassified++;
          int progress = static_cast<int>(
              static_cast<double>(p_progress->taskProgressMin()) +
              static_cast<double>(nClassified) * progressStepPerClassification);
          p_progress->updateProgress(progress);
        }
      }
      testVectors[i].setLabel(svm.classify(testVectors[i], model));
    }
    std::cout << "Classification finished" << std::endl;
  }

  void Features::trainMultiClassSVM(
      std::vector<svt::BasicFV> &trainVectors,
      std::string const &modelFileName,
      float cost, float gamma) 
  {
    std::cout << "Training multi-class SVM on " << trainVectors.size()
              << " training samples" << std::endl;
    
    svt::Model_MC_OneVsOne< svt::Model<svt::BasicFV> > model;
    svt::MultiClassSVMOneVsOne<svt::TwoClassSVMc<svt::Kernel_RBF> > svm;
    svm.twoClassSVM().kernel().setGamma(gamma);
    svm.twoClassSVM().setCost(cost); 
    svm.updateKernelCache(
        trainVectors.begin(), trainVectors.end(), svt::DirectAccessor());
    svm.train(trainVectors.begin(), trainVectors.end(), model);
    try
    {
      svt::StDataHdf5 modelMap(modelFileName.c_str(), H5F_ACC_RDWR);
      modelMap.setExceptionFlag(true);
      svm.saveParameters(modelMap);
      model.saveParameters(modelMap);
    }
    catch (std::exception& e) 
    {
      if (p_progress != NULL) p_progress->abortWithError(
          std::string("Could not save SVM model: ") + e.what());
    }
    std::cout << "Training finished" << std::endl;
  }

  void Features::classifyMultiClassSVM(
      std::vector<svt::BasicFV> &testVectors,
      std::string const &modelFileName) 
  {
    std::cout << "Classifying " << testVectors.size() << " test samples"
              << std::endl;
    svt::Model_MC_OneVsOne< svt::Model<svt::BasicFV> > model;
    svt::MultiClassSVMOneVsOne< svt::TwoClassSVMc<svt::Kernel_RBF> > svm;

    try
    {
      svt::StDataHdf5 modelMap(modelFileName.c_str());
      modelMap.setExceptionFlag(true);
      model.loadParameters(modelMap);
      svm.loadParameters(modelMap);
    }
    catch (std::exception &e)
    {
      if (p_progress != NULL) p_progress->abortWithError(
          "Could not load SVM model from '" + modelFileName + "': " + e.what());
      return;
    }

    svm.clearKernelCache();
    
    double progressStepPerClassification = 0.0;
    double progress = 0;
    if (p_progress != NULL)
    {
      progress = p_progress->progress();
      progressStepPerClassification =
          static_cast<double>(
              p_progress->taskProgressMax() - p_progress->taskProgressMin()) /
          static_cast<double>(testVectors.size());
      p_progress->updateProgressMessage("Classifying...");
    }

#ifdef _OPENMP
#pragma omp parallel for
#endif
    for (ptrdiff_t i = 0; i < static_cast<ptrdiff_t>(testVectors.size()); ++i)
    {
      if (p_progress != NULL)
      {
        if (p_progress->isAborted()) continue;
#ifdef _OPENMP
#pragma omp atomic
#endif
        progress += progressStepPerClassification;
        p_progress->updateProgress(static_cast<int>(progress));
      }
      
      testVectors[i].setLabel(svm.classify(testVectors[i], model));
    }
    std::cout << "Classification finished" << std::endl;
  }

  std::string Features::h5GroupName(const std::string& rawGroup)
  {
    std::string res = rawGroup;
    while(res.find("/") != std::string::npos) res[res.find("/")] = '_';
    return res;
  }

}
