/* devXxHP3458A.c */
/* @(#)devXxHP3458A.c    1.1 3/15/95 */
/*
 *      Author: John Winans
 *      Date:   02-18-92
 *
 *      Experimental Physics and Industrial Control System (EPICS)
 *
 *      Copyright 1995 the University of Chicago Board of Governors.
 *
 *      This software was produced under  U.S. Government contracts:
 *      (W-31-109-ENG-38) at Argonne National Laboratory.
 *
 *      Initial development by:
 *              The Controls and Computing Group
 *              Accelerator Systems Division
 *              Advanced Photon Source
 *              Argonne National Laboratory
 *
 * All rights reserved. No part of this publication may be reproduced, 
 * stored in a retrieval system, transmitted, in any form or by any
 * means,  electronic, mechanical, photocopying, recording, or otherwise
 * without prior written permission Argonne National Laboratory.
 *
 * Modification Log:
 * -----------------
 * .01  02-18-92    jrw     Initial Release
 * .02  03-13-95     frl     HP 3458A support under epics 3.12
 * .03  02-13-04     wen     R3.14/ASYN version
 */
#include    <epicsStdio.h>
#include    <devCommonGpib.h>


/******************************************************************************
 *
 * The following define statements are used to declare the names to be used
 * for the dset tables.   
 *
 * NOTE: The dsets are referenced by the entries in the command table.
 *
 ******************************************************************************/
#define    DSET_AI        devAiHP3458AGpib
#define    DSET_AO        devAoHP3458AGpib
#define    DSET_LI        devLiHP3458AGpib
#define    DSET_LO        devLoHP3458AGpib
#define    DSET_BI        devBiHP3458AGpib
#define    DSET_BO        devBoHP3458AGpib
#define    DSET_MBBO    devMbboHP3458AGpib
#define    DSET_MBBI    devMbbiHP3458AGpib
#define    DSET_SI        devSiHP3458AGpib
#define    DSET_SO        devSoHP3458AGpib

#include    <devGpib.h>

#define TIMEWINDOW   10.00    /* wait 10 seconds after device timeout */
#define TIMEOUT      1.0      /* I/O must complete within 1 second */

/******************************************************************************
 *
 * Strings used by the init routines to fill in the znam, onam, ...
 * fields in BI and BO record types.
 *
 ******************************************************************************/

static  char            *offOnList[] = { "Off", "On" };
static  struct  devGpibNames   offOn = { 2, offOnList, NULL, 1 };

static  char            *initNamesList[] = { "Init", "Init" };
static  struct  devGpibNames   initNames = { 2, initNamesList, NULL, 1 };

static  char    *disableEnableList[] = { "Disable", "Enable" };
static  struct  devGpibNames   disableEnable = { 2, disableEnableList, NULL, 1 };

static  char    *resetList[] = { "Reset", "Reset" };
static  struct  devGpibNames   reset = { 2, resetList, NULL, 1 };

static  char    *lozHizList[] = { "50 OHM", "HIGH Z" };
static  struct  devGpibNames   lozHiz = {2, lozHizList, NULL, 1};

static  char    *invertNormList[] = { "INVERT", "NORM" };
static  struct  devGpibNames   invertNorm = { 2, invertNormList, NULL, 1 };

static  char    *fallingRisingList[] = { "FALLING", "RISING" };
static  struct  devGpibNames   fallingRising = { 2, fallingRisingList, NULL, 1 };

static  char    *clearList[] = { "CLEAR", "CLEAR" };
static  struct  devGpibNames   clear = { 2, clearList, NULL, 1 };

static  char    *toneList[] = { "BEEP", "BEEP" };
static  struct  devGpibNames   tone = { 2, toneList, NULL, 1 };

/******************************************************************************
 *
 * Structures used by the init routines to fill in the onst, twst,... and the
 * onvl, twvl,... fields in MBBI and MBBO record types.
 *
 * Note that the intExtSsBm and intExtSsBmStop structures use the same
 * intExtSsBmStopList and intExtSsBmStopVal lists but have a different number
 * of elements in them that they use... The intExtSsBm structure only represents
 * 4 elements, while the intExtSsBmStop structure represents 5.
 *
 ******************************************************************************/

/*  status 1 
 *    L = Hi or Lo Limit exceeded 
 *    P = Power Cycled On
 *    E = Error (see error register )
 *    A = Data available 
******************************************************************************/
static char *status1List[] = {
                "    ",        /* zrst */
                "   L",        /* onst */
                "  P ",        /* twst */
                "  PL",        /* thst */
                " E  ",        /* frst */
                " E L",        /* fvst */
                " EP ",        /* sxst */
                " EPL",        /* svst */
                "A   ",        /* eist */
                "A  L",        /* nist */
                "A P ",        /* test */
                "A PL",        /* elst */
                "AE  ",        /* tvst */
                "AE L",        /* ttst */
                "AEP ",        /* ftst */
                "AEPL"};       /* ffst */
static unsigned long status1Val[] = {
                0x00,          /* zrvl */
                0x02,          /* onvl */
                0x08,          /* twvl */
                0x0a,          /* thvl */
                0x20,          /* frvl */
                0x22,          /* fvvl */
                0x28,          /* sxvl */
                0x2a,          /* svvl */
                0x80,          /* eivl */
                0x82,          /* nivl */
                0x88,          /* tevl */
                0x8a,          /* elvl */
                0xa0,          /* tvvl */
                0xa2,          /* ttvl */
                0xa8,          /* ftvl */
                0xaa};         /* ffvl */
static struct devGpibNames status1 = { 16, status1List, status1Val, 4};
                
/*  status 2
 *    SRQE = SRQ Command Executed 
 *    SRQ  = Service Requested 
 **********************************************************************/
static char        *status2List[] = {
                "        ",    /* zrst */
                "    SRQE",    /* onst */
                "SRQ     ",    /* twst */
                "SRQ SRQE"};   /* thst */
static unsigned long        status2Val[] = {
                0x00,          /* zrvl */
                0x04,          /* onvl */
                0x40,          /* twvl */
                0x44};         /* thvl */
static struct devGpibNames status2 = { 4, status2List, status2Val, 2};

/*  function
 **********************************************************************/
static char *functionList[] = {
                "DCV",         /* zrst */
                "ACV",         /* onst */
                "ACDCV",       /* twst */
                "OHM",         /* thst */
                "OHMF",        /* frst */
                "DCI",         /* fvst */
                "ACI",         /* sxst */
                "ACDCI",       /* svst */
                "FREQ",        /* eist */
                "PER",         /* nist */
                "DSAC",        /* test */
                "DSDC",        /* elst */
                "SSAC",        /* tvst */
                "SSDC"};       /* ttst */
static unsigned long        functionVal[] = {
                0x01,          /* zrvl */
                0x02,          /* onvl */
                0x03,          /* twvl */
                0x04,          /* thvl */
                0x05,          /* frvl */
                0x06,          /* fvvl */
                0x07,          /* sxvl */
                0x08,          /* svvl */
                0x09,          /* eivl */
                0x0a,          /* nivl */
                0x0b,          /* tevl */
                0x0c,          /* elvl */
                0x0d,          /* tvvl */
                0x0e};         /* ttvl */
static struct devGpibNames function = { 14, functionList, functionVal, 4};

/*  resolution
 **********************************************************************/
static char *resolutionList[] = {
                "0.00001",     /* zrst */
                "0.0001",      /* onst */
                "0.001",       /* twst */
                "0.01",        /* thst */
                "0.1"};        /* frst */
static unsigned long resolutionVal[] = {
                0x00,          /* zrvl */
                0x01,          /* onvl */
                0x02,          /* twvl */
                0x03,          /* thvl */
                0x04};         /* frvl */
static struct devGpibNames resolution = { 5, resolutionList, resolutionVal, 3};

/*  arrange
 **********************************************************************/
static char *arangeList[] = {
                "ON",          /* zrst */
                "OFF",         /* onst */
                "ONCE"};       /* twst */
static unsigned long arangeVal[] = {
                0x00,          /* zrvl */
                0x01,          /* onvl */
                0x02};         /* twvl */
static struct devGpibNames arange = { 3, arangeList, arangeVal, 2};

/*  trigger arming
 **********************************************************************/
static char *tarmList[] = {
                "Auto",        /* zrst */
                "External",    /* onst */
                "Single",      /* twst */
                "Hold",        /* thst */
                "Sync"};       /* frst */

static unsigned long tarmVal[] = {
                0x00,          /* zrvl */
                0x01,          /* onvl */
                0x02,          /* twvl */
                0x03,          /* thvl */
                0x04};         /* frvl */
static struct devGpibNames tarm = { 5, tarmList, tarmVal, 3};

/*  trigger
 **********************************************************************/
static char *trigList[] = {
                "Auto",        /* zrst */
                "External",    /* onst */
                "Single",      /* twst */
                "Hold",        /* thst */
                "Sync",        /* frst */
                "Level",       /* fvst */
                "Line"};       /* sxst */
static unsigned long trigVal[] = {
                0x00,          /* zrvl */
                0x01,          /* onvl */
                0x02,          /* twvl */
                0x03,          /* thvl */
                0x04,          /* frvl */
                0x05,          /* fvvl */
                0x07};         /* sxvl */
static struct devGpibNames trig = { 7, trigList, trigVal, 3};

/*  memory mode
 **********************************************************************/
static char *memList[] = {
                "MEM OFF",     /* zrst */
                "MEM LIFO",    /* onst */
                "MEM FIFO",    /* twst */
                "MEM CONT"};   /* thst */
static unsigned long memVal[] = {
                0x00,          /* zrvl */
                0x01,          /* onvl */
                0x02,          /* twvl */
                0x03};         /* thvl */
static struct devGpibNames mem = {4, memList, memVal, 2};

/*  EOI mode
 **********************************************************************/
static char *eoiList[] = {"Off", "On", "Always"};
static unsigned long eoiVal[] = {0, 1, 2};
static struct devGpibNames eoi = {3, eoiList, eoiVal, 3};

/******************************************************************************
 *
 * String arrays for EFAST operations.  Note that the last entry must be 
 * NULL.
 *
 * On input operations, only as many bytes as are found in the string array
 * elements are compared.  If there are more bytes than that in the input
 * message, they are ignored.  The first matching string found (starting
 * from the 0'th element) will be used as a match.
 *
 * NOTE: For the input operations, the strings are compared literally!  This
 * can cause problems if the instrument is returning things like \r and \n
 * characters.  You must take care when defining input strings so you include
 * them as well.
 *
 ******************************************************************************/
static char *userOffOn[] = {"USER OFF;", "USER ON;", NULL};
static char *endList[] = {"END OFF;", "END ON;", "END ALWAYS;", NULL};
static char *functionFList[] = {
                   "FUNC DCV",
                   "FUNC ACV",
                   "FUNC ACDCV",
                   "FUNC OHM",
                   "FUNC OHMF",
                   "FUNC DCI",
                   "FUNC ACI",
                   "FUNC ACDCI",
                   "FUNC FREQ",
                   "FUNC PER",
                   "FUNC DSAC",
                   "FUNC DSDC",
                   "FUNC SSAC",
                   "FUNC SSDC",
                   NULL};
static char *resolutionFList[] = {
                   "RES .00001",
                   "RES .0001",
                   "RES .001",
                   "RES .01",
                   "RES .1",
                   NULL};
static char *resolutioniFList[] = {
                   " 10.0000000E-06",
                   " 100.000000E-06",
                   " 1.00000000E-03",
                   " 10.0000000E-03",
                   " 100.000000E-03",
                   NULL};
static char *arangeFList[] = {
                   "ARANGE ON",
                   "ARANGE OFF",
                   "ARANGE ONCE",
                   NULL};
static char *arangeiFList[] = {"1", "0", "2", NULL};
static char *azeroFList[] = {
                   "AZERO ON",
                   "AZERO OFF",
                   "AZERO ONCE",
                   NULL};
static char *lockFList[] = {"LOCK OFF", "LOCK ON", NULL};
static char *tarmFList[] = {
                   "TARM AUTO",
                   "TARM EXT",
                   "TARM SGL",
                   "TARM HOLD",
                   "TARM SYN",
                   NULL};
static char *tarmiFList[] = {"1", "2", "3", "4", "5", NULL};
static char *trigFList[] = {
                   "TRIG AUTO",
                   "TRIG EXT",
                   "TRIG SGL",
                   "TRIG HOLD",
                   "TRIG SYN",
                   "TRIG LEVEL",
                   "TRIG LINE",
                   NULL};
static char *trigiFList[] = {"1", "2", "3", "4", "5", "7", "8", NULL};
static char *memFList[] = {"MEM OFF","MEM LIFO","MEM FIFO","MEM CONT", NULL};

/******************************************************************************
 *
 * Array of structures that define all GPIB messages
 * supported for this type of instrument.
 *
 ******************************************************************************/
static struct gpibCmd gpibCmds[] = {
   /* Param 0 */
    FILL,

   /* Param 1 Read MultiMeter */
  {&DSET_AI, GPIBRAWREAD, IB_Q_LOW, "", "%lf", 0, 32,
  NULL, 0, 0, NULL, NULL, NULL},

   /* Param 2 Read MultiMeter as String */
  {&DSET_SI, GPIBRAWREAD, IB_Q_LOW, "", NULL, 0, 32,
  NULL, 0, 0, NULL, NULL, NULL},


   /* Param 3 Read Status 1 */
  {&DSET_MBBI, GPIBREAD, IB_Q_LOW, "STB?", "%lu", 0, 32,
  NULL, 0, 0, NULL, &status1, NULL},

   /* Param 4 Read Status 2 */
  {&DSET_MBBI, GPIBREAD, IB_Q_LOW, "STB?", "%lu", 0, 32,
  NULL, 0, 0, NULL, &status2, NULL},

    /* Param 5  clear status byte */
  {&DSET_BO, GPIBCMD, IB_Q_HIGH, "CSB", NULL, 0, 32,
  NULL, 0, 0, NULL, &clear, NULL},

    /* Param 6  Disable Request Service  */
  {&DSET_BO, GPIBCMD, IB_Q_HIGH, "RQS 0", NULL, 0, 32,
  NULL, 0, 0, NULL, &clear, NULL},

    /* Param 7 EOI control */
  {&DSET_MBBO, GPIBEFASTO, IB_Q_HIGH, NULL, NULL, 0, 32,
  NULL, 0, 0, endList, &eoi, NULL},

    /* Param 8 Aux Error Status (Hw) */
  {&DSET_LI, GPIBREAD, IB_Q_HIGH, "AUXERR?", "%lu", 0, 32,
  NULL, 0, 0, NULL, NULL, NULL},

    /* Param 9  Error Status  */
  {&DSET_LI, GPIBREAD, IB_Q_HIGH, "ERR?", "%lu", 0, 32,
  NULL, 0, 0, NULL, NULL, NULL},

    /* Param 10 EOI control */
  {&DSET_MBBO, GPIBEFASTO, IB_Q_HIGH, NULL, NULL, 0, 32,
  NULL, 0, 0, endList, &eoi, NULL},

    /* Param 11 Set Function */
  {&DSET_MBBO, GPIBEFASTO, IB_Q_HIGH, NULL, NULL, 0, 32,
  NULL, 0, 0, functionFList, &function, NULL},

    /* Param 12  Generate Beep  */
  {&DSET_BO, GPIBCMD, IB_Q_HIGH, "TONE", NULL, 0, 32,
  NULL, 0, 0, NULL, &tone, NULL},

    /* Param 13 Status (Long In)*/
  {&DSET_LI, GPIBREAD, IB_Q_HIGH, "STB?", "%lu", 0, 32,
  NULL, 0, 0, NULL, NULL, NULL},

    /* Param 14 Set Range */
  {&DSET_AO, GPIBWRITE, IB_Q_HIGH, "RANGE", "RANGE %.2lf", 0, 32,
  NULL, 0, 0, NULL, NULL, NULL},

    /* Param 15 Set resolution */
  {&DSET_MBBO, GPIBEFASTO, IB_Q_HIGH, NULL, NULL, 0, 32,
  NULL, 0, 0, resolutionFList, &resolution, NULL},

    /* Param 16 Set AutoRange */
  {&DSET_MBBO, GPIBEFASTO, IB_Q_HIGH, NULL, NULL, 0, 32,
  NULL, 0, 0, arangeFList, &arange, NULL},

    /* Param 17 Set AutoZero */
  {&DSET_MBBO, GPIBEFASTO, IB_Q_HIGH, NULL, NULL, 0, 32,
  NULL, 0, 0, azeroFList, &arange, NULL},

    /* Param 18 Lock Front Panel */
  {&DSET_BO, GPIBEFASTO, IB_Q_HIGH, NULL, NULL, 0, 32,
  NULL, 0, 0, lockFList, &offOn, NULL},

    /* Param 19 Set Trigger ARM */
  {&DSET_MBBO, GPIBEFASTO, IB_Q_HIGH, NULL, NULL, 0, 32,
  NULL, 0, 0, tarmFList, &tarm, NULL},

    /* Param 20 Set Trigger */
  {&DSET_MBBO, GPIBEFASTO, IB_Q_HIGH, NULL, NULL, 0, 32,
  NULL, 0, 0, trigFList, &trig, NULL},

   /* Param 21 Read Instrument Temperature */
  {&DSET_AI, GPIBREAD, IB_Q_LOW, "TEMP?", "%lf", 0, 32,
  NULL, 0, 0, NULL, NULL, NULL},

    /* Param 22 Read Function */
  {&DSET_LI, GPIBREAD, IB_Q_HIGH, "FUNC?", "%lu,", 0, 32,
  NULL, 0, 0, NULL, NULL, NULL},

    /* Param 23 Read AutoRange */
  {&DSET_MBBI, GPIBEFASTI, IB_Q_HIGH, "ARANGE?", NULL, 0, 32,
  NULL, 0, 0, arangeiFList, &arange, NULL},

    /* Param 24 Read AutoZero */
  {&DSET_MBBI, GPIBEFASTI, IB_Q_HIGH, "AZERO?", NULL, 0, 32,
  NULL, 0, 0, arangeiFList, &arange, NULL},

    /* Param 25 READ Trigger ARM */
  {&DSET_MBBI, GPIBEFASTI, IB_Q_HIGH, "TARM?", NULL, 0, 32,
  NULL, 0, 0, tarmiFList, &tarm, NULL},

    /* Param 26 Read Trigger */
  {&DSET_MBBI, GPIBEFASTI, IB_Q_HIGH, "TRIG?", NULL, 0, 32,
  NULL, 0, 0, trigiFList, &trig, NULL},

    /* Param 27 Read resolution */
  {&DSET_MBBI, GPIBEFASTI, IB_Q_HIGH, "RES?", NULL, 0, 32,
  NULL, 0, 0, resolutioniFList, &resolution, NULL},

    /* Param 28 set mem lifo */
  {&DSET_BO, GPIBCMD, IB_Q_HIGH, "MEM LIFO", NULL, 0, 32,
  NULL, 0, 0, NULL, NULL, NULL},

    /* Param 29 Set mem */
  {&DSET_MBBO, GPIBEFASTO, IB_Q_HIGH, NULL, NULL, 0, 32,
  NULL, 0, 0, memFList, &mem, NULL},

   /* Param 30 Read MultiMeter */
  {&DSET_AI, GPIBREAD, IB_Q_HIGH, "RMEM;MEM CONT", "%lf,", 0, 32,
  NULL, 0, 0, NULL, NULL, NULL},

   /* Param 31 Read MultiMeter as String */
  {&DSET_SI, GPIBREAD, IB_Q_HIGH, "RMEM;MEM CONT", NULL, 0, 32,
  NULL, 0, 0, NULL, NULL, NULL},
};

/* The following is the number of elements in the command array above.  */
#define NUMPARAMS    sizeof(gpibCmds)/sizeof(struct gpibCmd)

/******************************************************************************
 *
 * Initialization for device support
 ******************************************************************************/
static long init_ai(int parm)
{
  if(parm==0)  {
    devSupParms.name = "devXxHP3458A";
    devSupParms.gpibCmds = gpibCmds;
    devSupParms.numparams = NUMPARAMS;
    devSupParms.timeout = TIMEOUT;
    devSupParms.timeWindow = TIMEWINDOW;
    devSupParms.respond2Writes = -1;
  }
  return 0;
}
