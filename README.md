# Zoomie Gauge
Zoomie Gauge was created as an effective DIY way of monitoring engine vitals, with the idea that visible alerts are most effective at preventing engine damage as compared to traditional gauges in motorsport applications.

<img src="https://github.com/randra99/zoomie-gauge/blob/main/zoomie.gif" width="450" title="Gauge GIF"/> <img src="https://github.com/randra99/zoomie-gauge/blob/main/zoomiealert.gif" width="456" title="Gauge GIF"/>

At the moment this has been developed and tested on an NC Mazda MX-5/Miata, however, given the right CAN frames this can be extended to virtually any CAN enabled vehicle.

## Hardware needed:
 - [Arduino UNO board](https://store.arduino.cc/products/arduino-uno-rev3)
 - [1.28 inch LCD Module](https://www.waveshare.com/wiki/1.28inch_LCD_Module)
   - Wiring instructions can be found on the same page
 - [DC to DC Buck Converter](https://www.amazon.co.uk/gp/product/B0823P6PW6/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&th=1)
 - [MCP2515 CAN Bus Module](https://www.amazon.co.uk/gp/product/B086TXSFD8/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&th=1)
   - Wiring instructions can be found [here](https://github.com/autowp/arduino-mcp2515)
 - [Bosch PST-F 1 Sensor](https://www.bosch-motorsport.com/content/downloads/Raceparts/en-GB/54249355.html)
   - Consult the data sheet for wiring 

### Nice to haves:
 - [Proto Shield](https://thepihut.com/products/arduino-proto-shield-dev-board-2-0?variant=39724524372163&country=GB&currency=GBP&utm_medium=product_sync&utm_source=google&utm_content=sag_organic&utm_campaign=sag_organic)
   - This will make splicing the connections for both the CAN Bus module and LCD module is a lot easier.
   - Furthermore, as the Arduino cannot measure resistance out of the box, a small circuit is required utilising a 4.7K ohm resistor to allow computation of the resistance based on voltage. Instructions [here](https://projecthub.arduino.cc/xristos_xatz/ohm-meter-with-arduino-uno-5576cd).

## STL Files
Currently included in `stl-files` is the stl of the screen mount, this will make it easier to mount the screen to the desired surface using double-sided tape. The centre is left empty to allow placing over an existing gauge once the needle is removed.
