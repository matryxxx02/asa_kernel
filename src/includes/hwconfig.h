/* ------------------------------
   $Id: hw_config.h 114 2009-12-01 13:06:43Z simon_duquennoy $
   ------------------------------------------------------------

   Fichier de configuration des acces au materiel

   Philippe Marquet, march 2007

   Code au niveau applicatif la description du materiel qui est fournie
   par hardware.ini
   
*/

#ifndef _HW_CONFIG_H_
#define _HW_CONFIG_H_

#define HARDWARE_INI	"hardware.ini"

/* HDA */
#define HDA_CMDREG	0x3F6
#define HDA_DATAREGS	0x110
#define HDA_IRQ		14
#define HDA_MAXCYLINDER 16
#define HDA_MAXSECTOR   16
#define HDA_SECTORSIZE  256    

/* HDB */
#define HDB_CMDREG	0x376
#define HDB_DATAREGS	0x170
#define HDB_IRQ		15
#define HDB_MAXCYLINDER 16
#define HDB_MAXSECTOR   16
#define HDB_SECTORSIZE  512

#define P0 (HDA_DATAREGS)
#define P1 (P0 + 1)
#define P2 (P1 + 1)
#define P3 (P2 + 1)
#define P4 (P3 + 1)
#define P5 (P4 + 1)
#define P6 (P5 + 1)
#define P7 (P6 + 1)
#define P8 (P7 + 1)
#define P9 (P8 + 1)
#define P10 (P9 + 1)
#define P11 (P10 + 1)
#define P12 (P11 + 1)
#define P13 (P12 + 1)
#define P14 (P13 + 1)
#define P15 (P14 + 1)

#endif
