/* ###################################################################
**     This component module is generated by Processor Expert. Do not modify it.
**     Filename    : pwrMan.c
**     Project     : S32K144XWOS
**     Processor   : S32K144_100
**     Component   : power_manager
**     Version     : Component SDK_S32K1xx_15, Driver 01.00, CPU db: 3.00.000
**     Repository  : SDK_S32K1xx_15
**     Compiler    : GNU C Compiler
**     Date/Time   : 2020-06-11, 02:18, # CodeGen: 3
**
**     Copyright 1997 - 2015 Freescale Semiconductor, Inc. 
**     Copyright 2016-2017 NXP 
**     All Rights Reserved.
**     
**     THIS SOFTWARE IS PROVIDED BY NXP "AS IS" AND ANY EXPRESSED OR
**     IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
**     OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
**     IN NO EVENT SHALL NXP OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
**     INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
**     (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
**     SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
**     HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
**     STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
**     IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
**     THE POSSIBILITY OF SUCH DAMAGE.
** ###################################################################*/
/*!
** @file pwrMan.c
** @version 01.00
*/         
/*!
**  @addtogroup pwrMan_module pwrMan module documentation
**  @{
*/         
/* pwrMan. */

/* MODULE pwrMan.
 *
 * @page misra_violations MISRA-C:2012 violations
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 8.7, External variable could be made static.
 * The external variables will be used in other source files that user initialize
 * to use this module.
 */
#include "pwrMan.h"
#include <stdbool.h>

/* *************************************************************************
 * Configuration structure for Power Manager Configuration 0
 * ************************************************************************* */
/*! @brief User Configuration structure power_managerCfg_0 */
power_manager_user_config_t pwrMan_InitConfigRun = {
    .powerMode = POWER_MANAGER_RUN,                                  /*!< Power manager mode  */
    .sleepOnExitValue = false,                                       /*!< Sleep on exit value */
};        
/* *************************************************************************
 * Configuration structure for Power Manager Configuration 1
 * ************************************************************************* */
/*! @brief User Configuration structure power_managerCfg_1 */
power_manager_user_config_t pwrMan_InitConfigHsrun = {
    .powerMode = POWER_MANAGER_HSRUN,                                /*!< Power manager mode  */
    .sleepOnExitValue = false,                                       /*!< Sleep on exit value */
};        

/*! @brief Array of pointers to User configuration structures */
power_manager_user_config_t * powerConfigsArr[] = {
    &pwrMan_InitConfigRun,
    &pwrMan_InitConfigHsrun
};
/*! @brief Array of pointers to User defined Callbacks configuration structures */
power_manager_callback_user_config_t * powerStaticCallbacksConfigsArr[] = {(void *)0};


/* END pwrMan. */
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.1 [05.21]
**     for the Freescale S32K series of microcontrollers.
**
** ###################################################################
*/