#!/usr/local/bin/node

/**
 * @file index.js
 * @project Web-panel
 * @author Pavel Shabardin (<bigbn@mail.ru>) Saturday, 26th December 2020 12:00:33 pm
 * @copyright 2015 - 2020 SKAT LLC, Delive LLC
 */
const gypBuild = require('node-gyp-build')
const bindings = gypBuild(__dirname)
const Oled = require('rpi-oled')
const _ = require('lodash')
const font = require('oled-font-5x7')
const raspi = require('raspi');
const gpio = require('raspi-gpio');


const instrument = (name) => {
  return {
    Gain: "",
    Threshold: "",
    ScanTime: "",
    MaskTime: ""
  }
}

const menu = {
  "Instuments": {
    "HiHat": instrument('HiHat'),
    "Snare": instrument('Snare'),
    "Kick": instrument('Kick'),
    "Crash": instrument('Crash'),
    "Tom": instrument('Tom'),
  }
}

const opts = {
  width: 128, // screen width
  height: 32, // screen height
  address: 0x3C, // Pass I2C address of screen if it is not the default of 0x3C
  datasize: 8, // Change the amount of bytes sent at once (default 16)
  device: '/dev/i2c-1', // Pass your i2c device here if it is not /dev/i2c-1
  microview: true, // set to true if you have a microview display
}


console.log('Init...')
const oled = new Oled(opts)

oled.update();
oled.dimDisplay(true);
oled.clearDisplay();
oled.setCursor(0, 0);
oled.writeString(font, 1, 'Ready - [OK]', 1, false);

let offTimeout = null
const enableDisplay = _.throttle(() => oled.dimDisplay(false), 5000)

const update = (volume) => {
  oled.clearDisplay();
  const value = Math.ceil(volume * 2 * 118)
  oled.fillRect(10, 14, value, 16, 1);
  oled.update()
}

bindings.init(10, _.debounce((data) => {  
//  console.log(data)
  if (data.instrumentId >= 0 || data.instrumentId < 10) {
//   update(data.volume)
//   setTimeout(update, 200, 0)
  }
}, 50))


const moveDown = () => {
  oled.clearDisplay()
  oled.setCursor(0, 0);
  oled.writeString(font, 1, 'DOWN - [OK]', 1, true)
  oled.update()
}

const moveUp = () => {
  oled.clearDisplay()
  oled.setCursor(0, 0);
  oled.writeString(font, 1, 'UP - [OK]', 1, true)
  oled.update()
}

const moveEnter = () => {
  oled.clearDisplay()
  oled.setCursor(0, 0);
  oled.writeString(font, 1, 'ENTER - [OK]', 1, true)
  oled.update()
}


raspi.init(function() {
  console.log('Initialized')
  const buttonUp = new gpio.DigitalInput({
    pin: 'GPIO17',
    pullResistor: gpio.PULL_DOWN
  });

  const buttonEnter = new gpio.DigitalInput({
    pin: 'GPIO22',
    pullResistor: gpio.PULL_DOWN
  });
  
  const buttonDown = new gpio.DigitalInput({
    pin: 'GPIO27',
    pullResistor: gpio.PULL_DOWN
  });
  
  const dim = () => {
    clearTimeout(offTimeout)
    enableDisplay()
    offTimeout = setTimeout(() => { 
      oled.clearDisplay()
      oled.setCursor(0, 0);
      oled.writeString(font, 1, 'Ready - [OK]', 1, false)
      oled.dimDisplay(true) 
    }, 10000)
  }

  buttonUp.on('change', (value) => {
   console.log('---', value)
   dim()
   if (value) moveUp()
  })

  buttonDown.on('change', (value) => {
   dim()
   if (value) moveDown()
  })

  buttonEnter.on('change', (value) => {
   dim()
   if (value) moveEnter()
  })

  dim()
})

console.log('Init complete')
