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
#include <errno.h>

#include "logger.h"

/*
 * ***************************************************************************
 * TYPEDEFs
 * ***************************************************************************
 */
typedef struct app_params_
{
    /* system params */
    int           argc;
    char**        argv;
    uint32_t      app_tid;
    volatile bool quit;

    /* user params */
    uint8_t       fps;
    bool          skip_fps;
    uint8_t       verbose_lvl;
} APP_PARAMS_T;

typedef struct event_data_
{
    int event_cnt;
    uint32_t event_delay;
} EVENT_DATA_T;


/*
 * ***************************************************************************
 * DEFINES
 * ***************************************************************************
 */
#define UNUSED( x ) (void) ( x )


/* see ASTRIAL pin map for gpio selection
GPIO(N) gpiochip(N)    lane
===========================
GPIO23  gpiochip1       1
GPIO24  gpiochip1       2
GPIO25  gpiochip1       3
*/

#define IMX8MP_CAM_GPIOCHIP "gpiochip1"
#define IMX8MP_CAM_GPIO ( 1 )

#define IMX8MP_CAM_FPS_MAX ( 120 )
#define IMX8MP_CAM_FPS_DEFAULT ( 15 )

#define OPTIONAL_ARGUMENT_IS_PRESENT                                    \
    ( ( optarg == NULL && optind < argc && argv[ optind ][ 0 ] != '-' ) \
      ? (bool) ( optarg = argv[ optind++ ] )                            \
      : ( optarg != NULL ) )


/*
 * ***************************************************************************
 * GLOBALS
 * ***************************************************************************
 */
const char    app_name[] = "timer_trigger";
APP_PARAMS_T* g_app_params = NULL;
const uint8_t fps_supported[] = { 1, 5, 10, 15, 24, 25, 30, 50, 60, 120, 0 };


/*
 * ***************************************************************************
 * PROTOTYPEs
 * ***************************************************************************
 */
pid_t       gettid( void );
int8_t      help( int argc, char** argv );
int8_t      parse_cli( APP_PARAMS_T* app_params, int argc, char** argv  );
static void timer_handler( int sig, siginfo_t* si, void* uc );
static void ctrlc_handler( int s );

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


static void timer_handler( int sig, siginfo_t* si, void* uc )
{
    UNUSED( sig );
    UNUSED( uc );

    EVENT_DATA_T* data = (EVENT_DATA_T*) si->_sifields._rt.si_sigval.sival_ptr;

    LOG_MSG( g_app_params->verbose_lvl, LOG_MEDIUM,
             "[%s][%d] Timer fired %d \n", app_name, g_app_params->app_tid, ++data->event_cnt );

    gpiod_ctxless_set_value( IMX8MP_CAM_GPIOCHIP, IMX8MP_CAM_GPIO, 1, 0, g_app_params->argv[ 0 ], NULL, NULL );
    usleep( data->event_delay );
    gpiod_ctxless_set_value( IMX8MP_CAM_GPIOCHIP, IMX8MP_CAM_GPIO, 0, 0, g_app_params->argv[ 0 ], NULL, NULL );
}


int8_t help( int argc, char** argv )
{
    UNUSED( argc );
    UNUSED( argv );

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
    /*
     * Set globals & defaults
     */
    int8_t            err = 0;
    APP_PARAMS_T      app_params = { 0 };

    g_app_params = &app_params;

    app_params.fps = IMX8MP_CAM_FPS_DEFAULT;
    app_params.quit = false;
    app_params.argc = argc;
    app_params.argv = argv;
    app_params.app_tid = gettid( );

    /*
     * Timer: event signal action & data
     */
    timer_t           timer_id = 0;
    EVENT_DATA_T      event_data = { .event_cnt = 0, .event_delay=1 };
    struct sigevent   sigTimerEvent = { 0 };
    struct sigaction  sigTimerHandler = { 0 };

    /* default delay: set to 1fps */
    struct itimerspec timer_delay =
    {
        .it_value.tv_sec     = 1,
        .it_value.tv_nsec    = 0,
        .it_interval.tv_sec  = 1,
        .it_interval.tv_nsec = 0
    };

    /*
     * Set CTRL-C handler
     */
    struct sigaction  sigIntHandler = { 0 };
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
            printf( "[%s][%d] error: unsupported fps\n", app_name, g_app_params->app_tid );
            err = -1;
            goto exit;
        }
    }

    /*
     * SANITY CHECK: fps must be reasonable!
     */
    if ( IMX8MP_CAM_FPS_MAX < app_params.fps )
    {
        printf( "[%s][%d] error: unsupported fps\n", app_name, g_app_params->app_tid );
        err = -1;
        goto exit;
    }

    /*
     * Timer create
     */
    LOG_MSG( g_app_params->verbose_lvl, LOG_LOW,
             "[%s] Signal Interrupt Timer - thread-id: %d\n", app_name, g_app_params->app_tid );

    if ( app_params.fps != 1 )
    {
        uint64_t delay_nsec = (uint64_t) ( (float) 1000000000.0 / app_params.fps );

        timer_delay.it_value.tv_sec = 0;
        timer_delay.it_value.tv_nsec = delay_nsec;
        timer_delay.it_interval.tv_sec = 0;
        timer_delay.it_interval.tv_nsec = delay_nsec;

        /* set delay */
        event_data.event_delay = (uint32_t) (delay_nsec/2000);
    }

    sigTimerEvent.sigev_notify = SIGEV_SIGNAL;
    sigTimerEvent.sigev_signo = SIGRTMIN;
    sigTimerEvent.sigev_value.sival_ptr = &event_data;

    err = timer_create( CLOCK_REALTIME, &sigTimerEvent, &timer_id );
    if ( err != 0 )
    {
        LOG_ERR( LOG_DEFAULT, LOG_ALWAYS, "[%s] Error timer_create: %s\n", app_name, strerror( errno ) );
        err = -1;
        goto exit;
    }

    /*
     * Timer handler create & register
     */
    sigTimerHandler.sa_flags = SA_SIGINFO;
    sigTimerHandler.sa_sigaction = timer_handler;
    sigemptyset( &sigTimerHandler.sa_mask );

    LOG_MSG( g_app_params->verbose_lvl, LOG_LOW,
             "[%s][%d] Setting handler for signal %d\n", app_name, g_app_params->app_tid, SIGRTMIN );

    /* Register signal handler */
    if ( sigaction( SIGRTMIN, &sigTimerHandler, NULL ) == -1 )
    {
        LOG_ERR( LOG_DEFAULT, LOG_ALWAYS, "[%s][%d] Error sigaction: %s\n", app_name, gettid( ), strerror( errno ) );
        err = -1;
        goto cleanup;
    }

    /* Timer start */
    err = timer_settime( timer_id, 0, &timer_delay, NULL );
    if ( err != 0 )
    {
        LOG_ERR( LOG_DEFAULT, LOG_ALWAYS, "[%s][%d] Error timer_settime: %s\n", app_name, gettid( ), strerror( errno ) );
        err = -1;
        goto cleanup;
    }

    // printf( "Press ENTER to Exit\n" );
    // while ( ( g_app_params->quit == false ) && ( getchar( ) != '\n' ) )

    printf( "Press CTRL-C to Exit\n" );
    while ( ( g_app_params->quit == false ) )
    {
        usleep( 100 );
    }

cleanup:
    timer_delete( timer_id );

exit:
    return err;
}
