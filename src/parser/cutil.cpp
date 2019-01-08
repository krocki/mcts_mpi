/*
* Copyright 1993-2009 NVIDIA Corporation.  All rights reserved.
*
* NVIDIA Corporation and its licensors retain all intellectual property and 
* proprietary rights in and to this software and related documentation and 
* any modifications thereto.  Any use, reproduction, disclosure, or distribution 
* of this software and related documentation without an express license 
* agreement from NVIDIA Corporation is strictly prohibited.
* 
*/


/* CUda UTility Library */

/* Credit: Cuda team for the PGM file reader / writer code. */

// includes, file
#include "cutil.h"
#include <string.h>

// includes, system
#include <fstream>
#include <vector>
#include <iostream>
#include <algorithm>
#include <math.h>


// includes, common
#include "cmd_arg_reader.h"
#include "error_checker.h"


////////////////////////////////////////////////////////////////////////////////
//! Check if command line argument \a flag-name is given
//! @return 1 if command line argument \a flag_name has been given, otherwise 0
//! @param argc  argc as passed to main()
//! @param argv  argv as passed to main()
//! @param flag_name  name of command line flag
////////////////////////////////////////////////////////////////////////////////
CUTBoolean CUTIL_API
cutCheckCmdLineFlag( const int argc, const char** argv, const char* flag_name) 
{
    CUTBoolean ret_val = CUTFalse;

    try 
    {
        // initalize 
        CmdArgReader::init( argc, argv);

        // check if the command line argument exists
        if( CmdArgReader::existArg( flag_name)) 
        {
            ret_val = CUTTrue;
        }
    }
    catch( const std::exception& /*ex*/) 
    {    
        std::cerr << "Error when parsing command line argument string." << std::endl;
    } 

    return ret_val;
}

////////////////////////////////////////////////////////////////////////////////
//! Get the value of a command line argument of type int
//! @return CUTTrue if command line argument \a arg_name has been given and
//!         is of the requested type, otherwise CUTFalse
//! @param argc  argc as passed to main()
//! @param argv  argv as passed to main()
//! @param arg_name  name of the command line argument
//! @param val  value of the command line argument
////////////////////////////////////////////////////////////////////////////////
CUTBoolean CUTIL_API
cutGetCmdLineArgumenti( const int argc, const char** argv, 
                        const char* arg_name, int* val) 
{
    CUTBoolean ret_val = CUTFalse;

    try 
    {
        // initialize
        CmdArgReader::init( argc, argv);

        // access argument
        const int* v = CmdArgReader::getArg<int>( arg_name);
        if( NULL != v) 
        {
            // assign value
            *val = *v;
            ret_val = CUTTrue;
        }		
		else {
			// fail safe
			val = NULL;
		}
    }
    catch( const std::exception& /*ex*/) 
    {    
        std::cerr << "Error when parsing command line argument string." << std::endl;
    } 

    return ret_val;
}

////////////////////////////////////////////////////////////////////////////////
//! Get the value of a command line argument of type float
//! @return CUTTrue if command line argument \a arg_name has been given and
//!         is of the requested type, otherwise CUTFalse
//! @param argc  argc as passed to main()
//! @param argv  argv as passed to main()
//! @param arg_name  name of the command line argument
//! @param val  value of the command line argument
////////////////////////////////////////////////////////////////////////////////
CUTBoolean CUTIL_API
cutGetCmdLineArgumentf( const int argc, const char** argv, 
                       const char* arg_name, float* val) 
{
    CUTBoolean ret_val = CUTFalse;

    try 
    {
        // initialize
        CmdArgReader::init( argc, argv);

        // access argument
        const float* v = CmdArgReader::getArg<float>( arg_name);
        if( NULL != v) 
        {
            // assign value
            *val = *v;
            ret_val = CUTTrue;
        }
		else {
			// fail safe
			val = NULL;
		}
    }
    catch( const std::exception& /*ex*/) 
    {    
        std::cerr << "Error when parsing command line argument string." << std::endl;
    } 

    return ret_val;
}

////////////////////////////////////////////////////////////////////////////////
//! Get the value of a command line argument of type string
//! @return CUTTrue if command line argument \a arg_name has been given and
//!         is of the requested type, otherwise CUTFalse
//! @param argc  argc as passed to main()
//! @param argv  argv as passed to main()
//! @param arg_name  name of the command line argument
//! @param val  value of the command line argument
////////////////////////////////////////////////////////////////////////////////
CUTBoolean CUTIL_API
cutGetCmdLineArgumentstr( const int argc, const char** argv, 
                         const char* arg_name, char** val) 
{
    CUTBoolean ret_val = CUTFalse;

    try 
    {
        // initialize
        CmdArgReader::init( argc, argv);

        // access argument
        const std::string* v = CmdArgReader::getArg<std::string>( arg_name);
        if( NULL != v) 
        {

            // allocate memory for the string
            *val = (char*) malloc( sizeof(char) * (v->length() + 1));
            // copy from string to c_str
            strcpy( *val, v->c_str());
            ret_val = CUTTrue;
        }		
		else {
			// fail safe
			*val = NULL;
		}
    }
    catch( const std::exception& /*ex*/) 
    {    
        std::cerr << "Error when parsing command line argument string."<< 
        std::endl;
    } 

    return ret_val;

}

////////////////////////////////////////////////////////////////////////////////
//! Extended assert
//! @return CUTTrue if the condition \a val holds, otherwise CUTFalse
//! @param val  condition to test
//! @param file  __FILE__ macro
//! @param line  __LINE__ macro
////////////////////////////////////////////////////////////////////////////////
CUTBoolean CUTIL_API
cutCheckCondition( int val, const char* file, const int line) 
{
    CUTBoolean ret_val = CUTTrue;

    try 
    {
        // check for error
        ErrorChecker::condition( (0 == val) ? false : true, file, line);
    }
    catch( const std::exception& ex) 
    {
        // print where the exception occured
        std::cerr << ex.what() << std::endl;
        ret_val = CUTFalse;
    }

    return ret_val;
}
