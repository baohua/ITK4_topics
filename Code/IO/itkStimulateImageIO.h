/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
/*=========================================================================
 *
 *  Portions of this file are subject to the VTK Toolkit Version 3 copyright.
 *
 *  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 *
 *  For complete copyright, license and disclaimer of warranty information
 *  please refer to the NOTICE file at the top of the ITK source tree.
 *
 *=========================================================================*/
#ifndef __itkStimulateImageIO_h
#define __itkStimulateImageIO_h

#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

#include <fstream>
#include "itkImageIOBase.h"

namespace itk
{
/** \class StimulateImageIO
 *
 *  \brief ImageIO class for reading SDT/SPR (Stimulate) images
 *  This format is similar to a MetaImageIO file:
 *  The user should specify the .spr file (not the data file : .sdt)
 *
 *  This is based on the notes from:
 *
 *      http://www.cmrr.umn.edu/stimulate/stimUsersGuide/node57.html
 *
 *  It has been tested on:
 *
 *      ftp://ftp.cmrr.umn.edu/pub/stimulate/data/
 *
 *  \warning Beware of the scalar range of some files (See GetDisplayRange).
 *
 * \author Mathieu Malaterre
 *  CREATIS team at INSA - Lyon 2002
 *    http://www.creatis.insa-lyon.fr
 *
 * \ingroup IOFilters
 *
 */
class ITK_EXPORT StimulateImageIO:public ImageIOBase
{
public:
  /** Standard class typedefs. */
  typedef StimulateImageIO     Self;
  typedef ImageIOBase          Superclass;
  typedef SmartPointer< Self > Pointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(StimulateImageIO, Superclass);

  /*-------- This part of the interface deals with reading data. ------ */

  /** Determine the file type. Returns true if this ImageIO can read the
   * file specified. */
  virtual bool CanReadFile(const char *);

  /** Set the spacing and dimesion information for the current filename. */
  virtual void ReadImageInformation();

  /** Reads the data from disk into the memory buffer provided. */
  virtual void Read(void *buffer);

  /*-------- This part of the interfaces deals with writing data. ----- */

  /** Determine the file type. Returns true if this ImageIO can read the
   * file specified. */
  virtual bool CanWriteFile(const char *);

  /** Writes the spacing and dimentions of the image.
   * Assumes SetFileName has been called with a valid file name. */
  virtual void WriteImageInformation() {}

  /** Writes the data to disk from the memory buffer provided. Make sure
   * that the IORegion has been set properly. */
  virtual void Write(const void *buffer);

  /** Two values used for applying intensity windowing to the data set. The pair
   *  of numbers represent a low value and a hight value. Pixel values below the
   *  low_value will be displayed as black and voxels with values above the
   *  high_value will be displayed as white. Pixels with values within the display
   *  range are displayed with a grey value that is scaled linearly between the
   *  low_value and high_value. */
  itkGetVectorMacro(DisplayRange, const float, 2);
protected:
  StimulateImageIO();
  ~StimulateImageIO();
  void PrintSelf(std::ostream & os, Indent indent) const;

  bool OpenStimulateFileForReading(std::ifstream & os, const char *filename);

  bool OpenStimulateFileForWriting(std::ofstream & os, const char *filename);

  void InternalReadImageInformation(std::ifstream & file);

private:
  std::string m_DataFileName;
  char m_SdtOrient[256];
  float m_DisplayRange[2];
  float m_DisplayThresh;
  char m_FidName[256];

  StimulateImageIO(const Self &); //purposely not implemented
  void operator=(const Self &);   //purposely not implemented
};
} // end namespace itk

#endif // __itkStimulateImageIO_h
