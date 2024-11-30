/*  SPDX-FileCopyrightText: 2022 Gianluca Filippini */
/*  SPDX-License-Identifier: MIT                    */

/* set GNU header inclusion for ulseep */
#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include <gpiod.h>
#include <getopt.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>

/*
 * ***************************************************************************
 * TYPEDEFs
 * ***************************************************************************
 */
typedef struct app_params
{
    uint8_t       fps;
    bool          skip_fps;
    uint8_t       verbose_lvl;
    volatile bool quit;
} APP_PARAMS_T;


/*
 * ***************************************************************************
 * DEFINES
 * ***************************************************************************
 */

/* see ASTRIAL pin map for gpio selection
 * GPIO(N) gpiochip(N)    lane
 * ===========================
 * GPIO23   gpiochip1   1
 * GPIO24   gpiochip1   2
 * GPIO25   gpiochip1   3
 */

#define IMX8MP_CAM_GPIOCHIP "gpiochip1"
#define IMX8MP_CAM_GPIO ( 1 )

#define IMX8MP_CAM_FPS_MAX ( 120 )
#define IMX8MP_CAM_FPS_DEFAULT ( 15 )

#define OPTIONAL_ARGUMENT_IS_PRESENT                                        \
        ( ( optarg == NULL && optind < argc && argv[ optind ][ 0 ] != '-' ) \
      ? (bool) ( optarg = argv[ optind++ ] )                                \
      : ( optarg != NULL ) )

/*
 * ***************************************************************************
 * GLOBALS
 * ***************************************************************************
 */
const char    app_name[] = "simple_trigger";
APP_PARAMS_T* g_app_params = NULL;
const uint8_t fps_supported[] = { 1, 5, 10, 15, 24, 25, 30, 50, 60, 120, 0 };

/*
 * ***************************************************************************
 * PROTOTYPEs
 * ***************************************************************************
 */
static void ctrlc_handler( int s );
int8_t      help( int argc, char** argv );
int8_t      parse_cli( APP_PARAMS_T* app_params, int argc, char** argv  );


/*
 * ***************************************************************************
 * UTILS
 * ***************************************************************************
 */
static void ctrlc_handler( int s )
{
    printf( "[%s] Caught CTRL-C signal %d\n", app_name, s );

    g_app_params->quit = true;
}


int8_t help( int argc, char** argv )
{
    /* suppress compiler warnings */
    (void) argc;
    (void) argv;

    printf( " Usage: %s [options] \n", app_name );
    printf( " Options:\n" );
    printf( "   --fps                  Sampling speed fps {Default=30}\n" );
    printf( "   --skip                 Skip fps check\n" );
    printf( " \n" );
    return 0;
}


int8_t parse_cli( APP_PARAMS_T* app_params, int argc, char** argv  )
{
    int8_t               err = 0;
    int                  c;
    int8_t               quit_f = 0;

    static struct option long_options[] =
    {
        { "help",    no_argument,       0, 'h' },
        { "skip",    no_argument,       0, 's' },
        { "fps",     required_argument, 0, 'f' },
        { "verbose", optional_argument, 0, 'v' },
        { 0,         0,                 0, 0   }
    };

    while ( !quit_f )
    {
        int option_index = 0;

        c = getopt_long( argc, argv, "f:v::hs", long_options, &option_index );
        if ( c == -1 )
        {
            break;
        }

        switch ( c )
        {
            case 0:
                printf( "option %s", long_options[ option_index ].name );
                if ( optarg )
                {
                    printf( "option with arg %s", optarg );
                }
                printf( "\n" );
                break;

            case 'v':
                app_params->verbose_lvl = 1;
                if ( OPTIONAL_ARGUMENT_IS_PRESENT )
                {
                    app_params->verbose_lvl = ( 1 > atoi( optarg ) ) ? 1 : atoi( optarg );
                }
                break;

            case 'f':
                app_params->fps = ( 1 > atoi( optarg ) ) ? 1 : atoi( optarg );
                break;

            case 's':
                app_params->skip_fps = true;
                break;

            case 'h':
                help( argc, argv );
                quit_f = 1;
                err = -1;
                goto quit_parse;
                break;

            default:
                printf( "?? getopt returned character code 0%o ??\n", c );
        }
    }

    /* SANITY CHECK */
    if ( optind < argc )
    {
        printf( "\n [ERROR] unknown options: " );
        while ( optind < argc )
        {
            printf( "%s ", argv[ optind++ ] );
        }
        printf( "\n\n" );
        help( argc, argv );
        err = -1;
    }

quit_parse:
    return err;
}


/*
 * ***************************************************************************
 * MAIN
 * ***************************************************************************
 */
int main( int argc, char** argv )
{
    int8_t           err = 0;
    APP_PARAMS_T     app_params = { 0 };


    /*
     * Set globals & defaults
     */
    g_app_params = &app_params;
    app_params.fps = IMX8MP_CAM_FPS_DEFAULT;
    app_params.quit = false;

    /*
     * Set CTRL-C handler
     */
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = ctrlc_handler;
    sigemptyset( &sigIntHandler.sa_mask );
    sigIntHandler.sa_flags = 0;
    sigaction( SIGINT, &sigIntHandler, NULL );

    /*
     * CLI parameter parsing
     */
    err = parse_cli( &app_params, argc, argv );


    /*
     * SANITY CHECK: fps must be in the list!
     */
    if ( 0 == app_params.skip_fps )
    {
        uint8_t i = 0;
        while ( ( 0 != fps_supported[ i ] ) && fps_supported[ i ] != app_params.fps )
        {
            i++;
        }

        if ( fps_supported[ i ] == 0 )
        {
            printf( "[%s] error: unsupported fps\n", app_name );
            err = -1;
        }
    }

    /*
     * SANITY CHECK: fps must be reasonable!
     */
    if ( IMX8MP_CAM_FPS_MAX < app_params.fps )
    {
        printf( "[%s] error: unsupported fps\n", app_name );
        err = -1;
    }

    /*
     * TRIGGER loop
     */
    if ( 0 == err )
    {
        useconds_t fps_delay = (useconds_t) ( (float) 1000000.0 / app_params.fps );

        if ( app_params.verbose_lvl > 0 )
        {
            printf( "[%s] setting %d (fps) with a capture delay of %2.3f (usec)\n", app_name, app_params.fps, ( (float) fps_delay / 1000.0 ) );
        }

        printf( "Press CTRL-C to Exit\n" );
        while ( 0 == app_params.quit )
        {
            // toggle as fast as possible from user space
            gpiod_ctxless_set_value( IMX8MP_CAM_GPIOCHIP, IMX8MP_CAM_GPIO, 1, 0, argv[ 0 ], NULL, NULL );
            gpiod_ctxless_set_value( IMX8MP_CAM_GPIOCHIP, IMX8MP_CAM_GPIO, 0, 0, argv[ 0 ], NULL, NULL );
        }
    }


    return err;
}
