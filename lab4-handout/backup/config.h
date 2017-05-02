/*********************************************************
 * config.h - Configuration data for the driver.c program.
 *********************************************************/
#ifndef _CONFIG_H_
#define _CONFIG_H_

/* 
 * CPEs for the baseline (naive) version of the rotate function that
 * was handed out to the students. Rd is the measured CPE for a dxd
 * image. Run the driver.c program on your system to get these
 * numbers.  
 */
#define R64    3.5                                  
#define R128   4.8     
#define R256   7.1     
#define R512   11.1    
#define R1024  13.9 
 
/* 
 * CPEs for the baseline (naive) version of the smooth function that
 * was handed out to the students. Sd is the measure CPE for a dxd
 * image. Run the driver.c program on your system to get these
 * numbers.  
 */
#define S32   78    
#define S64   77    
#define S128  79     
#define S256  96    
#define S512  115
#define S1024 115
#endif /* _CONFIG_H_ */

