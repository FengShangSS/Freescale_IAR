#include "headfile.h"

extern uint16 test_ad_max, test_ad_now, test_ad_min;

// AD参数
uint16 ad_val_org[5][6], ad_val_ave[5], ad_val_sum[5], ad_val_phl[5][4], point_og_transition[2] = {40, 40};
int16 gy_ad_val[5];
uint16 ad_val_max_temp[5], ad_val_min_temp[5];
uint16 AD_val_min = 0;
uint16 AD_val_max = 2600;

int16 ad_error;
int16 ad_err_ring;
uint16 position;
int16 ad_max_lef, ad_max_rig;

void AD_get()
{
    uint16  i, j;
    /*=========================每组AD获取6组数据==========================*/
    for(i = 0; i < 6; i++)
    {
        ad_val_org[0][i] = ad_ave(LEFT_ADC_CH, ADC_12bit, 6);  	        //左
        ad_val_org[1][i] = ad_ave(MID_LEFT_ADC_CH, ADC_12bit, 6);          //左垂直
        ad_val_org[2][i] = ad_ave(MID_ADC_CH, ADC_12bit, 6);  		//中
        ad_val_org[3][i] = ad_ave(MID_RIGHT_ADC_CH, ADC_12bit, 6);     	//右垂直
        ad_val_org[4][i] = ad_ave(RIGHT_ADC_CH, ADC_12bit, 6);     	//右
//        ad_val_org[5][i] = ad_ave(ADC0_SE4, ADC_12bit, 5);     		//空置
    }
    
    /*=========================中值滤波==========================*///舍弃最大值和最小值
    for(i = 0; i < 5; i++)
    {
        ad_val_sum[i] = ad_val_max_temp[i] = ad_val_min_temp[i] = ad_val_org[i][0];
        
        for(j = 1; j < 6; j++)
        {
            ad_val_sum[i] += ad_val_org[i][j];
            ad_val_max_temp[i] = (ad_val_max_temp[i] < ad_val_org[i][j]) ? ad_val_org[i][j] : ad_val_max_temp[i];
            ad_val_min_temp[i] = (ad_val_min_temp[i] > ad_val_org[i][j]) ? ad_val_org[i][j] : ad_val_min_temp[i];
        }
        
        ad_val_sum[i] = ad_val_sum[i] - ad_val_max_temp[i] - ad_val_min_temp[i];
        ad_val_ave[i] = ad_val_sum[i] >> 2;
        ad_val_sum[i] = 0;
    }   
    
    
    /*=========================滑动平均滤波==========================*/    
    for(i = 0; i < 3; i++)
    {
        ad_val_phl[0][i] = ad_val_phl[0][i + 1];
        ad_val_phl[1][i] = ad_val_phl[1][i + 1];
        ad_val_phl[2][i] = ad_val_phl[2][i + 1];
        ad_val_phl[3][i] = ad_val_phl[3][i + 1];
        ad_val_phl[4][i] = ad_val_phl[4][i + 1];
    }
    
    for(i = 0; i < 5; i++)
    {
        ad_val_phl[i][3] = ad_val_ave[i];
    }
    
    for(i = 0; i < 4; i++)
    {
        ad_val_sum[0] += ad_val_phl[0][i];
        ad_val_sum[1] += ad_val_phl[1][i];
        ad_val_sum[2] += ad_val_phl[2][i];
        ad_val_sum[3] += ad_val_phl[3][i];
        ad_val_sum[4] += ad_val_phl[4][i];
    }
    
    for(i = 0; i < 5; i++)
    {
        ad_val_ave[i] = ad_val_sum[i] >> 2; 

        //限幅 
        ad_val_ave[i] = (ad_val_ave[i] > AD_val_max) ? AD_val_max : ad_val_ave[i];
        ad_val_ave[i] = (ad_val_ave[i] < AD_val_min) ? AD_val_min : ad_val_ave[i];
        
        //归一化
        gy_ad_val[i] = 100 * (ad_val_ave[i] - AD_val_min) / (AD_val_max - AD_val_min);  
    }
    
    //test max and min
    test_ad_now = ad_val_sum[2] >> 2;
    test_ad_min = ad_val_sum[4] >> 2;
    test_ad_max = (test_ad_min > test_ad_max) ? test_ad_min : test_ad_max;
}

void Data_analyse(void)
{
    ad_error = 100 * (-gy_ad_val[LEFT_ADC] + gy_ad_val[RIGHT_ADC]) / (gy_ad_val[LEFT_ADC] + gy_ad_val[RIGHT_ADC]);
    ad_err_ring = 100 * (-gy_ad_val[MID_LEFT_ADC] + gy_ad_val[MID_RIGHT_ADC]) / (gy_ad_val[MID_LEFT_ADC] + gy_ad_val[MID_RIGHT_ADC]);
}
//
//void Data_analyse(void)
//{
//    uint16 i;
//    uint16 max_index = 0, max_crosstalk = 2;//, max_val;//, max_index_last = 1;//;
//
//    //找出最大值
//    for(i = 2; i < 5; i += 2)
//    {
//        if(gy_ad_val[max_index] < gy_ad_val[i])
//            max_index = i;
//    }
////    max_val = gy_ad_val[max_index];
//    
//    //丢线时最大值取旧值
////    if(max_val < 20)
////    {
////        max_index = max_index_last;
////        max_val = gy_ad_val[max_index];
////    }
////    else
////    {
////        max_index_last = max_index;
////    }
//    
//    //防串道
//    if(abs(max_index - max_crosstalk) <= 2)
//    {
//        max_crosstalk = max_index;
//    }
//    else
//    {
//        max_index = max_crosstalk;
//    }
//    
//    
//    //位置解算
//    if(max_index == LEFT_ADC && (gy_ad_val[MID_ADC] <= point_og_transition[0]))
//    {
//        position = 1;
//    }
//    else if((max_index == LEFT_ADC && (gy_ad_val[MID_ADC] > point_og_transition[0])) \
//             || (max_index == MID_ADC && (gy_ad_val[LEFT_ADC] - gy_ad_val[RIGHT_ADC]) > 0) )
//    {
//        position = 2;
//        ad_max_lef = gy_ad_val[LEFT_ADC];
//    }
//    else if((max_index == RIGHT_ADC && (gy_ad_val[MID_ADC] > point_og_transition[1])) \
//             || (max_index == MID_ADC && (gy_ad_val[RIGHT_ADC] - gy_ad_val[LEFT_ADC]) >= 0) )
//    {
//        position = 3;
//        ad_max_rig = gy_ad_val[RIGHT_ADC];
//    }
//    else if(max_index == RIGHT_ADC && (gy_ad_val[MID_ADC] <= point_og_transition[1]))
//    {
//        position = 4;
//    }
//    
//    //计算偏移量
//    switch(position)
//    {
//        case 1:
//            ad_error = (int16)((gy_ad_val[MID_ADC] - abs(ad_max_lef - gy_ad_val[LEFT_ADC]) - ad_max_lef) * 1.8) - 10;
//            break;
//        case 2:
//        case 3:
//            ad_error = -gy_ad_val[LEFT_ADC] + gy_ad_val[RIGHT_ADC];
//            break;
//        case 4:
//            ad_error = (int16)((-gy_ad_val[MID_ADC] + abs(ad_max_rig - gy_ad_val[RIGHT_ADC]) + ad_max_rig) * 1.8) + 16;
//            //ad_error = -gy_ad_val[MID_ADC] + ad_max_rig;
//            break;
//        default:
//            ad_error = 0;
//            break;
//    }
//    
//}
