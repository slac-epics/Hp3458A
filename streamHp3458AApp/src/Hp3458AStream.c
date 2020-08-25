/* for Agilent 3458A Multimeter
 * August 25th 2020 Shantha Condamoor (scondam@slac.stanford.edu)
 */
/******************************************************************************
 *
 * Test for stream support of GPIB
 *
 *****************************************************************************/

/******************************************************************************
 *
 * Use the TIME_WINDOW defn to indicate how long commands should be ignored
 * for a given device after it times out.  The ignored commands will be
 * returned as errors to device support.
 *
 * Use the IO_TIME to define how long you wish to wait for an I/O operation
 * to complete once started.
 *
 *****************************************************************************/
