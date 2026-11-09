#include "./BSP/LSENS/lsens.h"

ADC_HandleTypeDef g_adc3_handle;         /* ADC句柄 */

/**
 * @brief       ADC3初始化函数
 *   @note      本函数支持ADC1/ADC2任意通道, 但是不支持ADC3
 *              我们使用12位精度, ADC采样时钟=12M, 转换时间为: 采样周期 + 12.5个ADC周期
 *              设置最大采样周期: 239.5, 则转换时间 = 252 个ADC周期 = 21us
 * @param       无
 * @retval      无
 */
void Lsens_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    RCC_PeriphCLKInitTypeDef adc_clk_init = {0};

    LSENS_CHY_GPIO_CLK_ENABLE();                                /* IO口时钟使能 */
    LSENS_CHY_CLK_ENABLE();                                     /* ADC时钟使能 */
    LSENS_ADC3_CHX_GPIO_CLK_ENABLE();   /* IO口时钟使能 */

    adc_clk_init.PeriphClockSelection = RCC_PERIPHCLK_ADC;     /* ADC外设时钟 */
    adc_clk_init.AdcClockSelection = RCC_ADCPCLK2_DIV6;        /* 分频因子6时钟为72M/6=12MHz */

    HAL_RCCEx_PeriphCLKConfig(&adc_clk_init);                  /* 设置ADC时钟 */

    /* 设置AD采集通道对应IO引脚工作模式 */
    gpio_init_struct.Pin = LSENS_CHY_GPIO_PIN;                  /* ADC通道对应的IO引脚 */
    gpio_init_struct.Mode = GPIO_MODE_ANALOG;                  /* 模拟 */
    HAL_GPIO_Init(LSENS_CHY_GPIO_PORT, &gpio_init_struct);

    g_adc3_handle.Instance = LSENS_ADCX;                         /* 选择哪个ADC */
    g_adc3_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;        /* 数据对齐方式：右对齐 */
    g_adc3_handle.Init.ScanConvMode = ADC_SCAN_DISABLE;        /* 非扫描模式，仅用到一个通道 */
    g_adc3_handle.Init.ContinuousConvMode = DISABLE;           /* 关闭连续转换模式 */
    g_adc3_handle.Init.NbrOfConversion = 1;                    /* 1个转换在规则序列中 也就是只转换规则序列1 */
    g_adc3_handle.Init.DiscontinuousConvMode = DISABLE;        /* 禁止规则通道组间断模式 */
    g_adc3_handle.Init.NbrOfDiscConversion = 0;                /* 配置间断模式的规则通道个数，禁止规则通道组间断模式后，此参数忽略 */
    g_adc3_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;  /* 触发转换方式：软件触发 */
    
    HAL_ADC_Init(&g_adc3_handle);                              /* 初始化 */

    HAL_ADCEx_Calibration_Start(&g_adc3_handle);               /* 校准ADC */
}

/**
 * @brief       获得ADC转换后的结果
 * @param       ch: 通道值 0~17，取值范围为：ADC_CHANNEL_0~ADC_CHANNEL_17
 * @retval      无
 */
uint32_t Lsens_get_result(uint32_t ch)
{
    ADC_ChannelConfTypeDef adc_ch_conf = {0};
    
    adc_ch_conf.Channel = ch;
    adc_ch_conf.Rank = ADC_REGULAR_RANK_1;
    adc_ch_conf.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
    
    // 1. 先确保停止，防止状态机卡死
    HAL_ADC_Stop(&g_adc3_handle); 
    
    // 2. 配置通道
    if (HAL_ADC_ConfigChannel(&g_adc3_handle, &adc_ch_conf) != HAL_OK) {
        return 0; // 或者进行错误处理
    }
    
    // 3. 启动转换
    HAL_ADC_Start(&g_adc3_handle);
    
    // 4. 等待转换完成，增加超时机制
    if (HAL_ADC_PollForConversion(&g_adc3_handle, 10) == HAL_OK) {
        return HAL_ADC_GetValue(&g_adc3_handle);
    }
    
    return 0;
}



/**
 * @brief       读取光敏传感器值
 * @param       无
 * @retval      0~100:0,最暗;100,最亮
 */
uint16_t Lsens_get_val(void)
{
    uint32_t temp_val = 0;
    temp_val = Lsens_get_result(LSENS_CHX);  /* 读取平均值 */
    return (uint16_t)temp_val;
}

/**
 * @brief   计算光强 Lux
 * @param   adcv: ADC 原始值 (0~4095)
 * @retval  光强值 (Lux)
 */
float Lsens_get_lux(void)
{
    uint32_t adcv = Lsens_get_result(LSENS_CHX);
    if (adcv >= 4095) return 0.0;
    if (adcv == 0) return 100000.0; // 极强光限幅

    // 1. 计算当前电阻 R (单位: Ohm)
    // 根据电路图: R34=47K
    float R = (47000.0 * adcv) / (4095.0 - adcv);

    // 2. 根据特性曲线计算 Lux (以常用参数 A=100000, alpha=0.7 为例)
    // E = (A / R) ^ (1 / alpha)
    float lux = pow(100000.0 / R, 1.0 / 0.7);

    return lux;
}

