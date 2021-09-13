/**
  ******************************************************************************
  * @file    quadspi.c
  * @brief   This file provides code for the configuration
  *          of the QUADSPI instances.
  ******************************************************************************
  * @author
  * + 隐星魂 (Roy.Sun) <https://xwos.tech>
  * @copyright
  * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
  * > Licensed under the Apache License, Version 2.0 (the "License");
  * > you may not use this file except in compliance with the License.
  * > You may obtain a copy of the License at
  * >
  * >         http://www.apache.org/licenses/LICENSE-2.0
  * >
  * > Unless required by applicable law or agreed to in writing, software
  * > distributed under the License is distributed on an "AS IS" BASIS,
  * > WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * > See the License for the specific language governing permissions and
  * > limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "quadspi.h"

/* USER CODE BEGIN 0 */
#include <bm/stm32cube/standard.h>
#include <xwos/lib/xwbop.h>

enum w25q_mid_em {
  W25Q80 = 0xEF13,
  W25Q16 = 0xEF14,
  W25Q32 = 0xEF15,
  W25Q64 = 0xEF16,
  W25Q128 = 0xEF17,
  W25Q256 = 0xEF18,
};

enum w25q_bus_mode_em {
  W25Q_BUS_MODE_SPI = 1,
  W25Q_BUS_MODE_QPI = 4,
};

enum w25q_flash_cmd_em {
  W25Q_CMD_WRITE_DISABLE = 0x4U,
  W25Q_CMD_WRITE_ENABLE = 0x6U,

  W25Q_CMD_READ_STATUS_REG_1 = 0x05,
  W25Q_CMD_READ_STATUS_REG_2 = 0X35,
  W25Q_CMD_READ_STATUS_REG_3 = 0x15,
  W25Q_CMD_WRITE_STATUS_REG_1 = 0X01,
  W25Q_CMD_WRITE_STATUS_REG_2 = 0X31,
  W25Q_CMD_WRITE_STATUS_REG_3 = 0X11,

  W25Q_CMD_FAST_READ_DATA = 0X0B,
  W25Q_CMD_SET_READ_PARAMETERS = 0XC0,

  W25Q_CMD_PAGE_PROGRAM = 0X02,

  W25Q_CMD_BLOCK_ERASE = 0XD8,
  W25Q_CMD_SECTOR_ERASE = 0X20,
  W25Q_CMD_CHIP_ERASE = 0XC7,

  W25Q_CMD_POWER_DOWN = 0XB9,
  W25Q_CMD_RELEASE_POWER_DOWN = 0XAB,

  W25Q_CMD_UNIQUE_ID = 0X4B,
  W25Q_CMD_MANUFACTURER_DEVICE_ID = 0X90,
  W25Q_CMD_JEDEC_DEVICE_ID = 0X9F,

  W25Q_CMD_ENTER_4BYTE_ADDR_MODE = 0XB7,
  W25Q_CMD_EXIT_4BYTE_ADDR_MODE = 0XE9,

  W25Q_CMD_ENTER_QPI_MODE = 0X38,
  W25Q_CMD_EXIT_QPI_MODE = 0XFF,

  W25Q_CMD_ENABLE_RESET = 0x66,
  W25Q_CMD_RESET = 0x99,
};

#define W25Q_CMD_TIMEOUT  5000U

enum w25q_sr1_bit {
  W25Q_SR1_BUSY = 0x1,
  W25Q_SR1_WEL = 0x2,
  W25Q_SR1_BP0 = 0x4,
  W25Q_SR1_BP1 = 0x8,
  W25Q_SR1_BP2 = 0x10,
  W25Q_SR1_BP3 = 0x20,
  W25Q_SR1_TB = 0x40,
  W25Q_SR1_SRP0 = 0x80,
};

enum w25q_sr2_bit {
  W25Q_SR2_SRP1 = 0x1,
  W25Q_SR2_QE = 0x2,
  W25Q_SR2_R0 = 0x4,
  W25Q_SR2_LB1 = 0x8,
  W25Q_SR2_LB2 = 0x10,
  W25Q_SR2_LB3 = 0x20,
  W25Q_SR2_CMP = 0x40,
  W25Q_SR2_SUS = 0x80,
};

enum w25q_sr3_bit {
  W25Q_SR3_ADS = 0x1,
  W25Q_SR3_ADP = 0x2,
  W25Q_SR3_WPS = 0x4,
  W25Q_SR3_R1 = 0x8,
  W25Q_SR3_R2 = 0x10,
  W25Q_SR3_DRV0 = 0x20,
  W25Q_SR3_DRV1 = 0x40,
  W25Q_SR3_HOLD_RST = 0x80,
};

struct w25q_flash {
  xwu32_t origin;
  xwu32_t size;
  xwu64_t uid;
  xwu16_t mid;
  xwu32_t jid;
  xwu32_t bus;
  xwu32_t addrbits;
};

struct w25q_flash w25q = {
  .origin = 0x90000000U,
  .size = (1 << 25),
  .uid = 0,
  .mid = 0,
  .jid = 0,
  .bus = W25Q_BUS_MODE_SPI,
  .addrbits = QSPI_ADDRESS_24_BITS,
};

static
xwer_t MX_W25Q_Read_SR(xwu32_t sridx, xwu8_t * srbuf);

static
xwer_t MX_W25Q_Write_SR(xwu32_t sridx, xwu8_t sr);

static
xwer_t MX_W25Q_Enable_QPI_Mode(void);

static
xwer_t MX_W25Q_Disable_QPI_Mode(void);

static
xwer_t MX_W25Q_Enter_4Byte_Addr_Mode(void);

static
xwer_t MX_W25Q_Exit_4Byte_Addr_Mode(void);

static
xwer_t MX_W25Q_Set_ReadParameters(void);

/* USER CODE END 0 */

QSPI_HandleTypeDef hqspi;

/* QUADSPI init function */
void MX_QUADSPI_Init(void)
{

  /* USER CODE BEGIN QUADSPI_Init 0 */

  /* USER CODE END QUADSPI_Init 0 */

  /* USER CODE BEGIN QUADSPI_Init 1 */

  /* USER CODE END QUADSPI_Init 1 */
  hqspi.Instance = QUADSPI;
  hqspi.Init.ClockPrescaler = 1;
  hqspi.Init.FifoThreshold = 24;
  hqspi.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_HALFCYCLE;
  hqspi.Init.FlashSize = 24;
  hqspi.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_6_CYCLE;
  hqspi.Init.ClockMode = QSPI_CLOCK_MODE_0;
  hqspi.Init.FlashID = QSPI_FLASH_ID_1;
  hqspi.Init.DualFlash = QSPI_DUALFLASH_DISABLE;
  if (HAL_QSPI_Init(&hqspi) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN QUADSPI_Init 2 */

  /* USER CODE END QUADSPI_Init 2 */

}

void HAL_QSPI_MspInit(QSPI_HandleTypeDef* qspiHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(qspiHandle->Instance==QUADSPI)
  {
  /* USER CODE BEGIN QUADSPI_MspInit 0 */

  /* USER CODE END QUADSPI_MspInit 0 */
  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_QSPI;
    PeriphClkInitStruct.QspiClockSelection = RCC_QSPICLKSOURCE_D1HCLK;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* QUADSPI clock enable */
    __HAL_RCC_QSPI_CLK_ENABLE();

    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**QUADSPI GPIO Configuration
    PG6     ------> QUADSPI_BK1_NCS
    PF6     ------> QUADSPI_BK1_IO3
    PF7     ------> QUADSPI_BK1_IO2
    PF8     ------> QUADSPI_BK1_IO0
    PF9     ------> QUADSPI_BK1_IO1
    PB2     ------> QUADSPI_CLK
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_QUADSPI;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_QUADSPI;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_QUADSPI;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_QUADSPI;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN QUADSPI_MspInit 1 */

  /* USER CODE END QUADSPI_MspInit 1 */
  }
}

void HAL_QSPI_MspDeInit(QSPI_HandleTypeDef* qspiHandle)
{

  if(qspiHandle->Instance==QUADSPI)
  {
  /* USER CODE BEGIN QUADSPI_MspDeInit 0 */

  /* USER CODE END QUADSPI_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_QSPI_CLK_DISABLE();

    /**QUADSPI GPIO Configuration
    PG6     ------> QUADSPI_BK1_NCS
    PF6     ------> QUADSPI_BK1_IO3
    PF7     ------> QUADSPI_BK1_IO2
    PF8     ------> QUADSPI_BK1_IO0
    PF9     ------> QUADSPI_BK1_IO1
    PB2     ------> QUADSPI_CLK
    */
    HAL_GPIO_DeInit(GPIOG, GPIO_PIN_6);

    HAL_GPIO_DeInit(GPIOF, GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_2);

  /* USER CODE BEGIN QUADSPI_MspDeInit 1 */

  /* USER CODE END QUADSPI_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
void MX_W25Q_Init(void)
{
  xwer_t rc;
  xwu64_t uid;
  xwu16_t mid;
  xwu32_t jid;
  xwu8_t sr3;

  MX_QUADSPI_Init();
  w25q.bus = W25Q_BUS_MODE_QPI;
  rc = MX_W25Q_Reset();
  w25q.bus = W25Q_BUS_MODE_SPI;
  rc = MX_W25Q_Reset();
  rc = MX_W25Q_Read_UID(&uid);
  if (XWOK == rc) {
    w25q.uid = uid;
  }
  rc = MX_W25Q_Read_MID(&mid);
  if (XWOK == rc) {
    w25q.mid = mid;
  }
  rc = MX_W25Q_Read_JID(&jid);
  if (XWOK == rc) {
    w25q.jid = jid;
  }
  switch (w25q.mid) {
    case W25Q80:
      w25q.size = (1 << 20);
      break;
    case W25Q16:
      w25q.size = (1 << 21);
      break;
    case W25Q32:
      w25q.size = (1 << 22);
      break;
    case W25Q64:
      w25q.size = (1 << 23);
      break;
    case W25Q128:
      w25q.size = (1 << 24);
      break;
    case W25Q256:
      w25q.size = (1 << 25);
      break;
    default:
      Error_Handler();
      break;
  }

  sr3 = 0;
  rc = MX_W25Q_Read_SR(3, &sr3);
  if (XWOK == rc) {
    if (0 == (sr3 & W25Q_SR3_ADS)) {
      w25q.addrbits = QSPI_ADDRESS_24_BITS;
    } else {
      w25q.addrbits = QSPI_ADDRESS_32_BITS;
    }
  } else {
    Error_Handler();
  }

  rc = MX_W25Q_Set_BusMode(W25Q_BUS_MODE_QPI);
  if (rc < 0) {
        Error_Handler();
  }

  if (W25Q256 == w25q.mid) {
    rc = MX_W25Q_Set_AddrBits(QSPI_ADDRESS_32_BITS);
    if (rc < 0) {
      if (-EALREADY != rc) {
        Error_Handler();
      }
    }
  }

  rc = MX_W25Q_Set_ReadParameters();
  if (rc < 0) {
    Error_Handler();
  }

  rc = MX_W25Q_Mmap();
  if (rc < 0) {
    Error_Handler();
  }
}

void MX_W25Q_DeInit(void)
{
  MX_W25Q_Reset();
  HAL_QSPI_DeInit(&hqspi);
}

xwer_t MX_W25Q_Write_Enable(void)
{
  QSPI_CommandTypeDef cmd;
  HAL_StatusTypeDef halrc;
  xwer_t rc;

  cmd.Instruction = W25Q_CMD_WRITE_ENABLE;
  cmd.Address = 0;
  cmd.AlternateBytes = 0;
  cmd.AddressSize = QSPI_ADDRESS_8_BITS;
  cmd.AlternateBytesSize = QSPI_ALTERNATE_BYTES_8_BITS;
  cmd.DummyCycles = 0;
  if (W25Q_BUS_MODE_QPI == w25q.bus) {
    cmd.InstructionMode = QSPI_INSTRUCTION_4_LINES;
  } else {
    cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
  }
  cmd.AddressMode = QSPI_ADDRESS_NONE;
  cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  cmd.DataMode = QSPI_DATA_NONE;
  cmd.NbData = 0;
  cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
  cmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
  cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

  halrc = HAL_QSPI_Command(&hqspi, &cmd, W25Q_CMD_TIMEOUT);
  if (HAL_OK == halrc) {
    rc = XWOK;
  } else {
    rc = -EIO;
  }
  return rc;
}

xwer_t MX_W25Q_Write_Disable(void)
{
  QSPI_CommandTypeDef cmd;
  HAL_StatusTypeDef halrc;
  xwer_t rc;

  cmd.Instruction = W25Q_CMD_WRITE_DISABLE;
  cmd.Address = 0;
  cmd.AlternateBytes = 0;
  cmd.AddressSize = QSPI_ADDRESS_8_BITS;
  cmd.AlternateBytesSize = QSPI_ALTERNATE_BYTES_8_BITS;
  cmd.DummyCycles = 0;
  if (W25Q_BUS_MODE_QPI == w25q.bus) {
    cmd.InstructionMode = QSPI_INSTRUCTION_4_LINES;
  } else {
    cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
  }
  cmd.AddressMode = QSPI_ADDRESS_NONE;
  cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  cmd.DataMode = QSPI_DATA_NONE;
  cmd.NbData = 0;
  cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
  cmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
  cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

  halrc = HAL_QSPI_Command(&hqspi, &cmd, W25Q_CMD_TIMEOUT);
  if (HAL_OK == halrc) {
    rc = XWOK;
  } else {
    rc = -EIO;
  }
  return rc;
}

static
xwer_t MX_W25Q_Read_SR(xwu32_t sridx, xwu8_t * srbuf)
{
  QSPI_CommandTypeDef cmd;
  HAL_StatusTypeDef halrc;
  xwer_t rc;

  switch (sridx) {
    case 2:
      cmd.Instruction = W25Q_CMD_READ_STATUS_REG_2;
      break;
    case 3:
      cmd.Instruction = W25Q_CMD_READ_STATUS_REG_3;
      break;
    case 1:
    default:
      cmd.Instruction = W25Q_CMD_READ_STATUS_REG_1;
      break;
  }
  cmd.Address = 0;
  cmd.AlternateBytes = 0;
  cmd.AddressSize = QSPI_ADDRESS_8_BITS;
  cmd.AlternateBytesSize = QSPI_ALTERNATE_BYTES_8_BITS;
  cmd.DummyCycles = 0;
  if (W25Q_BUS_MODE_QPI == w25q.bus) {
    cmd.InstructionMode = QSPI_INSTRUCTION_4_LINES;
    cmd.DataMode = QSPI_DATA_4_LINES;
  } else {
    cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    cmd.DataMode = QSPI_DATA_1_LINE;
  }
  cmd.AddressMode = QSPI_ADDRESS_NONE;
  cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  cmd.NbData = 1;
  cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
  cmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
  cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

  halrc = HAL_QSPI_Command(&hqspi, &cmd, W25Q_CMD_TIMEOUT);
  if (HAL_OK == halrc) {
    halrc = HAL_QSPI_Receive(&hqspi, srbuf, W25Q_CMD_TIMEOUT);
    if (HAL_OK == halrc) {
      rc = XWOK;
    } else {
      rc = -EIO;
    }
  } else {
    rc = -EIO;
  }
  return rc;
}

static
xwer_t MX_W25Q_Write_SR(xwu32_t sridx, xwu8_t sr)
{
  QSPI_CommandTypeDef cmd;
  HAL_StatusTypeDef halrc;
  xwer_t rc;

  switch (sridx) {
    case 2:
      cmd.Instruction = W25Q_CMD_READ_STATUS_REG_2;
      break;
    case 3:
      cmd.Instruction = W25Q_CMD_READ_STATUS_REG_3;
      break;
    case 1:
    default:
      cmd.Instruction = W25Q_CMD_READ_STATUS_REG_1;
      break;
  }
  cmd.Address = 0;
  cmd.AlternateBytes = 0;
  cmd.AddressSize = QSPI_ADDRESS_8_BITS;
  cmd.AlternateBytesSize = QSPI_ALTERNATE_BYTES_8_BITS;
  cmd.DummyCycles = 0;
  if (W25Q_BUS_MODE_QPI == w25q.bus) {
    cmd.InstructionMode = QSPI_INSTRUCTION_4_LINES;
    cmd.DataMode = QSPI_DATA_4_LINES;
  } else {
    cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    cmd.DataMode = QSPI_DATA_1_LINE;
  }
  cmd.AddressMode = QSPI_ADDRESS_NONE;
  cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  cmd.NbData = 1;
  cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
  cmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
  cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

  halrc = HAL_QSPI_Command(&hqspi, &cmd, W25Q_CMD_TIMEOUT);
  if (HAL_OK == halrc) {
    halrc = HAL_QSPI_Transmit(&hqspi, &sr, W25Q_CMD_TIMEOUT);
    if (HAL_OK == halrc) {
      rc = XWOK;
    } else {
      rc = -EIO;
    }
  } else {
    rc = -EIO;
  }
  return rc;
}

xwer_t MX_W25Q_Read_UID(xwu64_t * uidbuf)
{
  QSPI_CommandTypeDef cmd;
  HAL_StatusTypeDef halrc;
  xwer_t rc;

  cmd.Instruction = W25Q_CMD_UNIQUE_ID;
  cmd.Address = 0;
  cmd.AlternateBytes = 0;
  cmd.AddressSize = w25q.addrbits;
  cmd.AlternateBytesSize = QSPI_ALTERNATE_BYTES_32_BITS;
  cmd.DummyCycles = 0;
  cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
  cmd.DataMode = QSPI_DATA_1_LINE;
  cmd.AddressMode = QSPI_ADDRESS_NONE;
  cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_1_LINE;
  cmd.NbData = 8;
  cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
  cmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
  cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

  halrc = HAL_QSPI_Command(&hqspi, &cmd, W25Q_CMD_TIMEOUT);
  if (HAL_OK == halrc) {
    halrc = HAL_QSPI_Receive(&hqspi, (uint8_t *)uidbuf, W25Q_CMD_TIMEOUT);
    if (HAL_OK == halrc) {
      *uidbuf = xwbop_re(xwu64_t, *uidbuf);
      rc = XWOK;
    } else {
      rc = -EIO;
    }
  } else {
    rc = -EIO;
  }
  return rc;
}

xwer_t MX_W25Q_Read_MID(xwu16_t * midbuf)
{
  QSPI_CommandTypeDef cmd;
  HAL_StatusTypeDef halrc;
  xwer_t rc;

  cmd.Instruction = W25Q_CMD_MANUFACTURER_DEVICE_ID;
  cmd.Address = 0;
  cmd.AlternateBytes = 0;
  cmd.AddressSize = QSPI_ADDRESS_24_BITS;
  cmd.AlternateBytesSize = QSPI_ALTERNATE_BYTES_8_BITS;
  cmd.DummyCycles = 0;
  cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
  cmd.DataMode = QSPI_DATA_1_LINE;
  cmd.AddressMode = QSPI_ADDRESS_1_LINE;
  cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  cmd.NbData = 2;
  cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
  cmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
  cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

  halrc = HAL_QSPI_Command(&hqspi, &cmd, W25Q_CMD_TIMEOUT);
  if (HAL_OK == halrc) {
    halrc = HAL_QSPI_Receive(&hqspi, (uint8_t *)midbuf, W25Q_CMD_TIMEOUT);
    if (HAL_OK == halrc) {
      *midbuf = xwbop_re(xwu16_t, *midbuf);
      rc = XWOK;
    } else {
      rc = -EIO;
    }
  } else {
    rc = -EIO;
  }
  return rc;
}

xwer_t MX_W25Q_Read_JID(xwu32_t * jidbuf)
{
  QSPI_CommandTypeDef cmd;
  HAL_StatusTypeDef halrc;
  xwer_t rc;

  cmd.Instruction = W25Q_CMD_JEDEC_DEVICE_ID;
  cmd.Address = 0;
  cmd.AlternateBytes = 0;
  cmd.AddressSize = QSPI_ADDRESS_8_BITS;
  cmd.AlternateBytesSize = QSPI_ALTERNATE_BYTES_8_BITS;
  cmd.DummyCycles = 0;
  if (W25Q_BUS_MODE_QPI == w25q.bus) {
    cmd.InstructionMode = QSPI_INSTRUCTION_4_LINES;
    cmd.DataMode = QSPI_DATA_4_LINES;
  } else {
    cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    cmd.DataMode = QSPI_DATA_1_LINE;
  }
  cmd.AddressMode = QSPI_ADDRESS_NONE;
  cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  cmd.NbData = 3;
  cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
  cmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
  cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

  halrc = HAL_QSPI_Command(&hqspi, &cmd, W25Q_CMD_TIMEOUT);
  if (HAL_OK == halrc) {
    halrc = HAL_QSPI_Receive(&hqspi, (uint8_t *)jidbuf, W25Q_CMD_TIMEOUT);
    if (HAL_OK == halrc) {
      *jidbuf = xwbop_re(xwu32_t, *jidbuf);
      rc = XWOK;
    } else {
      rc = -EIO;
    }
  } else {
    rc = -EIO;
  }
  return rc;
}

static
xwer_t MX_W25Q_Enable_QPI_Mode(void)
{
  QSPI_CommandTypeDef cmd;
  HAL_StatusTypeDef halrc;
  xwer_t rc;

  cmd.Instruction = W25Q_CMD_ENTER_QPI_MODE;
  cmd.Address = 0;
  cmd.AlternateBytes = 0;
  cmd.AddressSize = QSPI_ADDRESS_8_BITS;
  cmd.AlternateBytesSize = QSPI_ALTERNATE_BYTES_8_BITS;
  cmd.DummyCycles = 0;
  cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
  cmd.AddressMode = QSPI_ADDRESS_NONE;
  cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  cmd.DataMode = QSPI_DATA_NONE;
  cmd.NbData = 0;
  cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
  cmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
  cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

  halrc = HAL_QSPI_Command(&hqspi, &cmd, W25Q_CMD_TIMEOUT);
  if (HAL_OK == halrc) {
    rc = XWOK;
  } else {
    rc = -EIO;
  }
  return rc;
}

static
xwer_t MX_W25Q_Disable_QPI_Mode(void)
{
  QSPI_CommandTypeDef cmd;
  HAL_StatusTypeDef halrc;
  xwer_t rc;

  cmd.Instruction = W25Q_CMD_EXIT_QPI_MODE;
  cmd.Address = 0;
  cmd.AlternateBytes = 0;
  cmd.AddressSize = QSPI_ADDRESS_8_BITS;
  cmd.AlternateBytesSize = QSPI_ALTERNATE_BYTES_8_BITS;
  cmd.DummyCycles = 0;
  cmd.InstructionMode = QSPI_INSTRUCTION_4_LINES;
  cmd.AddressMode = QSPI_ADDRESS_NONE;
  cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  cmd.DataMode = QSPI_DATA_NONE;
  cmd.NbData = 0;
  cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
  cmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
  cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

  halrc = HAL_QSPI_Command(&hqspi, &cmd, W25Q_CMD_TIMEOUT);
  if (HAL_OK == halrc) {
    rc = XWOK;
  } else {
    rc = -EIO;
  }
  return rc;
}

xwer_t MX_W25Q_Set_BusMode(xwu32_t busmode)
{
  xwer_t rc;
  xwu8_t sr2;

  if (w25q.bus == busmode) {
    rc = -EALREADY;
  } else if (W25Q_BUS_MODE_QPI == busmode) {
    sr2 = 0;
    rc = MX_W25Q_Read_SR(2, &sr2);
    if ((XWOK == rc) && (0 == (sr2 & W25Q_SR2_QE))) {
      rc = MX_W25Q_Write_Enable();
      if (XWOK == rc) {
        sr2 |= W25Q_SR2_QE;
        rc = MX_W25Q_Write_SR(2, sr2);
      }
    }
    if (XWOK == rc) {
      rc = MX_W25Q_Enable_QPI_Mode();
      if (XWOK == rc) {
        w25q.bus = W25Q_BUS_MODE_QPI;
      }
    }
  } else if (W25Q_BUS_MODE_SPI == busmode) {
    rc = MX_W25Q_Disable_QPI_Mode();
    if (XWOK == rc) {
      rc = MX_W25Q_Read_SR(2, &sr2);
      if ((XWOK == rc) && (0 != (sr2 & W25Q_SR2_QE))) {
        sr2 &= ~W25Q_SR2_QE;
        rc = MX_W25Q_Write_SR(2, sr2);
      }
      w25q.bus = W25Q_BUS_MODE_SPI;
    }
  } else {
    rc = -EINVAL;
  }
  return rc;
}

static
xwer_t MX_W25Q_Enter_4Byte_Addr_Mode(void)
{
  QSPI_CommandTypeDef cmd;
  HAL_StatusTypeDef halrc;
  xwer_t rc;

  cmd.Instruction = W25Q_CMD_ENTER_4BYTE_ADDR_MODE;
  cmd.Address = 0;
  cmd.AlternateBytes = 0;
  cmd.AddressSize = QSPI_ADDRESS_8_BITS;
  cmd.AlternateBytesSize = QSPI_ALTERNATE_BYTES_8_BITS;
  cmd.DummyCycles = 0;
  if (W25Q_BUS_MODE_QPI == w25q.bus) {
    cmd.InstructionMode = QSPI_INSTRUCTION_4_LINES;
  } else {
    cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
  }
  cmd.AddressMode = QSPI_ADDRESS_NONE;
  cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  cmd.DataMode = QSPI_DATA_NONE;
  cmd.NbData = 0;
  cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
  cmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
  cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

  halrc = HAL_QSPI_Command(&hqspi, &cmd, W25Q_CMD_TIMEOUT);
  if (HAL_OK == halrc) {
    rc = XWOK;
  } else {
    rc = -EIO;
  }
  return rc;
}

static
xwer_t MX_W25Q_Exit_4Byte_Addr_Mode(void)
{
  QSPI_CommandTypeDef cmd;
  HAL_StatusTypeDef halrc;
  xwer_t rc;

  cmd.Instruction = W25Q_CMD_EXIT_4BYTE_ADDR_MODE;
  cmd.Address = 0;
  cmd.AlternateBytes = 0;
  cmd.AddressSize = QSPI_ADDRESS_8_BITS;
  cmd.AlternateBytesSize = QSPI_ALTERNATE_BYTES_8_BITS;
  cmd.DummyCycles = 0;
  if (W25Q_BUS_MODE_QPI == w25q.bus) {
    cmd.InstructionMode = QSPI_INSTRUCTION_4_LINES;
  } else {
    cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
  }
  cmd.AddressMode = QSPI_ADDRESS_NONE;
  cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  cmd.DataMode = QSPI_DATA_NONE;
  cmd.NbData = 0;
  cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
  cmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
  cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

  halrc = HAL_QSPI_Command(&hqspi, &cmd, W25Q_CMD_TIMEOUT);
  if (HAL_OK == halrc) {
    rc = XWOK;
  } else {
    rc = -EIO;
  }
  return rc;
}

xwer_t MX_W25Q_Set_AddrBits(xwu32_t addrbits)
{
  xwer_t rc;
  xwu8_t sr3;

  if (w25q.addrbits == addrbits) {
    rc = -EALREADY;
  } else if (QSPI_ADDRESS_24_BITS == addrbits) {
    sr3 = 0;
    rc = MX_W25Q_Read_SR(3, &sr3);
    if ((XWOK == rc) && (0 != (sr3 & W25Q_SR3_ADS))) {
      rc = MX_W25Q_Write_Enable();
      if (XWOK == rc) {
        rc = MX_W25Q_Exit_4Byte_Addr_Mode();
      }
    }
    if (XWOK == rc) {
      w25q.addrbits = QSPI_ADDRESS_24_BITS;
    }
  } else if (QSPI_ADDRESS_32_BITS == addrbits) {
    sr3 = 0;
    rc = MX_W25Q_Read_SR(3, &sr3);
    if ((XWOK == rc) && (0 == (sr3 & W25Q_SR3_ADS))) {
      rc = MX_W25Q_Write_Enable();
      if (XWOK == rc) {
        rc = MX_W25Q_Enter_4Byte_Addr_Mode();
      }
    }
    if (XWOK == rc) {
      w25q.addrbits = QSPI_ADDRESS_32_BITS;
    }
  } else {
    rc = -EINVAL;
  }
  return rc;
}

static
xwer_t MX_W25Q_Set_ReadParameters(void)
{
  QSPI_CommandTypeDef cmd;
  HAL_StatusTypeDef halrc;
  xwer_t rc;
  xwu8_t rdpara;

  cmd.Instruction = W25Q_CMD_SET_READ_PARAMETERS;
  cmd.Address = 0;
  cmd.AlternateBytes = 0;
  cmd.AddressSize = QSPI_ADDRESS_8_BITS;
  cmd.AlternateBytesSize = QSPI_ALTERNATE_BYTES_8_BITS;
  cmd.DummyCycles = 0;
  if (W25Q_BUS_MODE_QPI == w25q.bus) {
    cmd.InstructionMode = QSPI_INSTRUCTION_4_LINES;
    cmd.DataMode = QSPI_DATA_4_LINES;
  } else {
    cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    cmd.DataMode = QSPI_DATA_1_LINE;
  }
  cmd.AddressMode = QSPI_ADDRESS_NONE;
  cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  cmd.NbData = 1;
  cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
  cmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
  cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

  rc = MX_W25Q_Write_Enable();
  if (XWOK == rc) {
    halrc = HAL_QSPI_Command(&hqspi, &cmd, W25Q_CMD_TIMEOUT);
    if (HAL_OK == halrc) {
      rdpara = 0x30; /* 8 dummy clock @ 100MHz */
      halrc = HAL_QSPI_Transmit(&hqspi, &rdpara, W25Q_CMD_TIMEOUT);
      if (HAL_OK == halrc) {
        rc = XWOK;
      } else {
        rc = -EIO;
      }
    } else {
      rc = -EIO;
    }
  }
  return rc;
}

xwer_t MX_W25Q_Mmap(void)
{
  QSPI_CommandTypeDef cmd;
  QSPI_MemoryMappedTypeDef mapcfg;
  HAL_StatusTypeDef halrc;
  xwer_t rc;

  cmd.Instruction = W25Q_CMD_FAST_READ_DATA;
  cmd.Address = 0;
  cmd.AlternateBytes = 0;
  cmd.AddressSize = w25q.addrbits;
  cmd.AlternateBytesSize = QSPI_ALTERNATE_BYTES_8_BITS;
  cmd.DummyCycles = 8;
  if (W25Q_BUS_MODE_QPI == w25q.bus) {
    cmd.InstructionMode = QSPI_INSTRUCTION_4_LINES;
    cmd.AddressMode = QSPI_ADDRESS_4_LINES;
    cmd.DataMode = QSPI_DATA_4_LINES;
  } else {
    cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    cmd.AddressMode = QSPI_ADDRESS_1_LINE;
    cmd.DataMode = QSPI_DATA_1_LINE;
  }
  cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  cmd.NbData = 0;
  cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
  cmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
  cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

  mapcfg.TimeOutActivation = QSPI_TIMEOUT_COUNTER_DISABLE;

  halrc = HAL_QSPI_MemoryMapped(&hqspi, &cmd, &mapcfg);
  if (HAL_OK == halrc) {
    rc = XWOK;
  } else {
    rc = -EIO;
  }
  return rc;
}

xwer_t MX_W25Q_Reset(void)
{
  QSPI_CommandTypeDef cmd;
  HAL_StatusTypeDef halrc;
  xwer_t rc;

  cmd.Instruction = W25Q_CMD_ENABLE_RESET;
  cmd.Address = 0;
  cmd.AlternateBytes = 0;
  cmd.AddressSize = QSPI_ADDRESS_8_BITS;
  cmd.AlternateBytesSize = QSPI_ALTERNATE_BYTES_8_BITS;
  cmd.DummyCycles = 0;
  if (W25Q_BUS_MODE_QPI == w25q.bus) {
    cmd.InstructionMode = QSPI_INSTRUCTION_4_LINES;
  } else {
    cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
  }
  cmd.DataMode = QSPI_DATA_NONE;
  cmd.AddressMode = QSPI_ADDRESS_NONE;
  cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  cmd.NbData = 0;
  cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
  cmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
  cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

  halrc = HAL_QSPI_Command(&hqspi, &cmd, W25Q_CMD_TIMEOUT);
  if (HAL_OK == halrc) {
    cmd.Instruction = W25Q_CMD_RESET;
    halrc = HAL_QSPI_Command(&hqspi, &cmd, W25Q_CMD_TIMEOUT);
    if (HAL_OK == halrc) {
      rc = XWOK;
    } else {
      rc = -EIO;
    }
  } else {
    rc = -EIO;
  }
  return rc;
}

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
