/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkWriteRawImage.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) 2000 National Library of Medicine
  All rights reserved.

  See COPYRIGHT.txt for copyright details.

=========================================================================*/
#include "itkWriteRawImage.h"
#include "itkObjectFactory.h"
#include "itkByteSwap.h"
#include <fstream>

namespace itk
{

/**
 *
 */
template <class TInputImage> 
WriteRawImage<TInputImage>
::WriteRawImage()
{
  m_ByteOrder = WriteRawImage::BigEndian;
}

/*
 * Internal method for writing data
 */
template <class TInputImage>
static void WriteDataArray(std::ofstream& f, TInputImage *image, 
                           int fileType, int byteOrder)
{
  // Define/declare an iterator that will walk the output region for this
  // thread.
  typedef typename TInputImage::PixelType InputImagePixelType;
  typedef
    ImageRegionIterator<InputImagePixelType, TInputImage::ImageDimension>
    InputIterator;

  typename TInputImage::Region region = image->GetLargestPossibleRegion();

  InputIterator inIt(image, region);

  if ( fileType == Writer::ASCII )
    {
    for ( int i=0; !inIt.IsAtEnd(); ++inIt, i++ )
      {
      if ( i && !(i%6) )
        {
        f << std::endl;
        }
      f << *inIt << " ";
      }
    }
  else
    {
    typedef typename TInputImage::ScalarValueType scalarType;
    scalarType foo;
    for ( int i=0; !inIt.IsAtEnd(); ++inIt, i++ )
      {
      foo = (scalarType) *inIt;
      if ( byteOrder == WriteRawImage<TInputImage>::BigEndian )
        {
        ByteSwap<scalarType>::SwapBE(&foo);
        }
      else
        {
        ByteSwap<scalarType>::SwapLE(&foo);
        }
      f.write((char *)&foo, sizeof(scalarType));
      }
    }
}

/**
 *
 */
template <class TInputImage>
void 
WriteRawImage<TInputImage>
::WriteData()
{
  std::ofstream f;

  itkDebugMacro(<<"Writing image in raw format");
  
  // Update the input
  //
  typename TInputImage::Pointer input=this->GetInput();
  input->SetRequestedRegionToLargestPossibleRegion();
  input->Update();

  // Open the file
  //
  if ( !this->GetFileName() )
    {
    itkErrorMacro(<<"No FileName specified! Can't write!");
    return;
    }
  
  if ( this->GetFileType() == Writer::ASCII )
    {
    f.open(this->GetFileName(), std::ios::out);
    }
  else
    { 
    f.open(this->GetFileName(), std::ios::out | std::ios::binary);
    }

  if ( f.fail() )
    {
    itkErrorMacro(<< "Unable to open file: "<< m_FileName);
    return;
    }

  // Now write the data
  //
  WriteDataArray(f, (TInputImage *)input, this->GetFileType(), m_ByteOrder);
}

/**
 *
 */
template <class TInputImage>
void 
WriteRawImage<TInputImage>
::PrintSelf(std::ostream& os, Indent indent)
{
  Superclass::PrintSelf(os,indent);

  if ( m_ByteOrder == WriteRawImage::BigEndian )
    {
    os << indent << "Byte Order: Big Endian\n";
    }
  else
    {
    os << indent << "Byte Order: Little Endian\n";
    }
}

} // end namespace itk
