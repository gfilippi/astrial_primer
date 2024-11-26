/*  SPDX-FileCopyrightText: 2022 Gianluca Filippini */
/*  SPDX-License-Identifier: MIT                    */


/**
 *
 * logging macros for console output. These macros will be disabled when building
 * in RELEASE mode so that the code from printf is NOT part of the binary app.
 *
 */
#ifndef _H_BRWS_LOGGER_H_
#define _H_BRWS_LOGGER_H_

#include <stdio.h>
#include <stdint.h>


#define LOG_ALWAYS ( (uint8_t) ( 0 ) )
#define LOG_LOW ( (uint8_t) ( 1 ) )
#define LOG_MEDIUM  ( (uint8_t) ( 2 ) )
#define LOG_HIGH  ( (uint8_t) ( 3 ) )
#define LOG_MAX  ( (uint8_t) ( 4 ) )
#define LOG_ALL  ( (uint8_t) ( 5 ) )
#define LOG_DEFAULT ( (uint8_t) ( 254 ) )
#define LOG_NONE ( (uint8_t) ( 255 ) )

#define LOG_INFO "INFO"
#define LOG_WARNING "WARNING"
#define LOG_ERROR "ERROR"

typedef struct log_
{
    FILE* log_file;
} log_t;


// #if ( NDEBUG )
#if ( 0 )

/*
 * FLAG that tells the status of the LOGGER
 */
    #define LOGGER_ENABLED ( 0 )

/*
 * EMPTY wrapping macros
 */
    #define LOG_MSG( verbose, log_lvl, ... ) { }
    #define LOG_ERR( verbose, log_lvl, ... ) { }

#else // _DEBUG

/*
 * FLAG that tells the status of the LOGGER
 */
    #define LOGGER_ENABLED ( 1 )

/*
 * WRAPPING macros
 */

    #define LOG_MSG( verbose, log_lvl, ... ) \
    do                                       \
    {                                        \
        if ( ( verbose ) >= ( log_lvl ) ) {  \
            fprintf( stdout, __VA_ARGS__ );  \
        }                                    \
    } while ( 0 );

    #define LOG_ERR( verbose, log_lvl, ... ) \
    do                                       \
    {                                        \
        if ( ( verbose ) >= ( log_lvl ) ) {  \
            fprintf( stderr, __VA_ARGS__ );  \
        }                                    \
    } while ( 0 );


#endif /* NDEBUG */


#endif /* _H_LOGGER_H_ */
