// this code adapted from HAP-NodeJS fountain_LIGHT

// MQTT Setup
var mqtt = require('mqtt');
console.log("Connecting to MQTT broker...");
var mqtt = require('mqtt');
var options = {
  port: 1883,
  host: '127.0.0.17',
  clientId: 'deck_light'
};
var client = mqtt.connect(options);
console.log("Decklight Connected to MQTT broker");

client.subscribe('LED01-Power');
client.subscribe('LED01-Hue');
client.subscribe('LED01-Saturation');
client.subscribe('LED01-Brightness');

var Accessory = require('../').Accessory;
var Service = require('../').Service;
var Characteristic = require('../').Characteristic;
var uuid = require('../').uuid;

// here's a fake hardware device that we'll expose to HomeKit
var DECK_LIGHT = {
  powerOn: 0,
  brightness: 100, // percentage
  hue: 0,
  saturation: 0,

  setPowerOn: function(on) {
    console.log("Turning decklight %s!", on ? "on" : "off");

    client.publish('LED01-Power', on ? "1" : "0", {qos: 0, retain: true});
    DECK_LIGHT.powerOn = on;
  },

  setBrightness: function(brightness) {
    console.log("Setting light brightness to %s", brightness);
    client.publish('LED01-Brightness',String(brightness), {qos: 0, retain: true});
    DECK_LIGHT.brightness = brightness;
  },

  setHue: function(hue){
    console.log("Setting light Hue to %s", hue);
    client.publish('LED01-Hue',String(hue), {qos: 0, retain: true});
    DECK_LIGHT.hue = hue;
  },

  setSaturation: function(saturation){
    console.log("Setting light Saturation to %s", saturation);
    client.publish('LED01-Saturation',String(saturation), {qos: 0, retain: true});
    DECK_LIGHT.saturation = saturation;
  },

  identify: function() {
    console.log("Identify the light!");
  }
}

// Generate a consistent UUID for our light Accessory that will remain the same even when
// restarting our server. We use the `uuid.generate` helper function to create a deterministic
// UUID based on an arbitrary "namespace" and the word "Fountain".
var lightUUID = uuid.generate('hap-nodejs:accessories:LED01');

// This is the Accessory that we'll return to HAP-NodeJS that represents our fake light.
var light = exports.accessory = new Accessory('Light', lightUUID);

// Add properties for publishing (in case we're using Core.js and not BridgedCore.js)
light.username = "1A:2B:3C:5D:6E:FF";
light.pincode = "031-45-154";

// set some basic properties (these values are arbitrary and setting them is optional)
light
  .getService(Service.AccessoryInformation)
  .setCharacteristic(Characteristic.Manufacturer, "Oltica")
  .setCharacteristic(Characteristic.Model, "Rev-1")
  .setCharacteristic(Characteristic.SerialNumber, "A1S2NASF88EW");

// listen for the "identify" event for this Accessory
light.on('identify', function(paired, callback) {
  DECK_LIGHT.identify();
  callback(); // success
});

// Add the actual Lightbulb Service and listen for change events from iOS.
// We can see the complete list of Services and Characteristics in `lib/gen/HomeKitTypes.js`
light
  .addService(Service.Lightbulb, "Deck Light") // services exposed to the user should have "names" like "Fake Light" for us
  .getCharacteristic(Characteristic.On)
  .on('set', function(value, callback) {
    DECK_LIGHT.setPowerOn(value);
    callback(); // Our fake Light is synchronous - this value has been successfully set
  });

// We want to intercept requests for our current power state so we can query the hardware itself instead of
// allowing HAP-NodeJS to return the cached Characteristic.value.
light
  .getService(Service.Lightbulb)
  .getCharacteristic(Characteristic.On)
  .on('get', function(callback) {

    // this event is emitted when you ask Siri directly whether your light is on or not. you might query
    // the light hardware itself to find this out, then call the callback. But if you take longer than a
    // few seconds to respond, Siri will give up.

    var err = null; // in case there were any problems

    if (DECK_LIGHT.powerOn) {
      console.log("Are we on? Yes.");
      callback(err, true);
    }
    else {
      console.log("Are we on? No.");
      callback(err, false);
    }

  });

// also add an "optional" Characteristic for Brightness
light
  .getService(Service.Lightbulb)
  .addCharacteristic(Characteristic.Brightness)
  .on('get', function(callback) {
    callback(null, DECK_LIGHT.brightness);
  })
  .on('set', function(value, callback) {
    DECK_LIGHT.setBrightness(value);
    callback();
  });

light
  .getService(Service.Lightbulb)
  .addCharacteristic(Characteristic.Hue)
  .on('get',function(callback){
   callback(null,DECK_LIGHT.hue);
   })
   .on('set',function(value,callback){
   DECK_LIGHT.setHue(value);
   callback();
 });

light
  .getService(Service.Lightbulb)
  .addCharacteristic(Characteristic.Saturation)
  .on('get',function(callback){
   callback(null,DECK_LIGHT.saturation);
   })
   .on('set',function(value,callback){
   DECK_LIGHT.setSaturation(value);
   callback();
 });

// mqtt subscription handler

client.on('message', function(topic, message) {
  console.log("topic is ")
  console.log(topic);
  console.log(message.toString());

  if (topic == "LED01-Power" || topic == "LED01-Hue" || topic == "LED01-Saturation" || topic == "LED01-Brightness") {

    tempValue = parseFloat(message);
    if ( isNaN(tempValue) ) {
      console.log("not a number");
      return;
    }
    console.log(tempValue);
  }

  if (topic == "LED01-Power" ) {
    if ( DECK_LIGHT.powerOn != tempValue ) {
     console.log("updating power");
     DECK_LIGHT.powerOn = tempValue;
      light
        .getService(Service.Lightbulb)
        .setCharacteristic(Characteristic.On, tempValue);
    } // deck_light.powerOn
  } // power

  if (topic == "LED01-Hue" ) {
    if ( DECK_LIGHT.hue != tempValue ) {
      console.log("updating hue");
      DECK_LIGHT.hue = tempValue;
      light
        .getService(Service.Lightbulb)
        .setCharacteristic(Characteristic.Hue, tempValue);
    }
  }

  if (topic == "LED01-Saturation" ) {
    if ( DECK_LIGHT.saturation != tempValue ) {
      console.log("updating saturation");
      DECK_LIGHT.saturation = tempValue;
      light
        .getService(Service.Lightbulb)
        .setCharacteristic(Characteristic.Saturation, tempValue);
    }
  } 

  if (topic == "LED01-Brightness" ) {
    if ( DECK_LIGHT.brightness != tempValue ) {
      console.log("updating brightness");
      DECK_LIGHT.brightness = tempValue;
      light
        .getService(Service.Lightbulb)
        .setCharacteristic(Characteristic.Brightness, tempValue);
    }
  } 


}); // client.on

