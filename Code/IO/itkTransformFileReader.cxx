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
#ifndef __itkTransformFileReader_cxx
#define __itkTransformFileReader_cxx

#include "itkTransformFileReader.h"
#include "itkTransformFactoryBase.h"

#include "itksys/ios/sstream"

namespace itk
{
std::string trim(std::string const & source, char const *delims = " \t\r\n")
{
  std::string            result(source);
  std::string::size_type index = result.find_last_not_of(delims);

  if ( index != std::string::npos )
    {
    result.erase(++index);
    }

  index = result.find_first_not_of(delims);
  if ( index != std::string::npos )
    {
    result.erase(0, index);
    }
  else
    {
    result.erase();
    }
  return result;
}

/** Constructor */
TransformFileReader
::TransformFileReader()
{
  m_FileName = "";
  TransformFactoryBase::RegisterDefaultTransforms();
}

/** Destructor */
TransformFileReader
::~TransformFileReader()
{}

/** Update the Reader */
void TransformFileReader
::Update()
{
  TransformPointer transform;
  std::ifstream    in;

  in.open (m_FileName.c_str(), std::ios::in | std::ios::binary);
  if ( in.fail() )
    {
    in.close();
    itkExceptionMacro ("The file could not be opened for read access "
                       << std::endl << "Filename: \"" << m_FileName << "\"");
    }

  std::ostringstream InData;

  // in.get ( InData );
  std::filebuf *pbuf;
  pbuf = in.rdbuf();

  // get file size using buffer's members
  int size = static_cast< int >( pbuf->pubseekoff (0, std::ios::end, std::ios::in) );
  pbuf->pubseekpos (0, std::ios::in);

  // allocate memory to contain file data
  char *buffer = new char[size + 1];

  // get file data
  pbuf->sgetn (buffer, size);
  buffer[size] = '\0';
  itkDebugMacro ("Read file transform Data");
  InData << buffer;

  delete[] buffer;
  std::string data = InData.str();
  in.close();

  // Read line by line
  vnl_vector< double >   VectorBuffer;
  std::string::size_type position = 0;

  Array< double > TmpParameterArray;
  Array< double > TmpFixedParameterArray;
  TmpParameterArray.clear();
  TmpFixedParameterArray.clear();
  bool haveFixedParameters = false;
  bool haveParameters = false;

  //
  // check for line end convention
  std::string line_end("\n");
  if ( data.find('\n') == std::string::npos )
    {
    if ( data.find('\r') == std::string::npos )
      {
      itkExceptionMacro ("No line ending character found, not a valid ITK Transform TXT file");
      }
    line_end = "\r";
    }
  while ( position != std::string::npos && position < data.size() )
    {
    // Find the next string
    std::string::size_type end = data.find (line_end, position);
    //
    // detect files lacking terminal \n or \r
    if(end == std::string::npos)
      {
      position = std::string::npos;
      continue;
      }
    std::string            line = trim ( data.substr (position, end - position) );
    position = end + 1;
    itkDebugMacro ("Found line: \"" << line << "\"");

    if ( line.length() == 0 )
      {
      continue;
      }
    if ( line[0] == '#' || std::string::npos == line.find_first_not_of (" \t") )
      {
      // Skip lines beginning with #, or blank lines
      continue;
      }

    // Get the name
    end = line.find (":");
    if ( end == std::string::npos )
      {
      // Throw an error
      itkExceptionMacro ("Tags must be delimited by :");
      }
    std::string Name = trim ( line.substr (0, end) );
    std::string Value = trim ( line.substr ( end + 1, line.length() ) );
    // Push back
    itkDebugMacro ("Name: \"" << Name << "\"");
    itkDebugMacro ("Value: \"" << Value << "\"");
    itksys_ios::istringstream parse (Value);
    VectorBuffer.clear();
    if ( Name == "Transform" )
      {
      // Instantiate the transform
      itkDebugMacro ("About to call ObjectFactory");
      LightObject::Pointer i;
      i = ObjectFactoryBase::CreateInstance ( Value.c_str() );
      itkDebugMacro ("After call ObjectFactory");
      TransformType *ptr = dynamic_cast< TransformBase * >( i.GetPointer() );
      if ( ptr == NULL )
        {
        std::ostringstream msg;
        msg << "Could not create an instance of " << Value << std::endl
            << "The usual cause of this error is not registering the "
            << "transform with TransformFactory" << std::endl;
        msg << "Currently registered Transforms: " << std::endl;
        std::list< std::string >           names = TransformFactoryBase::GetFactory()->GetClassOverrideWithNames();
        std::list< std::string >::iterator it;
        for ( it = names.begin(); it != names.end(); it++ )
          {
          msg << "\t\"" << *it << "\"" << std::endl;
          }
        itkExceptionMacro ( << msg.str() );
        return;
        }
      transform = ptr;
      m_TransformList.push_back (transform);
      }
    else if ( Name == "Parameters" || Name == "FixedParameters" )
      {
      VectorBuffer.clear();

      // Read them
      parse >> VectorBuffer;
      itkDebugMacro ("Parsed: " << VectorBuffer);
      if ( Name == "Parameters" )
        {
        TmpParameterArray = VectorBuffer;
        itkDebugMacro ("Setting Parameters: " << TmpParameterArray);
        if ( haveFixedParameters )
          {
          transform->SetFixedParameters (TmpFixedParameterArray);
          itkDebugMacro ("Set Transform Fixed Parameters");
          transform->SetParametersByValue (TmpParameterArray);
          itkDebugMacro ("Set Transform Parameters");
          TmpParameterArray.clear();
          TmpFixedParameterArray.clear();
          haveFixedParameters = false;
          haveParameters = false;
          }
        else
          {
          haveParameters = true;
          }
        }
      else if ( Name == "FixedParameters" )
        {
        TmpFixedParameterArray = VectorBuffer;
        itkDebugMacro ("Setting Fixed Parameters: " << TmpFixedParameterArray);
        if ( !transform )
          {
          itkExceptionMacro ("Please set the transform before parameters or fixed parameters");
          }
        if ( haveParameters )
          {
          transform->SetFixedParameters (TmpFixedParameterArray);
          itkDebugMacro ("Set Transform Fixed Parameters");
          transform->SetParametersByValue (TmpParameterArray);
          itkDebugMacro ("Set Transform Parameters");
          TmpParameterArray.clear();
          TmpFixedParameterArray.clear();
          haveFixedParameters = false;
          haveParameters = false;
          }
        else
          {
          haveFixedParameters = true;
          }
        }
      }
    }
}
} // namespace itk

#endif
