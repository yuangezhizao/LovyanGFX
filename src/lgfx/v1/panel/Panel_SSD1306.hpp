/*----------------------------------------------------------------------------/
  Lovyan GFX - Graphics library for embedded devices.

Original Source:
 https://github.com/lovyan03/LovyanGFX/

Licence:
 [FreeBSD](https://github.com/lovyan03/LovyanGFX/blob/master/license.txt)

Author:
 [lovyan03](https://twitter.com/lovyan03)

Contributors:
 [ciniml](https://github.com/ciniml)
 [mongonta0716](https://github.com/mongonta0716)
 [tobozo](https://github.com/tobozo)
/----------------------------------------------------------------------------*/
#pragma once

#include "Panel_HasBuffer.hpp"
#include "../misc/range.hpp"

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  struct Panel_1bitOLED : public Panel_HasBuffer
  {
    bool init(bool use_reset) override;

    void waitDisplay(void) override;
    bool displayBusy(void) override;
    color_depth_t setColorDepth(color_depth_t depth) override;

    void setInvert(bool invert) override;
    void setSleep(bool flg) override;
    void setPowerSave(bool) override {}

    void writeFillRectPreclipped(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, uint32_t rawcolor) override;
    void writeImage(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, pixelcopy_t* param, bool use_dma) override;
    void writePixels(pixelcopy_t* param, uint32_t len, bool use_dma) override;

    uint32_t readCommand(uint_fast16_t, uint_fast8_t, uint_fast8_t) override { return 0; }
    uint32_t readData(uint_fast8_t, uint_fast8_t) override { return 0; }

    void readRect(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, void* dst, pixelcopy_t* param) override;
    void setTilePattern(uint_fast8_t);

  protected:

    static constexpr uint8_t CMD_SETSTARTLINE        = 0x40;
    static constexpr uint8_t CMD_DISPLAYALLON_RESUME = 0xA4;
    static constexpr uint8_t CMD_DISPLAYALLON        = 0xA5;
    static constexpr uint8_t CMD_NORMALDISPLAY       = 0xA6;
    static constexpr uint8_t CMD_INVERTDISPLAY       = 0xA7;
    static constexpr uint8_t CMD_SETMULTIPLEX        = 0xA8;
    static constexpr uint8_t CMD_DISP_OFF            = 0xAE;
    static constexpr uint8_t CMD_DISP_ON             = 0xAF;
    static constexpr uint8_t CMD_SETPRECHARGE        = 0xD9;
    static constexpr uint8_t CMD_SETVCOMDETECT       = 0xDB;

    uint8_t _bayer_offset = 0;

    size_t _get_buffer_length(void) const override;
    bool _read_pixel(uint_fast16_t x, uint_fast16_t y);
    void _draw_pixel(uint_fast16_t x, uint_fast16_t y, uint32_t value);
    virtual void _update_transferred_rect(uint_fast16_t &xs, uint_fast16_t &ys, uint_fast16_t &xe, uint_fast16_t &ye);

  };

  struct Panel_SSD1306 : public Panel_1bitOLED
  {
    Panel_SSD1306(void)
    {
      _cfg.memory_width  = _cfg.panel_width  = 128;
      _cfg.memory_height = _cfg.panel_height = 64;
    }

    bool init(bool use_reset) override;
    void setBrightness(uint8_t brightness) override;
    void display(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h) override;

    /// OLEDパネルのCOMピン配置を設定する。;
    /// This command sets the COM signals pin configuration to match the OLED panel hardware layout.
    /// see SSD1306 data sheet  ( 10.1.18 Set COM Pins Hardware Configuration (DAh)
    /// @param data SETCOMPINSで送信するデータ (0x02 or 0x12 or 0x22 or 0x32)
    void setComPins(uint8_t data = 0x02);

  protected:

    uint8_t _compins = 0x12;

    static constexpr uint8_t CMD_MEMORYMODE  = 0x20;

    static constexpr uint8_t CMD_COLUMNADDR  = 0x21;
    static constexpr uint8_t CMD_PAGEADDR    = 0x22;

    static constexpr uint8_t CMD_SETCONTRAST = 0x81;
    static constexpr uint8_t CMD_CHARGEPUMP  = 0x8D;
    static constexpr uint8_t CMD_SEGREMAP    = 0xA0;
//  static constexpr uint8_t CMD_SETMLTPLX   = 0xA8;
    static constexpr uint8_t CMD_COMSCANINC  = 0xC0;
//  static constexpr uint8_t CMD_COMSCANDEC  = 0xC8;
    static constexpr uint8_t CMD_SETOFFSET   = 0xD3;
    static constexpr uint8_t CMD_SETCLKDIV   = 0xD5;
    static constexpr uint8_t CMD_SETCOMPINS  = 0xDA;

    static constexpr uint8_t CMD_DEACTIVATE_SCROLL   = 0x2E;

    const uint8_t* getInitCommands(uint8_t listno) const override
    {
      static constexpr uint8_t list0[] = {
        CMD_DISP_OFF           ,
        CMD_SETCLKDIV          , 0x80,
        CMD_SETMULTIPLEX       , 0x3F,
        CMD_SETOFFSET          , 0x00,
        CMD_SETSTARTLINE       ,
        CMD_MEMORYMODE         , 0x00,
        CMD_SEGREMAP           ,
        CMD_COMSCANINC         ,
        CMD_SETVCOMDETECT      , 0x10,
        CMD_DISPLAYALLON_RESUME,
        CMD_DEACTIVATE_SCROLL  ,
        CMD_CHARGEPUMP         , 0x14,
        CMD_DISP_ON            ,
        CMD_SETCONTRAST        , 0x00,
        CMD_SETPRECHARGE       , 0x11,
        0xFF,0xFF, // end
      };
      switch (listno) {
      case 0: return list0;
      default: return nullptr;
      }
    }
  };

  struct Panel_SH110x : public Panel_1bitOLED
  {
    Panel_SH110x(void)
    {
      _cfg.memory_width  = _cfg.panel_width  = 128;
      _cfg.memory_height = _cfg.panel_height = 128;
      _auto_display = true;
    }

    void setBrightness(uint8_t brightness) override;

    void display(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h) override;

  protected:
    static constexpr uint8_t CMD_PAGEADDRESSINGMODE  = 0x20;
    static constexpr uint8_t CMD_VERTADDRESSINGMODE  = 0x21;
    static constexpr uint8_t CMD_PAGEADDR            = 0x22;
    static constexpr uint8_t CMD_SETCONTRAST         = 0x81;
//  static constexpr uint8_t CMD_CHARGEPUMP          = 0x8D;
    static constexpr uint8_t CMD_SEGREMAP            = 0xA0;
    static constexpr uint8_t CMD_DCDC                = 0xAD;
    static constexpr uint8_t CMD_SETPAGEADDR         = 0xB0;
    static constexpr uint8_t CMD_COMSCANINC          = 0xC0;
    static constexpr uint8_t CMD_COMSCANDEC          = 0xC8;
    static constexpr uint8_t CMD_SETDISPLAYOFFSET    = 0xD3;
    static constexpr uint8_t CMD_SETDISPLAYCLOCKDIV  = 0xD5;
    static constexpr uint8_t CMD_SETCOMPINS          = 0xDA;
    static constexpr uint8_t CMD_SETDISPSTARTLINE    = 0xDC;
    static constexpr uint8_t CMD_SETLOWCOLUMN        = 0x00;
    static constexpr uint8_t CMD_SETHIGHCOLUMN       = 0x10;
    static constexpr uint8_t CMD_READMODIFYWRITE     = 0xE0;
    static constexpr uint8_t CMD_READMODIFYWRITE_END = 0xEE;

    const uint8_t* getInitCommands(uint8_t listno) const override
    {
      static constexpr uint8_t list0[] = {
        CMD_DISP_OFF   ,
        CMD_SETSTARTLINE       ,
        CMD_READMODIFYWRITE_END,
        CMD_PAGEADDRESSINGMODE ,
        CMD_SETDISPSTARTLINE   , 0x00,
        CMD_SETDISPLAYCLOCKDIV , 0x50,
        CMD_SETMULTIPLEX       , 0x7F,
        CMD_SETDISPLAYOFFSET   , 0x00,
        CMD_DCDC               , 0x8B,
        CMD_SEGREMAP           ,
        CMD_COMSCANINC         ,
        CMD_SETPRECHARGE       , 0x20,
        CMD_SETVCOMDETECT      , 0x35,
        CMD_DISPLAYALLON_RESUME,
        CMD_SETCONTRAST, 0x00,
        CMD_SETCOMPINS, 0x12,
        CMD_DISP_ON    ,
        0xFF,0xFF, // end
      };
      switch (listno) {
      case 0: return list0;
      default: return nullptr;
      }
    }
  };


  struct Panel_ST7565 : public Panel_1bitOLED
  {
    Panel_ST7565(void)
    {
      _cfg.memory_width  = _cfg.panel_width  = 128;
      _cfg.memory_height = _cfg.panel_height = 64;
      _auto_display = true;
    }

    // void setBrightness(uint8_t brightness) override;
    void display(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h) override;

  protected:
    static constexpr uint8_t CMD_SETPAGEADDR         = 0xB0;
    static constexpr uint8_t CMD_SETLOWCOLUMN        = 0x00;
    static constexpr uint8_t CMD_SETHIGHCOLUMN       = 0x10;

    const uint8_t* getInitCommands(uint8_t listno) const override
    {
      static constexpr uint8_t list0[] = {
        0xAE,           //Display = OFF
        0xA0,           //ADC = normal
        0xC8,           //Common output = revers
        0xA3,           //LCD bias = 1/7

  //内部レギュレータON
        0x2C,
        //delay(2);
        0x2E,
        //delay(2);
        0x2F,

  //コントラスト設定
        0x23,           //Vo voltage regulator internal resistor ratio set
        0x81,           //Electronic volume mode set
        0x1C,           //Electronic volume register set

  //表示設定
        0xA4,           //Display all point ON/OFF = normal
        0x40,           //Display start line = 0
        // 0xA7,           //Display normal/revers = normal
        0xAF,           //Dsiplay = ON

        0xFF,0xFF, // end
      };
      switch (listno) {
      case 0: return list0;
      default: return nullptr;
      }
    }
  };

  struct Panel_SSD1312 : public Panel_SSD1306
  {
    Panel_SSD1312(void)
    {
      _cfg.memory_width  = _cfg.panel_width  = 128;
      _cfg.memory_height = _cfg.panel_height = 64;
    }

    // void setBrightness(uint8_t brightness) override;

  protected:

  // Refer 1/3
  // https://github.com/olikraus/u8g2/issues/1806
  // https://github.com/olikraus/u8g2/blob/master/csrc/u8x8_d_ssd1306_128x64_noname.c#L175-L205
  // static const uint8_t u8x8_d_ssd1312_128x64_noname_init_seq[] = {

  //   U8X8_START_TRANSFER(),             	/* enable chip, delay is part of the transfer start */

  //   U8X8_C(0x0ae),		                /* display off */
  //   U8X8_CA(0x0d5, 0x080),		/* clock divide ratio (0x00=1) and oscillator frequency (0x8) */
  //   U8X8_CA(0x0a8, 0x03f),		/* multiplex ratio */
  //   U8X8_CA(0x0d3, 0x000),		/* display offset */
  //   U8X8_C(0x040),		                /* set display start line to 0 */
  //   U8X8_CA(0x08d, 0x014),		/* [2] charge pump setting (p62): 0x014 enable, 0x010 disable, SSD1306 only, should be removed for SH1106 */
  //   U8X8_CA(0x020, 0x000),		/* horizontal addressing mode */

  //   U8X8_C(0x0a1),				/* segment remap a0/a1*/
  //   U8X8_C(0x0c0),				/* c0: scan dir normal, c8: reverse */

  //   U8X8_CA(0x0da, 0x012),		/* com pin HW config, sequential com pin config (bit 4), disable left/right remap (bit 5) */

  //   U8X8_CA(0x081, 0x0cf), 		/* [2] set contrast control */
  //   U8X8_CA(0x0d9, 0x0f1), 		/* [2] pre-charge period 0x022/f1*/
  //   U8X8_CA(0x0db, 0x040), 		/* vcomh deselect level */  
  //   // if vcomh is 0, then this will give the biggest range for contrast control issue #98
  //   // restored the old values for the noname constructor, because vcomh=0 will not work for all OLEDs, #116

  //   U8X8_C(0x02e),				/* Deactivate scroll */ 
  //   U8X8_C(0x0a4),				/* output ram to display */
  //   U8X8_C(0x0a6),				/* none inverted normal display mode */

  //   U8X8_END_TRANSFER(),             	/* disable chip */
  //   U8X8_END()             			/* end of sequence */
  // };

  // Refer 2/3
  // https://github.com/olikraus/u8g2/issues/2368
  // https://github.com/olikraus/u8g2/blob/master/csrc/u8x8_d_ssd1312.c#L144-L177
  // static const uint8_t u8x8_d_ssd1312_128x32_init_seq[] = {

  //   U8X8_START_TRANSFER(),             	/* enable chip, delay is part of the transfer start */

  //   U8X8_C(0x0ae),		                /* display off */
  //   U8X8_CA(0x0d5, 0x080),		/* clock divide ratio (0x00=1) and oscillator frequency (0x8) */
  //   U8X8_CA(0x0a8, 0x03f),		/* multiplex ratio */
  //   U8X8_CA(0x0d3, 0x000),		/* display offset */
  //   U8X8_C(0x040),		                /* set display start line to 0 */
  //   U8X8_CA(0x08d, 0x014),		/* [2] charge pump setting (p62): 0x014 enable, 0x010 disable, SSD1306 only, should be removed for SH1106 */
  //   U8X8_CA(0x020, 0x000),		/* horizontal addressing mode */

  //   U8X8_C(0x0a1),				/* segment remap a0/a1*/
  //   U8X8_C(0x0c0),				/* c0: scan dir normal, c8: reverse */

  //   /* 11 Aug 2025: The value below doesn't mach the bits 4 and 5, actually 0x12 would enable the alternate odd/even remapping */
  //   /* probably the correct configuration value should be 0x00, see https://github.com/olikraus/u8g2/issues/2690 ??? */
  //   U8X8_CA(0x0da, 0x012),		/* com pin HW config, sequential com pin config (bit 4), disable left/right remap (bit 5) */

  //   U8X8_CA(0x0d3, 0x030),		/* line shift by 3*16 = 48 */

  //   U8X8_CA(0x081, 0x0cf), 		/* [2] set contrast control */
  //   U8X8_CA(0x0d9, 0x0f1), 		/* [2] pre-charge period 0x022/f1*/
  //   U8X8_CA(0x0db, 0x040), 		/* vcomh deselect level */  
  //   // if vcomh is 0, then this will give the biggest range for contrast control issue #98
  //   // restored the old values for the noname constructor, because vcomh=0 will not work for all OLEDs, #116

  //   U8X8_C(0x02e),				/* Deactivate scroll */ 
  //   U8X8_C(0x0a4),				/* output ram to display */
  //   U8X8_C(0x0a6),				/* none inverted normal display mode */

  //   U8X8_END_TRANSFER(),             	/* disable chip */
  //   U8X8_END()             			/* end of sequence */
  // };

  // Refer 3/3
  // 1.3OLED64128-SPI(LCM)
  // #define OLED_CMD_NUMBER   25
  // const unsigned char  OLED_init_cmd[OLED_CMD_NUMBER]=      //SSD1312
  // {
  //       0xAE,//关闭显示

  //       0xD5,//设置时钟分频因子,震荡频率
  //       0x50,  //[3:0],分频因子;[7:4],震荡频率 默认0x50

  //       0xA8,//设置驱动路数
  //       0X7F,//默认(1/64)

  //       0xD3,//设置显示偏移
  //       0X00,//默认为0

  //       0x40,//设置显示开始行 [5:0],行数.

  //       0x8D,//电荷泵设置
  //       0x14,//bit2，开启/关闭
  //       0x20,//设置内存地址模式     页地址模式
  //     0x01,//bit3:0  0001 COM-PAGE模式  0010PAGE-ADDR模式   1001SEG-PAGE 模式
  //         //0x01  横向8点扫描，先从左到右，再从上到下 用0x21  0x22设置地址
  //         //0X09  类似于竖屏 页面模式，从左到右，从上到下，每次8个竖向点  用0x21  0x22设置地址   
  //         //0x02 （横屏时方便）   页面模式，每次横向8个点，从上到下依次扫描完毕0-127行，再从第二页开始
  //         //    用 0X0x  0X1x   和0Xbx 设置地址

  //   /************显示翻转命令**********************************************/
  //       0xA1,//段重定义设置,bit0:0,0->0;1,0->127;  A1
  //       0xC8,//设置COM扫描方向;bit3:0,普通模式;1,重定义模式 COM[N-1]->COM0;N:驱动路数 (C0 翻转显示) C8
  //   /********************************************************************/

  //       0xDA,//设置COM硬件引脚配置
  //       0x10,//[5:4]配置  

  //       0x81,//对比度设置
  //       0x7f,//1~255;默认0X7F (亮度设置,越大越亮)

  //       0xD9,//设置预充电周期
  //       0x22,//[3:0],PHASE 1;[7:4],PHASE 2;

  //       0xDB,//设置VCOMH 电压倍率
  //       0x37,//[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

  //       0xA4,//全局显示开启;bit0:1,开启;0,关闭;(白屏/黑屏)

  //       0xA6,//设置显示方式;bit0:1,反相显示;0,正常显示 

  //       0xAF,//开启显示
  // };

    const uint8_t* getInitCommands(uint8_t listno) const override
    {
      static constexpr uint8_t list0[] = {
        CMD_DISP_OFF, // 0xAE 关闭显示

        // CMD_SETCLKDIV, 0x80, // 0xD5, 0x80 设置时钟分频
        CMD_SETCLKDIV, 0x50, // 设置时钟分频因子 震荡频率
        // 0x50, // [3:0],分频因子;[7:4],震荡频率 默认0x50

        CMD_SETMULTIPLEX, 0x3F, // 0xA8, 0x3F 设置复用比 (64-1)
        // CMD_SETMULTIPLEX, 0X7F, // 设置驱动路数
        // 0X7F,//默认(1/64)

        CMD_SETOFFSET, 0x00, // 0xD3, 0x00 显示偏移为0
        // 设置显示偏移 默认为0

        CMD_SETSTARTLINE, // 0x40 显示开始行为0
        // 设置显示开始行 [5:0],行数

        CMD_CHARGEPUMP, 0x14, // 0x8D, 0x14 开启电荷泵
        // 电荷泵设置
        // 0x14,//bit2，开启/关闭

        CMD_MEMORYMODE, 0x00, // 0x20, 0x00 水平寻址模式
        // CMD_MEMORYMODE, 0x01, // 设置内存地址模式     页地址模式
  //     0x01,//bit3:0  0001 COM-PAGE模式  0010PAGE-ADDR模式   1001SEG-PAGE 模式
  //         //0x01  横向8点扫描，先从左到右，再从上到下 用0x21  0x22设置地址
  //         //0X09  类似于竖屏 页面模式，从左到右，从上到下，每次8个竖向点  用0x21  0x22设置地址   
  //         //0x02 （横屏时方便）   页面模式，每次横向8个点，从上到下依次扫描完毕0-127行，再从第二页开始
  //    用 0X0x  0X1x   和0Xbx 设置地址

        /************显示翻转命令**********************************************/
        CMD_SEGREMAP | 0x01, // 0xA1 段重映射
        // 段重定义设置,bit0:0,0->0;1,0->127;  A1

        CMD_COMSCANINC, // 0xC0 COM扫描方向
        // CMD_COMSCANDEC, //设置COM扫描方向;bit3:0,普通模式;1,重定义模式 COM[N-1]->COM0;N:驱动路数 (C0 翻转显示) C8
        /*********************************************************************/

        // CMD_SETCOMPINS, 0x12, // 0xDA, 0x12 COM引脚配置
        CMD_SETCOMPINS, 0x10, // 设置COM硬件引脚配置
        // 0x10,//[5:4]配置

        // CMD_SETOFFSET, 0x30, // 0xD3, 0x30 额外显示偏移设置 (参考u8g2 128x32版本，3*16=48)

        // CMD_SETCONTRAST, 0xCF, // 0x81, 0xCF 使用与u8g2相同的对比度设置
        CMD_SETCONTRAST, 0x7f, //对比度设置
        // 0x7f,//1~255;默认0X7F (亮度设置,越大越亮)

        // CMD_SETPRECHARGE, 0xF1, // 0xD9, 0xF1 使用与u8g2相同的预充电周期
        CMD_SETPRECHARGE, 0x22, // 设置预充电周期
        // 0x22,//[3:0],PHASE 1;[7:4],PHASE 2;

        CMD_SETVCOMDETECT, 0x40, // 0xDB, 0x40 VCOM检测电平 0.77*vcc
        // CMD_SETVCOMDETECT, 0x37, // 0xDB,//设置VCOMH 电压倍率
        // 0x37,//[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

        CMD_DEACTIVATE_SCROLL, // 0x2E 禁用滚动

        CMD_DISPLAYALLON_RESUME, // 0xA4 正常显示
        // 0xA4,//全局显示开启;bit0:1,开启;0,关闭;(白屏/黑屏)

        CMD_NORMALDISPLAY, // 0xA6 正常显示(非反色)
        // 0xA6,//设置显示方式;bit0:1,反相显示;0,正常显示

        CMD_DISP_ON, // 0xAF 开启显示
        0xFF,0xFF, // end
      };
      switch (listno) {
      case 0: return list0;
      default: return nullptr;
      }
    }
  };

//----------------------------------------------------------------------------
 }
}
