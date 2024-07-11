### 备注信息

ADCOFFSET 单位 mV
ADCGAIN 单位 uV

Cell 电压计算
V(cell) = GAIN x ADC(cell) + OFFSET

    BQ76920_Init(&afe, &hi2c1);

    HAL_GPIO_TogglePin(LED_A_GPIO_Port, LED_A_Pin);

    BQ76920_Step(&afe);
    BQ76920_UpdateBalanceCell(&afe);

    BQ76920_SYS_STAT_T state;
    BQ76920_SysStat(&afe, &state);
    CAW_LOG_DEBUG("state: %d %d %d %d %d %d %d %d", state.CC_READY, state.RSVD,
                  state.DEVICE_XREADY, state.OVRD_ALERT, state.UV, state.OV,
                  state.SCD, state.OCD);
    CAW_LOG_DEBUG("c1: %f c2: %f c3: %f c4: %f c5 %f", afe.CellVoltage[0],
                  afe.CellVoltage[1], afe.CellVoltage[2], afe.CellVoltage[3],
                  afe.CellVoltage[4]);
    CAW_LOG_DEBUG("VPack: %f, Current: %f", afe.VPack, afe.current);
    BQ76920_SYS_CTRL2_T ctl2;
    BQ76920_SysCtrl2(&afe, &ctl2);

    CAW_LOG_DEBUG("DSG: %d CHG: %d", ctl2.DSG_ON, ctl2.CHG_ON);

    BQ76920_CELLBAL1_T cellbal;
    BQ76920_CellBal1(&afe, &cellbal);
    CAW_LOG_DEBUG("cb1: %d cb2: %d cb3: %d cb4: %d cb5: %d", cellbal.CB1,
                  cellbal.CB2, cellbal.CB3, cellbal.CB4, cellbal.CB5);
