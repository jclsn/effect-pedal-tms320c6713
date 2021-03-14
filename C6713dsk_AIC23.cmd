/*----------------------------------------------------------------------
   DSK6713 command file                                                 
  ----------------------------------------------------------------------*/
/* these two steps must be done for each project manually*/
/* set in "Compiler -> Basic -> target version C671x" -mv6710 */
/* set in "Compiler -> Preprocessor -> define symbols -d" _debug;CHIP_6713 */

/* these two steps must be done for each project, using CCS 3.1 or 3.3*/
/* set in "Compiler -> Preprocessor -> include search path -i" $(Install_dir)\C6000\dsk6713\include */
/* (CCS 3.1:) set in "Linker -> library search path -i" $(Install_dir)\C6000\dsk6713\lib */
/* (CCS 3.3:) set in "Linker -> Libraries -> Search Path -i" $(Install_dir)\C6000\dsk6713\lib */

/* for AIC23-project:*/
/* set in "Compiler -> Advanced -> Memory Models" Far (--mem_model:data=far) 

/* the required libraries are:
   csl6713.lib; rts6700.lib; dsk6713bsl.lib   */
/* history tracking, changes:
   US :  8-05-2006 : version 1.0 tested without EMIF 
   US : 26-07-2006 : now with EMIF
*/
/*----------------------------------------------------------------------*/

-l csl6713.lib
-l rts6700.lib
-l dsk6713bsl.lib

-stack 0x400
-heap  0x400

--diag_suppress=16002

MEMORY
{
  IVECS:      org =         0h, len =      0x220
  IRAM:       org = 0x00000220, len = 0x0002FDE0 /*internal memory*/ 
  SDRAM:      org = 0x80000000, len = 0x00100000 /*external memory*/ 
  FLASH:      org = 0x90000000, len = 0x00020000 /*flash memory*/
}

/* now correct 26-07-06:
DSK6713 settings */
SECTIONS
{                
  .EXT_RAM :> SDRAM
  .vectors :> IVECS
  .text	   :> IRAM
  .bss     :> IRAM
  .cinit   :> IRAM
  .stack   :> IRAM
  .sysmem  :> IRAM
  .const   :> IRAM
  .switch  :> IRAM
  .far     :> IRAM
  .cio     :> IRAM   
}

