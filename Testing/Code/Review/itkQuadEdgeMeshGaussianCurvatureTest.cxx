/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkQuadEdgeMeshGaussianCurvatureTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "itkQuadEdgeMesh.h"
#include "itkVTKPolyDataReader.h"

#include "itkQuadEdgeMeshExtendedTraits.h"
#include "itkQuadEdgeMeshDiscreteGaussianCurvatureEstimator.h"
#include "itkQuadEdgeMeshScalarDataVTKPolyDataWriter.h"

using namespace itk;

int itkQuadEdgeMeshGaussianCurvatureTest( int argc, char* argv[] )
{
  if( argc < 2 )
    {
    std::cout <<"*** GaussianCurvature ***" <<std::endl;
    std::cout <<"This example requires at least one argument:" <<std::endl;
    std::cout <<" 1- FileName" <<std::endl;
    return EXIT_FAILURE;
    }

  const unsigned int Dimension = 3;
  typedef double CoordType;

  typedef QuadEdgeMeshExtendedTraits <
    CoordType,
    Dimension,
    2,
    CoordType,
    CoordType,
    CoordType,
    bool,
    bool > Traits;

  typedef QuadEdgeMesh< CoordType, Dimension, Traits > MeshType;
  typedef QuadEdgeMeshDiscreteGaussianCurvatureEstimator<MeshType,MeshType>
    CurvatureFilterType;

  typedef VTKPolyDataReader< MeshType > ReaderType;

  ReaderType::Pointer reader = ReaderType::New( );
  reader->SetFileName( argv[1] );
  try
    {
    reader->Update( );
    }
  catch( itk::ExceptionObject & exp )
    {
    std::cerr << "Exception thrown while reading the input file " << std::endl;
    std::cerr << exp << std::endl;
    return EXIT_FAILURE;
    }

  MeshType::Pointer mesh = reader->GetOutput();

  CurvatureFilterType::Pointer gaussian_curvature = CurvatureFilterType::New();
  gaussian_curvature->SetInput( mesh );
  gaussian_curvature->Update();

  MeshType::Pointer output = gaussian_curvature->GetOutput();

  typedef QuadEdgeMeshScalarDataVTKPolyDataWriter< MeshType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( output );
  writer->SetFileName( "gaussian_curvature.vtk" );
  writer->Update();

  return EXIT_SUCCESS;
}